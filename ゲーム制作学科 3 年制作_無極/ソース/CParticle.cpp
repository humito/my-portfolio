//=============================================================================
//パーティクル処理[CParticle.cpp]
//Author:HUMITO KIMURA
//=============================================================================

//*****************************************************************************
//定数定義
//*****************************************************************************
#define PARTICLE_DISP_COUNT_MAX (150)				//パーティクルが表示される間のカウント数
#define PARTICLE_FLASH (PARTICLE_DISP_COUNT_MAX-50)	//パーティクルが点滅開始する時間
#define PARTICLE_COUNT_MAX (50)						//パーティクルの移動できる間のカウント数
#define ITEM_VELOCITY_MAX (20)						//アイテム時の速度上限
#define FIREWORKS_VELOCITY (0.25f)					//花火時の速度上限
#define PARTICLE_GRAVITY (0.5f)						//パーティクルの重力
#define PARTICLE_VERTEX_MAX (4)						//パーティクルの頂点数
#define REVOLUTION (D3DX_PI*2)						//1回転の角度(360°)
#define PARTICLE_REVOLUTION_NUM (8)					//１週分の個数
#define COUNT_DELAY (4)								//カウント遅延
#define ALPHA_PATTERN (2)							//パターン数
#define RGBA_NUM_MAX (255)							//RGBAの値の最大値

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CParticle.h"
#include "renderer.h"
#include "manager.h"
#include "Ccamera.h"
#include "CGame.h"
#include "CScore.h"
#include "CMeshField.h"
#include <stdlib.h>
#include <time.h>

//=============================================================================
//コンストラクタ
//=============================================================================
CParticle::CParticle()
{
	//パーティクルオブジェクトとする
	m_objType=OBJECT_TYPE_PARTICLE;
	//パーティクルポインタNULLセット
	m_pParticle=NULL;
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
HRESULT CParticle::Init(PARTICLE_TYPE type,int nNumParticle,D3DXVECTOR3 pos,float fWidth,float fHeight)
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//乱数の初期化
	srand((unsigned int) time(NULL));

	//パーティクルの発生位置の保存
	m_startPos=pos;

	//表示カウント初期化
	m_nDispCnt=0;

	//パーティクルの種類代入
	m_type=type;

	//パーティクル数が0以下なら生成する意味がないので
	if(nNumParticle<=0)
	{
		//終了処理をしてE_FAILを返す
		Uninit();
		return E_FAIL;
	}

	//パーティクル数の設定
	m_nNumParticle=nNumParticle;

	//タイプが花火の場合のみ
	if(type==TYPE_FIREWORKS)
	{
		//パーティクル数分スコアを加算
		CScore::AddScore(m_nNumParticle);
	}

	//パーティクルのポインタを確保
	m_pParticle=new PARTICLE[m_nNumParticle];

	//パーティクルの一定の角度(花火用で使用)
	float fRot=REVOLUTION/16;

	//各パーティクル情報の初期化
	for(int i=0;i<m_nNumParticle;i++)
	{
		//座標初期化
		m_pParticle[i].pos=pos;

		//パーティクルの高さ設定
		m_pParticle[i].fHeight=fHeight;

		//パーティクルの種類によって初期化する速度は異なる
		//アイテム用の速度
		if(m_type==TYPE_ITEM)
		{
			m_pParticle[i].velocity=D3DXVECTOR3(cosf((float)rand())*(rand()%ITEM_VELOCITY_MAX),
												(float)(rand()%ITEM_VELOCITY_MAX),
												sinf((float)rand())*(rand()%ITEM_VELOCITY_MAX));
		}//花火用の速度
		else if(m_type==TYPE_FIREWORKS)
		{
			//各パーティクルの速度を設定
			int nSizeY=(int)(sinf(fRot*(i/PARTICLE_REVOLUTION_NUM))*(FIREWORKS_VELOCITY+FIREWORKS_VELOCITY*i/PARTICLE_REVOLUTION_NUM));
			int nLength=(int)(cosf(fRot*(i/PARTICLE_REVOLUTION_NUM))*(FIREWORKS_VELOCITY+FIREWORKS_VELOCITY*i/PARTICLE_REVOLUTION_NUM));
			int nSizeX=(int)(cosf(fRot*i)*nLength);
			int nSizeZ=(int)(sinf(fRot*i)*nLength);

			//粒をそれぞれ360°全方向へ移動するようにする
			m_pParticle[i].velocity=D3DXVECTOR3((float)nSizeX,(float)nSizeY,(float)nSizeZ);
		}

		//カウント初期化
		m_pParticle[i].nCnt=0;

		//使用フラグ初期化
		m_pParticle[i].bUse=true;

		//表示フラグ初期化
		m_pParticle[i].bDisp=false;

		//着地フラグ初期化
		m_pParticle[i].bLand=false;
	}

	///////////////////////////////////
	//		頂点バッファの生成		//
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
	//頂点バッファ設定開始
	m_pD3DVtxBuffBill->Lock(0,0,(void**)&pVtx,0);

	//頂点座標
	pVtx[0].vtx=D3DXVECTOR3(-(fWidth/2),-(fHeight/2),0.0f);
	pVtx[1].vtx=D3DXVECTOR3(-(fWidth/2),(fHeight/2),0.0f);
	pVtx[2].vtx=D3DXVECTOR3((fWidth/2),-(fHeight/2),0.0f);
	pVtx[3].vtx=D3DXVECTOR3((fWidth/2),(fHeight/2),0.0f);

	//法線ベクトル
	pVtx[0].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);
	pVtx[1].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);
	pVtx[2].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);
	pVtx[3].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);

	//乱数から光源を設定
	for(int i=0;i<PARTICLE_VERTEX_MAX;i++)
	{
		//光源を変更するときに変数として保存
		m_color[i].r=(float)(rand()%RGBA_NUM_MAX);
		m_color[i].g=(float)(rand()%RGBA_NUM_MAX);
		m_color[i].b=(float)(rand()%RGBA_NUM_MAX);
		m_color[i].a=RGBA_NUM_MAX;

		//光源の設定
		pVtx[i].diffuse=D3DCOLOR_RGBA((int)m_color[i].r,(int)m_color[i].g,(int)m_color[i].b,(int)m_color[i].a);
	}

	//テクスチャ座標
	pVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	pVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	pVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	//頂点バッファ設定終了
	m_pD3DVtxBuffBill->Unlock();

	///////////////////////////////////////////
	//		インデックスバッファの生成		//
	/////////////////////////////////////////

	//インデックスバッファの生成
	if(FAILED(	pDevice->CreateIndexBuffer
				(sizeof(WORD)*4,
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&m_pD3DIndexBuffBill,
				NULL)))
	{
		return E_FAIL;
	}

	//インデックスのポインタ
	WORD *pIndex;

	//インデックスバッファの設定開始
	m_pD3DIndexBuffBill->Lock(0,0,(void**)&pIndex,0);

	pIndex[0]=0;
	pIndex[1]=1;
	pIndex[2]=2;
	pIndex[3]=3;

	//インデックスバッファの設定終了
	m_pD3DIndexBuffBill->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								"data/TEXTURE/effect000.png",
								&m_pD3DTextureBill);

	return S_OK;
}

