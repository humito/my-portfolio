//=============================================================================
//フィードバックブラー[FeedBackBlur.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "FeedBackBlur.h"
#include "main.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define BLUR_FILTER_ALPHA (250)//フィードバックブラーのα値

//*****************************************************************************
//スタティックメンバ変数
//*****************************************************************************
bool CFeedBackBlur::m_bBlur = false;//ブラー使用フラグ

//=============================================================================
//コンストラクタ
//=============================================================================
CFeedBackBlur::CFeedBackBlur()
{
	m_Texture1 = NULL;
	m_Texture2 = NULL;
	m_TexSurface1 = NULL;
	m_TexSurface2 = NULL;
	m_TexZSBuff1 = NULL;
	m_TexZSBuff2 = NULL;
	m_backBuffOrg = NULL;
	m_ZSBuffOrg = NULL;
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CFeedBackBlur::Init(LPDIRECT3DDEVICE9 pDevice)
{

	//レンダーターゲット用テクスチャの生成
	pDevice->CreateTexture(SCREEN_WIDTH,			//バックバッファのサイズ
							SCREEN_HEIGHT,
							1,						//ミップマップレベル
							D3DUSAGE_RENDERTARGET,	//レンダーターゲットに指定
							D3DFMT_A8R8G8B8,		//ピクセルフォーマット
							D3DPOOL_DEFAULT,
							&m_Texture1,			//生成したテクスチャのポインタ
							NULL);

	//2枚目も同様
	pDevice->CreateTexture(SCREEN_WIDTH,
							SCREEN_HEIGHT,
							1,
							D3DUSAGE_RENDERTARGET,
							D3DFMT_A8R8G8B8,
							D3DPOOL_DEFAULT,
							&m_Texture2,
							NULL);

	//サーフェイスの取得
	m_Texture1->GetSurfaceLevel(0, &m_TexSurface1);
	m_Texture2->GetSurfaceLevel(0, &m_TexSurface2);

	//Zバッファ生成
	pDevice->CreateDepthStencilSurface(SCREEN_WIDTH,
										SCREEN_HEIGHT,
										D3DFMT_D24S8,
										D3DMULTISAMPLE_NONE,
										0,
										TRUE,
										&m_TexZSBuff1,
										NULL);

	//2枚目も同様
	pDevice->CreateDepthStencilSurface(SCREEN_WIDTH,
										SCREEN_HEIGHT,
										D3DFMT_D24S8,
										D3DMULTISAMPLE_NONE,
										0,
										TRUE,
										&m_TexZSBuff2,
										NULL);

	//ビューポートの取得
	pDevice->GetViewport(&m_ViewPort);

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
	m_aVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, BLUR_FILTER_ALPHA);
	m_aVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, BLUR_FILTER_ALPHA);
	m_aVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, BLUR_FILTER_ALPHA);
	m_aVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, BLUR_FILTER_ALPHA);

	//テクスチャ座標
	m_aVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
	m_aVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	m_aVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	m_aVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CFeedBackBlur::Uninit()
{
	//レンダリング用テクスチャ解放
	if (m_Texture1)
	{
		m_Texture1->Release();
		m_Texture1 = NULL;
	}

	if (m_Texture2)
	{
		m_Texture2->Release();
		m_Texture2 = NULL;
	}

	//サーフェイス解放
	if (m_TexSurface1)
	{
		m_TexSurface1->Release();
		m_TexSurface1 = NULL;
	}

	if (m_TexSurface2)
	{
		m_TexSurface2->Release();
		m_TexSurface2 = NULL;
	}

	//Zバッファ解放
	if (m_TexZSBuff1)
	{
		m_TexZSBuff1->Release();
		m_TexZSBuff1 = NULL;
	}

	if (m_TexZSBuff2)
	{
		m_TexZSBuff2->Release();
		m_TexZSBuff2 = NULL;
	}

	//バックアップ用解放
	if (m_backBuffOrg)
	{
		m_backBuffOrg->Release();
		m_backBuffOrg = NULL;
	}

	if (m_ZSBuffOrg)
	{
		m_ZSBuffOrg->Release();
		m_ZSBuffOrg = NULL;
	}
}
//=============================================================================
//バックバッファポインタの保持
//=============================================================================
void CFeedBackBlur::SetupBackBuffer(LPDIRECT3DDEVICE9 pDevice)
{
	//バックバッファのポインタ保持
	pDevice->GetRenderTarget(0, &m_backBuffOrg);
	pDevice->GetDepthStencilSurface(&m_ZSBuffOrg);
	pDevice->GetViewport(&m_viewportOrg);

	//レンダーターゲットをテクスチャ0に設定
	pDevice->SetRenderTarget(0, m_TexSurface1);
	pDevice->SetDepthStencilSurface(m_TexZSBuff1);
	pDevice->SetViewport(&m_ViewPort);
}
//=============================================================================
//バックバッファを戻す
//=============================================================================
void CFeedBackBlur::ReturnBackBuffer(LPDIRECT3DDEVICE9 pDevice)
{
	//元のレンダーターゲットをバックバッファに戻す
	pDevice->SetRenderTarget(0, m_backBuffOrg);
	pDevice->SetDepthStencilSurface(m_ZSBuffOrg);
	pDevice->SetViewport(&m_viewportOrg);

	//バックバッファ用を解放
	m_backBuffOrg->Release();
	m_backBuffOrg = NULL;
	m_ZSBuffOrg->Release();
	m_ZSBuffOrg = NULL;
}
//=============================================================================
//最初の描画
//=============================================================================
void CFeedBackBlur::DrawFirst(LPDIRECT3DDEVICE9 pDevice)
{
	//Zバッファ無効
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_Texture2);

	//ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
							2,
							&m_aVtx[0],
							sizeof(VERTEX_2D));
}
//=============================================================================
//次の描画
//=============================================================================
void CFeedBackBlur::DrawSecond(LPDIRECT3DDEVICE9 pDevice)
{
	//画面のクリア
	pDevice->Clear(0,
				NULL,
				(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
				D3DCOLOR_RGBA(0, 0, 0, 0),
				1.0f,
				0);

	//描画開始
	pDevice->BeginScene();

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_Texture1);

	//ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
							2,
							&m_aVtx[0],
							sizeof(VERTEX_2D));
	//Zバッファ元に戻す
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}
//=============================================================================
//2回目の描画終了
//=============================================================================
void CFeedBackBlur::EndDrawSecond(LPDIRECT3DDEVICE9 pDevice)
{
	//テクスチャ1とテクスチャ2を交換
	LPDIRECT3DTEXTURE9 tmpTexture = m_Texture1;
	m_Texture1 = m_Texture2;
	m_Texture2 = tmpTexture;

	//サーフェイスも同様に交換
	LPDIRECT3DSURFACE9 tmpSurface = m_TexSurface1;
	m_TexSurface1 = m_TexSurface2;
	m_TexSurface2 = tmpSurface;

	//念のためZバッファも入れ替え
	LPDIRECT3DSURFACE9 tmpTexZSBuff = m_TexZSBuff1;
	m_TexZSBuff1 = m_TexZSBuff2;
	m_TexZSBuff2 = tmpTexZSBuff;
}
//=============================================================================
//インスタンス生成
//=============================================================================
CFeedBackBlur *CFeedBackBlur::Create(LPDIRECT3DDEVICE9 pDevice)
{
	//インスタンス生成して初期化
	CFeedBackBlur *pFeedBack = new CFeedBackBlur();
	pFeedBack->Init(pDevice);
	return pFeedBack;
}
//EOF