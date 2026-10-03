//=============================================================================
//シェーダー使用のフィルター[Filter.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Filter.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CFilter::CFilter()
{
	m_pD3DTexture = NULL;
	m_TexSurface = NULL;
	m_TexZSBuff = NULL;
	m_backBuffOrg = NULL;
	m_ZSBuffOrg = NULL;
}
//=============================================================================
//フィルターの初期化
//=============================================================================
void CFilter::Init()
{
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
	m_aVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 250);
	m_aVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 250);
	m_aVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 250);
	m_aVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 250);

	//テクスチャ座標
	m_aVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
	m_aVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	m_aVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	m_aVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);
}
//=============================================================================
//終了
//=============================================================================
void CFilter::Uninit()
{
	//テクスチャの終了
	if (m_pD3DTexture)
	{
		m_pD3DTexture->Release();
		m_pD3DTexture = NULL;
	}

	//サーフェイスの終了
	if (m_TexSurface)
	{
		m_TexSurface->Release();
		m_TexSurface = NULL;
	}

	//バックアップ用サーフェイスの終了
	if (m_backBuffOrg)
	{
		m_backBuffOrg->Release();
		m_backBuffOrg = NULL;
	}

	//Zバッファの終了
	if (m_TexZSBuff)
	{
		m_TexZSBuff->Release();
		m_TexZSBuff = NULL;
	}

	//バックアップ用Zバッファの終了
	if (m_ZSBuffOrg)
	{
		m_ZSBuffOrg->Release();
		m_ZSBuffOrg = NULL;
	}
}
//=============================================================================
//レンダーターゲット切替1
//※主にブラーフィルターに使用
//=============================================================================
void CFilter::ChangeSurface1(LPDIRECT3DDEVICE9 pDevice)
{
	//バックバッファのポインタ保持
	pDevice->GetRenderTarget(0, &m_backBuffOrg);
	pDevice->GetDepthStencilSurface(&m_ZSBuffOrg);

	//レンダーターゲットをテクスチャに設定
	pDevice->SetRenderTarget(0, m_TexSurface);
	pDevice->SetDepthStencilSurface(m_TexZSBuff);
}
//=============================================================================
//レンダーターゲット戻す1
//※主にブラーフィルターに使用
//=============================================================================
void CFilter::ReturnSurface1(LPDIRECT3DDEVICE9 pDevice)
{
	//レンダーターゲットをバックバッファに戻す
	pDevice->SetRenderTarget(0, m_backBuffOrg);
	pDevice->SetDepthStencilSurface(m_ZSBuffOrg);

	//バックバッファ用を解放
	m_backBuffOrg->Release();
	m_ZSBuffOrg->Release();
}
//=============================================================================
//レンダーターゲット切替2
//※主にエッジフィルターに使用
//=============================================================================
void CFilter::ChangeSurface2(LPDIRECT3DDEVICE9 pDevice)
{
	//バックバッファのポインタ保持
	pDevice->GetRenderTarget(0, &m_backBuffOrg);

	//レンダーターゲットをテクスチャに設定
	pDevice->SetRenderTarget(0, m_TexSurface);
	pDevice->SetRenderTarget(1, m_TexZSBuff);
}
//=============================================================================
//レンダーターゲット戻す2
//※主にエッジフィルターに使用
//=============================================================================
void CFilter::ReturnSurface2(LPDIRECT3DDEVICE9 pDevice)
{
	//レンダーターゲットをバックバッファに戻す
	pDevice->SetRenderTarget(0, m_backBuffOrg);
	pDevice->SetRenderTarget(1, NULL);

	//バックバッファ用を解放
	m_backBuffOrg->Release();
}
//=============================================================================
//フィルターの描画
//=============================================================================
void CFilter::Draw(LPDIRECT3DDEVICE9 pDevice)
{
	//Zバッファ無効
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pD3DTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
							2,
							&m_aVtx[0],
							sizeof(VERTEX_2D));

	//Zバッファ戻す
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}
//EOF