//=============================================================================
//終了
//=============================================================================
void CParticle::Uninit()
{
	//パーティクルNULLチェック
	if(m_pParticle!=NULL)
	{
		//パーティクル情報の解放
		delete[] m_pParticle;
		//NULLセット
		m_pParticle=NULL;
	}

	//自身の終了
	CSceneBillboard::Uninit();
}

//=============================================================================
//更新
//=============================================================================
void CParticle::Update()
{
	//マネージャーからカメラインスタンスを取得
	CCamera *pCamera=CManager::GetCamera();

	//カメラ範囲の頂点座標
	D3DXVECTOR3 cameraVertexPos[CAMERA_RECT_VERTEX_MAX];

	//4頂点数分ループ
	for(int i=0;i<CAMERA_RECT_VERTEX_MAX;i++)
	{
		//カメラ範囲の頂点座標取得
		pCamera->GetVertexPos(&cameraVertexPos[i],i);
	}

	//パーティクルの種類で処理を分ける
	switch(m_type)
	{
		///////////////////////////
		//		アイテム用		//
		/////////////////////////
		case TYPE_ITEM:
		{
			//表示カウントを超える前のカウントのなら点滅させる
			if(m_nDispCnt>PARTICLE_FLASH)
			{
				//頂点情報の変更
				ChangeBuffer();
			}

			//表示カウントが最大値に達したら
			if(m_nDispCnt>PARTICLE_DISP_COUNT_MAX)
			{
				//終了処理をする
				Uninit();
			}
			else
			{
				//パーティクル数分更新
				for(int i=0;i<m_nNumParticle;i++)
				{
					//座標を速度分加算
					m_pParticle[i].pos+=m_pParticle[i].velocity;
					m_pParticle[i].velocity.y-=PARTICLE_GRAVITY;

					///////////////////////////////////////////////////////////
					//		モデルをフィールドの高さに合わせてY座標取得		//
					/////////////////////////////////////////////////////////

					//フィールドインスタンスを取得
					CMeshField *pMeshField=CGame::GetField();
					//高さを取得
					float fHeight=pMeshField->GetHeight(m_pParticle[i].pos);

					//プレイヤーのY座標がフィールドの高さより下ならフィールドの高さに合わせる
					if(m_pParticle[i].pos.y-m_pParticle[i].fHeight<fHeight)
					{
						//フィールドの高さに合わせる
						m_pParticle[i].pos.y=fHeight+m_pParticle[i].fHeight;
					}

					//各パーティクルのカウントアップ
					m_pParticle[i].nCnt++;

					//各パーティクルのカウントが一定に達した場合
					if(m_pParticle[i].nCnt>PARTICLE_COUNT_MAX)
					{
						//着地フラグtrue
						m_pParticle[i].bLand=true;
					}

				}//パーティクル数forループ終端
			}//else 終端

			break;
		}//case 終端

		///////////////////////
		//		花火用		//
		/////////////////////
		case TYPE_FIREWORKS:
		{
			//表示カウントが最大値に達したら
			if(m_nDispCnt>PARTICLE_DISP_COUNT_MAX)
			{
				//終了処理をする
				Uninit();
			}
			else
			{
				//パーティクル数分更新
				for(int i=0;i<m_nNumParticle;i++)
				{
					//座標を速度分加算
					m_pParticle[i].pos+=m_pParticle[i].velocity;
					m_pParticle[i].velocity.y-=PARTICLE_GRAVITY;
				}
			}
			break;
		}//case 終端
	}//switch 終端

	///////////////////////////////////////////////
	//		カメラ描画範囲との当たり判定		//
	/////////////////////////////////////////////

	//パーティクルNULLチェック
	if(m_pParticle!=NULL)
	{
		//パーティクル数分ループ
		for(int i=0;i<m_nNumParticle;i++)
		{
			//カメラ頂点の各方向ベクトルの内側である回数(0～4回)
			int nCrossCnt=0;

			//カメラ範囲4頂点数分ループ
			for(int j=0;j<CAMERA_RECT_VERTEX_MAX;j++)
			{
				//カメラ範囲の頂点座標の方向ベクトル
				D3DXVECTOR3 vec1=cameraVertexPos[(j+1)%CAMERA_RECT_VERTEX_MAX]-cameraVertexPos[j];
				//カメラ範囲の頂点座標からオブジェクトへの方向ベクトル
				D3DXVECTOR3 vec2=m_pParticle[i].pos-cameraVertexPos[j];

				//各パーティクルの座標がカメラ頂点の方向ベクトルの内側なら
				if(vec1.x*vec2.z-vec1.z*vec2.x<0.0f)
				{
					//カウントアップ
					nCrossCnt++;
				}
				else
				{
					//外側なら表示フラグfalseにしループから抜ける
					m_pParticle[i].bDisp=false;
					break;
				}
			}//カメラ描画範囲4頂点ループ終端

			//カメラ範囲内にオブジェクトの座標がある場合
			if(nCrossCnt>=CAMERA_RECT_VERTEX_MAX)
			{
				//表示フラグtrueにする
				m_pParticle[i].bDisp=true;
			}

		}//パーティクル数forループ終端
	}//パーティクルNULLチェックif 終端

	//表示カウントアップ
	m_nDispCnt++;
}

