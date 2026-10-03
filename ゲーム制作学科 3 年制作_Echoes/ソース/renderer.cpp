//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "renderer.h"
#include "FeedBackBlur.h"
#include "ShaderManager.h"
#include "Edge.h"
#include "Fade.h"
#include <windows.h>
#include "d3dx9.h"

//デバッグのみ
#ifdef _DEBUG
	#include "DebugProc.h"
#endif

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
CFade *CRenderer::m_pFade = NULL;	//フェード
bool CRenderer::m_bEdge = false;	//エッジフラグ

#ifdef _DEBUG
int CRenderer::m_nCountFPS=NULL;		//FPSカウンタ
#endif

//=============================================================================
//コンストラクタ
//=============================================================================
CRenderer::CRenderer()
{
	m_pBlur = NULL;
	m_pFade = NULL;
}

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
	{
		return E_FAIL;
	}

	//Direct3DオブジェクトがNULLなら
	if(m_pD3D == NULL)
	{
		//エラーを返す
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// ワークをゼロクリア
	d3dpp.BackBufferCount			= 1;						// バックバッファの数
	d3dpp.BackBufferWidth			= SCREEN_WIDTH;				// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight			= SCREEN_HEIGHT;			// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat			= D3DFMT_UNKNOWN;			// バックバッファのフォーマットは現在設定されているものを使う
	d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;	// 映像信号に同期してフリップする
	d3dpp.Windowed					= bWindow;					// ウィンドウモード
	d3dpp.EnableAutoDepthStencil	= TRUE;						// デプスバッファ（Ｚバッファ）とステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D24S8;				// デプスバッファとして16bitを使う

	if(bWindow)
	{// ウィンドウモード
		d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;					// バックバッファ
		d3dpp.FullScreen_RefreshRateInHz = 0;								// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// インターバル
	}
	else
	{// フルスクリーンモード D3DFMT_R5G6B5
		d3dpp.BackBufferFormat			 = D3DFMT_R5G6B5;					// バックバッファ
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

	//フィードバックブラー生成
	m_pBlur = CFeedBackBlur::Create(m_pD3DDevice);

	//フェードインスタンス生成
	m_pFade = CFade::Create();

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

	//エッジフラグ
	m_bEdge = true;

//デバッグ
#ifdef _DEBUG
	m_pD3DXFont=NULL;//FPS用フォント初期化

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
	if(m_pD3D!=NULL)
	{
		m_pD3D->Release();	//解放
		m_pD3D=NULL;		//NULLセット
	}

	//DirectXデバイスの終了
	if(m_pD3DDevice!=NULL)
	{
		m_pD3DDevice->Release();//解放
		m_pD3DDevice=NULL;		//NULLセット
	}

	//フィードバックブラー終了
	if (m_pBlur)
	{
		m_pBlur->Uninit();
		delete m_pBlur;
		m_pBlur = NULL;
	}

	//フェード終了
	m_pFade->Uninit();
	delete m_pFade;
	m_pFade = NULL;

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
	//エッジフィルター取得
	CEdge *pEdge = (CEdge*)CShaderManager::GetInstance()->GetShader(EDGE_FILTER);

	//フィードバックブラー使用可能かチェック
	bool bUseBlur = CFeedBackBlur::IsOK();

	//画面のクリア
	m_pD3DDevice->Clear(0,NULL,
						(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
						D3DCOLOR_RGBA(0,0,0,0),
						1.0f,0);

	//描画の開始
	if(SUCCEEDED(m_pD3DDevice->BeginScene()))
	{
		//==============================
		//様々なオブジェクトの描画処理
		//==============================
		//レンダーターゲット変更
		pEdge->ChangeSurface2(m_pD3DDevice);

		//画面のクリア
		m_pD3DDevice->Clear(0, NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
			D3DCOLOR_RGBA(0, 0, 0, 0),
			1.0f, 0);

		//全インスタンス描画
		CScene::DrawAll();

		pEdge->ReturnSurface2(m_pD3DDevice);

		//フィードバッグブラー使用してる場合
		if (bUseBlur)
		{
			//バックバッファポインタを保持
			m_pBlur->SetupBackBuffer(m_pD3DDevice);
		}

		//エッジ描画判定
		if (m_bEdge)
		{
			//エッジフィルター描画
			pEdge->Draw(m_pD3DDevice);
		}
		else
		{
			//通常の描画
			m_pD3DDevice->SetFVF(FVF_VERTEX_2D);
			m_pD3DDevice->SetTexture(0, pEdge->GetTexture());
			m_pD3DDevice->DrawPrimitiveUP(	D3DPT_TRIANGLESTRIP,
											2,
											&m_aVtx[0],
											sizeof(VERTEX_2D));
		}
		
		//フェードポリゴン描画
		m_pFade->Draw();

		//フィードバッグブラー使用してる場合
		if (bUseBlur)
		{
			//ポリゴンの描画
			m_pBlur->DrawFirst(m_pD3DDevice);
			//描画終了
			m_pD3DDevice->EndScene();

			m_pBlur->ReturnBackBuffer(m_pD3DDevice);
			m_pBlur->DrawSecond(m_pD3DDevice);
		}

#ifdef _DEBUG
		//FPSの描画
		DrawFPS();

		//デバッグの描画
		CDebug::Draw();
#endif
		//フィードバッグブラー使用してる場合
		if (bUseBlur)
		{
			//２回目の描画後始末
			m_pBlur->EndDrawSecond(m_pD3DDevice);
		}

		//描画終了
		m_pD3DDevice->EndScene();
	}

	//3Dデバイスの中身を空にする
	m_pD3DDevice->Present(NULL,NULL,NULL,NULL);
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