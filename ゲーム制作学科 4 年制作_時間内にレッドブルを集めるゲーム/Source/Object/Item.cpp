//=============================================================================
//アイテム処理[Item.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Item.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../State/Game.h"
#include "../Object/Mesh/MeshField.h"
#include "../Shader/RimLight.h"
#include "../Shader/MotionBlur.h"
#include "../Shader/Shadow.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define ITEM_MODEL_NAME ("data/MODEL/can.x")//アイテムファイル名
#define ITEM_GRAVITY (0.1f)				//アイテム重力
#define ADD_ITEM_ROT_Y (0.05f)				//アイテムY軸回転量

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
LPDIRECT3DTEXTURE9 *CItem::m_pD3DTextureArray = NULL;
int					CItem::m_nNumMat = 0;

//=============================================================================
//コンストラクタ
//=============================================================================
CItem::CItem()
{
	m_type = ITEM_TYPE;
}

//=============================================================================
//初期化
//=============================================================================
HRESULT CItem::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//座標・角度・スケール設定
	m_pos = pos;
	m_rot = rot;
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//シェーダーキャストの種類
	m_castType = CAST_NONE;

	//モデルの生成
	HRESULT hr = CreateModel(ITEM_MODEL_NAME);

	//Xファイルの生成
	if (hr == E_FAIL)
		return E_FAIL;

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//マテリアル関係取得
	m_pD3DXMat = (D3DXMATERIAL*)m_pD3DXBuffMatModel->GetBufferPointer();

	//テクスチャの読込
	if (!m_pD3DTextureArray)
	{
		m_nNumMat = m_nNumMatModel;

		m_pD3DTextureArray = new LPDIRECT3DTEXTURE9[m_nNumMat];
		for (unsigned int i = 0; i < m_nNumMat; ++i)
		{
			//マテリアルのテクスチャファイル名からテクスチャ読込
			m_pD3DTextureArray[i] = NULL;
			if (m_pD3DXMat[i].pTextureFilename)
			{
				hr = D3DXCreateTextureFromFile(pDevice,
					m_pD3DXMat[i].pTextureFilename,
					&m_pD3DTextureArray[i]);

				if (hr == E_FAIL)
					continue;
			}
		}
	}

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CItem::Uninit()
{
	//終了処理
	CSceneX::Uninit();
}
//=============================================================================
//テクスチャ配列解放
//=============================================================================
void CItem::DeleteTextureArray()
{
	//テクスチャ配列解放
	if (m_pD3DTextureArray)
	{
		for (unsigned int i = 0; i < m_nNumMat; ++i)
		{
			if (m_pD3DTextureArray[i])
			{
				m_pD3DTextureArray[i]->Release();
				m_pD3DTextureArray[i] = NULL;
			}
		}

		delete[] m_pD3DTextureArray;
		m_pD3DTextureArray = NULL;
	}
}
//=============================================================================
//更新
//=============================================================================
void CItem::Update()
{
	//回転する
	m_rot.y += ADD_ITEM_ROT_Y;

	//アイテムが重力で下に落ちる
	m_pos.y -= ITEM_GRAVITY;

	//フィールドの高さに合わせる
	float fFieldHeight = CGame::GetField()->GetHeight(m_pos);
	if (m_pos.y < fFieldHeight)
		m_pos.y = fFieldHeight;
}
//=============================================================================
//描画
//=============================================================================
void CItem::Draw()
{
	//レンダラーの取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//マテリアルプロパティ
	D3DMATERIAL9 matDef;
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;//サイズ,回転,位置

	//プロジェクションマトリックスを反映
	D3DXMatrixIdentity(&m_mtxWorld);

	//サイズ設定
	D3DXMatrixScaling(&mtxScl,
					m_scl.x,
					m_scl.y,
					m_scl.z);

	//サイズ反映
	D3DXMatrixMultiply(&m_mtxWorld,
						&m_mtxWorld,
						&mtxScl);

	//回転設定
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
									m_rot.y,
									m_rot.x,
									m_rot.z);
	//回転反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
						&mtxRot);

	//位置設定
	D3DXMatrixTranslation(&mtxTranslate,
						m_pos.x,
						m_pos.y,
						m_pos.z);

	//位置反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
						&mtxTranslate);

	//マテリアルプロパティの取得
	pDevice->GetMaterial(&matDef);

	//キャストするシェーダーのタイプで使用するシェーダーを分ける

	CRimLight *pRimLight = NULL;	//リムライトシェーダー
	CShadow *pShadow = NULL;		//投影シャドウキャスト

	//現在のキャストの種類から取得するインスタンスを判別
	switch (m_castType)
	{
		//キャストシェーダーなし
		case CAST_NONE:
		{
			//キャストがない場合はリムライトシェーダー使う
			//リムライトシェーダーにワールドマトリックスセット
			pRimLight = pRenderer->GetRimLight();
			pRimLight->SetMatrix(pDevice, &m_mtxWorld, m_pos);
			pRimLight->SetPower(pDevice, 3.0f);

			//リムライトシェーダー開始
			pRimLight->Begin(pDevice);

			break;
		}

	//シャドウ
	case CAST_SHADOW:
	{
		//シャドウシェーダーにマトリックスをセット
		pShadow = pRenderer->GetShadow();
		pShadow->SetMatrix(pDevice, SHADER_SHADOW_CAST, &m_mtxWorld);
		pShadow->Begin(pDevice, SHADER_SHADOW_CAST);
		break;
	}

		default:
		break;
	}

	//ポリゴンの描画ループ
	for (int nCntMat = 0; nCntMat < (int)m_nNumMatModel; ++nCntMat)
	{
		//リムライトシェーダーのインスタンスがある場合
		if (pRimLight)
		{
			//シェーダーにマテリアル情報のセット
			D3DXVECTOR4 color =
				D3DXVECTOR4(m_pD3DXMat[nCntMat].MatD3D.Diffuse.r,
							m_pD3DXMat[nCntMat].MatD3D.Diffuse.g,
							m_pD3DXMat[nCntMat].MatD3D.Diffuse.b,
							m_pD3DXMat[nCntMat].MatD3D.Diffuse.a);

			pRimLight->SetMaterial(pDevice, color);
		}

		//テクスチャセット
		pDevice->SetTexture(0, m_pD3DTextureArray[nCntMat]);

		//ポリゴンの描画
		m_pD3DXMeshModel->DrawSubset(nCntMat);
	}

	//リムライトシェーダー完了
	if (pRimLight)
		pRimLight->End(pDevice);

	//投影シャドウ完了
	else if (pShadow)
		pShadow->End(pDevice);
}
//=============================================================================
//インスタンスの生成
//=============================================================================
CItem *CItem::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//インスタンス生成して初期化
	CItem *pItem = new CItem();
	pItem->Init(pos, rot);

	return pItem;
}
//EOF