//=============================================================================
//描画
//=============================================================================
void CParticle::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//パーティクル数分ループ
	for(int i=0;i<m_nNumParticle;i++)
	{
		//使用・表示フラグtrueのみ描画処理をする
		if(m_pParticle[i].bUse==true && m_pParticle[i].bDisp==true)
		{
			//カメラのゲット
			CCamera *pCamera=CManager::GetCamera();

			//加算合成
			pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
			pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

			//アルファテスト
			pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

			//不透明にする値の設定
			pDevice->SetRenderState( D3DRS_ALPHAREF, 0x86 );

			//マトリックス変換用変数
			D3DXMATRIX mtxScl,mtxRot,mtxTranslate,mtxView;

			//カメラ情報取得
			mtxView=pCamera->GetMtxView();

			//ワールドマトリックスの初期化
			D3DXMatrixIdentity(&m_mtxWorld);
			D3DXMatrixInverse(&m_mtxWorld,NULL,&mtxView);

			m_mtxWorld._41=0.0f;
			m_mtxWorld._42=0.0f;
			m_mtxWorld._43=0.0f;

			//スケールを設定
			D3DXMatrixScaling(&mtxScl,1.0f,1.0f,1.0f);

			//スケールを反映
			D3DXMatrixMultiply(	&m_mtxWorld,
								&m_mtxWorld,
								&mtxScl);

			//回転を設定
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
											0,
											0,
											0);

			//回転を反映
			D3DXMatrixMultiply(	&m_mtxWorld,&m_mtxWorld,
								&mtxRot);

			//位置を設定
			D3DXMatrixTranslation(	&mtxTranslate,
									m_pParticle[i].pos.x,
									m_pParticle[i].pos.y,
									m_pParticle[i].pos.z);

			//位置を反映
			D3DXMatrixMultiply(	&m_mtxWorld,&m_mtxWorld,
								&mtxTranslate);

			//ワールドマトリックスの設定
			pDevice->SetTransform(	D3DTS_WORLD,
									&m_mtxWorld);

			//頂点バッファのバインド
			pDevice->SetStreamSource(0,m_pD3DVtxBuffBill, 0, sizeof(VERTEX_3D));

			//インデックスをバインド
			pDevice->SetIndices(m_pD3DIndexBuffBill);

			//頂点フォーマットのセット
			pDevice->SetFVF(FVF_VERTEX_3D);

			//テクスチャの設定
			pDevice->SetTexture(0,m_pD3DTextureBill);

			//ポリゴンの描画(インデックス)
			pDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLESTRIP,
											0,
											0,
											4,
											0,
											2);
		}
	}

	//元に戻す
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
}
//=============================================================================
//頂点バッファ情報の変更
//=============================================================================
void CParticle::ChangeBuffer()
{
	//頂点バッファのポインタ
	VERTEX_3D *pVtx;

	//頂点バッファ設定開始
	m_pD3DVtxBuffBill->Lock(0,0,(void**)&pVtx,0);

	for(int i=0;i<PARTICLE_VERTEX_MAX;i++)
	{
		//α値をカウントから設定
		m_color[i].a=(float)(RGBA_NUM_MAX*((m_nDispCnt/COUNT_DELAY)%ALPHA_PATTERN));

		//光源の設定
		pVtx[i].diffuse=D3DCOLOR_RGBA((int)m_color[i].r,(int)m_color[i].g,(int)m_color[i].b,(int)m_color[i].a);
	}
	//頂点バッファ設定終了
	m_pD3DVtxBuffBill->Unlock();
}
//=============================================================================
//パーティクルのセット
//=============================================================================
void CParticle::SetParticle(D3DXVECTOR3 pos,int nIndex)
{
	//座標を発進地へセット
	m_pParticle[nIndex].pos=m_startPos;

	//乱数により速度を変更
	m_pParticle[nIndex].velocity=D3DXVECTOR3(cosf((float)rand())*(rand()%ITEM_VELOCITY_MAX),
											(float)(rand()%ITEM_VELOCITY_MAX),
											sinf((float)rand())*(rand()%ITEM_VELOCITY_MAX));

}
//=============================================================================
//速度のセット
//=============================================================================
void CParticle::SetVelocity(int nIndex,D3DXVECTOR3 velocity)
{
	m_pParticle[nIndex].velocity=velocity;
}
//=============================================================================
//使用フラグのセット
//=============================================================================
void CParticle::SetUseFlag(int nIndex,bool bFlag)
{
	m_pParticle[nIndex].bUse=bFlag;
}
//=============================================================================
//使用フラグの取得
//=============================================================================
bool CParticle::GetUseFlag(int nIndex)
{
	return m_pParticle[nIndex].bUse;
}
//=============================================================================
//パーティクル数取得
//=============================================================================
int CParticle::GetNum()
{
	return m_nNumParticle;
}
//=============================================================================
//着地フラグ取得
//=============================================================================
bool CParticle::CheckLand(int nIndex)
{
	return m_pParticle[nIndex].bLand;
}
//=============================================================================
//各パーティクルの座標取得
//=============================================================================
D3DXVECTOR3 CParticle::GetParticlePos(int nIndex)
{
	return m_pParticle[nIndex].pos;
}
//=============================================================================
//タイプの取得
//=============================================================================
PARTICLE_TYPE CParticle::GetType()
{
	return m_type;
}
//=============================================================================
//インスタンス生成
//=============================================================================
void CParticle::Create(PARTICLE_TYPE type,int nNumParticle,D3DXVECTOR3 pos,float fWidth,float fHeight)
{
	//パーティクルインスタンス生成
	CParticle *pParticle=new CParticle();

	//初期化
	pParticle->Init(type,nNumParticle,pos,fWidth,fHeight);
}
//EOF