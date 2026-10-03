//=============================================================================
//その他オブジェクト処理[Object.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Object.h"
#include "Mesh/MeshField.h"
#include "../System/renderer.h"
#include "../manager.h"
#include "../System/Camera.h"
#include "../System/Input/InputKeyboard.h"
#include "../Shader/DeferredRendering.h"

#ifdef _DEBUG
	#include "../System/DebugProc.h"
#endif

//*****************************************************************************
//定数定義
//*****************************************************************************
#define POS_MOVE (3.5f)			//プレイヤー移動量

//=============================================================================
//初期化
//=============================================================================
HRESULT CObject::Init(char *FileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot)
{
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//座標・角度・スケール設定
	m_pos = pos;
	m_rot = rot;
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//Xファイルの生成
	CSceneX::CreateModel(FileName);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CObject::Uninit()
{
	//終了処理
	CSceneX::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CObject::Update()
{
}
//=============================================================================
//描画
//=============================================================================
void CObject::Draw()
{
	//レンダラーの取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//マテリアルプロパティ
	D3DMATERIAL9 matDef;
	D3DXMATRIX mtxScl,mtxRot,mtxTranslate;//サイズ,回転,位置
	
	//プロジェクションマトリックスを反映
	D3DXMatrixIdentity(&m_mtxWorld);

	//サイズを反映
	D3DXMatrixScaling(&mtxScl,
					   m_scl.x,
					   m_scl.y,
					   m_scl.z);

	//位置を反映
	D3DXMatrixMultiply(&m_mtxWorld,
					   &m_mtxWorld,
					   &mtxScl);
	//回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
									m_rot.y,
									m_rot.x,
									m_rot.z);

	
	D3DXMatrixMultiply(&m_mtxWorld,&m_mtxWorld,
					   &mtxRot);

	//位置の設定
	D3DXMatrixTranslation(&mtxTranslate,
						  m_pos.x,
						  m_pos.y,
						  m_pos.z);

	//ワールドマトリックスの設定
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
						&mtxTranslate);

	//ワールドマトリクスの反映
	//pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//ディファードレンダリングのキャストとしてセット
	CDeferred *pDeferred = pRenderer->GetDeferred();
	pDeferred->SetMatrix(pDevice, &m_mtxWorld);

	//マテリアル関係取得
	//pDevice->GetMaterial(&matDef);
	m_pD3DXMat=(D3DXMATERIAL*)m_pD3DXBuffMatModel->GetBufferPointer();

	//シェーダー開始
	pDeferred->Begin(pDevice);

	//テクスチャセット
	pDevice->SetTexture(0, m_pD3DTexture1);

	for (int nCntMat = 0; nCntMat < (int)m_nNumMatModel; nCntMat++)
	{
		//pDevice->SetMaterial(&m_pD3DXMat[nCntMat].MatD3D);

		//拡散光のセット
		D3DXVECTOR4 diffuse = D3DXVECTOR4(	m_pD3DXMat[nCntMat].MatD3D.Diffuse.r,
											m_pD3DXMat[nCntMat].MatD3D.Diffuse.g,
											m_pD3DXMat[nCntMat].MatD3D.Diffuse.b,
											m_pD3DXMat[nCntMat].MatD3D.Diffuse.a);
		pDeferred->SetMaterial(pDevice, diffuse);

		//ポリゴンの描画
		m_pD3DXMeshModel->DrawSubset(nCntMat);
	}

	//シェーダー終了
	pDeferred->End(pDevice);
}
//=============================================================================
//その他オブジェクトインスタンス生成
//=============================================================================
CObject *CObject::Create(char *FileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//オブジェクトインスタンス生成	
	CObject *pObject = new CObject();

	//初期化
	pObject->Init(FileName,pos,rot);

	return pObject;
}
//EOF