//=============================================================================
// テクスチャアニメーションエフェクト [AnimationEffect.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "AnimationEffect.h"
#include "Camera.h"
#include "FrustumCulling.h"
#include "manager.h"
#include "renderer.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define SMOKE_ANIM_NUM (8)		//煙アニメ数
#define EXPLOSION_ANIM_NUM (5)	//爆発アニメ数
#define COUNT_WAIT (2)			//テクスチャアニメーションの重み

//=============================================================================
//初期化
//=============================================================================
HRESULT CAnimEffect::Init(D3DXVECTOR3 pos,
						int nCountMax,
						float fSize,
						ANIMEFFECT type)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//ファイル名
	char *pFileName = NULL;

	//座標・角度
	m_pos = pos;
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//サイズ
	m_fSize = fSize;

	//描画フラグ
	m_bDraw = false;

	//カウント数
	m_fCount = 0.0f;

	//表示カウント最大
	m_nCountMax = nCountMax;

	//エフェクトの種類からアニメーション数とファイル名を分ける
	switch (type)
	{
		//煙
		case SMOKE_EFFECT:
			pFileName = "data/TEXTURE/explosion000.png";
			m_nAnimNum = SMOKE_ANIM_NUM;
			m_fMoveV = 1.0f;
		break;
		
		//爆発
		case EXPLOSION_EFFECT:
			pFileName = "data/TEXTURE/explosion001.png";
			m_nAnimNum = EXPLOSION_ANIM_NUM;
			m_fMoveV = 0.5f;
		break;

		default:
		break;
	}

	//U座標移動量
	m_fMoveU = 1.0f / m_nAnimNum;

	///////////////////////////////////
	//		頂点バッファ生成		//
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

	//半分のサイズ
	float fHalfSize = m_fSize / 2;

	m_pD3DVtxBuffBill->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標
	pVtx[0].vtx = D3DXVECTOR3(-fHalfSize, 0.0f, 0.0f);
	pVtx[1].vtx = D3DXVECTOR3(-fHalfSize, m_fSize, 0.0f);
	pVtx[2].vtx = D3DXVECTOR3(fHalfSize, 0.0f, 0.0f);
	pVtx[3].vtx = D3DXVECTOR3(fHalfSize, m_fSize, 0.0f);

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
	pVtx[0].tex = D3DXVECTOR2(0.0f, m_fMoveV);
	pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(m_fMoveU, m_fMoveV);
	pVtx[3].tex = D3DXVECTOR2(m_fMoveU, 0.0f);

	m_pD3DVtxBuffBill->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							pFileName,
							&m_pD3DTextureBill);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CAnimEffect::Uninit()
{
	//終了
	CSceneBillboard::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CAnimEffect::Update()
{
	//カウントアップ
	m_fCount++;

	//カウント最大値に達したら終了
	if (m_fCount > m_nCountMax)
	{
		Uninit();
	}

	//描画判定
	m_bDraw = CFrustum::MeshFOVCheck(m_pos, m_fSize / 2);
}
//=============================================================================
//描画
//=============================================================================
void CAnimEffect::Draw()
{
	//描画フラグtrueのみ描画処理をする
	if (m_bDraw == true)
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
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0x85);

		//テクスチャの変更
		ChangeTexture();

		//マトリックスの各変数
		D3DXMATRIX mtxScl, mtxRot, mtxTranslate, mtxView;

		//カメラ情報取得
		mtxView = pCamera->GetMtxView();

		D3DXMatrixIdentity(&m_mtxWorld);
		D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);

		m_mtxWorld._41 = 0.0f;
		m_mtxWorld._42 = 0.0f;
		m_mtxWorld._43 = 0.0f;

		//サイズの設定
		D3DXMatrixScaling(&mtxScl, 1.0f, 1.0f, 1.0f);

		//サイズのセット
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScl);

		//回転を設定
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
										0,
										0,
										0);

		//回転を反映
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

		//位置を反映
		D3DXMatrixTranslation(&mtxTranslate,
							m_pos.x,
							m_pos.y,
							m_pos.z);

		//位置をセット
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTranslate);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);


		//３Ｄポリゴンの描画
		pDevice->SetStreamSource(0, m_pD3DVtxBuffBill, 0, sizeof(VERTEX_3D));

		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0, m_pD3DTextureBill);

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
								0,//ポリゴンの数
								2);

		//元に戻す
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}
}
//=============================================================================
//テクスチャ座標変更
//=============================================================================
void CAnimEffect::ChangeTexture()
{
	//頂点バッファのポインタ
	VERTEX_3D *pVtx;

	//ビルボードテクスチャ座標の変更
	m_pD3DVtxBuffBill->Lock(0, 0, (void**)&pVtx, 0);

	//テクスチャ座標をカウントの経過によって変更させる
	pVtx[0].tex = D3DXVECTOR2(0.0f + (m_fMoveU*((int)(m_fCount / COUNT_WAIT) % m_nAnimNum)), m_fMoveV + (m_fMoveV*((int)(m_fCount / COUNT_WAIT) / m_nAnimNum)));
	pVtx[1].tex = D3DXVECTOR2(0.0f + (m_fMoveU*((int)(m_fCount / COUNT_WAIT) % m_nAnimNum)), 0.0f + (m_fMoveV*((int)(m_fCount / COUNT_WAIT) / m_nAnimNum)));
	pVtx[2].tex = D3DXVECTOR2(m_fMoveU + (m_fMoveU*((int)(m_fCount / COUNT_WAIT) % m_nAnimNum)), m_fMoveV + (m_fMoveV*((int)(m_fCount / COUNT_WAIT) / m_nAnimNum)));
	pVtx[3].tex = D3DXVECTOR2(m_fMoveU + (m_fMoveU*((int)(m_fCount / COUNT_WAIT) % m_nAnimNum)), 0.0f + (m_fMoveV*((int)(m_fCount / COUNT_WAIT) / m_nAnimNum)));

	m_pD3DVtxBuffBill->Unlock();
}
//=============================================================================
//インスタンス生成
//=============================================================================
void CAnimEffect::Create(D3DXVECTOR3 pos,
						int nCountMax,
						float fSize,
						ANIMEFFECT type)
{
	//インスタンス生成して初期化
	CAnimEffect *pAnimEffect = new CAnimEffect();
	pAnimEffect->Init(pos, nCountMax, fSize, type);
}
//EOF