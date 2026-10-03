//=============================================================================
//エネミー処理[CEnemy.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CEnemy.h"
#include "CShadow.h"
#include "CSceneX.h"
#include "CMeshField.h"
#include "CObject.h"
#include "Ccamera.h"
#include "CGame.h"
#include "renderer.h"
#include "manager.h"
#include "CInputKeyboard.h"
#include "HitCheck.h"
#include "CToonShader.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define ENEMY_SIZE (90.0f)			//敵のサイズ
#define ENEMY_GRAVITY (4.0f)		//敵の重力
#define ENEMY_RECT_VERTEX_MAX (4)	//エネミー短形の頂点座標数

//=============================================================================
//コンストラクタ
//=============================================================================
CEnemy::CEnemy()
{
	//エネミーオブジェクト代入
	m_objType=OBJECT_TYPE_ENEMY;
}
//=============================================================================
//デストラクタ
//=============================================================================
CEnemy::~CEnemy()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CEnemy::Init(ENEMY_TYPE type,ENEMY_LEVEL level,D3DXVECTOR3 pos)
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	m_pos=pos;
	m_rot=D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_scl=D3DXVECTOR3(1.0f,1.0f,1.0f);
	m_bDisp=false;
	m_fSizeX=ENEMY_SIZE;
	m_fSizeY=ENEMY_SIZE;
	m_fSizeZ=ENEMY_SIZE;

	//敵のレベルによって移動量を変更
	switch(level)
	{
		//ベリーイージー
		case ENEMY_VERYEASY:
			m_posMove=D3DXVECTOR3(3.0f,3.0f,3.0f);
		break;
		//イージー
		case ENEMY_EASY:
			m_posMove=D3DXVECTOR3(5.0f,3.0f,5.0f);
		break;
		//ノーマル
		case ENEMY_NORMAL:
			m_posMove=D3DXVECTOR3(9.0f,5.0f,9.0f);
		break;
		//ハード
		case ENEMY_HARD:
			m_posMove=D3DXVECTOR3(14.0f,8.8f,15.0f);
		break;
		//ベリーハード
		case ENEMY_VERYHARD:
			m_posMove=D3DXVECTOR3(20.0f,10.0f,20.0f);
		break;
	}

	//Xファイルのロード
	if(FAILED(D3DXLoadMeshFromX("data/MODEL/enemy00.x",
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

	//影インスタンス生成
	m_pShadow=CShadow::Create(m_fSizeX,m_fSizeZ);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CEnemy::Uninit()
{
	//影インスタンス終了
	m_pShadow->Uninit();

	//自身の終了
	CSceneX::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CEnemy::Update()
{
	//敵の移動
	m_pos.x+=m_posMove.x;
	m_pos.y-=ENEMY_GRAVITY;
	m_pos.z+=m_posMove.z;

	///////////////////////////////////////////////
	//		各オブジェクトの当たり判定処理		//
	/////////////////////////////////////////////

	//シーンXの先頭からポインタを取得
	CScene *scene=CScene::GetListTop(PRIORITY_SCENEX);

	//シーンが終端になるまでループ
	while(scene)
	{
		//次ポインタ取得
		CScene *pNext=scene->GetNextScene();

		//シーンのタイプで当たり判定処理を分ける
		switch(scene->GetType())
		{
			///////////////////////////////////////////////
			//		その他オブジェクトの当たり判定		//
			/////////////////////////////////////////////
			case OBJECT_TYPE_OBJECT:
			{
				//シーンをオブジェクトインスタンスにキャスト変換
				CObject *pObject=(CObject*)scene;

				//オブジェクトのサイズを取得
				float fSizeX,fSizeY,fSizeZ;
				pObject->GetSize(&fSizeX,&fSizeY,&fSizeZ);

				//オブジェクトの座標を取得
				D3DXVECTOR3 objPos=pObject->GetPos();

				//オブジェクトの頂点座標設定
				D3DXVECTOR3 objVec[OBJECT_RECT_VERTEX_MAX];
				objVec[0]=D3DXVECTOR3(objPos.x-fSizeX,0.0f,objPos.z-fSizeZ);
				objVec[1]=D3DXVECTOR3(objPos.x-fSizeX,0.0f,objPos.z+fSizeZ);
				objVec[2]=D3DXVECTOR3(objPos.x+fSizeX,0.0f,objPos.z+fSizeZ);
				objVec[3]=D3DXVECTOR3(objPos.x+fSizeX,0.0f,objPos.z-fSizeZ);

				//オブジェクトとの外積当たり判定
				//敵の短形の頂点数分ループ
				for(int j=0;j<ENEMY_RECT_VERTEX_MAX;j++)
				{
					//外積当たり判定回数
					int nCrossCnt=0;

					//敵の横,奥の幅含めた(短形)各頂点座標を求める
					D3DXVECTOR3 enemyVertexPos=D3DXVECTOR3(	m_pos.x+(-m_fSizeX+((m_fSizeX*2)*(j/2))),
															0.0f,
															m_pos.z+(-m_fSizeZ+((m_fSizeZ*2)*(j%2)))
														);

					//カメラの頂点数分ループ
					for(int i=0;i<OBJECT_RECT_VERTEX_MAX;i++)
					{
						//オブジェクトの頂点座標の方向ベクトル
						D3DXVECTOR3 vec1=objVec[(i+1)%OBJECT_RECT_VERTEX_MAX]-objVec[i];
						//オブジェクトの頂点座標から敵への方向ベクトル
						D3DXVECTOR3 vec2=enemyVertexPos-objVec[i];

						//敵頂点とオブジェクトの外積当たり判定
						if(vec1.x*vec2.z-vec1.z*vec2.x<0.0f)
						{
							//当った場合はカウント加算
							nCrossCnt++;
						}
						else
						{
							//当ってない場合はループから抜ける
							break;
						}
					}//for OBJECT_RECT_VERTEX_MAX

					//敵の１つの頂点がオブジェクトの範囲内にいる場合
					if(nCrossCnt>=OBJECT_RECT_VERTEX_MAX)
					{
						///////////////////////////////////////////
						//		めり込んだ分座標を引き戻す		//
						/////////////////////////////////////////
						D3DXVECTOR3 vec1=objPos-m_posOld;	//前座標とオブジェクトの方向ベクトル
						D3DXVECTOR3 vec2=objPos-m_pos;		//現在とオブジェクトのの方向ベクトル
						D3DXVECTOR3 distVec=vec1-vec2;		//２つの方向ベクトルの差分

						//外積当たり判定回数リセット
						nCrossCnt=0;

						//当ってる頂点のX座標を前座標との差分引いた値にする
						enemyVertexPos.x=enemyVertexPos.x-distVec.x;

						//オブジェクトの頂点数分ループ
						for(int i=0;i<OBJECT_RECT_VERTEX_MAX;i++)
						{
							//オブジェクトの頂点座標の方向ベクトル
							D3DXVECTOR3 vec1=objVec[(i+1)%OBJECT_RECT_VERTEX_MAX]-objVec[i];
							//オブジェクトの頂点座標から敵への方向ベクトル
							D3DXVECTOR3 vec2=enemyVertexPos-objVec[i];

							//敵頂点とオブジェクトの外積当たり判定
							if(vec1.x*vec2.z-vec1.z*vec2.x<0.0f)
							{
								//当った場合はカウント加算
								nCrossCnt++;
							}
							else
							{
								//当ってない場合はループから抜ける
								break;
							}

						}//for OBJECT_RECT_VERTEX_MAX

						//引き戻した頂点座標が範囲内にある場合
						if(nCrossCnt>=OBJECT_RECT_VERTEX_MAX)
						{
							//エネミーのZ座標を差分だけ戻す
							m_pos.z-=distVec.z;
							//Z座標の移動量を反転させる
							m_posMove.z=-m_posMove.z;
						}
						else
						{
							//X座標差し引いた結果をエネミーの座標へ代入
							m_pos.x-=distVec.x;
							//X座標の移動量を反転させる
							m_posMove.x=-m_posMove.x;
						}

						//引き戻したことで他の頂点を調べる必要がないためループから抜ける
						break;

					}//if 頂点当たり判定チェック

				}//for ENEMY_RECT_VERTEX_MAX

				break;
			}//case OBJECT_TYPE_OBJECT

		}//type種別 switch 終端

		//次のシーンポインタへ切替
		scene=pNext;

	}//scene whileループ 終端

	///////////////////////////////////////////////
	//		フィールド範囲外(壁)当たり判定		//
	/////////////////////////////////////////////

	//フィールド横幅範囲外の場合
	if(	m_pos.x>(FIELD_NUM_X/2)*FIELDSIZE_X ||
		m_pos.x<-((FIELD_NUM_X/2)*FIELDSIZE_X))
	{
		//プレイヤー座標を元に戻す
		m_pos.x=m_posOld.x;
		//X座標の移動量を反転させる
		m_posMove.x=-m_posMove.x;
	}

	//フィールド奥行範囲外の場合
	if(	m_pos.z>(FIELD_NUM_Z/2)*FIELDSIZE_Z ||
		m_pos.z<-((FIELD_NUM_Z/2)*FIELDSIZE_Z))
	{
		//プレイヤー座標を元に戻す
		m_pos.z=m_posOld.z;
		//Z座標の移動量を反転させる
		m_posMove.z=-m_posMove.z;
	}

	///////////////////////////////////////////////////////////
	//		モデルをフィールドの高さに合わせてY座標取得		//
	/////////////////////////////////////////////////////////

	//フィールドインスタンスを取得
	CMeshField *pMeshField=CGame::GetField();

	//プレイヤーのY座標がフィールドの高さより低い場合
	if(m_pos.y<pMeshField->GetHeight(m_pos))
	{
		//フィールドの高さに合わせる
		m_pos.y=pMeshField->GetHeight(m_pos);
	}

	///////////////////////////////////////////////////
	//		カメラの範囲内との外積当たり判定		//
	/////////////////////////////////////////////////

	//カメラの取得
	CCamera *pCamera=CManager::GetCamera();

	//カメラ範囲の頂点座標
	D3DXVECTOR3 cameraVertexPos[CAMERA_RECT_VERTEX_MAX];

	//カメラベクトルの内側である回数
	int nCrossCnt=0;

	//4頂点数分ループ
	for(int i=0;i<CAMERA_RECT_VERTEX_MAX;i++)
	{
		//カメラ範囲の頂点座標取得
		pCamera->GetVertexPos(&cameraVertexPos[i],i);
	}

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

			//影非表示
			m_pShadow->SetDisp(false);
			break;
		}
	}

	//カメラ範囲内に弾の座標がある場合
	if(nCrossCnt>=CAMERA_RECT_VERTEX_MAX)
	{
		//表示フラグtrue
		m_bDisp=true;
		//影を表示させる
		m_pShadow->SetDisp(true);
	}

	//前座標を保存
	m_posOld=m_pos;

	//影に座標をセット
	m_pShadow->SetPos(m_pos);
}
//=============================================================================
//描画
//=============================================================================
void CEnemy::Draw()
{
	//表示フラグtrueの場合のみ描画処理をする
	if(m_bDisp==true)
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
		D3DXMatrixScaling(	&mtxScl,
							m_scl.x,
							m_scl.y,
							m_scl.z);

		//サイズを反映
		D3DXMatrixMultiply(	&m_mtxWorld,
							&m_mtxWorld,
							&mtxScl);
		//回転を設定
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
										m_rot.y,
										m_rot.x,
										m_rot.z);

		//回転を反映
		D3DXMatrixMultiply(	&m_mtxWorld,&m_mtxWorld,
							&mtxRot);

		//位置を反映
		D3DXMatrixTranslation(	&mtxTranslate,
								m_pos.x,
								m_pos.y,
								m_pos.z);

		//ワールドマトリックスの設定
		D3DXMatrixMultiply(&m_mtxWorld,&m_mtxWorld,
					 &mtxTranslate);

		//マテリアルの取得
		pDevice->GetMaterial(&matDef);

		//バッファポインタの取得
		pD3DXMat=(D3DXMATERIAL*)m_pD3DXBuffMatModel->GetBufferPointer();

		//ワールドマトリクスのセット
		pDevice->SetTransform(	D3DTS_WORLD,
								&m_mtxWorld);

		//トゥーンシェーダー開始
		CToonShader::Begin();

		//マテリアル数分描画
		for(int nCntMat=0;nCntMat<(int)m_nNumMatModel;nCntMat++)
		{
			pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);
			m_pD3DXMeshModel->DrawSubset(nCntMat);
		}

		//トゥーンシェーダー終了
		CToonShader::End();

		//マテリアルセット
		pDevice->SetMaterial(&matDef);
	}
}
//=============================================================================
//サイズの取得
//=============================================================================
void CEnemy::GetSize(float *fSizeX,float *fSizeY,float *fSizeZ)
{
	*fSizeX=m_fSizeX;
	*fSizeY=m_fSizeY;
	*fSizeZ=m_fSizeZ;
}
//=============================================================================
//敵インスタンス生成
//=============================================================================
void CEnemy::Create(ENEMY_TYPE type,ENEMY_LEVEL level,D3DXVECTOR3 pos)
{
	//敵インスタンス
	CEnemy *pEnemy;

	//敵インスタンス生成
	pEnemy=new CEnemy();

	//敵インスタンス初期化
	pEnemy->Init(type,level,pos);
}
//EOF