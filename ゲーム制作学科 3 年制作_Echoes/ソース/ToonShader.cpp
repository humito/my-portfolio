//=============================================================================
//トゥーンシェーダー処理[ToonShader.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "ToonShader.h"
#include "manager.h"
#include "Light.h"

//=============================================================================
//シェーダーの読込
//=============================================================================
HRESULT CToonShader::Load()
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	D3DCAPS9 caps;
	pDevice->GetDeviceCaps(&caps);

	if (caps.VertexShaderVersion >= D3DVS_VERSION(1,1)
	&&	caps.PixelShaderVersion >= D3DPS_VERSION(2,0))
	{
		LPD3DXBUFFER pErr = NULL;
		if (FAILED(D3DXCreateEffectFromFile(pDevice,
											"ToonShader.hlsl",
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

		//トゥーンマップテクスチャ生成
		D3DXCreateTextureFromFileEx(pDevice,
									"data/TEXTURE/toon.bmp",
									D3DX_DEFAULT,
									D3DX_DEFAULT,
									1,
									0,
									D3DFMT_UNKNOWN,
									D3DPOOL_MANAGED,
									D3DX_DEFAULT,
									D3DX_DEFAULT,
									0x0,
									NULL,
									NULL,
									&m_pD3DToonMap);

		//シェーダーからグローバル変数取得
		m_pTechnique = m_pEffect->GetTechniqueByName("TShader");
		m_pWVP = m_pEffect->GetParameterByName(NULL,"m_WVP");
		m_pWorld = m_pEffect->GetParameterByName(NULL, "m_World");
		m_pLightDir = m_pEffect->GetParameterByName(NULL,"m_LightDir");
		m_pColor = m_pEffect->GetParameterByName(NULL, "m_Color");

		//テクニックのセット
		m_pEffect->SetTechnique(m_pTechnique);

	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CToonShader::Uninit()
{
	//トゥーンマップ用テクスチャ解放
	if (m_pD3DToonMap)
	{
		m_pD3DToonMap->Release();
		m_pD3DToonMap = NULL;
	}
}
//=============================================================================
//シェーダー開始
//=============================================================================
void CToonShader::Begin()
{
	if (m_pEffect)
	{
		//デバイスの取得
		LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();
		
		pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		
		//ビュー、プロジェクションマトリクス取得
		pDevice->GetTransform(D3DTS_VIEW, &m_matView);
		pDevice->GetTransform(D3DTS_PROJECTION, &m_matProj);

		//トゥーンマップをテクスチャ１にセット
		pDevice->SetTexture(1, m_pD3DToonMap);

		m_pEffect->Begin(NULL,0);
	}
}
//=============================================================================
//パスの開始
//=============================================================================
void CToonShader::BeginPass(UINT Pass)
{
	if (m_pEffect)
	{
		m_pEffect->BeginPass(Pass);
	}
}
//=============================================================================
//パスの終了
//=============================================================================
void CToonShader::EndPass()
{
	if (m_pEffect)
	{
		m_pEffect->EndPass();
	}
}
//=============================================================================
//シェーダー終了
//=============================================================================
void CToonShader::End()
{
	if (m_pEffect)
	{
		//デバイスの取得
		LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

		//テクスチャ１にNULLセット
		pDevice->SetTexture(1, NULL);

		m_pEffect->End();
	}
}
//=============================================================================
//ワールド座標系の行列変換マトリックスを設定
//=============================================================================
void CToonShader::SetMatrix(D3DXMATRIX *pMatWorld)
{
	if (m_pEffect)
	{
		D3DXMATRIX m, m1;
		D3DXVECTOR4 v;
		//D3DXVECTOR4 lightDir = CManager::GetLight()->GetVecDir();

		//通常のワールドマトリクスをセット
		m_pEffect->SetMatrix(m_pWorld, pMatWorld);

		//ワールド×ビュー×プロジェクションマトリクスをセット
		m = (*pMatWorld) * m_matView * m_matProj;
		m_pEffect->SetMatrix(m_pWVP,&m);

		//TODO:必要なときに使用
		//D3DXMatrixInverse(&m1,NULL,pMatWorld);
		//D3DXVec4Transform(&v,&lightDir,&m1);
		//D3DXVec4Normalize(&v,&v);
		//m_pEffect->SetVector(m_pLightDir,&v);
	}
}
//=============================================================================
//色(ディフューズ色)のセット
//=============================================================================
void CToonShader::SetColor(D3DXVECTOR4 diffuse)
{
	if (m_pEffect)
	{
		D3DXVECTOR4 diffuseVec = diffuse;
		m_pEffect->SetVector(m_pColor, &diffuseVec);
	}
}
//EOF