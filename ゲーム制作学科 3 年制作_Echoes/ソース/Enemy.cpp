//=============================================================================
// 敵の処理 [Enemy.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Enemy.h"
#include "MeshField.h"
#include "manager.h"
#include "renderer.h"
#include "Camera.h"
#include "FrustumCulling.h"
#include "Game.h"
#include "HitCheck.h"
#include "AnimationEffect.h"
#include "Particle.h"
#include "Score.h"
#include "Time.h"
#include "Item.h"
#include "InforMation.h"
#include "Combo.h"
#include "EraseArea.h"
#include "Sound.h"

//デバッグ用
#ifdef _DEBUG
	#include "DebugProc.h"
	#include "HitCheckSphere.h"
	#include "InputKeyboard.h"
#endif

//*****************************************************************************
//定数定義
//*****************************************************************************
#define ENEMY_VELOCITY_Z (-1.5f)//敵のZ座標移動量
#define DEFAULT_BONUS (10)		//通常のボーナス
#define PARTICLE_NUM (3)		//セットするパーティクル数
#define PARTICLE_SIZE (100.0f)	//セットするパーティクルサイズ
#define COLOR_VALUE (245)		//セットする色値
#define ALPHA_VALUE (190)		//セットする透明度
#define ADD_FIELD_Z (500)		//補正用のフィールドZ座標
#define LOD_LENGTH (3900.0f)	//LOD描画する距離

