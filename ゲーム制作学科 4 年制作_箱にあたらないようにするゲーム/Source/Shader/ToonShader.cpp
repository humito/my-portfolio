//=============================================================================
// トゥーンシェーダー [ToonShader.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "ToonShader.h"
#include "../main.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../System/Light.h"
#include "../System/Common.h"

//=============================================================================
//シェーダー読込
//=============================================================================
void CToonShader::Load()
{
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//トゥーンシェーダー生成
	CreateShader(pDevice, "data/HLSL/ToonShader.hlsl",
				"VertexShader3D", "PixelShader3D",
				"vs_2_0", "ps_2_0",
				&m_pVSConstantTable, &m_pPSConstantTable,
				&m_pVertexShader, &m_pPixelShader);

	///////////////////////////////////////////
	//		トゥーンテクスチャを生成		//
	/////////////////////////////////////////
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
}
//=============================================================================
//終了
//=============================================================================
void CToonShader::Uninit()
{
	//トゥーンテクスチャ終了
	RELEASE_OBJECT(m_pD3DToonMap);

	//シェーダー関連の解放
	CShader::Release();
}
//=============================================================================
//シェーダー開始
//=============================================================================
void CToonShader::Begin(LPDIRECT3DDEVICE9 pDevice)
{
	//トゥーンマップをテクスチャにセット
	pDevice->SetTexture(m_pPSConstantTable->GetSamplerIndex("g_ToonMap"), m_pD3DToonMap);

	//頂点シェーダーのセット
	if (m_pVertexShader)
		pDevice->SetVertexShader(m_pVertexShader);

	//ピクセルシェーダーのセット
	if (m_pPixelShader)
		pDevice->SetPixelShader(m_pPixelShader);
}
//=============================================================================
//シェーダー完了
//=============================================================================
void CToonShader::End(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->SetTexture(m_pPSConstantTable->GetSamplerIndex("g_ToonMap"), NULL);

	//シェーダーを戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//マトリックスのセット
//=============================================================================
void CToonShader::SetMatrix(LPDIRECT3DDEVICE9 pDevice,
	D3DXMATRIX *pMtxWorld)
{
	//ビュー、プロジェクションマトリクス取得
	D3DXMATRIX matView, matProj;
	pDevice->GetTransform(D3DTS_VIEW, &matView);
	pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	//ワールドマトリクスセット
	m_pVSConstantTable->SetMatrix(pDevice, "g_WorldViewProjection", &(*pMtxWorld * matView * matProj));
	m_pVSConstantTable->SetMatrix(pDevice, "g_World", pMtxWorld);

	//正規化したライトベクトルのセット
	D3DXMATRIX m;
	D3DXVECTOR4 v;
	D3DXVECTOR4 lightDir = D3DXVECTOR4(CManager::GetLight()->GetVecDir(), 1.0f);
	D3DXMatrixInverse(&m, NULL, pMtxWorld);
	D3DXVec4Transform(&v, &lightDir, &m);
	D3DXVec3Normalize((D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v);
	m_pPSConstantTable->SetVector(pDevice, "g_LightDir", &v);
}
//=============================================================================
//マテリアルのセット
//=============================================================================
void CToonShader::SetMaterial(LPDIRECT3DDEVICE9 pDevice,
	D3DXVECTOR4 materialVec)
{
	m_pVSConstantTable->SetVector(pDevice, "g_Color", &materialVec);
}
//EOF