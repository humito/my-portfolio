//=============================================================================
//弾処理[CBullet.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CBullet.h"
#include "renderer.h"
#include "manager.h"
#include "CGame.h"
#include "Ccamera.h"
#include "CEffect.h"
#include "CEnemy.h"
#include "CScore.h"
#include "CSound.h"
#include "CFace.h"
#include "CLeftoverBullet.h"
#include "CParticle.h"
#include "CMeshField.h"
#include "HitCheck.h"
#include "CToonShader.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define BULLET_SIZE (45.0f)	//弾のサイズ

//=============================================================================
//コンストラクタ
//=============================================================================
CBullet::CBullet()
{
	//弾オブジェクト代入
	m_objType=OBJECT_TYPE_BULLET;
}
//=============================================================================
//デストラクタ
//=============================================================================
CBullet::~CBullet()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CBullet::Init(D3DXVECTOR3 pos,D3DXVECTOR3 rot,D3DXVECTOR3 velocity)
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//ポリゴンの設定
	m_pos=pos;							//座標
	m_velocity=velocity;				//速度
	m_rot=rot;							//角度
	m_bDisp=true;						//表示フラグ
	m_scl=D3DXVECTOR3(1.0f,1.0f,1.0f);	//スケール

	//Xファイルのロード
	if(FAILED(D3DXLoadMeshFromX("data/MODEL/kunai.x",
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

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CBullet::Uninit()
{
	//ビルボードの終了
	CSceneX::Uninit();

	//残弾数を増やす
	CLeftoverBullet::AddNum();
}
//=============================================================================
//更新
//=============================================================================
void CBullet::Update()
{
	///////////////////////////////////
	//			弾の移動			//
	/////////////////////////////////

	m_pos.x+=cosf(m_rot.y+D3DX_PI/2)*m_velocity.x;//X
	m_pos.z-=sinf(m_rot.y+D3DX_PI/2)*m_velocity.z;//Z

	//モデルZ軸回転
	m_rot.z++;

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
			///////////////////////////////////
			//		敵との当たり判定		//
			/////////////////////////////////
			case OBJECT_TYPE_ENEMY:
			{
				//敵インスタンスへキャスト変換
				CEnemy *pEnemy=(CEnemy*)scene;

				//敵のサイズを取得
				float fSizeX,fSizeY,fSizeZ;
				pEnemy->GetSize(&fSizeX,&fSizeY,&fSizeZ);

				//敵の座標を取得
				D3DXVECTOR3 enemyPos=pEnemy->GetPos();

				//弾のY座標が敵の高さの間にある場合
				if(	m_pos.y>enemyPos.y &&
					m_pos.y<enemyPos.y+fSizeY)
				{
					//弾の座標が敵の範囲内にある場合
					if(CubeCheck(enemyPos,m_pos,fSizeX,0.0f,fSizeZ,BULLET_SIZE,0.0f,BULLET_SIZE))
					{
						//弾の終了処理
						Uninit();

						//敵の終了処理
						pEnemy->Uninit();

						//パーティクルの発生
						CParticle::Create(TYPE_ITEM,ITEM_NUM,m_pos,ITEM_SIZE,ITEM_SIZE);

						//スコア加算
						CScore::AddScore(ADD_SCORE_NUM);

						//驚いた表情をセット
						CFace::SetFace(SURPRISE_FACE);

						//爆発音再生
						CSound::PlaySoundA(SOUND_LABEL_SE_EXPLOSION);
					}
				}

				break;
			}

		}//sceneType種別 switch 終端

		//次ポインタへ移動
		scene=pNext;

	}//sceneNULLチェック whileループ 終端

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
			break;
		}
	}

	//カメラ範囲内に弾の座標がある場合
	if(nCrossCnt>=CAMERA_RECT_VERTEX_MAX)
	{
		//表示フラグtrue
		m_bDisp=true;
	}

	///////////////////////////////////////////////
	//		フィールド範囲外(壁)当たり判定		//
	/////////////////////////////////////////////

	//フィールド横幅範囲外の場合
	if(	m_pos.x>(FIELD_NUM_X/2)*FIELDSIZE_X ||
		m_pos.x<-((FIELD_NUM_X/2)*FIELDSIZE_X))
	{
		//エフェクトインスタンス生成
		CEffect::Create(m_pos);
		//終了処理
		Uninit();
	}

	//フィールド奥行範囲外の場合
	if(	m_pos.z>(FIELD_NUM_Z/2)*FIELDSIZE_Z ||
		m_pos.z<-((FIELD_NUM_Z/2)*FIELDSIZE_Z))
	{
		//エフェクトインスタンス生成
		CEffect::Create(m_pos);
		//終了処理
		Uninit();
	}
}
//=============================================================================
//描画
//=============================================================================
void CBullet::Draw()
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

		//スケールを設定
		D3DXMatrixScaling(	&mtxScl,
							m_scl.x,
							m_scl.y,
							m_scl.z);

		//スケールを反映
		D3DXMatrixMultiply(	&m_mtxWorld,
							&m_mtxWorld,
							&mtxScl);
		//回転を設定
		D3DXMatrixRotationYawPitchRoll(	&mtxRot,
										m_rot.y,
										m_rot.x,
										m_rot.z);

		//回転を反映
		D3DXMatrixMultiply(	&m_mtxWorld,&m_mtxWorld,
							&mtxRot);


		//位置の設定
		D3DXMatrixTranslation(	&mtxTranslate,
								m_pos.x,
								m_pos.y,
								m_pos.z);

		//位置の反映
		D3DXMatrixMultiply(	&m_mtxWorld,&m_mtxWorld,
							&mtxTranslate);

		//ワールドマトリックスのセット
		pDevice->SetTransform(	D3DTS_WORLD,
								&m_mtxWorld);

		//マテリアルの取得
		pDevice->GetMaterial(&matDef);

		//頂点バッファのポインタをマテリアルに変換
		pD3DXMat=(D3DXMATERIAL*)m_pD3DXBuffMatModel->GetBufferPointer();

		//トゥーンシェーダー開始
		CToonShader::Begin();

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
//弾インスタンス生成
//=============================================================================
void CBullet::Create(D3DXVECTOR3 pos,D3DXVECTOR3 rot,D3DXVECTOR3 velocity)
{
	//残弾数が残っている場合のみインスタンス生成
	if(CLeftoverBullet::GetLeftoverNum()>0)
	{
		//弾インスタンスのポインタ
		CBullet *pBullet;

		//インスタンスの生成
		pBullet=new CBullet();

		//初期化
		pBullet->Init(pos,rot,velocity);

		//残弾数減算
		CLeftoverBullet::SubNum();

		//発射音再生
		CSound::PlaySoundA(SOUND_LABEL_SE_SHOT);
	}
}
//EOF