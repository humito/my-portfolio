//=============================================================================
// プレイヤー処理 [Player.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#define _CRT_SECURE_NO_WARNINGS	//警告対策用
#include <stdio.h>
#include "Player.h"
#include "manager.h"
#include "renderer.h"
#include "Camera.h"
#include "InputKeyboard.h"
#include "InputJoystick.h"
#include "Game.h"
#include "FeedBackBlur.h"
#include "MeshField.h"
#include "Model.h"
#include "Enemy.h"
#include "Item.h"
#include "Shadow.h"
#include "HitCheck.h"
#include "Time.h"
#include "InforMation.h"
#include "Particle.h"
#include "Sound.h"
#include "ShaderManager.h"
#include "ToonShader.h"

//デバッグ用
#ifdef _DEBUG
	#include "DebugProc.h"
	#include "HitCheckSphere.h"
#endif

//*****************************************************************************
//定数定義
//*****************************************************************************
#define POS_MOVE (1.0f)											//プレイヤー移動量
#define CLOCK_UP_MOVE (2.0f)									//クロックアップ時の移動量
#define CLOCK_UP_COUNT_MAX (65)									//クロックアップカウント最大値
#define GRAVITY (0.1f)											//重力
#define PLAYER_RADIUS (50.0f)									//プレイヤーサイズの半径
#define VELOCITY_RATE (0.15f)									//移動量の割合
#define DIFF_ROT_RATE_Y (0.1f)									//回転の差分の割合
#define NOT_HOLD_COUNT_MAX (30)									//掴めないカウントの最大値
#define FIXATION_ENEMY_POS (D3DXVECTOR3(-42.0f, -40.0f, 20.0f))	//掴んだ敵の固定位置
#define THROW_ENEMY_POS (D3DXVECTOR3(10.0f, -40.0f, 0.0f))		//投げる敵のセット位置
#define SETTING_BULLET_FRONT (15.0f)							//プレイヤーから前方への距離
#define SETTING_BULLET_VELOCITY (D3DXVECTOR3(30.0f,0.0f,30.0f))	//弾(敵)を投げる時の速度
#define PART_BODY (0)											//体パーツ
#define PART_HAND_LEFT (1)										//左手パーツ
#define PART_HAND_RIGHT (2)										//右手パーツ
#define PART_FOOT_LEFT (3)										//左足パーツ
#define PART_FOOT_RIGHT (4)										//右足パーツ
#define NEUTORAL_BLEND_FRAME (40)								//ニュートラル時のブレンドフレーム
#define WALK_BLEND_FRAME (20)									//歩く時のブレンドフレーム
#define HOLD_BLEND_FRAME (10)									//持つ時のブレンドフレーム
#define HOLDWALK_BLEND_FRAME (20)								//持つ時のブレンドフレーム
#define THROW_BLEND_FRAME (10)									//投げる時のブレンドフレーム
#define INIT_POS_Z (-3800.0f)									//初期座標Z

