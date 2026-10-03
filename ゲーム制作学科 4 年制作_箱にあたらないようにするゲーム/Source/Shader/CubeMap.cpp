//=============================================================================
// キューブマップ [CubeMap.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CubeMap.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../System/Common.h"
#include "../System/Camera.h"
#include "../System/Light.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CCubeMap::CCubeMap()
{
	m_pD3DCubeMap = NULL;
}
//=============================================================================
//シェーダー読込
//=============================================================================
void CCubeMap::Load()
{
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//キューブマップ生成
	CreateShader(pDevice, "data/HLSL/CubeMap.hlsl",
				"VertexShader3D", "PixelShader3D",
				"vs_2_0", "ps_3_0",
				&m_pVSConstantTable, &m_pPSConstantTable,
				&m_pVertexShader, &m_pPixelShader);

	//キューブマップテクスチャ生成
	D3DXCreateCubeTextureFromFile(	pDevice, "data/TEXTURE/LobbyCube.dds",
									&m_pD3DCubeMap);
}
//=============================================================================
//終了
//=============================================================================
void CCubeMap::Uninit()
{
	RELEASE_OBJECT(m_pD3DCubeMap);

	//シェーダーの解放
	CShader::Release();
}
//=============================================================================
//シェーダー開始
//=============================================================================
void CCubeMap::Begin(LPDIRECT3DDEVICE9 pDevice)
{
	//トゥーンマップをテクスチャにセット
	pDevice->SetTexture(m_pPSConstantTable->GetSamplerIndex("g_CubeMap"), m_pD3DCubeMap);

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
void CCubeMap::End(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//マトリックスのセット
//=============================================================================
void CCubeMap::SetMatrix(LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX *pMtxWorld)
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

	//カメラ座標のセット
	D3DXVECTOR4 cameraPos = D3DXVECTOR4(CManager::GetCamera()->GetPosCamera(), 1.0f);
	m_pVSConstantTable->SetVector(pDevice, "g_CameraPos", &cameraPos);
	m_pPSConstantTable->SetVector(pDevice, "g_CameraPos", &cameraPos);
}
//EOF