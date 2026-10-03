//=============================================================================
//エフェクト処理[CEffect.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CEffect.h"
#include "manager.h"
#include "Ccamera.h"
//*****************************************************************************
//定数定義
//*****************************************************************************
#define COUNT_MAX (20)		//カウント最大値
#define TEX_NUM (5)			//テクスチャのアニメーション数
#define COUNT_WAIT (2)		//テクスチャアニメーションの重み
#define TEX_MOVE_X (0.2f)	//テクスチャ座標移動量X
#define TEX_MOVE_Y (0.5f)	//テクスチャ座標移動量Y
//=============================================================================
//コンストラクタ
//=============================================================================
CEffect::CEffect()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CEffect::~CEffect()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CEffect::Init(D3DXVECTOR3 pos)
{
	//カウント初期化
	m_fCount=0.0f;

	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//ポリゴンの設定
	m_pos=pos;							//座標
	m_rot=D3DXVECTOR3(0.0f,0.0f,0.0f);	//角度
	m_bDisp=true;						//表示フラグ

	///////////////////////////////////
	//		頂点バッファ生成		//
	/////////////////////////////////

	if(FAILED(	pDevice->CreateVertexBuffer
				(sizeof(VERTEX_3D)*4,
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

	m_pD3DVtxBuffBill->Lock(0,0,(void**)&pVtx,0);

	//頂点座標
	pVtx[0].vtx=D3DXVECTOR3(-25.0f,0.0f,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(-25.0f,25.0f,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(25.0f,0.0f,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(25.0f,25.0f,0.0f);

	//法線ベクトル
	pVtx[0].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);
	pVtx[1].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);
	pVtx[2].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);
	pVtx[3].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);

	//光源
	pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

	//テクスチャ座標
	pVtx[0].tex=D3DXVECTOR2(0.0f,0.5f);
	pVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	pVtx[2].tex=D3DXVECTOR2(0.2f,0.5f);
	pVtx[3].tex=D3DXVECTOR2(0.2f,0.0f);

	m_pD3DVtxBuffBill->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							"data/TEXTURE/explosion001.png",
							&m_pD3DTextureBill);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CEffect::Uninit()
{
	//ビルボードの終了
	CSceneBillboard::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CEffect::Update()
{
	///////////////////////////////////////////////////
	//		カメラの範囲内との外積当たり判定		//
	/////////////////////////////////////////////////

	//カメラの取得
	CCamera *pCamera=CManager::GetCamera();

	//カメラ範囲の頂点座標
	D3DXVECTOR3 cameraVertexPos[CAMERA_RECT_VERTEX_MAX];

	//4頂点数分ループ
	for(int i=0;i<CAMERA_RECT_VERTEX_MAX;i++)
	{
		//カメラ範囲の頂点座標取得
		pCamera->GetVertexPos(&cameraVertexPos[i],i);
	}

	//カメラベクトルの内側である回数
	int nCrossCnt=0;

	//カメラ範囲4頂点数分ループ
	for(int j=0;j<CAMERA_RECT_VERTEX_MAX;j++)
	{
		//カメラ範囲の頂点座標の方向ベクトル
		D3DXVECTOR3 vec1=cameraVertexPos[(j+1)%CAMERA_RECT_VERTEX_MAX]-cameraVertexPos[j];
		//カメラ範囲の頂点座標から弾への方向ベクトル
		D3DXVECTOR3 vec2=m_pos-cameraVertexPos[j];

		//弾の座標がカメラ頂点の方向ベクトルの内側なら
		if(vec1.x*vec2.z-vec1.z*vec2.x<0.0f)
		{
			//カウントアップ
			nCrossCnt++;
		}
		else
		{
			//外側なら表示フラグをfalseにしてループから抜ける
			m_bDisp=false;
			break;
		}
	}

	//カメラ範囲内に弾の座標がある場合
	if(nCrossCnt>=CAMERA_RECT_VERTEX_MAX)
	{
		//表示フラグtrue
		m_bDisp=true;
	}

	//カウントアップ
	m_fCount++;

	//カウント一定に達したら
	if(m_fCount>COUNT_MAX)
	{
		//終了処理
		Uninit();
	}
}
//=============================================================================
//描画
//=============================================================================
void CEffect::Draw()
{
	//表示フラグtrueのみ描画処理をする
	if(m_bDisp==true)
	{
		//レンダラー情報取得
		CRenderer *pRenderer=CManager::GetRenderer();

		//デバイスのゲット
		LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

		//カメラのゲット
		CCamera *pCamera=CManager::GetCamera();

		//テクスチャの変更
		ChangeTexture();

		//マトリックスの各変数
		D3DXMATRIX mtxScl,mtxRot,mtxTranslate,mtxView;

		//αブレンド処理
		pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//カメラ情報取得
		mtxView=pCamera->GetMtxView();

		D3DXMatrixIdentity(&m_mtxWorld);
		D3DXMatrixInverse(&m_mtxWorld,NULL,&mtxView);

		m_mtxWorld._41=0.0f;
		m_mtxWorld._42=0.0f;
		m_mtxWorld._43=0.0f;

		//サイズの設定
		D3DXMatrixScaling(&mtxScl,1.0f,1.0f,1.0f);

		//サイズのセット
		D3DXMatrixMultiply(&m_mtxWorld,
							&m_mtxWorld,
							&mtxScl);

		//回転を設定
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
										0,
										0,
										0);

		//回転を反映
		D3DXMatrixMultiply(&m_mtxWorld,&m_mtxWorld,
							&mtxRot);

		//位置を反映
		D3DXMatrixTranslation(&mtxTranslate,
							m_pos.x,
							m_pos.y,
							m_pos.z);

		//位置をセット
		D3DXMatrixMultiply(&m_mtxWorld,&m_mtxWorld,
							&mtxTranslate);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD,
								&m_mtxWorld);


		//３Ｄポリゴンの描画
		pDevice->SetStreamSource(0,m_pD3DVtxBuffBill, 0, sizeof(VERTEX_3D));

		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0,m_pD3DTextureBill);

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
										0,//ポリゴンの数
										2);
	}
}
//=============================================================================
//テクスチャ座標の変更
//=============================================================================
void CEffect::ChangeTexture()
{
	//頂点バッファのポインタ
	VERTEX_3D *pVtx;

	//ビルボードテクスチャ座標の変更
	m_pD3DVtxBuffBill->Lock(0,0,(void**)&pVtx,0);

	//テクスチャ座標をカウントの経過によって変更させる
	pVtx[0].tex=D3DXVECTOR2(0.0f+(TEX_MOVE_X*((int)(m_fCount/COUNT_WAIT)%TEX_NUM)),TEX_MOVE_Y+(TEX_MOVE_Y*((int)(m_fCount/COUNT_WAIT)/TEX_NUM)));
	pVtx[1].tex=D3DXVECTOR2(0.0f+(TEX_MOVE_X*((int)(m_fCount/COUNT_WAIT)%TEX_NUM)),0.0f+(TEX_MOVE_Y*((int)(m_fCount/COUNT_WAIT)/TEX_NUM)));
	pVtx[2].tex=D3DXVECTOR2(TEX_MOVE_X+(TEX_MOVE_X*((int)(m_fCount/COUNT_WAIT)%TEX_NUM)),TEX_MOVE_Y+(TEX_MOVE_Y*((int)(m_fCount/COUNT_WAIT)/TEX_NUM)));
	pVtx[3].tex=D3DXVECTOR2(TEX_MOVE_X+(TEX_MOVE_X*((int)(m_fCount/COUNT_WAIT)%TEX_NUM)),0.0f+(TEX_MOVE_Y*((int)(m_fCount/COUNT_WAIT)/TEX_NUM)));

	m_pD3DVtxBuffBill->Unlock();
}
//=============================================================================
//エフェクトインスタンス生成
//=============================================================================
void CEffect::Create(D3DXVECTOR3 pos)
{
	//エフェクトポインタ
	CEffect *pEffect;

	//インスタンス生成
	pEffect=new CEffect();

	//初期化
	pEffect->Init(pos);
}
//EOF