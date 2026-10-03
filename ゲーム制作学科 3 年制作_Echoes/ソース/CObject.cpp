//=============================================================================
//その他オブジェクト処理[CObject.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CObject.h"
#include "renderer.h"
#include "manager.h"
#include "CMeshOrbit.h"
#include "Ccamera.h"
#include "CInputKeyboard.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define POS_MOVE (24.5f)			//プレイヤー移動量

//=============================================================================
//コンストラクタ
//=============================================================================
CObject::CObject()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CObject::~CObject()
{
}

//=============================================================================
//初期化
//=============================================================================
HRESULT CObject::Init(char *FileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot)
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//ポリゴンの設定
	m_posModel=pos;
	m_rotModel=rot;
	m_sclModel=D3DXVECTOR3(1.0f,1.0f,1.0f);


	if(FAILED(D3DXLoadMeshFromX(	FileName,
									D3DXMESH_SYSTEMMEM,
									pDevice,
									NULL,
									&m_pD3DXBuffMatModel,
									NULL,
									&m_nNumMatModel,
									&m_pD3DXMeshModel)))
	{
		return E_FAIL;
	}

	//軌跡エフェクト生成
	m_pMeshOrbit = CMeshOrbit::Create(	D3DXVECTOR3(0.0f, 0.0f, 0.0f),
										D3DXVECTOR3(0.0f, 0.0f, 0.0f),
										10,
										1,
										50.0f,
										10.0f);

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
	//カメラインスタンス取得
	CCamera *m_pCamera = CManager::GetCamera();

	//カメラの向き取得
	D3DXVECTOR3 rot = m_pCamera->m_rotCamera;

	///////////////////////////////////
	//			操作処理			//
	/////////////////////////////////

	//前
	if (CInputKeyboard::GetKeyPress(DIK_UP))
	{
		m_posModel.x -= cosf(rot.y + D3DX_PI / 2)*POS_MOVE;//X
		m_posModel.z += sinf(rot.y + D3DX_PI / 2)*POS_MOVE;//Z
	}

	//後
	if (CInputKeyboard::GetKeyPress(DIK_DOWN))
	{
		m_posModel.x += cosf(rot.y + D3DX_PI / 2)*POS_MOVE;//X
		m_posModel.z -= sinf(rot.y + D3DX_PI / 2)*POS_MOVE;//Z
	}

	//左
	if (CInputKeyboard::GetKeyPress(DIK_LEFT))
	{
		m_posModel.x -= sinf(rot.y + D3DX_PI / 2)*POS_MOVE;//X
		m_posModel.z -= cosf(rot.y + D3DX_PI / 2)*POS_MOVE;//Z
	}

	//右
	if (CInputKeyboard::GetKeyPress(DIK_RIGHT))
	{
		m_posModel.x += sinf(rot.y + D3DX_PI / 2)*POS_MOVE;//X
		m_posModel.z += cosf(rot.y + D3DX_PI / 2)*POS_MOVE;//Z
	}

	//右回転
	if (CInputKeyboard::GetKeyPress(DIK_RSHIFT))
	{
		m_rotModel.x -= 0.1f;
	}

	//左回転
	if (CInputKeyboard::GetKeyPress(DIK_LSHIFT))
	{
		m_rotModel.x += 0.1f;
	}

	//親マトリクスのセット
	m_pMeshOrbit->SetParentMtx(m_mtxWorld);
}
//=============================================================================
//描画
//=============================================================================
void CObject::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();
	
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;
	D3DXMATRIX mtxScl,mtxRot,mtxTranslate;//サイズ,回転,位置
	
	//プロジェクションマトリックスを反映
	D3DXMatrixIdentity(&m_mtxWorld);

	//サイズを反映
	D3DXMatrixScaling(&mtxScl,
					   m_sclModel.x,
					   m_sclModel.y,
					   m_sclModel.z);

	//位置を反映
	D3DXMatrixMultiply(&m_mtxWorld,
					   &m_mtxWorld,
					   &mtxScl);
	//回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
									m_rotModel.y,
									m_rotModel.x,
									m_rotModel.z);

	
	D3DXMatrixMultiply(&m_mtxWorld,&m_mtxWorld,
					   &mtxRot);


	//位置を反映
	D3DXMatrixTranslation(&mtxTranslate,
						  m_posModel.x,
						  m_posModel.y,
						  m_posModel.z);

	//ワールドマトリックスの設定
	D3DXMatrixMultiply(&m_mtxWorld,&m_mtxWorld,
				 &mtxTranslate);

	pDevice->GetMaterial(&matDef);

	pD3DXMat=(D3DXMATERIAL*)m_pD3DXBuffMatModel->GetBufferPointer();

	//座標のセット
	pDevice->SetTransform(D3DTS_WORLD,
						  &m_mtxWorld);

	for(int nCntMat=0;nCntMat<(int)m_nNumMatModel;nCntMat++)
	{
		pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);
		pDevice->SetTexture(0,NULL);
		m_pD3DXMeshModel->DrawSubset(nCntMat);
	}

	//マテリアルセット
	pDevice->SetMaterial(&matDef);
}

//=============================================================================
//その他オブジェクトインスタンス生成
//=============================================================================
void CObject::Create(char *FileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot)
{
	//プレイヤーポインタ
	CObject *pObject;

	//プレイヤーインスタンス生成
	pObject=new CObject();

	//プレイヤー初期化
	pObject->Init(FileName,pos,rot);
}
//EOF