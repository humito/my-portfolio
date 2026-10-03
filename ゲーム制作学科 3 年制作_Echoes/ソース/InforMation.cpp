//=============================================================================
// 情報表示 [InforMation.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "manager.h"
#include "renderer.h"
#include "Camera.h"
#include "Game.h"
#include "Player.h"
#include "InforMation.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define INFO_POS_Y (100.0f)		//Y座標補正
#define INFO_DISP_COUNT_MAX (50)//表示カウント最大値

//=============================================================================
//初期化
//=============================================================================
HRESULT CInfo::Init(INFO_TYPE type)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//ポリゴンの設定
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//座標
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//角度

	//カウント初期化
	m_nCount = 0;

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
	pVtx[0].vtx = D3DXVECTOR3(-50.0f, 0.0f, 0.0f);
	pVtx[1].vtx = D3DXVECTOR3(-50.0f, 50.0f, 0.0f);
	pVtx[2].vtx = D3DXVECTOR3(50.0f, 0.0f, 0.0f);
	pVtx[3].vtx = D3DXVECTOR3(50.0f, 50.0f, 0.0f);

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

	//情報の種類によって読み込むファイルを分ける
	switch (type)
	{
		//タイムアップ
		case TIME_UP:
			pFileName = "data/TEXTURE/info000.png";
		break;

		//クロックアップ
		case CLOCK_UP:
			pFileName = "data/TEXTURE/info001.png";
		break;

		default:
		break;
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							pFileName,
							&m_pD3DTextureBill);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CInfo::Uninit()
{
	//終了
	CSceneBillboard::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CInfo::Update()
{
	//表示カウントが一定に達したら終了
	if (m_nCount > INFO_DISP_COUNT_MAX)
	{
		Uninit();
	}
	else
	{
		//それ以外はカウントアップしてプレイヤーの上に設置
		m_nCount++;

		m_pos = CGame::GetPlayer()->GetPos();
		m_pos.y = INFO_POS_Y;
	}
}
//=============================================================================
//描画
//=============================================================================
void CInfo::Draw()
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

	//Zバッファ無視
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

	//行列合成用変数
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate, mtxView;

	//カメラ情報取得
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
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}
//=============================================================================
//インスタンス生成
//=============================================================================
void CInfo::Create(INFO_TYPE type)
{
	//インスタンス生成して初期化
	CInfo *pInfo = new CInfo();
	pInfo->Init(type);
}
//EOF