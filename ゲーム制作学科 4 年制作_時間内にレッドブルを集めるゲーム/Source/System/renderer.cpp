//=============================================================================
// レンダラー [renderer.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "renderer.h"
#include "Common.h"
#include "../main.h"
#include "../manager.h"
#include "../State/Game.h"
#include "../Object/Player.h"
#include "../System/DebugProc.h"
#include "../Shader/ToonShader.h"
#include "../Shader/Fur.h"
#include "../Shader/RimLight.h"
#include "../Shader/Gaussian.h"
#include "../Shader/DepthOfField.h"
#include "../Shader/MotionBlur.h"
#include "../Shader/Shadow.h"
#include "../Shader/Fade.h"

#include <windows.h>
#include "d3dx9.h"

//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#pragma comment (lib,"d3d9.lib")
#pragma comment (lib,"d3dx9.lib")
#pragma comment (lib,"dxguid.lib")

//*****************************************************************************
//スタティックメンバ変数
//*****************************************************************************
DWORD CRenderer::m_dwExecLastTime=0;
DWORD CRenderer::m_dwFPSLastTime=0;
DWORD CRenderer::m_dwCurrentTime=0;
DWORD CRenderer::m_dwFrameCount=0;

//ステート番号
int CRenderer::m_nStateIndex = 0;

//フェードフィルター
CFade *CRenderer::m_pFade = NULL;

#ifdef _DEBUG
	int CRenderer::m_nCountFPS=NULL;		//FPSカウンタ
#endif

//=============================================================================
//DirectX初期化
//=============================================================================
HRESULT CRenderer::Init(HINSTANCE hInstance,HWND hWnd, BOOL bWindow)
{
	///////////////////////////////////////
	//			DirectXの初期化			//
	/////////////////////////////////////

	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;

	// Direct3Dオブジェクトの生成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	// 現在のディスプレイモードを取得
	if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
		return E_FAIL;

	//Direct3Dオブジェクトチェック
	if(m_pD3D == NULL)
		return E_FAIL;

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// ワークをゼロクリア
	d3dpp.BackBufferCount			= 1;						// バックバッファの数
	d3dpp.BackBufferWidth			= SCREEN_WIDTH;				// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight			= SCREEN_HEIGHT;			// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat			= D3DFMT_UNKNOWN;			// バックバッファのフォーマットは現在設定されているものを使う
	d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;	// 映像信号に同期してフリップする
	d3dpp.Windowed					= bWindow;					// ウィンドウモード
	d3dpp.EnableAutoDepthStencil	= TRUE;						// デプスバッファ（Ｚバッファ）とステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D24S8;				/* デプスバッファとして24bitを
																   ステンシルバッファを8bit使う*/
	if(bWindow)
	{// ウィンドウモード
		d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;					// バックバッファ
		d3dpp.FullScreen_RefreshRateInHz = 0;								// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// インターバル
	}
	else
	{// フルスクリーンモード
		d3dpp.BackBufferFormat           = D3DFMT_R5G6B5;					// バックバッファ
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;			// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;		// インターバル
	}

	// デバイスオブジェクトの生成
	// [デバイス作成制御]<描画>と<頂点処理>をハードウェアで行なう
	if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,						// ディスプレイアダプタ
									D3DDEVTYPE_HAL,							// ディスプレイタイプ
									hWnd,									// フォーカスするウインドウへのハンドル
									D3DCREATE_HARDWARE_VERTEXPROCESSING,	// デバイス作成制御の組み合わせ
									&d3dpp,									// デバイスのプレゼンテーションパラメータ
									&m_pD3DDevice)))						// デバイスインターフェースへのポインタ
	{
		// 上記の設定が失敗したら
		// [デバイス作成制御]<描画>をハードウェアで行い、<頂点処理>はCPUで行なう
		if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
										D3DDEVTYPE_HAL, 
										hWnd, 
										D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
										&d3dpp,
										&m_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// [デバイス作成制御]<描画>と<頂点処理>をCPUで行なう
			if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
											D3DDEVTYPE_REF,
											hWnd, 
											D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
											&d3dpp,
											&m_pD3DDevice)))
			{
				// 初期化失敗
				return E_FAIL;
			}
		}
	}

	// レンダーステートパラメータの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面をカリング
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						// Zバッファを使用
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドを行う
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定

	// サンプラーステートパラメータの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(U値)を設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(V値)を設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// テクスチャ縮小フィルタモードを設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// テクスチャ拡大フィルタモードを設定
	
	// テクスチャステージステートの設定
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// アルファブレンディング処理
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 最初のアルファ引数
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// ２番目のアルファ引数

	///////////////////////////////////////////
	//		シェーダーインスタンス生成		//
	/////////////////////////////////////////

	//トゥーンシェーダー生成
	m_pToonShader = new CToonShader();
	m_pToonShader->Load();

	//ファーシェーダー生成
	m_pFur = new CFur();
	m_pFur->Load();

	//リムライトシェーダー生成
	m_pRimLight = new CRimLight();
	m_pRimLight->Load();

	//ガウスフィルター生成
	m_pGauss = new CGaussian();
	m_pGauss->Load();

	//影用ガウスフィルター生成
	m_pShadowGauss = new CGaussian();
	m_pShadowGauss->Load();

	//被写界深度生成
	m_pDepth = new CDepthOfField();
	m_pDepth->Load();

	//モーションブラー生成
	m_pMotionBlur = new CMotionBlur();
	m_pMotionBlur->Load();

	//投影シャドウ生成
	m_pShadow = new CShadow();
	m_pShadow->Load();

	//フェードフィルター
	m_pFade = new CFade();
	m_pFade->Load();

	///////////////////////////
	//	頂点バッファの設定	//
	/////////////////////////

	//頂点座標の代入
	m_aVtx[0].vtx = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);
	m_aVtx[1].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_aVtx[2].vtx = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	m_aVtx[3].vtx = D3DXVECTOR3(SCREEN_WIDTH, 0.0f, 0.0f);

	//幅
	m_aVtx[0].rhw = 1.0f;
	m_aVtx[1].rhw = 1.0f;
	m_aVtx[2].rhw = 1.0f;
	m_aVtx[3].rhw = 1.0f;

	//反射光
	m_aVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_aVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_aVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_aVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	//テクスチャ座標
	m_aVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
	m_aVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	m_aVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	m_aVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