//=============================================================================
//コンストラクタ
//=============================================================================
CEnemy::CEnemy()
{
	m_type = OBJECT_ENEMY;
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CEnemy::Init(D3DXVECTOR3 pos, ENEMY_COLOR color)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//自身の色を設定
	m_myColor = color;

	for (int i = 0; i < COLOR_NUM; ++i)
	{
		//所持色数初期化
		m_nStickColor[i] = 0;
		//その色のみフラグ
		m_bOnlyColor[i] = false;
	}

	//座標・角度・スケール設定
	m_pos = pos;
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//速度
	m_velocity = D3DXVECTOR3(0.0f, 0.0f, ENEMY_VELOCITY_Z);

	//発射中のカウント
	m_nShotCount = 0;

	//移動フラグ
	m_bMove = true;

	//固定フラグ
	m_bFixity = false;

	//弾フラグ
	m_bShot = false;

	//描画フラグ
	m_bDraw = false;

	//LODフラグ
	m_bLOD = false;

	//接触している敵ポインタ
	m_pNext = NULL;
	m_pPrev = NULL;

	//LOD用マテリアル
	D3DCOLORVALUE matLOD;
	matLOD.r = 0.0f;
	matLOD.g = 0.0f;
	matLOD.b = 0.0f;
	matLOD.a = 1.0f;

	//色によって読込ファイルを分ける
	char *pFileName = "";
	switch (m_myColor)
	{
		//赤
		case ENEMY_RED:
			pFileName = "data/MODEL/ball_red.x";
			matLOD.r = 1.0f;
		break;

		//緑
		case ENEMY_GREEN:
			pFileName = "data/MODEL/ball_green.x";
			matLOD.g = 1.0f;
		break;

		//青
		case ENEMY_BLUE:
			pFileName = "data/MODEL/ball_blue.x";
			matLOD.b = 1.0f;
		break;

		default:
		break;
	}

	///////////////////////////////////////////
	//		スキンメッシュ関連の初期化		//
	/////////////////////////////////////////
	//アニメーション情報付きXファイルを読み込む
	if (FAILED(D3DXLoadMeshHierarchyFromX(	pFileName,
											D3DXMESH_MANAGED,
											pDevice,
											&m_alloc,
											NULL,
											&m_pFrameRoot,
											&m_pAnimController)))
	{
		return E_FAIL;
	}

	//フレームルートの先頭をセット
	m_alloc.SetFrameRoot(m_pFrameRoot);

	//ボーン行列の初期化
	m_alloc.SetupBoneMatrixPointers(m_pFrameRoot);

	//アニメーションの種類をアニメーション数分確保
	m_pAnimSet = new LPD3DXANIMATIONSET[m_pAnimController->GetNumAnimationSets()];

	//アニメーション数分ループ
	for (unsigned int i = 0; i<m_pAnimController->GetNumAnimationSets(); i++)
	{
		//NULLセット
		m_pAnimSet[i] = NULL;
		//アニメーションの取得
		m_pAnimController->GetAnimationSet(i, &m_pAnimSet[i]);
	}

	//アニメーションのセット
	CSceneX::SetAnimation(0, 0.01, true);

	///////////////////////////////
	//		LOD関連の初期化		//
	/////////////////////////////

	//Xファイルのロード
	if (FAILED(D3DXLoadMeshFromX("data/MODEL/ball_LOD1.x",
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

	//バッファポインタの取得
	m_pD3DXMat = (D3DXMATERIAL*)m_pD3DXBuffMatModel->GetBufferPointer();

	//LOD用の拡散光の設定
	for (int nCntMat = 0; nCntMat<(int)m_nNumMatModel; nCntMat++)
	{
		m_pD3DXMat[nCntMat].MatD3D.Diffuse = matLOD;
	}

	///////////////////////////////////////////////////////
	//		フィールドからサイズとブロック数取得		//
	/////////////////////////////////////////////////////

	//フィールドインスタンス取得
	CMeshField *pField = CGame::GetField();

	//計算用ブロック数とサイズ
	int nBlockX = 0, nBlockZ = 0;
	float fBlockSizeX = 0.0f,fBlockSizeZ = 0.0f;
	//ブロック数とサイズ取得
	pField->GetBlockNum(&nBlockX, &nBlockZ);
	pField->GetBlockSize(&fBlockSizeX, &fBlockSizeZ);
	//フィールドの半分のサイズ計算
	m_fHalfFieldSizeX = (nBlockX * fBlockSizeX) / 2;
	m_fHalfFieldSizeZ = (nBlockZ * fBlockSizeZ) / 2;

//デバッグ用
#ifdef _DEBUG
	//球体生成
	m_pSphere = CHitCheckSphere::Create(ENEMY_RADIUS);
#endif

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CEnemy::Uninit()
{
	//終了処理
	CSceneX::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CEnemy::Update()
{
	//移動フラグtrue時のみ手前に移動させる
	if (m_bMove)
	{
		//移動
		m_pos.x += m_velocity.x;
		m_pos.y += m_velocity.y;

		//通常は手前に移動する
		m_pos.z += m_velocity.z;

		//フィールドの高さ取得
		float fHeight = CGame::GetField()->GetHeight(m_pos);
		//フィールドの高さに合わせる
		m_pos.y = fHeight;

		//アニメーションの再生
		CSceneX::PlayAnimation();
	}

	//固定化されている場合はオフセットと座標変換
	if (m_bFixity)
	{
		D3DXVec3TransformCoord(	&m_pos,			//座標変換結果
								&m_offSet,		//座標変換対象
								&m_parentMtx);	//座標変換マトリクス対象
	}

	//弾として扱われるなら
	if (m_bShot)
	{
		//カウントアップ
		m_nShotCount++;

		if (m_nShotCount % 10 == 0)
		{
			CAnimEffect::Create(m_pos, 30, 100.0f, SMOKE_EFFECT);
		}

		//当たり判定を行う
		HitCheck();
	}

	//接触したオブジェクトがある場合
	if (m_pNext)
	{
		//そのオブジェクトの座標との差分に合わせる
		m_pos = m_pNext->GetPos() + m_diffPos;
	}

	//フィールドから取得
	if ((m_pos.z < -(m_fHalfFieldSizeZ - ADD_FIELD_Z)
		|| m_pos.z > m_fHalfFieldSizeZ
		|| m_pos.x < -m_fHalfFieldSizeX
		|| m_pos.x > m_fHalfFieldSizeX)
		&& !m_bFixity)
	{
		//最終処理
		End();
	}

	//視錐台カリング内にある場合は描画許可させる
	m_bDraw = CFrustum::MeshFOVCheck(m_pos, ENEMY_RADIUS);

//デバッグ用
#ifdef _DEBUG
	//球体に座標をセット
	m_pSphere->SetPos(m_pos);
#endif
}
//=============================================================================
//描画
//=============================================================================
void CEnemy::Draw()
{
	//描画許可がある場合のみ描画
	if (m_bDraw)
	{
		//レンダラー情報取得
		CRenderer *pRenderer = CManager::GetRenderer();
		//デバイスのゲット
		LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

		//サイズ,回転,位置
		D3DXMATRIX mtxScl, mtxRot, mtxTranslate, mtxView;

		//プロジェクションマトリックス初期化
		D3DXMatrixIdentity(&m_mtxWorld);

		//サイズを設定
		D3DXMatrixScaling(&mtxScl,
			m_scl.x,
			m_scl.y,
			m_scl.z);

		//サイズを反映
		D3DXMatrixMultiply(&m_mtxWorld,
			&m_mtxWorld,
			&mtxScl);

		//回転を設定
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
			m_rot.y,
			m_rot.x,
			m_rot.z);

		//回転を反映
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
			&mtxRot);

		//位置を設定
		D3DXMatrixTranslation(&mtxTranslate,
			m_pos.x,
			m_pos.y,
			m_pos.z);

		//位置のセット
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
			&mtxTranslate);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

		///////////////////////
		//		LOD判定		//
		/////////////////////

		//ビューマトリクス取得
		mtxView = CManager::GetCamera()->GetMtxView();
		//カメラのZ距離計算
		D3DXVECTOR3 vi(mtxView._13, mtxView._23, mtxView._33);
		float fCameraLenZ = D3DXVec3Dot(&m_pos,&vi) + mtxView._43;

		//一定の距離を超えたかでLOD判定する
		if (fCameraLenZ > LOD_LENGTH)
			m_bLOD = true;
		else
			m_bLOD = false;

		//LOD判定で描画方法を変える
		if (m_bLOD)
		{
			//マテリアルプロパティ
			D3DMATERIAL9 matDef;

			//マテリアル取得
			pDevice->GetMaterial(&matDef);

			//マテリアルとテクスチャの設定
			for (int nCntMat = 0; nCntMat<(int)m_nNumMatModel; nCntMat++)
			{
				pDevice->SetMaterial(&m_pD3DXMat[nCntMat].MatD3D);
				pDevice->SetTexture(0, NULL);
				m_pD3DXMeshModel->DrawSubset(nCntMat);
			}

			//マテリアルセット
			pDevice->SetMaterial(&matDef);
		}
		//スキンメッシュの描画
		else
		{
			//フレームのマトリクスを変換
			m_alloc.MatricesFrame(m_pFrameRoot, &m_mtxWorld);
			//フレームの描画
			m_alloc.DrawFrame(pDevice, m_pFrameRoot);
		}
	}
}
//=============================================================================
//当たり判定
//=============================================================================
void CEnemy::HitCheck()
{
	//Xファイルモデルのプライオリティ取得
	CScene *pScene = CScene::GetListTop(PRIORITY_SCENEX);

	while (pScene)
	{
		//次シーンポインタ取得
		CScene *pNext = pScene->GetNext();

		///////////////////////////////////
		//		敵との当たり判定		//
		/////////////////////////////////
		if (pScene->GetType() == OBJECT_ENEMY && pScene != this)
		{
			//敵インスタンスの型に変換
			CEnemy *pEnemy = (CEnemy*)pScene;

			//敵の座標
			D3DXVECTOR3 enemyPos = pEnemy->GetPos();

			//敵と当たった場合
			if (EllipsCheck(m_pos, enemyPos, ENEMY_RADIUS, ENEMY_RADIUS))
			{
				//差分を求める
				m_diffPos = m_pos - enemyPos;

				//敵の移動を止める
				m_bMove = false;

				//弾として扱わない
				m_bShot = false;

				//先頭ポインタ探索
				CEnemy *pp = pEnemy;
				//探索ループ
				while (pp->GetNext())
				{
					pp = pp->GetNext();
				}

				//先頭ポインタに色を加算
				pp->AddColorNum(m_myColor);

				//接触しているオブジェクト保存
				m_pNext = pEnemy;
				if (!m_pNext->GetPrev())
				{
					m_pNext->SetPrev(this);
				}

				///////////////////////////////////////////////
				//		自身と同じ色ならアイテムの生成		//
				/////////////////////////////////////////////
				if (pEnemy->GetColor() == m_myColor)
				{
					//アイテムの種類
					ITEM_TYPE type = ITEM_CLOCK_UP;

					//色の種類からアイテムの種類を決める
					switch (m_myColor)
					{
						//赤
						case ENEMY_RED:
							type = ITEM_CLOCK_UP;
						break;
						
						//緑
						case ENEMY_GREEN:
							type = ITEM_TIME_UP;
						break;

						//青
						case ENEMY_BLUE:
							type = ITEM_TIME_UP;
						break;
					}//switch (m_myColor)


					//アイテムの生成
					CItem::Create(m_pos + (m_diffPos / 2),type);

					//ヒット２サウンド再生
					CSound::PlaySoundA(SOUND_LABEL_SE_HIT_B);
				}
				else
				{
					//違う色はヒット１サウンド再生
					CSound::PlaySoundA(SOUND_LABEL_SE_HIT_A);
				}

				//パーティクルの発生
				CParticle::Create(	TYPE_EFFECT, PARTICLE_NUM, m_pos + (m_diffPos / 2),
									PARTICLE_SIZE, PARTICLE_SIZE);

				break;

			}//if (EllipsCheck(m_pos, enemyPos, ENEMY_RADIUS, ENEMY_RADIUS))
		}//if (pScene->GetType() == OBJECT_ENEMY)

		//次シーンへ
		pScene = pNext;
	}//while (pScene)
}
//=============================================================================
//色加算
//=============================================================================
void CEnemy::AddColorNum(ENEMY_COLOR color)
{
	switch (color)
	{
		//赤
		case ENEMY_RED:
			m_nStickColor[ENEMY_RED]++;
		break;

		//緑
		case ENEMY_GREEN:
			m_nStickColor[ENEMY_GREEN]++;
		break;

		//青
		case ENEMY_BLUE:
			m_nStickColor[ENEMY_BLUE]++;
		break;

		default:
		break;
	};
}
//=============================================================================
//敵の終了
//=============================================================================
void CEnemy::End()
{
	//各色のボーナス得点
	int nBonusR = DEFAULT_BONUS;
	int nBonusG = DEFAULT_BONUS;
	int nBonusB = DEFAULT_BONUS;

	//他オブジェクトと接触した場合コンボ加算
	if (m_pNext || m_pPrev)
	{
		//コンボ加算
		CCombo::AddCombo();

		//削除効果音再生
		CSound::PlaySoundA(SOUND_LABEL_SE_VOICED);
	}

	//接触してかつ先頭ポインタなら自身の色追加
	if (!m_pNext && m_pPrev)
	{
		//消滅エリアに設定する色
		D3DCOLOR color;

		//自身の色の加算
		switch (m_myColor)
		{
			//赤
			case ENEMY_RED:
				m_nStickColor[ENEMY_RED]++;
				color = D3DCOLOR_RGBA(COLOR_VALUE, 0, 0, ALPHA_VALUE);
			break;

			//緑
			case ENEMY_GREEN:
				m_nStickColor[ENEMY_GREEN]++;
				color = D3DCOLOR_RGBA(0, COLOR_VALUE, 0, ALPHA_VALUE);
			break;

			//青
			case ENEMY_BLUE:
				m_nStickColor[ENEMY_BLUE]++;
				color = D3DCOLOR_RGBA(0, 0, COLOR_VALUE, ALPHA_VALUE);
			break;

			default:
			break;
		}

		//エリアの色を設定
		CGame::GetArea()->SetColor(color);
	}

	//各色のみの場合はその色のみフラグtrue
	//赤
	if (m_nStickColor[ENEMY_RED] > 0
		&& m_nStickColor[ENEMY_GREEN] == 0
		&& m_nStickColor[ENEMY_BLUE] == 0)
	{
		m_bOnlyColor[ENEMY_RED] = true;
		nBonusR = DEFAULT_BONUS * m_bOnlyColor[ENEMY_RED];
	}
	//緑
	else if (m_nStickColor[ENEMY_RED] == 0
		&& m_nStickColor[ENEMY_GREEN] > 0
		&& m_nStickColor[ENEMY_BLUE] == 0)
	{
		m_bOnlyColor[ENEMY_GREEN] = true;
		nBonusG = DEFAULT_BONUS * m_bOnlyColor[ENEMY_GREEN];
	}
	//青
	else if (m_nStickColor[ENEMY_RED] == 0
		&& m_nStickColor[ENEMY_GREEN] == 0
		&& m_nStickColor[ENEMY_BLUE] > 0)
	{
		m_bOnlyColor[ENEMY_BLUE] = true;
		nBonusB = DEFAULT_BONUS * m_bOnlyColor[ENEMY_BLUE];
	}

	//スコアの加算
	CScore::AddScore(m_nStickColor[ENEMY_RED] * nBonusR);
	CScore::AddScore(m_nStickColor[ENEMY_GREEN] * nBonusG);
	CScore::AddScore(m_nStickColor[ENEMY_BLUE] * nBonusB);

	//パーティクル生成
	CParticle::Create(	TYPE_EFFECT, PARTICLE_NUM, m_pos,
						PARTICLE_SIZE, PARTICLE_SIZE);

	//終了
	Uninit();
}
//=============================================================================
//敵の発射
//=============================================================================
void CEnemy::Shot(D3DXVECTOR3 rot, D3DXVECTOR3 velocity)
{
	//移動させる
	m_bMove = true;

	//弾として扱う
	m_bShot = true;

	//固定をはずす
	m_bFixity = false;

	//移動量設定
	m_velocity.x = -sinf(rot.y) * velocity.x;
	m_velocity.z = -cosf(rot.y) * velocity.z;
}
//=============================================================================
//敵の固定
//=============================================================================
void CEnemy::Fixation(D3DXVECTOR3 offset)
{
	//敵の移動を止める
	m_bMove = false;

	//固定フラグtrue
	m_bFixity = true;

	//オフセット値セット
	m_offSet = offset;
}
//=============================================================================
//親マトリクスセット
//=============================================================================
void CEnemy::SetParentMtx(D3DXMATRIX parentMtx)
{
	m_parentMtx = parentMtx;
}
//=============================================================================
//インスタンス生成
//=============================================================================
CEnemy *CEnemy::Create(D3DXVECTOR3 pos, ENEMY_COLOR color)
{
	//インスタンス生成
	CEnemy *pEnemy = new CEnemy();
	//初期化
	pEnemy->Init(pos, color);
	return pEnemy;
}
//EOF