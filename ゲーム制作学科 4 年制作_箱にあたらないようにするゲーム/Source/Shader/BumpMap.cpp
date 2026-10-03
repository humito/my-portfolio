//=============================================================================
// バンプマップ [BumpMap.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "BumpMap.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../System/Light.h"
#include "../System/Camera.h"
#include "../System/Common.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CBumpMap::CBumpMap()
{
	m_pD3DNormalMap = NULL;
	m_pNormalMapShader = NULL;
}
//=============================================================================
//シェーダー読込
//=============================================================================
void CBumpMap::Load()
{
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//シェーダー作成
	CreateShader(pDevice, "data/HLSL/BumpMap.hlsl",
				"VertexShader3D", "PixelShader3D",
				"vs_2_0", "ps_3_0",
				&m_pVSConstantTable, &m_pPSConstantTable,
				&m_pVertexShader, &m_pPixelShader);

	//法線マップテクスチャの読込
	D3DXCreateTextureFromFileEx(pDevice,
								"data/TEXTURE/Metal_Normal.bmp",
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
								&m_pD3DNormalMap);
}
//=============================================================================
//シェーダー終了
//=============================================================================
void CBumpMap::Uninit()
{
	//法線マップテクスチャ解放
	RELEASE_OBJECT(m_pD3DNormalMap);

	//法線計算用シェーダー解放
	RELEASE_OBJECT(m_pNormalMapShader);

	//シェーダー関連の解放
	CShader::Release();
}
//=============================================================================
//シェーダー開始
//=============================================================================
void CBumpMap::Begin(LPDIRECT3DDEVICE9 pDevice)
{
	//法線マップをテクスチャにセット
	pDevice->SetTexture(1, m_pD3DNormalMap);

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
void CBumpMap::End(LPDIRECT3DDEVICE9 pDevice)
{
	//テクスチャを戻す
	pDevice->SetTexture(1, NULL);

	//シェーダーを戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//マトリックスのセット
//=============================================================================
void CBumpMap::SetMatrix(LPDIRECT3DDEVICE9 pDevice,
						D3DXMATRIX *pMtxWorld)
{
	//ビュー、プロジェクションマトリクス取得
	D3DXMATRIX matView, matProj;
	pDevice->GetTransform(D3DTS_VIEW, &matView);
	pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	//ワールドマトリクスセット
	m_pVSConstantTable->SetMatrix(pDevice, "g_World", pMtxWorld);
	m_pVSConstantTable->SetMatrix(pDevice, "g_WorldViewProjection", &(*pMtxWorld * matView * matProj));
	
	//ワールドの逆行列と転置行列を掛けたマトリックスをセット
	D3DXMATRIX inverseTranspose;
	float determinant = D3DXMatrixDeterminant(pMtxWorld);
	D3DXMatrixInverse(&inverseTranspose, &determinant, pMtxWorld);
	D3DXMatrixTranspose(&inverseTranspose, &inverseTranspose);
	m_pVSConstantTable->SetMatrix(pDevice, "g_WorldInverseTranspose", &inverseTranspose);

	//カメラ座標のセット
	D3DXVECTOR4 cameraPos = D3DXVECTOR4(CManager::GetCamera()->GetPosCamera(), 1.0f);
	m_pPSConstantTable->SetVector(pDevice, "g_CameraPos", &cameraPos);

	///////////////////////////////////
	//		ライト関連のセット		//
	/////////////////////////////////
	
	//ライトインスタンス取得
	CLight *pLight = CManager::GetLight();

	//ポイントライトの個数セット
	int nPointLightNum = 3;
	m_pPSConstantTable->SetInt(pDevice, "g_PointLightNum", nPointLightNum);

	//4つのポイントライトから定数レジスタに送る
	D3DXVECTOR3 pointLightPos[3];		//ポイントライト座標
	D3DXVECTOR3 pointLightColor[3];		//ポイントライト色
	D3DXVECTOR3 pointLightSpecColor[3];	//ポイントライトスペキュラ色

	//ライトから各情報を取得
	for (int i = 1; i <= nPointLightNum; ++i)
	{
		pointLightPos[i - 1] = pLight->GetLightPos(i);
		pointLightColor[i - 1] = pLight->GetLightColor(i);
		pointLightSpecColor[i - 1] = pLight->GetLightSpecular(i);
	}

	//ポイントライト座標のセット
	m_pPSConstantTable->SetVectorArray(pDevice, "g_PointLightPosW",
										(D3DXVECTOR4*)pointLightPos,
										nPointLightNum);

	//ポイントライト色のセット
	m_pPSConstantTable->SetVectorArray(pDevice, "g_LightColor",
										(D3DXVECTOR4*)pointLightColor,
										nPointLightNum);

	//ポイントライトスペキュラ色のセット
	m_pPSConstantTable->SetVectorArray(pDevice, "g_LightSpecColor",
										(D3DXVECTOR4*)pointLightSpecColor,
										nPointLightNum);

	//ポイントライト減衰率のセット
	float fAttenuation[3] = { 0.0f, 0.1f, 0.3f };
	m_pPSConstantTable->SetFloatArray(pDevice, "g_Attenuation",
										fAttenuation, 3);
}
//=============================================================================
//マテリアルのセット
//=============================================================================
void CBumpMap::SetMaterial(LPDIRECT3DDEVICE9 pDevice,
							D3DXVECTOR4 materialVec,
							D3DXVECTOR4 materialSpecVec)
{
	m_pPSConstantTable->SetVector(pDevice, "g_Color", &materialVec);
	m_pPSConstantTable->SetVector(pDevice, "g_SpecColor", &materialSpecVec);
}
//EOF