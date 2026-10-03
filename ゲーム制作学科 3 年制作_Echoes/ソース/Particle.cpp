//=============================================================================
//パーティクル処理[Particle.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Particle.h"
#include "renderer.h"
#include "manager.h"
#include "Camera.h"
#include "FrustumCulling.h"
#include <stdlib.h>
#include <time.h>

//*****************************************************************************
//定数定義
//*****************************************************************************
#define DISP_COUNT_MAX (100)			//表示カウント最大値
#define PARTICLE_DRAW_RADIUS (100.0f)	//描画範囲
#define VELOCITY_SUB_Y (0.1f)			//速度減算量
#define VELOCITY_RADIUS (1.5f)			//速度の半径
#define VELOCITY_STARS_Y (5.0f)			//星パーティクルのY座標速度
#define POS_RANGE (50)					//座標範囲
#define VELOCITY_RANGE (30)				//速度範囲
#define VELOCITY_SMOKE_Y (1.3f)			//煙パーティクルのY座標速度
#define SMOKE_UNIT (10)					//煙速度の単位

//=============================================================================
//コンストラクタ
//=============================================================================
CParticle::CParticle()
{
	//パーティクルオブジェクト
	m_type = OBJECT_PARTICLE;
	//パーティクルポインタNULL
	m_pParticle = NULL;
}
//=============================================================================
//デストラクタ
//=============================================================================
CParticle::~CParticle()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CParticle::Init(PARTICLE_TYPE type,int nNumParticle,D3DXVECTOR3 pos,
						float fWidth, float fHeight)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//乱数の初期化
	srand((unsigned int)time(NULL));

	//発生位置設定
	m_startPos = pos;

	//表示カウント初期化
	m_nDispCnt = 0;

	//α値
	m_nAlpha = 255;

	//描画フラグ
	m_bDraw = false;

	//パーティクルの種類
	m_typeParticle = type;

	//パーティクル数がない場合
	if (nNumParticle < 0)
	{
		//終了処理
		Uninit();
		return E_FAIL;
	}

	//パーティクル数の設定
	m_nNumParticle = nNumParticle;

	//パーティクル情報の動的確保
	m_pParticle = new PARTICLE[m_nNumParticle];

	//設定用速度
	D3DXVECTOR3 velocity;

	//設定角度
	float fRotY = 0.0f;
	
	//角度加算量
	float fAddRotY = (D3DX_PI * 2) / m_nNumParticle;

	//パーティクル情報の初期化
	for (int i = 0; i < m_nNumParticle; ++i)
	{
		//カウント
		m_pParticle[i].nCnt = 0;

		//幅
		m_pParticle[i].fWidth = fWidth;

		//高さ
		m_pParticle[i].fHeight = fHeight;

		//エフェクト用
		if (m_typeParticle == TYPE_EFFECT)
		{
			//座標
			m_pParticle[i].pos = pos;
			//速度
			velocity = D3DXVECTOR3(	sinf(fRotY) * VELOCITY_RADIUS,
									VELOCITY_STARS_Y,
									cosf(fRotY) * VELOCITY_RADIUS);
		}
		//煙用
		else if (m_typeParticle == TYPE_SMOKE)
		{
			//座標
			m_pParticle[i].pos = D3DXVECTOR3(pos.x + sinf(fRotY)*(rand() % POS_RANGE),
											pos.y,
											pos.z + cos(fRotY)*(rand() % POS_RANGE));

			//速度
			velocity = D3DXVECTOR3(sinf(fRotY) * ((rand() % VELOCITY_RANGE) / SMOKE_UNIT),
				VELOCITY_SMOKE_Y,
				cosf(fRotY) * ((rand() % VELOCITY_RANGE) / SMOKE_UNIT));

		}

		fRotY += fAddRotY;
		
		//速度
		m_pParticle[i].velocity = velocity;
	}

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
	pVtx[0].vtx = D3DXVECTOR3(-(fWidth / 2), -(fHeight / 2), 0.0f);
	pVtx[1].vtx = D3DXVECTOR3(-(fWidth / 2), (fHeight / 2), 0.0f);
	pVtx[2].vtx = D3DXVECTOR3((fWidth / 2), -(fHeight / 2), 0.0f);
	pVtx[3].vtx = D3DXVECTOR3((fWidth / 2), (fHeight / 2), 0.0f);

	//法線ベクトル
	pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//光源
	pVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);

	//テクスチャ座標
	pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

	//頂点バッファ設定終了
	m_pD3DVtxBuffBill->Unlock();

	//パーティクルの種類によってファイル名変更
	char *pFimeName = NULL;
	//エフェクト
	if (m_typeParticle == TYPE_EFFECT)
	{
		pFimeName = "data/TEXTURE/star.png";
	}
	//煙
	else if (m_typeParticle == TYPE_SMOKE)
	{
		pFimeName = "data/TEXTURE/smoke000.png";
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								pFimeName,
								&m_pD3DTextureBill);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CParticle::Uninit()
{
	//パーティクル情報解放
	if (m_pParticle != NULL)
	{
		delete[] m_pParticle;
		m_pParticle = NULL;
	}

	//終了
	CSceneBillboard::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CParticle::Update()
{
	///////////////////////////////////////////////////////////////
	//		各パーティクルの種類によって更新処理を分ける		//
	/////////////////////////////////////////////////////////////
	switch (m_typeParticle)
	{
		//エフェクト
		case TYPE_EFFECT:
		{
			//パーティクル座標更新
			for (int i = 0; i < m_nNumParticle; ++i)
			{
				m_pParticle[i].pos += m_pParticle[i].velocity;
				m_pParticle[i].velocity.y -= VELOCITY_SUB_Y;
			}

			//表示カウント最大値に達したら終了
			if (m_nDispCnt > DISP_COUNT_MAX)
			{
				Uninit();
			}

			break;
		}
		
		//煙
		case TYPE_SMOKE:
		{
			//座標更新
			for (int i = 0; i < m_nNumParticle; ++i)
			{
				m_pParticle[i].pos += m_pParticle[i].velocity;
			}

			//α値減算
			m_nAlpha--;

			//α値が0になると終了
			if (m_nAlpha < 0)
			{
				Uninit();
			}

			break;
		}

		default:
		break;
	}

	//視錐台カリング内にある場合は描画許可
	m_bDraw = CFrustum::MeshFOVCheck(m_startPos, PARTICLE_DRAW_RADIUS);

	//表示カウント加算
	m_nDispCnt++;
}
//=============================================================================
//描画
//=============================================================================
void CParticle::Draw()
{
	//描画許可あるなら描画
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

		//ワイヤーフレーム
		//pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);

		//加算合成
		/*pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);*/

		//アルファテスト
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		//不透明にする値の設定
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0x35);

		//頂点情報の変更
		ChangeBuffer();

		//パーティクル数分ループ
		for (int i = 0; i < m_nNumParticle; i++)
		{
			//マトリックス変換用変数
			D3DXMATRIX mtxScl, mtxRot, mtxTranslate, mtxView;

			//カメラ情報取得
			mtxView = pCamera->GetMtxView();

			//ワールドマトリックスの初期化
			D3DXMatrixIdentity(&m_mtxWorld);
			D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);

			m_mtxWorld._41 = 0.0f;
			m_mtxWorld._42 = 0.0f;
			m_mtxWorld._43 = 0.0f;

			//スケールを設定
			D3DXMatrixScaling(&mtxScl, 1.0f, 1.0f, 1.0f);

			//スケールを反映
			D3DXMatrixMultiply(&m_mtxWorld,
							&m_mtxWorld,
							&mtxScl);

			//回転を設定
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
											0.0f,
											0.0f,
											0.0f);

			//回転を反映
			D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
								&mtxRot);

			//位置を設定
			D3DXMatrixTranslation(&mtxTranslate,
								m_pParticle[i].pos.x,
								m_pParticle[i].pos.y,
								m_pParticle[i].pos.z);

			//位置を反映
			D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
							&mtxTranslate);

			//ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

			//頂点バッファのバインド
			pDevice->SetStreamSource(0, m_pD3DVtxBuffBill, 0, sizeof(VERTEX_3D));

			//頂点フォーマットのセット
			pDevice->SetFVF(FVF_VERTEX_3D);

			//テクスチャの設定
			pDevice->SetTexture(0, m_pD3DTextureBill);

			//ポリゴンの描画
			pDevice->DrawPrimitive(	D3DPT_TRIANGLESTRIP,
									0,
									2);
		}

		//pDevice->SetRenderState(D3DRS_FILLMODE, NULL);

		//元に戻す
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		/*pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);*/
	}
}
//=============================================================================
//頂点情報の変更
//=============================================================================
void CParticle::ChangeBuffer()
{
	//頂点バッファのポインタ
	VERTEX_3D *pVtx;

	//頂点バッファ設定開始
	m_pD3DVtxBuffBill->Lock(0, 0, (void**)&pVtx, 0);

	//光源
	pVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);

	//頂点バッファ設定終了
	m_pD3DVtxBuffBill->Unlock();
}
//=============================================================================
//インスタンス生成
//=============================================================================
void CParticle::Create( PARTICLE_TYPE type, int nNumParticle, D3DXVECTOR3 pos,
						float fWidth, float fHeight)
{
	//インスタンス生成
	CParticle *pParticle = new CParticle();

	//初期化
	pParticle->Init(type,nNumParticle,pos,fWidth,fHeight);
}
//EOF