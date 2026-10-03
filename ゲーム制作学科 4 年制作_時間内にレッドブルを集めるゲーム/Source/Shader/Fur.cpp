//=============================================================================
//ファーシェーダー[Fur.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Fur.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../System/Light.h"
#include "../System/Camera.h"
#include "../System/Common.h"

//=============================================================================
//シェーダー読込
//=============================================================================
void CFur::Load()
{
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//ファーシェーダー作成
	CreateShader(pDevice, "data/HLSL/Fur.hlsl", "VertexShader3D", "PixelShader3D",
				"vs_2_0", "ps_2_0", &m_pVSConstantTable, &m_pPSConstantTable,
				&m_pVertexShader, &m_pPixelShader);

	//ファーテクスチャ生成
	D3DXCreateTextureFromFileEx(pDevice,
								"data/TEXTURE/fur.png",
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
								&m_pD3DFurTex);
}
//=============================================================================
//終了
//=============================================================================
void CFur::Uninit()
{
	//トゥーンテクスチャ終了
	RELEASE_OBJECT(m_pD3DFurTex);

	//シェーダー関連の解放
	CShader::Release();
}
//=============================================================================
//シェーダー開始
//=============================================================================
void CFur::Begin(LPDIRECT3DDEVICE9 pDevice)
{
	//ファーをテクスチャにセット
	pDevice->SetTexture(1, m_pD3DFurTex);

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
void CFur::End(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->SetTexture(1, NULL);

	//シェーダーを戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//マトリックスのセット
//=============================================================================
void CFur::SetMatrix(LPDIRECT3DDEVICE9 pDevice,
					D3DXMATRIX *pMtxWorld,
					D3DXVECTOR3 pos)
{
	//ビュー、プロジェクションマトリクス取得
	D3DXMATRIX matView, matProj;
	pDevice->GetTransform(D3DTS_VIEW, &matView);
	pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	//ワールドマトリクスセット
	m_pVSConstantTable->SetMatrix(pDevice, "g_WorldViewProjection", &(*pMtxWorld * matView * matProj));

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
//オフセットのセット
//=============================================================================
void CFur::SetOffset(LPDIRECT3DDEVICE9 pDevice, float fOffset)
{
	m_pVSConstantTable->SetFloat(pDevice, "g_fOffset", fOffset);
}
//=============================================================================
//マテリアルのセット
//=============================================================================
void CFur::SetMaterial(LPDIRECT3DDEVICE9 pDevice,
						D3DXVECTOR4 materialVec)
{
	m_pVSConstantTable->SetVector(pDevice, "g_Color", &materialVec);
}
//EOF