//デバッグ
#ifdef _DEBUG
	m_pD3DXFont = NULL;//FPS用フォント初期化

	// 情報表示用フォント生成
	D3DXCreateFont(m_pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Terminal", &m_pD3DXFont);
#endif

	return S_OK;
}
//=============================================================================
//DirectX終了
//=============================================================================
void CRenderer::Uninit()
{
	//Direct3Dオブジェクトの終了
	RELEASE_OBJECT(m_pD3D);

	//DirectXデバイスの終了
	RELEASE_OBJECT(m_pD3DDevice);

	//トゥーンの終了
	DELETE_OBJECT(m_pToonShader);

	//ファーの終了
	DELETE_OBJECT(m_pFur);

	//リムライトの終了
	DELETE_OBJECT(m_pRimLight);

	//ガウスの終了
	DELETE_OBJECT(m_pGauss);

	//影用ガウス終了
	DELETE_OBJECT(m_pShadowGauss);

	//被写界深度の終了
	DELETE_OBJECT(m_pDepth);

	//モーションブラー終了
	DELETE_OBJECT(m_pMotionBlur);

	//シャドウの終了
	DELETE_OBJECT(m_pShadow);

	//フェードの終了
	DELETE_OBJECT(m_pFade);

	//全シーンオブジェクト解放
	CScene::ReleaseAll();
}
//=============================================================================
//DirectX更新
//==============================================================================
void CRenderer::Update()
{
	//フェード更新
	m_pFade->Update();
}
//=============================================================================
//DirectX描画
//=============================================================================
void CRenderer::Draw()
{
	//ゲーム以外のステートはここで画面のクリア
	if (m_nStateIndex != (int)GAME_STATE)
	{
		//画面のクリア
		m_pD3DDevice->Clear(0,
			NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
			D3DCOLOR_RGBA(0, 0, 0, 0),
			1.0f,
			0);
	}

	//描画の開始
	if(SUCCEEDED(m_pD3DDevice->BeginScene()))
	{
		///////////////////////////////////////////////
		//		様々なオブジェクトの描画処理		//
		/////////////////////////////////////////////

		//ゲーム時のみのレンダリング(描画処理)
		if (m_nStateIndex == (int)GAME_STATE)
		{
			//影のレンダリング
			RenderShadow();

			//モーションブラーのレンダリング
			RenderMotionBlur();

			//ブラーのレンダリング
			RenderBlur();

			//被写界深度のレンダリング
			RenderDepth();
		}

		//それ以外のレンダリング(通常の描画処理)
		else
		{
			//全インスタンス描画
			CScene::DrawAll();
		}

		//フェードの描画
		m_pFade->Draw(m_pD3DDevice);

//デバッグ用処理
#ifdef _DEBUG
		//FPSの描画
		DrawFPS();

		//デバッグの描画
		CDebug::Draw();
#endif

		//描画終了
		m_pD3DDevice->EndScene();
	}

	//3Dデバイスの中身を空にする
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}
//=============================================================================
//モーションブラーのレンダリング
//=============================================================================
void CRenderer::RenderMotionBlur()
{
	///////////////////////////////////
	//		通常のレンダリング		//
	/////////////////////////////////
	m_pMotionBlur->ChangeBlurSurface(m_pD3DDevice, BACK_SURFACE);

	//画面のクリア
	m_pD3DDevice->Clear(0,
						NULL,
						(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
						D3DCOLOR_RGBA(0, 0, 0, 0),
						1.0f,
						0);

	//全インスタンス描画
	CScene::DrawAll();

	m_pMotionBlur->ReturnBlurSurface(m_pD3DDevice);

	///////////////////////////////////////////
	//		速度マップのレンダリング		//
	/////////////////////////////////////////
	m_pMotionBlur->ChangeBlurSurface(m_pD3DDevice, VELOCITY_SURFACE);

	//画面のクリア
	m_pD3DDevice->Clear(0,
						NULL,
						(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
						D3DCOLOR_RGBA(0, 0, 0, 0),
						1.0f,
						0);

	//速度マップ用にプレイヤーのみで描画する
	CPlayer *pPlayer = CGame::GetPlayer();
	pPlayer->SetCastType(CAST_MOTIONBLUR);
	//描画
	pPlayer->Draw();

	//後始末
	pPlayer->SetCastType(CAST_NONE);
	m_pMotionBlur->ReturnBlurSurface(m_pD3DDevice);
}
//=============================================================================
//ブラーのレンダリング
//=============================================================================
void CRenderer::RenderBlur()
{
	///////////////////////////////////////////////////////////////
	//		X方向へのブラー処理のためレンダーターゲット切替		//
	/////////////////////////////////////////////////////////////

	m_pGauss->ChangeSurface2(m_pD3DDevice);

	//画面のクリア
	m_pD3DDevice->Clear(0,
						NULL,
						(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
						D3DCOLOR_RGBA(0, 0, 0, 0),
						1.0f,
						0);

	//モーションブラーフィルターの描画
	m_pMotionBlur->Draw(m_pD3DDevice);

	m_pGauss->ReturnSurface2(m_pD3DDevice);

	///////////////////////////////////////////////////////////////
	//		Y方向へのブラー処理のためレンダーターゲット切替		//
	/////////////////////////////////////////////////////////////

	m_pGauss->ChangeSurfaceBlurY(m_pD3DDevice);

	//画面のクリア
	m_pD3DDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
		D3DCOLOR_RGBA(0, 0, 0, 0),
		1.0f,
		0);

	//レンダリングしたX方向ブラーの描画
	m_pGauss->Draw(m_pD3DDevice);

	m_pGauss->ReturnSurfaceBlurY(m_pD3DDevice);
}
//=============================================================================
//被写界深度のレンダリング
//=============================================================================
void CRenderer::RenderDepth()
{
	//画面のクリア
	m_pD3DDevice->Clear(0,
						NULL,
						(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
						D3DCOLOR_RGBA(0, 0, 0, 0),
						1.0f,
						0);

	//被写界深度開始
	m_pDepth->Begin(m_pD3DDevice);

	//Zバッファ無効
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	//頂点フォーマットのセット
	m_pD3DDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	m_pD3DDevice->SetTexture(0, m_pGauss->GetBlurTexture());	//ブラーレンダリング
	m_pD3DDevice->SetTexture(1, m_pMotionBlur->GetTexture());	//通常のレンダリング
	m_pD3DDevice->SetTexture(2, m_pMotionBlur->GetZBuff());		//バックバッファ(深度情報)

	//ポリゴンの描画
	m_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
									2,
									&m_aVtx[0],
									sizeof(VERTEX_2D));

	//Zバッファ戻す
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	//被写界深度終了
	m_pDepth->End(m_pD3DDevice);

	//テクスチャを戻す
	m_pD3DDevice->SetTexture(0, NULL);
	m_pD3DDevice->SetTexture(1, NULL);
	m_pD3DDevice->SetTexture(2, NULL);
}
//=============================================================================
//影のレンダリング
//=============================================================================
void CRenderer::RenderShadow()
{
	///////////////////////////////////////////
	//		影のキャストレンダリング		//
	/////////////////////////////////////////

	m_pShadow->ChangeSurface1(m_pD3DDevice);

	//画面のクリア
	m_pD3DDevice->Clear(0,
						NULL,
						(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
						D3DCOLOR_RGBA(255, 255, 255, 255),
						1.0f,
						0);

	//投影シャドウ用にXファイルのみ描画
	CSceneX *pModel = (CSceneX*)CScene::GetListTop(PRIORITY_MODEL);

	while (pModel)
	{
		//投影シャドウキャストを使用して描画
		pModel->SetCastType(CAST_SHADOW);
		pModel->Draw();
		pModel->SetCastType(CAST_NONE);

		//次ポインタ取得
		pModel = (CSceneX*)pModel->GetNext();
	}

	m_pShadow->ReturnSurface1(m_pD3DDevice);

	///////////////////////////////////////////////////
	//		影テクスチャにブラーをかける(X方向)		//
	/////////////////////////////////////////////////

	m_pShadowGauss->ChangeSurface2(m_pD3DDevice);

	//画面のクリア
	m_pD3DDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
		D3DCOLOR_RGBA(0, 0, 0, 0),
		1.0f,
		0);

	//キャストした影フィルターの描画
	m_pShadow->Draw(m_pD3DDevice);

	m_pShadowGauss->ReturnSurface2(m_pD3DDevice);

	///////////////////////////////////////////////////
	//		影テクスチャにブラーをかける(Y方向)		//
	/////////////////////////////////////////////////

	m_pShadowGauss->ChangeSurfaceBlurY(m_pD3DDevice);

	//画面のクリア
	m_pD3DDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
		D3DCOLOR_RGBA(0, 0, 0, 0),
		1.0f,
		0);

	//レンダリングしたX方向ブラーの描画
	m_pShadowGauss->Draw(m_pD3DDevice);

	m_pShadowGauss->ReturnSurfaceBlurY(m_pD3DDevice);
}
//=============================================================================
//デバイスのゲッター
//=============================================================================
LPDIRECT3DDEVICE9 CRenderer::GetDevice (void)
{
	//Directデバイスを返す
	return m_pD3DDevice;
}
//=============================================================================
//FPSの表示
//=============================================================================
#ifdef _DEBUG
void CRenderer::DrawFPS()
{
	RECT rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};//描画する範囲
	char str[256];									//表示用文字列
	wsprintf(str, "FPS:%d\n", m_nCountFPS);			//FPSを文字列に入れる

	// テキスト描画
	m_pD3DXFont->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));
}
#endif
//EOF