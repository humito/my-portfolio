//=============================================================================
// アイテム処理 [Item.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Item.h"
#include "manager.h"
#include "renderer.h"
#include "Camera.h"
#include "Game.h"
#include "MeshField.h"
#include "FrustumCulling.h"
#include <stdio.h>
#include <time.h>

//デバッグ用
#ifdef _DEBUG
	#include "HitCheckSphere.h"
#endif

//*****************************************************************************
//定数定義
//*****************************************************************************
#define ITEM_VELOCITY_XZ (1.5f)		//アイテムが飛ぶ速度XZ
#define ITEM_VELOCITY_Y (5.0f)		//アイテムが飛ぶ速度Y
#define ITEM_VELOCITY_SUB_Y (0.1f)	//アイテム速度減算量Y
#define ITEM_HEIGHT (50.0f)			//アイテムの高さ

//=============================================================================
//コンストラクタ
//=============================================================================
CItem::CItem()
{
	//アイテムタイプ
	m_type = OBJECT_ITEM;
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CItem::Init(D3DXVECTOR3 pos, ITEM_TYPE type)
{
	//乱数初期化
	srand((unsigned int)time(NULL));

	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//ポリゴンの設定
	m_pos = pos;							//座標
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//角度

	//速度
	m_velocity = D3DXVECTOR3(sinf((float)(rand() % (int)(D3DX_PI * 2)))*ITEM_VELOCITY_XZ,
							ITEM_VELOCITY_Y,
							cosf((float)(rand() % (int)(D3DX_PI * 2)))*ITEM_VELOCITY_XZ);

	//アイテムの種類
	m_itemType = type;

	//描画フラグ
	m_bDraw = false;

	//ファイル名
	char *pFileName = "";

	///////////////////////////////////
	//		頂点バッファの生成		//
	/////////////////////////////////

	if (FAILED(pDevice->CreateVertexBuffer
		(sizeof(VERTEX_3D)* 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pD3DVtxBuffBill,
		NULL)))
	{
		return E_FAIL;
	}

	//頂点バッファのポインタ
	VERTEX_3D *pVtx;

	///////////////////////////////////
	//		頂点バッファの設定		//
	/////////////////////////////////

	//頂点バッファ設定開始
	m_pD3DVtxBuffBill->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標
	pVtx[0].vtx = D3DXVECTOR3(-ITEM_RADIUS, 0.0f, 0.0f);
	pVtx[1].vtx = D3DXVECTOR3(-ITEM_RADIUS, ITEM_HEIGHT, 0.0f);
	pVtx[2].vtx = D3DXVECTOR3(ITEM_RADIUS, 0.0f, 0.0f);
	pVtx[3].vtx = D3DXVECTOR3(ITEM_RADIUS, ITEM_HEIGHT, 0.0f);

	//法線ベクトル
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, -1.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, -1.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, -1.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, -1.0f);

	//光源
	pVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	//テクスチャ座標
	pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

	//頂点バッファ設定終了
	m_pD3DVtxBuffBill->Unlock();


	//アイテムの種類によって設定を分ける
	switch (type)
	{
		//タイムアップ
		case ITEM_TIME_UP:
			pFileName = "data/TEXTURE/item_time.png";
		break;

		//クロックアップ
		case ITEM_CLOCK_UP:
			pFileName = "data/TEXTURE/item_battery.png";
		break;

		default:
		break;
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							pFileName,
							&m_pD3DTextureBill);

//デバッグ用
#ifdef _DEBUG
	//球体ポリゴン生成
	m_pSphere = CHitCheckSphere::Create(ITEM_RADIUS);
#endif

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CItem::Uninit()
{
	//終了
	CSceneBillboard::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CItem::Update()
{
	//座標更新
	m_pos += m_velocity;
	m_velocity.y -= ITEM_VELOCITY_SUB_Y;

	//フィールドの下にめり込まないようにする
	float fHeight = CGame::GetField()->GetHeight(m_pos);
	if (m_pos.y < fHeight)
	{
		m_pos.y = fHeight;
		m_velocity.x = 0.0f;
		m_velocity.z = 0.0f;
	}

	//描画チェック
	m_bDraw = CFrustum::MeshFOVCheck(m_pos,ITEM_RADIUS);

//デバッグ用
#ifdef _DEBUG
	//球体ポリゴン生成
	m_pSphere->SetPos(m_pos);
#endif
}
//=============================================================================
//描画
//=============================================================================
void CItem::Draw()
{
	//描画許可ある場合のみ描画処理
	if (m_bDraw)
	{
		//レンダラー情報取得
		CRenderer *pRenderer = CManager::GetRenderer();
		//デバイスのゲット
		LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

		//カメラのゲット
		CCamera *pCamera = CManager::GetCamera();

		//ライティングOFF
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		//アルファテスト
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

		//不透明にする値の設定
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0x66);

		//行列合成用変数
		D3DXMATRIX mtxScl, mtxRot, mtxTranslate, mtxView;

		//カメラのビューマトリクス取得
		mtxView = pCamera->GetMtxView();

		//ワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_mtxWorld);
		D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);

		m_mtxWorld._41 = 0.0f;
		m_mtxWorld._42 = 0.0f;
		m_mtxWorld._43 = 0.0f;

		//スケールの設定
		D3DXMatrixScaling(&mtxScl, 1.0f, 1.0f, 1.0f);

		//スケールを反映
		D3DXMatrixMultiply(&m_mtxWorld,
						&m_mtxWorld,
						&mtxScl);

		//回転を設定
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
										0,
										0,
										0);

		//回転を反映
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
						&mtxRot);

		//位置を設定
		D3DXMatrixTranslation(&mtxTranslate,
							m_pos.x,
							m_pos.y,
							m_pos.z);

		//位置をセット
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
						&mtxTranslate);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD,
							&m_mtxWorld);

		//３Ｄポリゴンの描画
		pDevice->SetStreamSource(0, m_pD3DVtxBuffBill, 0, sizeof(VERTEX_3D));

		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0, m_pD3DTextureBill);

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
							0,
							2);

		//元に戻す
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}
}
//=============================================================================
//インスタンス生成
//=============================================================================
void CItem::Create(D3DXVECTOR3 pos, ITEM_TYPE type)
{
	//インスタンス生成して初期化
	CItem *pItem = new CItem();
	pItem->Init(pos,type);
}
//EOF