//=============================================================================
//コンストラクタ
//=============================================================================
CPlayer::CPlayer()
{
	//プレイヤーオブジェクト
	m_type = OBJECT_PLAYER;
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CPlayer::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//座標・角度・スケール設定
	m_pos = D3DXVECTOR3(0.0f, 0.0f, INIT_POS_Z);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//目的の角度
	m_rotDestModel.y = D3DX_PI;

	//移動量
	m_fMove = POS_MOVE;

	//速度設定
	m_velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//移動フラグ
	m_bMove = false;

	//掴みフラグ
	m_bHold = false;

	//掴めないフラグ
	m_bNotHold = false;

	//クロックアップフラグ
	m_bClock = false;

	//保持用敵インスタンス初期化
	m_pHoldEnemy = NULL;

	//つかめないカウント
	m_nNotHoldCnt = 0;

	//モーションデータの読込
	LoadMotionData();

	//プレイヤーパーツの準備
	SetupPlayerPart();

	//モーションのセット
	SetMotion(MOTION_NEUTORAL);

	//影インスタンス生成
	m_pShadow = CShadow::Create();

	///////////////////////////////////////////////////////
	//		フィールドからサイズとブロック数取得		//
	/////////////////////////////////////////////////////

	//フィールドインスタンス取得
	CMeshField *pField = CGame::GetField();

	//計算用ブロック数とサイズ
	int nBlockX = 0, nBlockZ = 0;
	//ブロック数とサイズ取得
	pField->GetBlockNum(&nBlockX, &nBlockZ);
	pField->GetBlockSize(&m_fBlockSizeX, &m_fBlockSizeZ);
	//フィールドの半分のサイズ計算
	m_fHalfFieldSizeX = (nBlockX * m_fBlockSizeX) / 2;
	m_fHalfFieldSizeZ = (nBlockZ * m_fBlockSizeZ) / 2;

//デバッグ用
#ifdef _DEBUG

	//球体ポリゴン生成
	m_pSphere = CHitCheckSphere::Create(PLAYER_RADIUS);

#endif

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CPlayer::Uninit()
{
	//生成した各パーツの解放
	for (int i = 0; i < PART_MAX; ++i)
	{
		m_pModel[i]->Uninit();
		delete m_pModel[i];
		m_pModel[i] = NULL;
	}

	//終了処理
	CSceneX::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CPlayer::Update()
{
	//プレイヤー操作処理
	PlayerInput();

	//投げる動作の途中で持ってる敵を投げる
	if (m_MotionType == MOTION_THROW
	&&	m_KeyData.nKey == MIDDLE_MOTION - 1
	&&  m_pHoldEnemy)
	{
		//敵の座標をプレイヤ―前方にセット
		m_pHoldEnemy->SetPos(D3DXVECTOR3(m_pos.x - sinf(m_rot.y)*SETTING_BULLET_FRONT,
											m_pos.y,
											m_pos.z - cosf(m_rot.y)*SETTING_BULLET_FRONT));

		//敵を弾として扱う
		m_pHoldEnemy->Shot(m_rot, SETTING_BULLET_VELOCITY);
		m_pHoldEnemy = NULL;

		//投げサウンド再生
		CSound::PlaySoundA(SOUND_LABEL_SE_THROW);
	}

	//クロックアップ時の処理
	if (m_bClock)
	{
		//カウントアップ
		m_nClockUpCnt++;

		//カウントが一定に達したらクロックアップ止める
		if (m_nClockUpCnt > CLOCK_UP_COUNT_MAX)
		{
			m_bClock = false;
			m_fMove = POS_MOVE;
			CFeedBackBlur::SetUse(false);
		}
	}

	///////////////////////////////////
	//			向きの旋回			//
	/////////////////////////////////

	//目的の向きと現在の向きの角度の差を求める
	float fDiffRotY = m_rotDestModel.y - m_rot.y;

	//差分補正
	//右上 270°
	if (fDiffRotY > D3DX_PI)
	{
		fDiffRotY = fDiffRotY - D3DX_PI * 2;//360°減算
	}

	//右上 -270°
	if (fDiffRotY < -D3DX_PI)
	{
		fDiffRotY = fDiffRotY + D3DX_PI * 2;//360°加算
	}

	//現在のモデルの角度(向き)を加算
	m_rot.y += fDiffRotY * DIFF_ROT_RATE_Y;

	//角度補正
	//モデルの角度が3.14より大きい場合
	if (m_rot.y > D3DX_PI)
	{
		m_rot.y = -D3DX_PI;
	}

	//モデルの角度が-3.14より小さい場合
	if (m_rot.y < -D3DX_PI)
	{
		m_rot.y = D3DX_PI;
	}

	///////////////////////////////////////
	//		プレイヤーの座標更新		//
	/////////////////////////////////////

	//座標を移動量分加算
	m_pos += m_velocity;
	//速度減算
	m_velocity -= m_velocity * VELOCITY_RATE;

	//フィールドの高さに合わせる
	m_pos.y = CGame::GetField()->GetHeight(m_pos);

	//掴めないフラグ更新
	if (m_bNotHold)
	{
		m_nNotHoldCnt++;
	}
	//掴めないカウントが最大に達したらフラグを戻す
	if (m_nNotHoldCnt > NOT_HOLD_COUNT_MAX)
	{
		m_nNotHoldCnt = 0;
		m_bNotHold = false;
	}

	//掴んだ敵にマトリクスを渡す
	if (m_pHoldEnemy)
	{
		m_pHoldEnemy->SetParentMtx(m_pModel[PART_HAND_RIGHT]->GetMatrix());
	}

	//当たり判定チェック
	HitCheck();

	//モーション切替
	ChangeMotion();

	//影に座標セット
	m_pShadow->SetPos(m_pos);

	//前座標更新
	m_posPrev = m_pos;

//デバッグ用
#ifdef _DEBUG

	//球体ポリゴン生成
	m_pSphere->SetPos(m_pos);

	//プレイヤー座標表示
	CDebug::Print("\nPlayerPos(X:%f,Y:%f,Z:%f)",m_pos.x,m_pos.y,m_pos.z);
#endif

	//各パーツのキーフレームアニメーション
	PartAnimation();
}
//=============================================================================
//描画
//=============================================================================
void CPlayer::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//サイズ,回転,位置
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//プロジェクションマトリックスを反映
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
	D3DXMatrixMultiply(	&m_mtxWorld, &m_mtxWorld,
						&mtxRot);


	//位置を設定
	D3DXMatrixTranslation(	&mtxTranslate,
							m_pos.x,
							m_pos.y,
							m_pos.z);

	//位置のセット
	D3DXMatrixMultiply(	&m_mtxWorld, &m_mtxWorld,
						&mtxTranslate);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//トゥーンシェーダー開始
	CToonShader *pToon = (CToonShader*)CShaderManager::GetInstance()->GetShader(TOON_SHADER);
	pToon->Begin();

	//パーツの描画
	for (int i = 0; i < PART_MAX; ++i)
	{
		m_pModel[i]->Draw();
	}
	//トゥーンシェーダー終了
	pToon->End();
}
//=============================================================================
//プレイヤー操作処理
//=============================================================================
void CPlayer::PlayerInput()
{
	//カメラインスタンス取得
	CCamera *pCamera = CManager::GetCamera();
	//カメラの向き取得
	D3DXVECTOR3 rot = pCamera->GetRotCamera();

	///////////////////////////////////
	//			操作処理			//
	/////////////////////////////////

	//前
	if (CInputKeyboard::GetKeyPress(DIK_W)
		|| CInputJoystick::GetPadPress(STICK_L_UP))
	{
		//移動フラグtrue
		m_bMove = true;

		//移動量加算
		m_velocity.x -= cosf(rot.y + D3DX_PI / 2)*m_fMove;//X
		m_velocity.z += sinf(rot.y + D3DX_PI / 2)*m_fMove;//Z
		//目的の向き
		m_rotDestModel.y = rot.y + D3DX_PI;

		//他のキー押下で目的の向きを変える
		//左
		if (CInputKeyboard::GetKeyPress(DIK_A)
			|| CInputJoystick::GetPadPress(STICK_L_LEFT))
		{
			m_velocity.x -= sinf(rot.y + D3DX_PI / 2)*m_fMove;//X
			m_velocity.z -= cosf(rot.y + D3DX_PI / 2)*m_fMove;//Z
			//目的の向き
			m_rotDestModel.y = rot.y + (D3DX_PI / 4) * 3;

		}//右
		else if (CInputKeyboard::GetKeyPress(DIK_D)
			|| CInputJoystick::GetPadPress(STICK_L_RIGHT))
		{
			m_velocity.x += sinf(rot.y + D3DX_PI / 2)*m_fMove;//X
			m_velocity.z += cosf(rot.y + D3DX_PI / 2)*m_fMove;//Z
			//目的の向き
			m_rotDestModel.y = rot.y - (D3DX_PI / 4) * 3;
		}
	}
	//後
	else if (CInputKeyboard::GetKeyPress(DIK_S)
		|| CInputJoystick::GetPadPress(STICK_L_DOWN))
	{
		//移動フラグtrue
		m_bMove = true;

		//移動量加算
		m_velocity.x += cosf(rot.y + D3DX_PI / 2)*m_fMove;//X
		m_velocity.z -= sinf(rot.y + D3DX_PI / 2)*m_fMove;//Z
		//目的の向き
		m_rotDestModel.y = rot.y;

		//他のキー押下で目的の向きを変える
		//左
		if (CInputKeyboard::GetKeyPress(DIK_A)
			|| CInputJoystick::GetPadPress(STICK_L_LEFT))
		{
			m_velocity.x -= sinf(rot.y + D3DX_PI / 2)*m_fMove;//X
			m_velocity.z -= cosf(rot.y + D3DX_PI / 2)*m_fMove;//Z
			//目的の向き
			m_rotDestModel.y = rot.y + D3DX_PI / 4;

		}//右
		else if (CInputKeyboard::GetKeyPress(DIK_D)
			|| CInputJoystick::GetPadPress(STICK_L_RIGHT))
		{
			m_velocity.x += sinf(rot.y + D3DX_PI / 2)*m_fMove;//X
			m_velocity.z += cosf(rot.y + D3DX_PI / 2)*m_fMove;//Z
			//目的の向き
			m_rotDestModel.y = rot.y - D3DX_PI / 4;
		}
	}
	//左
	else if (CInputKeyboard::GetKeyPress(DIK_A)
		|| CInputJoystick::GetPadPress(STICK_L_LEFT))
	{
		//移動フラグtrue
		m_bMove = true;

		//移動量加算
		m_velocity.x -= sinf(rot.y + D3DX_PI / 2)*m_fMove;//X
		m_velocity.z -= cosf(rot.y + D3DX_PI / 2)*m_fMove;//Z
		//目的の向き
		m_rotDestModel.y = rot.y + D3DX_PI / 2;
	}
	//右
	else if (CInputKeyboard::GetKeyPress(DIK_D)
		|| CInputJoystick::GetPadPress(STICK_L_RIGHT))
	{
		//移動フラグtrue
		m_bMove = true;

		//移動量加算
		m_velocity.x += sinf(rot.y + D3DX_PI / 2)*m_fMove;//X
		m_velocity.z += cosf(rot.y + D3DX_PI / 2)*m_fMove;//Z
		//目的の向き
		m_rotDestModel.y = rot.y - D3DX_PI / 2;
	}
	//移動してない場合
	else
	{
		//移動フラグfalse
		m_bMove = false;
	}

	//投げ操作
	if (CInputKeyboard::GetKeyTrigger(DIK_SPACE)
		|| CInputJoystick::GetPadPress(BUTTON_1)
		|| CInputJoystick::GetPadPress(TRIGGER_R))
	{
		//投げるモーションに移る
		if (m_bHold)
		{
			//掴みフラグfalse
			m_bHold = false;

			//しばらくつかめないフラグtrue
			m_bNotHold = true;

			//投げモーションセット
			SetMotion(MOTION_THROW);
			m_pHoldEnemy->Fixation(THROW_ENEMY_POS);
		}
	}
}
//=============================================================================
//当たり判定チェック
//=============================================================================
void CPlayer::HitCheck()
{
	///////////////////////////////////////////
	//		オブジェクトとの当たり判定		//
	/////////////////////////////////////////
	//プライオリティ分ループ
	for (int i = 0; i < PRIORITY_MAX; ++i)
	{
		//Xファイルかビルボード以外のシーンは飛ばす
		if (i != PRIORITY_SCENEX && i != PRIORITY_BILLBOARD)
		{
			continue;
		}

		//Xファイルモデルのプライオリティ取得
		CScene *pScene = CScene::GetListTop(i);

		//オブジェクト当たり判定処理ループ
		while (pScene)
		{
			//次シーンポインタ取得
			CScene *pNext = pScene->GetNext();

			//カメラの範囲内に入ってる(描画されてる)場合、当たり判定開始
			if (pScene->DrawCheck())
			{
				//オブジェクトのタイプ取得
				OBJECT_TYPE type = pScene->GetType();

				///////////////////////////////////
				//		敵との当たり判定		//
				/////////////////////////////////
				if (type == OBJECT_ENEMY)
				{
					//敵インスタンスの型に変換
					CEnemy *pEnemy = (CEnemy*)pScene;

					//敵とプレイヤーとの当たり判定
					if (EllipsCheck(m_pos, pEnemy->GetPos(), PLAYER_RADIUS, ENEMY_RADIUS)
						&& m_bHold == false								//掴み状態でないか
						&& m_bNotHold == false							//掴めない状態でないか
						&& m_MotionType != MOTION_THROW)				//投げ状態以外であるか
					{
						//接触してない敵なら
						if (!pEnemy->GetPrev() && !pEnemy->GetNext())
						{
							//掴み状態にする
							m_bHold = true;

							//敵を固定化して保持用として保存
							m_pHoldEnemy = pEnemy;
							m_pHoldEnemy->Fixation(FIXATION_ENEMY_POS);

							//掴みサウンド再生
							CSound::PlaySoundA(SOUND_LABEL_SE_HOLD);
						}
						//接触してる敵なら
						else
						{
							///////////////////////////////////////////
							//		めり込んだ分だけ引き戻す		//
							/////////////////////////////////////////
							//敵座標
							D3DXVECTOR3 enemyPos = pEnemy->GetPos();
							D3DXVECTOR3 vec1 = enemyPos - m_posPrev;
							D3DXVECTOR3 vec2 = enemyPos - m_pos;
							D3DXVECTOR3 diffPos = vec1 - vec2;

							//X座標だけ引き戻した座標
							D3DXVECTOR3 backPosX = D3DXVECTOR3(m_pos.x - diffPos.x,
								m_pos.y,
								m_pos.z);

							//それでも当たる場合
							if (EllipsCheck(backPosX, pEnemy->GetPos(), PLAYER_RADIUS, ENEMY_RADIUS))
							{
								//Z座標引き戻す
								m_pos.z -= diffPos.z;
							}
							else
							{
								m_pos = backPosX;
							}
						}
					}
				}//if (type == OBJECT_ENEMY)

				///////////////////////////////////////
				//		アイテムとの当たり判定		//
				/////////////////////////////////////
				else if (type == OBJECT_ITEM)
				{
					//アイテムインスタンスの型に変換
					CItem *pItem = (CItem*)pScene;

					//アイテムと当った場合
					if (EllipsCheck(m_pos, pItem->GetPos(), PLAYER_RADIUS, ITEM_RADIUS))
					{
						//アイテムの種類から処理を分ける
						switch (pItem->GetItemType())
						{
							//タイムアップ
						case ITEM_TIME_UP:
						{
							//タイム加算
							CTime::AddTime(5.0f);
							//インフォメーション
							CInfo::Create(TIME_UP);
							//タイム加算サウンド再生
							CSound::PlaySoundA(SOUND_LABEL_SE_ITEM_TIME);
							break;
						}

							//クロックアップ
						case ITEM_CLOCK_UP:
						{
							//クロックアップ開始
							m_bClock = true;
							m_fMove = CLOCK_UP_MOVE;
							m_nClockUpCnt = 0;
							//インフォメーション
							CInfo::Create(CLOCK_UP);
							//フィードバック開始
							CFeedBackBlur::SetUse(true);
							//クロックアップサウンド再生
							CSound::PlaySoundA(SOUND_LABEL_SE_ITEM_CLOCK);
							break;
						}

						default:
							break;
						}//switch (pItem->GetItemType())

						//アイテムを削除
						pItem->Uninit();
					}

				}//else if (type == OBJECT_ITEM)
			}//if (pScene->DrawCheck())

			//次のシーンへ
			pScene = pNext;
		}//while (pScene)
	}//for (int i = 0; i < PRIORITY_MAX; ++i)

	///////////////////////////////////////////
	//		フィールド外との当たり判定		//
	/////////////////////////////////////////
	if (m_pos.x > m_fHalfFieldSizeX
	||	m_pos.x < -m_fHalfFieldSizeX)
	{
		m_pos.x = m_posPrev.x;
	}
	else if (m_pos.z > m_fHalfFieldSizeZ
	||		 m_pos.z < -m_fHalfFieldSizeZ)
	{
		m_pos.z = m_posPrev.z;
	}
}
//=============================================================================
//パーツアニメーション
//=============================================================================
void CPlayer::PartAnimation()
{
	//各パーツを操作
	for (int j = 0; j<PART_MAX; j++)
	{
		float rate;						//レート(現在と総フレームまでの割合)
		float rateBlend;
		float rateMotionBlend;
		KEY *key, *keyNext;				//現在と次のキー情報
		KEY *keyBlend, *keyNextBlend;
		D3DXVECTOR3 pos, rot;			//設定する座標、角度
		D3DXVECTOR3 posCur, rotCur;
		D3DXVECTOR3 posBlend, rotBlend;

		//現在のキー情報
		key = &m_KeyData.KeyInfo[m_KeyData.nKey%m_KeyData.nNumKey].key[j];
		//現在で次のキー情報ポインタ
		keyNext = &m_KeyData.KeyInfo[(m_KeyData.nKey + 1) % m_KeyData.nNumKey].key[j];
		//現在キーのレート計算
		rate = m_KeyData.fMotionTime / (float)m_KeyData.KeyInfo[m_KeyData.nKey%m_KeyData.nNumKey].Frame;
		
		//ブレンド中の場合
		if (m_bBlend)
		{
			//ブレンド先のキー情報ポインタ代入
			keyBlend = &m_KeyDataBlend.KeyInfo[m_KeyDataBlend.nKey].key[j];

			keyNextBlend = &m_KeyDataBlend.KeyInfo[m_KeyDataBlend.nKey].key[j];

			//ブレンドと切替時のレート計算
			rateBlend = m_KeyDataBlend.fMotionTime / (float)m_KeyDataBlend.KeyInfo[m_KeyDataBlend.nKey].Frame;
			rateMotionBlend = (float)m_nCountBlend / (float)m_nFrameBlend;

			//現在の座標角度を線形補間
			posCur = D3DXVECTOR3(key->PosX*(1.0f - rate) + keyNext->PosX*rate,
								key->PosY*(1.0f - rate) + keyNext->PosY*rate,
								key->PosZ*(1.0f - rate) + keyNext->PosZ*rate);

			rotCur = D3DXVECTOR3(key->RotX*(1.0f - rate) + keyNext->RotX*rate,
								key->RotY*(1.0f - rate) + keyNext->RotY*rate,
								key->RotZ*(1.0f - rate) + keyNext->RotZ*rate);

			//ブレンド先の座標角度を線形補間
			posBlend = D3DXVECTOR3(keyBlend->PosX*(1.0f - rateBlend) + keyNextBlend->PosX*rateBlend,
								keyBlend->PosY*(1.0f - rateBlend) + keyNextBlend->PosY*rateBlend,
								keyBlend->PosZ*(1.0f - rateBlend) + keyNextBlend->PosZ*rateBlend);

			rotBlend = D3DXVECTOR3(keyBlend->RotX*(1.0f - rateBlend) + keyNextBlend->RotX*rateBlend,
								keyBlend->RotY*(1.0f - rateBlend) + keyNextBlend->RotY*rateBlend,
								keyBlend->RotZ*(1.0f - rateBlend) + keyNextBlend->RotZ*rateBlend);

			//現在からブレンド先への座標角度を線形補間
			pos = D3DXVECTOR3(posCur.x*(1.0f - rateMotionBlend) + posBlend.x*rateMotionBlend,
							posCur.y*(1.0f - rateMotionBlend) + posBlend.y*rateMotionBlend,
							posCur.z*(1.0f - rateMotionBlend) + posBlend.z*rateMotionBlend);

			rot = D3DXVECTOR3(rotCur.x*(1.0f - rateMotionBlend) + rotBlend.x*rateMotionBlend,
							rotCur.y*(1.0f - rateMotionBlend) + rotBlend.y*rateMotionBlend,
							rotCur.z*(1.0f - rateMotionBlend) + rotBlend.z*rateMotionBlend);
		}
		else
		{
			//座標
			pos = D3DXVECTOR3(key->PosX*(1.0f - rate) + keyNext->PosX*rate,
				key->PosY*(1.0f - rate) + keyNext->PosY*rate,
				key->PosZ*(1.0f - rate) + keyNext->PosZ*rate);

			//角度
			rot = D3DXVECTOR3(key->RotX*(1.0f - rate) + keyNext->RotX*rate,
				key->RotY*(1.0f - rate) + keyNext->RotY*rate,
				key->RotZ*(1.0f - rate) + keyNext->RotZ*rate);
		}

		//パーツの座標、角度をセット
		m_pModel[j]->SetPos(pos);
		m_pModel[j]->SetRot(rot);
	}

	//ブレンドフラグによって動作時間を加算する
	if (m_bBlend)
	{
		//ブレンド用カウント加算
		m_nCountBlend++;
	}
	else
	{
		//現在キーの動作時間を加算
		m_KeyData.fMotionTime++;
	}

	//ブレンド時のカウントがフレーム数に達したら
	if (m_nCountBlend >= m_nFrameBlend)
	{
		//ブレンド先のキーを現在のキーとする
		m_KeyData.KeyInfo = m_KeyDataBlend.KeyInfo;
		//ブレンド先のキーを現在のキーにする
		m_KeyData.nKey = m_KeyDataBlend.nKey;
		//ブレンド先総キー数を代入
		m_KeyData.nNumKey = m_KeyDataBlend.nNumKey;
		//ブレンドフラグfalse設定
		m_bBlend = false;
		//ブレンドカウントリセット
		m_nCountBlend = 0;
		//モーション時間リセット
		m_KeyData.fMotionTime = 0.0f;
	}

	//動作時間がキーのフレーム数に達したら
	if ((int)m_KeyData.fMotionTime >= m_KeyData.KeyInfo[m_KeyData.nKey].Frame
		&& !m_bBlend)
	{
		//キー数加算
		m_KeyData.nKey++;

		//キー数が総キー数に達したら
		if (m_KeyData.nKey >= m_KeyData.nNumKey)
		{
			//1回のみのアニメーションの場合
			if (!m_bAnimLoop && !m_bBlend)
			{
				//ニュートラルに戻す
				SetMotion(MOTION_NEUTORAL);
			}
			else
			{
				//ループさせるためキー数をリセット
				m_KeyData.nKey = 0;
			}
		}

		//動作時間をリセット
		m_KeyData.fMotionTime = 0.0f;
	}
}
//=============================================================================
//モーション切替
//=============================================================================
void CPlayer::ChangeMotion()
{
	//ニュートラルからの変更
	if (m_MotionType == MOTION_NEUTORAL)
	{
		//移動しているなら歩きモーション
		if (m_bMove)
		{
			SetMotion(MOTION_WALK);
		}
		//何か持っているなら
		else if (m_bHold)
		{
			SetMotion(MOTION_HOLD_WAIT);
		}
	}
	//歩きからの変更
	else if (m_MotionType == MOTION_WALK)
	{
		//何か持っている場合
		if (m_bHold)
		{
			SetMotion(MOTION_HOLD_WALK);
		}
		else if (!m_bMove)
		{
			SetMotion(MOTION_NEUTORAL);
		}
	}
	//掴み待ちからの変更
	else if (m_MotionType == MOTION_HOLD_WAIT)
	{
		if (m_bMove)
		{
			SetMotion(MOTION_HOLD_WALK);
		}
		else if (!m_bHold)
		{
			SetMotion(MOTION_NEUTORAL);
		}
	}
	//掴み歩きからの変更
	else if (m_MotionType == MOTION_HOLD_WALK)
	{
		if (!m_bMove)
		{
			SetMotion(MOTION_HOLD_WAIT);
		}
		else if (!m_bHold)
		{
			SetMotion(MOTION_WALK);
		}
	}
}
//=============================================================================
//モーションのセット
//=============================================================================
void CPlayer::SetMotion(MOTION_TYPE type)
{
	///////////////////////////////////////////////////
	//		モーションタイプによってキーを変更		//
	/////////////////////////////////////////////////
	/*キーのポインタを代入
	総キー数 = 設定元のキー数 / １つのキー数*/
	switch (type)
	{
		//ニュートラル
		case MOTION_NEUTORAL:
		{
			//キー情報と総キー数
			m_KeyDataBlend.KeyInfo = m_KeyNeutral;
			m_KeyDataBlend.nNumKey = sizeof(m_KeyNeutral) / sizeof(KEY_INFO);
			//ループ
			m_bAnimLoop = true;
			//ブレンド時のフレーム数設定
			m_nFrameBlend = NEUTORAL_BLEND_FRAME;
			break;
		}

		//歩き
		case MOTION_WALK:
		{
			//キー情報と総キー数
			m_KeyDataBlend.KeyInfo = m_KeyWalk;
			m_KeyDataBlend.nNumKey = sizeof(m_KeyWalk) / sizeof(KEY_INFO);
			//ループ
			m_bAnimLoop = true;
			//ブレンド時のフレーム数設定
			m_nFrameBlend = WALK_BLEND_FRAME;
			break;
		}

		//持ったまま待つ
		case MOTION_HOLD_WAIT:
		{
			//キー情報と総キー数
			m_KeyDataBlend.KeyInfo = m_KeyHoldWait;
			m_KeyDataBlend.nNumKey = sizeof(m_KeyHoldWait) / sizeof(KEY_INFO);
			//ループ
			m_bAnimLoop = true;
			//ブレンド時のフレーム数設定
			m_nFrameBlend = HOLD_BLEND_FRAME;
			break;
		}

		//持ったまま歩く
		case MOTION_HOLD_WALK:
		{
			//キー情報と総キー数
			m_KeyDataBlend.KeyInfo = m_KeyHoldWalk;
			m_KeyDataBlend.nNumKey = sizeof(m_KeyHoldWalk) / sizeof(KEY_INFO);
			//ループ
			m_bAnimLoop = true;
			//ブレンド時のフレーム数設定
			m_nFrameBlend = HOLDWALK_BLEND_FRAME;
			break;
		}

		//投げる
		case MOTION_THROW:
		{
			//キー情報と総キー数
			m_KeyDataBlend.KeyInfo = m_KeyThrow;
			m_KeyDataBlend.nNumKey = sizeof(m_KeyThrow) / sizeof(KEY_INFO);
			//ループ
			m_bAnimLoop = false;
			//ブレンド時のフレーム数設定
			m_nFrameBlend = THROW_BLEND_FRAME;
			break;
		}
	}

	//アニメーションに関する情報を設定
	m_MotionType = type;				//モーションタイプセット
	m_bBlend = true;					//モーションブレンド開始
	m_nCountBlend = 0;					//ブレンド用カウントリセット
	m_KeyDataBlend.nKey = 0;			//ブレンド先の現在のキーリセット
	m_KeyDataBlend.fMotionTime = 0.0f;	//ブレンド先の動作時間リセット
}
//=============================================================================
//モーションデータの読込
//=============================================================================
void CPlayer::LoadMotionData()
{
	///////////////////////////////////////////////////////
	//		CSVファイルからモーションデータの読込		//
	/////////////////////////////////////////////////////

	char data[MOTION_MAX][MAX_PATH] =
	{
		{ "data/ANM/animation_neutral.csv" },
		{ "data/ANM/animation_walk.csv" },
		{ "data/ANM/animation_hold_wait.csv" },
		{ "data/ANM/animation_hold_walk.csv" },
		{ "data/ANM/animation_throw.csv" }
	};

	//ファイルポインタ
	FILE *pFile;
	for (int nDataCnt = 0; nDataCnt < MOTION_MAX; ++nDataCnt)
	{
		//設定対象のキーフレームデータポインタ
		KEY_INFO *pKeyInfo = NULL;
		//総キー数
		int nMotionNum = 0;

		//設定対象のキーフレームとモーション数を決める
		switch (nDataCnt)
		{
			//ニュートラル
			case MOTION_NEUTORAL:
				pKeyInfo = m_KeyNeutral;
				nMotionNum = sizeof(m_KeyNeutral) / sizeof(KEY_INFO);
			break;

			//歩き
			case MOTION_WALK:
				pKeyInfo = m_KeyWalk;
				nMotionNum = sizeof(m_KeyWalk) / sizeof(KEY_INFO);
			break;

			//掴み状態の待ち
			case MOTION_HOLD_WAIT:
				pKeyInfo = m_KeyHoldWait;
				nMotionNum = sizeof(m_KeyHoldWait) / sizeof(KEY_INFO);
			break;

			//掴み状態の歩き
			case MOTION_HOLD_WALK:
				pKeyInfo = m_KeyHoldWalk;
				nMotionNum = sizeof(m_KeyHoldWalk) / sizeof(KEY_INFO);
			break;

			//投げる
			case MOTION_THROW:
				pKeyInfo = m_KeyThrow;
				nMotionNum = sizeof(m_KeyThrow) / sizeof(KEY_INFO);
			break;
		}

		//CSVファイルの読込
		pFile = fopen(data[nDataCnt], "r");

		//モーション数分ループ
		for (int i = 0; i < nMotionNum; ++i)
		{
			//パーツ数分ループ
			for (int j = 0; j < PART_MAX; ++j)
			{
				//座標・角度の読込
				fscanf(pFile, "%f,%f,%f,%f,%f,%f,", &pKeyInfo[i].key[j].PosX,
													&pKeyInfo[i].key[j].PosY,
													&pKeyInfo[i].key[j].PosZ,
													&pKeyInfo[i].key[j].RotX,
													&pKeyInfo[i].key[j].RotY,
													&pKeyInfo[i].key[j].RotZ);
			}

			//キーフレームのフレーム数の読込
			fscanf(pFile, "%d,", &pKeyInfo[i].Frame);
		}

		//ファイルを閉じる
		fclose(pFile);
	}
}
//=============================================================================
//プレイヤーパーツの準備
//=============================================================================
void CPlayer::SetupPlayerPart()
{
	//各パーツの生成
	//体
	m_pModel[PART_BODY] = CModel::Create("data/MODEL/light_body.x", m_pos, m_rot);

	//左手
	m_pModel[PART_HAND_LEFT] = CModel::Create("data/MODEL/light_hand_l.x",
		D3DXVECTOR3(19.0f, 35.0f, 0.0f),
		m_rot);
	//右手
	m_pModel[PART_HAND_RIGHT] = CModel::Create("data/MODEL/light_hand_r.x",
		D3DXVECTOR3(-19.0f, 35.0f, 0.0f),
		m_rot);

	//左足
	m_pModel[PART_FOOT_LEFT] = CModel::Create("data/MODEL/light_foot_l.x",
		D3DXVECTOR3(15.0f, 0.0f, 0.0f),
		m_rot);

	//右足
	m_pModel[PART_FOOT_RIGHT] = CModel::Create("data/MODEL/light_foot_r.x",
		D3DXVECTOR3(-15.0f, 0.0f, 0.0f),
		m_rot);

	//各パーツの階層構造の設定
	//体
	m_pModel[PART_BODY]->SetParent(NULL);
	//左手
	m_pModel[PART_HAND_LEFT]->SetParent(m_pModel[PART_BODY]);
	//右手
	m_pModel[PART_HAND_RIGHT]->SetParent(m_pModel[PART_BODY]);
	//左足
	m_pModel[PART_FOOT_LEFT]->SetParent(m_pModel[PART_BODY]);
	//右足
	m_pModel[PART_FOOT_RIGHT]->SetParent(m_pModel[PART_BODY]);

	//初期のモーションはニュートラル
	m_KeyData.KeyInfo = m_KeyNeutral;
	//キーの初期化
	m_KeyData.nKey = 0;
	m_KeyData.nNumKey = SHORT_MOTION;
}
//=============================================================================
//インスタンス生成
//=============================================================================
CPlayer *CPlayer::Create()
{
	//インスタンス生成
	CPlayer *pPlayer = new CPlayer();
	//初期化
	pPlayer->Init();
	return pPlayer;
}
//EOF