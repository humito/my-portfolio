//=============================================================================
//被写界深度[DepthOfField.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "DepthOfField.h"
#include "../main.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../System/Common.h"
#include "Gaussian.h"
//TODO:http://maverickproj.web.fc2.com/pg31.html
//=============================================================================
//シェーダー読込
//=============================================================================
void CDepthOfField::Load()
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//ガウスフィルターシェーダー生成
	CreateShader(pDevice, "data/HLSL/DepthOfField.hlsl",
				"VertexShader3D", "PixelShader3D",
				"vs_2_0", "ps_2_0",
				&m_pVSConstantTable, &m_pPSConstantTable,
				&m_pVertexShader, &m_pPixelShader);

	//レンダーターゲット用テクスチャの生成
	D3DXCreateTexture(pDevice,
					SCREEN_WIDTH,			//バックバッファのサイズ
					SCREEN_HEIGHT,
					1,						//ミップマップレベル
					D3DUSAGE_RENDERTARGET,	//レンダーターゲットに指定
					D3DFMT_A8R8G8B8,		//ピクセルフォーマット
					D3DPOOL_DEFAULT,
					&m_pD3DTexture);		//生成したテクスチャのポインタ

	//Zバッファテクスチャ生成(X方向ブラー用)
	D3DXCreateTexture(pDevice,
					SCREEN_WIDTH,
					SCREEN_HEIGHT,
					1,
					D3DUSAGE_RENDERTARGET,
					D3DFMT_A8R8G8B8,
					D3DPOOL_DEFAULT,
					&m_pD3DZBuffTexture);

	//サーフェイスの取得
	m_pD3DTexture->GetSurfaceLevel(0, &m_TexSurface);
	m_pD3DZBuffTexture->GetSurfaceLevel(0, &m_TexZSBuff);
}
//=============================================================================
//シェーダー終了
//=============================================================================
void CDepthOfField::Uninit()
{
	//Zバッファテクスチャの終了
	RELEASE_OBJECT(m_pD3DZBuffTexture);

	//フィルターの終了
	CFilter::Uninit();
}
//=============================================================================
//描画
//=============================================================================
void CDepthOfField::Draw(LPDIRECT3DDEVICE9 pDevice,
						LPDIRECT3DTEXTURE9 pBlurTexture,
						LPDIRECT3DTEXTURE9 pRenderTexture,
						LPDIRECT3DTEXTURE9 pZBuffTexture)
{
	//頂点シェーダーのセット
	if (m_pVertexShader)
		pDevice->SetVertexShader(m_pVertexShader);

	//ピクセルシェーダーのセット
	if (m_pPixelShader)
		pDevice->SetPixelShader(m_pPixelShader);

	//ブラーテクスチャコピー
	m_pD3DTexture = pBlurTexture;

	//通常のレンダリングテクスチャを1にセット
	pDevice->SetTexture(1, pRenderTexture);

	//Zバッファをテクスチャ2にセット
	pDevice->SetTexture(2, pZBuffTexture);

	//フィルターの描画
	CFilter::Draw(pDevice);

	//シェーダーを戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);

	pDevice->SetTexture(1, NULL);
	pDevice->SetTexture(2, NULL);
}
//EOF