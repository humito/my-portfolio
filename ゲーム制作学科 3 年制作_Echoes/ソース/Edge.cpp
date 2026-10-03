//=============================================================================
//エッジフィルター[Edge.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Edge.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"

//=============================================================================
//エフェクトファイル読み込み
//=============================================================================
HRESULT CEdge::Load()
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//ハードウェア能力の取得
	D3DCAPS9 caps;
	pDevice->GetDeviceCaps(&caps);

	if (caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)
	&&	caps.PixelShaderVersion >= D3DPS_VERSION(2, 0))
	{
		LPD3DXBUFFER pErr = NULL;
		//エフェクトファイル読み込み
		if (FAILED(D3DXCreateEffectFromFile(pDevice,
											"EdgeFilter.hlsl",
											NULL,
											NULL,
											0,
											NULL,
											&m_pEffect,
											&pErr)))
		{
			MessageBox(NULL, (LPCSTR)pErr->GetBufferPointer(),
				"Shader_compile_error", MB_OK);

			return E_FAIL;
		}
		//テクニックと他変数のハンドル取得
		m_pTechnique = m_pEffect->GetTechniqueByName("TShader");
		m_pTex = m_pEffect->GetParameterByName(NULL, "m_Tex");
		m_pID = m_pEffect->GetParameterByName(NULL, "m_ID");
		m_pEdgeColor = m_pEffect->GetParameterByName(NULL, "m_Color");

		//テクニックの設定
		m_pEffect->SetTechnique(m_pTechnique);

		//エッジの幅の設定
		D3DXVECTOR2 texel = D3DXVECTOR2(2.5f / SCREEN_WIDTH,
										2.5f / SCREEN_HEIGHT);
		m_pEffect->SetValue(m_pTex, texel, sizeof(float)* 2);

		//テクスチャとサーフェイスの生成
		//レンダーターゲット用テクスチャの生成
		D3DXCreateTexture(pDevice,
						SCREEN_WIDTH,			//バックバッファのサイズ
						SCREEN_HEIGHT,
						1,						//ミップマップレベル
						D3DUSAGE_RENDERTARGET,	//レンダーターゲットに指定
						D3DFMT_A8R8G8B8,		//ピクセルフォーマット
						D3DPOOL_DEFAULT,
						&m_pD3DTexture);		//生成したテクスチャのポインタ


		D3DXCreateTexture(	pDevice,
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

		//フィルターの初期化
		CFilter::Init();
	}

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CEdge::Uninit()
{
	//Zバッファ用テクスチャ解放
	if (m_pD3DZBuffTexture)
	{
		m_pD3DZBuffTexture->Release();
		m_pD3DZBuffTexture = NULL;
	}

	//フィルターの終了
	CFilter::Uninit();
}
//=============================================================================
//シェーダー開始
//=============================================================================
void CEdge::Begin()
{
	if (m_pEffect)
	{
		//シェーダー開始
		m_pEffect->Begin(NULL, 0);
	}
}
//=============================================================================
//パスの開始
//=============================================================================
void CEdge::BeginPass(UINT Pass)
{
	if (m_pEffect)
	{
		m_pEffect->BeginPass(Pass);
	}
}
//=============================================================================
//シェーダー終了
//=============================================================================
void CEdge::End()
{
	if (m_pEffect)
	{
		m_pEffect->End();
	}
}
//=============================================================================
//パスの終了
//=============================================================================
void CEdge::EndPass()
{
	if (m_pEffect)
	{
		m_pEffect->EndPass();
	}
}
//=============================================================================
//描画
//=============================================================================
void CEdge::Draw(LPDIRECT3DDEVICE9 pDevice)
{
	//シェーダーを使ったフィルターの描画
	if (m_pEffect)
	{
		//Zバッファをテクスチャ１にセット
		pDevice->SetTexture(1, m_pD3DZBuffTexture);

		m_pEffect->Begin(NULL, 0);
		m_pEffect->BeginPass(1);

		CFilter::Draw(pDevice);

		m_pEffect->EndPass();
		m_pEffect->End();
	}
}
//=============================================================================
//テクセルのセット
//=============================================================================
void CEdge::SetTexel(D3DXVECTOR2 *pTexel)
{
	if (m_pEffect)
	{
		D3DXVECTOR2 tex = *pTexel;
		m_pEffect->SetValue(m_pTex, tex, sizeof(float)* 2);
	}
}
//=============================================================================
//IDのセット
//=============================================================================
void CEdge::SetID(int nID)
{
	if (m_pEffect)
	{
		m_pEffect->SetInt(m_pID, nID);
	}
}
//=============================================================================
//エッジの色セット
//=============================================================================
void CEdge::SetColor(D3DXVECTOR4 color)
{
	if (m_pEffect)
	{
		m_pEffect->SetVector(m_pEdgeColor,&color);
	}
}
//EOF