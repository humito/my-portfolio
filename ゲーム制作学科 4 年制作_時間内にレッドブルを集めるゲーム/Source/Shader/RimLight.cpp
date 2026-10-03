//=============================================================================
// リムライト [RimLight.cpp]
// Author : 木村　文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "RimLight.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../System/Light.h"
#include "../System/Camera.h"
#include "../State/Game.h"

//=============================================================================
//シェーダー読込
//=============================================================================
void CRimLight::Load()
{
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//リムライト生成
	CreateShader(pDevice, "data/HLSL/RimLight.hlsl",
				"VertexShader3D", "PixelShader3D",
				"vs_2_0", "ps_2_0",
				&m_pVSConstantTable, &m_pPSConstantTable,
				&m_pVertexShader, &m_pPixelShader);
}
//=============================================================================
//終了
//=============================================================================
void CRimLight::Uninit()
{
	//シェーダー関連の解放
	CShader::Release();
}
//=============================================================================
//シェーダー開始
//=============================================================================
void CRimLight::Begin(LPDIRECT3DDEVICE9 pDevice)
{
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
void CRimLight::End(LPDIRECT3DDEVICE9 pDevice)
{
	//シェーダーを戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//マトリックスのセット
//=============================================================================
void CRimLight::SetMatrix(	LPDIRECT3DDEVICE9 pDevice,
							D3DXMATRIX *pMtxWorld,
							D3DXVECTOR3 pos)
{
	//ビュー、プロジェクションマトリクス取得
	D3DXMATRIX matView, matProj;
	pDevice->GetTransform(D3DTS_VIEW, &matView);
	pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	//ワールドマトリクスセット
	m_pVSConstantTable->SetMatrix(pDevice, "g_WorldViewProjection", &(*pMtxWorld * matView * matProj));
	m_pVSConstantTable->SetMatrix(pDevice, "g_World", pMtxWorld);

	//計算用行列・ベクトル
	D3DXMATRIX m;
	D3DXVECTOR4 v;

	//正規化したカメラベクトルセット
	D3DXVECTOR4 cameraPos = D3DXVECTOR4(CManager::GetCamera()->GetPosCamera(), 1.0f);
	m = (*pMtxWorld);
	D3DXMatrixInverse(&m, NULL, &m);
	D3DXVec4Transform(&v, &cameraPos, &m);
	m_pVSConstantTable->SetVector(pDevice, "g_CameraPos", &v);

	//正規化したライトベクトルのセット
	D3DXVECTOR3 vec = (D3DXVECTOR3)cameraPos - pos;
	D3DXVECTOR4 lightDir = D3DXVECTOR4(vec, 1.0f);
	
	D3DXMatrixInverse(&m, NULL, pMtxWorld);
	D3DXVec4Transform(&v, &lightDir, &m);
	D3DXVec4Normalize(&v, &v);
	m_pVSConstantTable->SetVector(pDevice, "g_LightDir", &v);
}
//=============================================================================
//リムライトの強さセット
//=============================================================================
void CRimLight::SetPower(LPDIRECT3DDEVICE9 pDevice, float fPower)
{
	m_pPSConstantTable->SetFloat(pDevice, "g_Power", fPower);
}
//=============================================================================
//マテリアルのセット
//=============================================================================
void CRimLight::SetMaterial(LPDIRECT3DDEVICE9 pDevice,
							D3DXVECTOR4 materialVec)
{
	m_pVSConstantTable->SetVector(pDevice, "g_Color", &materialVec);
}
//EOF