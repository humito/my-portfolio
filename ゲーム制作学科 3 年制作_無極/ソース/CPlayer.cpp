//=============================================================================
//プレイヤー処理[CPlayer.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CPlayer.h"
#include "CSceneX.h"
#include "CBullet.h"
#include "CEnemy.h"
#include "renderer.h"
#include "manager.h"
#include "Ccamera.h"
#include "CInputKeyboard.h"
#include "CInputJoystick.h"
#include "CMeshField.h"
#include "CParticle.h"
#include "CFace.h"
#include "CLife.h"
#include "CItemPossession.h"
#include "CObject.h"
#include "HitCheck.h"
#include "CGame.h"
#include "CSound.h"
#include "CToonShader.h"
#include "CShadow.h"

//デバッグ時
#ifdef _DEBUG
	#include "CDebugproc.h"
	#include "HitCheckSphere.h"
#endif

//*****************************************************************************
//定数定義
//*****************************************************************************
#define PLAYER_DEFAULT_POS_X (1000.0f)				//プレイヤーの既定X位置
#define PLAYER_SIZE_X (50.0f)						//プレイヤーサイズX
#define PLAYER_SIZE_Z (50.0f)						//プレイヤーサイズZ
#define PLAYER_USE_SIZE (350.0f)					//プレイヤーの有効範囲
#define POS_MOVE (5.5f)								//プレイヤー移動量
#define PLAYER_CENTER (60.0f)						//プレイヤーの中心(腹の位置)
#define POS_JUMP_MOVE_PROPORTION (0.1f)				//ジャンプ量の割合
#define POS_JUMP_DOWN (0.6f)						//ジャンプ減算量
#define ADD_POS_Y (200.0f)							//Y座標の加算量
#define MOVE_DEST_PROPORTION (0.5f)					//プレイヤーの移動量を減らす割合
#define PARTICLE_MOVETO_PLAYER_PROPORTION (0.08f)	//パーティクルがプレイヤーへ移動する割合
#define PLAYER_RETURN_COUNT (200)					//復帰時のカウント
#define PLAYER_USE_PRIORITY (2)						//プレイヤーが使用するプライオリティ
#define LIMIT_SOUND_COUNT (5)						//サウンド再生回数の上限
#define ELAPSED_CERTAIN (5)							//一定の経過カウント(アイテム取得再生する際に)

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
CShadow *CPlayer::m_pShadow=NULL;//影インスタンス

//=============================================================================
//コンストラクタ
//=============================================================================
CPlayer::CPlayer()
{
	//プレイヤーオブジェクト代入
	m_objType=OBJECT_TYPE_PLAYER;
	//当たり判定用球体NULLセット
	m_pSphere=NULL;
}
//=============================================================================
//デストラクタ
//=============================================================================
CPlayer::~CPlayer()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CPlayer::Init()
{
	//カメラインスタンス取得
	CCamera *m_pCamera=CManager::GetCamera();

	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//モデルの設定
	m_pos=D3DXVECTOR3(PLAYER_DEFAULT_POS_X,0.0f,0.0f);	//座標
	m_posMove=D3DXVECTOR3(0.0f,0.0f,0.0f);				//プレイヤーの移動量
	m_rot=D3DXVECTOR3(0.0f,0.0f,0.0f);					//角度
	m_rotDestModel=D3DXVECTOR3(0.0f,0.0f,0.0f);			//目的の角度
	m_scl=D3DXVECTOR3(1.0f,1.0f,1.0f);					//サイズ

	m_nNumItem=0;										//アイテム所持数
	m_nDownCnt=0;										//ダウンしている間のカウント
	m_nSoundCnt=0;										//アイテム取得効果音の再生回数
	m_nElapsedCnt=0;									//経過カウント
	m_fDestPosY=0.0f;									//プレイヤージャンプ後の高さ
	m_bDisp=true;										//表示フラグ
	m_bJumpFlag=false;									//ジャンプフラグ
	m_bLandFlag=true;									//着地フラグ

	m_nAnimeNum=0;										//アニメーション番号
	m_animeType=POSE_ANIME;								//アニメーションの種類

	//アニメーション情報付きXファイルを読み込む
	if(FAILED(D3DXLoadMeshHierarchyFromX(	"data/MODEL/ninja.x",
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
	m_pAnimSet=new LPD3DXANIMATIONSET[m_pAnimController->GetNumAnimationSets()];

	//アニメーション数分ループ
	for(unsigned int i=0;i<m_pAnimController->GetNumAnimationSets();i++)
	{
		//NULLセット
		m_pAnimSet[i]=NULL;
		//アニメーションの取得
		m_pAnimController->GetAnimationSet(i,&m_pAnimSet[i]);
	}

	//アニメーションのセット
	CSceneX::SetAnimation(POSE_ANIME,0.01,true);

	//カメラ固定フラグをはずす
	m_pCamera->SetPosLock(false);

	//影インスタンスを生成
	m_pShadow=CShadow::Create(PLAYER_SIZE_X,PLAYER_SIZE_Z);

//デバッグ
#ifdef _DEBUG
	//当たり判定用の球体を生成
	m_pSphere=CHitCheckSphere::Create(PLAYER_CENTER);
#endif

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CPlayer::Uninit()
{
	//影の終了
	m_pShadow->Uninit();

	//終了処理
	CSceneX::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CPlayer::Update()
{
	//カメラインスタンス取得
	CCamera *m_pCamera=CManager::GetCamera();

	//カメラの向き取得
	D3DXVECTOR3 rot=m_pCamera->GetRotCamera();

	///////////////////////////////////
	//			操作処理			//
	/////////////////////////////////

	//着地していてかつ移動キーを押している場合
	if(	(CInputKeyboard::GetKeyPress(DIK_W)			||
		CInputKeyboard::GetKeyPress(DIK_S)			||
		CInputKeyboard::GetKeyPress(DIK_A)			||
		CInputKeyboard::GetKeyPress(DIK_D))			||
		(CInputJoystick::GetPadPress(STICK_L_UP)	||
		CInputJoystick::GetPadPress(STICK_L_DOWN)	||
		CInputJoystick::GetPadPress(STICK_L_LEFT)	||
		CInputJoystick::GetPadPress(STICK_L_RIGHT))	&&
		m_bLandFlag==true && m_animeType!=DOWN_ANIME)
	{
		//アニメーションタイプを走りにする
		m_animeType=RUN_ANIME;
	}
	else if(m_bLandFlag==true && m_animeType!=DOWN_ANIME)
	{
		//着地のみの場合はアニメーションタイプをポーズにする
		m_animeType=POSE_ANIME;
	}

	//弾の発射
	if(	(CInputKeyboard::GetKeyTrigger(DIK_H)	||
		CInputJoystick::GetPadTrigger(BUTTON_3)	||
		CInputJoystick::GetPadTrigger(BUTTON_4)	||
		CInputJoystick::GetPadTrigger(TRIGGER_R))&&
		m_animeType!=DOWN_ANIME)
	{
		//アニメーション状態を発射にする
		m_animeType=SHOT_ANIME;

		//弾インスタンス生成
		CBullet::Create(D3DXVECTOR3(m_pos.x,m_pos.y+PLAYER_CENTER,m_pos.z),m_rot,D3DXVECTOR3(10.0f,0.0f,10.0f));
	}

	//前
	if(CInputKeyboard::GetKeyPress(DIK_W) ||
		CInputJoystick::GetPadPress(STICK_L_UP))
	{
		m_posMove.x-=cosf(rot.y+D3DX_PI/2)*POS_MOVE;//X
		m_posMove.z+=sinf(rot.y+D3DX_PI/2)*POS_MOVE;//Z

		m_rotDestModel.y=rot.y+D3DX_PI;//目的の向き
	}
	
	//後
	if(CInputKeyboard::GetKeyPress(DIK_S) ||
		CInputJoystick::GetPadPress(STICK_L_DOWN))
	{
		m_posMove.x+=cosf(rot.y+D3DX_PI/2)*POS_MOVE;//X
		m_posMove.z-=sinf(rot.y+D3DX_PI/2)*POS_MOVE;//Z

		m_rotDestModel.y=rot.y;//目的の向き
	}

	//左
	if(CInputKeyboard::GetKeyPress(DIK_A) ||
		CInputJoystick::GetPadPress(STICK_L_LEFT))
	{
		m_posMove.x-=sinf(rot.y+D3DX_PI/2)*POS_MOVE;//X
		m_posMove.z-=cosf(rot.y+D3DX_PI/2)*POS_MOVE;//Z

		m_rotDestModel.y=rot.y+D3DX_PI/2;//目的の向き
	}

	//右
	if(CInputKeyboard::GetKeyPress(DIK_D) ||
		CInputJoystick::GetPadPress(STICK_L_RIGHT))
	{
		m_posMove.x+=sinf(rot.y+D3DX_PI/2)*POS_MOVE;//X
		m_posMove.z+=cosf(rot.y+D3DX_PI/2)*POS_MOVE;//Z

		m_rotDestModel.y=rot.y-D3DX_PI/2;//目的の向き
	}

	///////////////////////////////////////////////
	//			斜め入力の時は向きのみ			//
	/////////////////////////////////////////////

	//右上
	if((CInputKeyboard::GetKeyPress(DIK_UP)		&&
		CInputKeyboard::GetKeyPress(DIK_RIGHT))		||
		(CInputJoystick::GetPadPress(STICK_L_UP)	&&
		CInputJoystick::GetPadPress(STICK_L_RIGHT)))
	{
		m_rotDestModel.y=rot.y-(D3DX_PI/4)*3;//目的の向き
	}

	//右下
	if((CInputKeyboard::GetKeyPress(DIK_DOWN)	&&
		CInputKeyboard::GetKeyPress(DIK_RIGHT))	||
		(CInputJoystick::GetPadPress(STICK_L_DOWN)&&
		CInputJoystick::GetPadPress(STICK_L_RIGHT)))
	{
		m_rotDestModel.y=rot.y-D3DX_PI/4;//目的の向き
	}

	//左上
	if((CInputKeyboard::GetKeyPress(DIK_UP)		&&
		CInputKeyboard::GetKeyPress(DIK_LEFT))	||
		(CInputJoystick::GetPadPress(STICK_L_UP)&&
		CInputJoystick::GetPadPress(STICK_L_LEFT)))
	{
		m_rotDestModel.y=rot.y+(D3DX_PI/4)*3;//目的の向き
	}

	//左下
	if((CInputKeyboard::GetKeyPress(DIK_DOWN)	&&
		CInputKeyboard::GetKeyPress(DIK_LEFT))	||
		(CInputJoystick::GetPadPress(STICK_L_DOWN)&&
		CInputJoystick::GetPadPress(STICK_L_LEFT)))
	{
		m_rotDestModel.y=rot.y+D3DX_PI/4;//目的の向き
	}

	///////////////////////////////
	//		ジャンプ処理		//
	/////////////////////////////

	//スペースキーでジャンプ
	if(	CInputKeyboard::GetKeyTrigger(DIK_SPACE)||
		CInputJoystick::GetPadTrigger(BUTTON_1)	||
		CInputJoystick::GetPadTrigger(BUTTON_2))
	{
		//現在のジャンプフラグがfalseの場合
		if(	m_bJumpFlag==false &&
			m_bLandFlag==true &&
			m_animeType!=DOWN_ANIME)
		{
			//目的の高さを設定する
			m_fDestPosY=m_pos.y+ADD_POS_Y;
			m_bJumpFlag=true;
			m_bLandFlag=false;

			//アニメーションタイプを走りに変更
			m_animeType=ROCKET_ANIME;
		}//空中にいる場合でジャンプキーが押されたら
		else if(m_bLandFlag==false &&
				m_nNumItem!=0)
		{
			//花火(パーティクル)の生成
			CParticle::Create(TYPE_FIREWORKS,m_nNumItem,m_pos,50.0f,50.0f);

			//ライフを減算
			CLife::DestLife();

			//所持アイテム数リセット
			m_nNumItem=0;

			//プレイヤー非表示
			m_bDisp=false;

			//影を非表示にする
			m_pShadow->SetDisp(false);

			//喜び顔をセットする
			CFace::SetFace(HAPPY_FACE);

			//花火発射効果音再生
			CSound::PlaySoundA(SOUND_LABEL_SE_FIREWORKS);

			//カメラを固定させる
			m_pCamera->SetPosLock(true);
		}
	}
	else
	{
		//ジャンプフラグをfalseにし、Y座標を一定量減算
		if(m_bJumpFlag==true)
		{
			m_bJumpFlag=false;
		}

		//プレイヤーY座標を一定量減算
		m_posMove.y-=POS_JUMP_DOWN;
	}

	//ジャンプフラグtrueなら
	if(m_bJumpFlag==true)
	{
		//Y座標を目標の高さとの割合分上げる
		m_posMove.y=(m_fDestPosY-m_pos.y)*POS_JUMP_MOVE_PROPORTION;
	}

	///////////////////////////////////
	//			向きの旋回			//
	/////////////////////////////////

	//目的の向きと現在の向きの角度の差を求める
	float fDiffRotY=m_rotDestModel.y-m_rot.y;

	//差分補正
	//右上 270°
	if(fDiffRotY>D3DX_PI)
	{
		fDiffRotY=fDiffRotY-D3DX_PI*2;//360°減算
	}

	//右上 -270°
	if(fDiffRotY<-D3DX_PI)
	{
		fDiffRotY=fDiffRotY+D3DX_PI*2;//360°加算
	}

	//現在のモデルの角度(向き)を加算
	m_rot.y+=fDiffRotY*0.1f;

	//角度補正
	//モデルの角度が3.14より大きい場合
	if(m_rot.y>D3DX_PI)
	{
		m_rot.y=-D3DX_PI;
	}

	//モデルの角度が-3.14より小さい場合
	if(m_rot.y<-D3DX_PI)
	{
		m_rot.y=D3DX_PI;
	}

	///////////////////////////////////////////
	//		プレイヤーの慣性による移動		//
	/////////////////////////////////////////

	//プレイヤーの前座標を更新
	m_posOld=m_pos;

	//アニメ状態がダウン時でないなら座標更新する
	if(m_animeType!=DOWN_ANIME)
	{
		//プレイヤーの座標を移動量分加算
		m_pos+=m_posMove;
	}

	//移動量の減算
	m_posMove=D3DXVECTOR3(	m_posMove.x-(m_posMove.x*MOVE_DEST_PROPORTION),
							m_posMove.y,
							m_posMove.z-(m_posMove.z*MOVE_DEST_PROPORTION));

	///////////////////////////////////////////////
	//		各オブジェクトの当たり判定処理		//
	/////////////////////////////////////////////

	//プライオリティ数分ループ
	for(int i=0;i<PLAYER_USE_PRIORITY;i++)
	{
		//プライオリティの先頭からシーンポインタを取得
		CScene *scene=CScene::GetListTop((i+1)+(i/1));

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
					//表示されているオブジェクトのみ当たり判定させる
					if(scene->GetDisp())
					{
					
						//シーンをオブジェクトインスタンスにキャスト変換
						CObject *pObject=(CObject*)scene;

						//オブジェクトのサイズを取得
						float fSizeX,fSizeY,fSizeZ;
						pObject->GetSize(&fSizeX,&fSizeY,&fSizeZ);

						//オブジェクトの座標を取得
						D3DXVECTOR3 objPos=pObject->GetPos();

						//オブジェクトとプレイヤーが当たった場合
						if(	CubeCheck(objPos,m_pos,fSizeX,0.0f,fSizeZ,PLAYER_SIZE_X,0.0f,PLAYER_SIZE_Z) &&
							m_pos.y+PLAYER_CENTER>=objPos.y && m_pos.y+PLAYER_CENTER<=objPos.y+fSizeY)
						{
							///////////////////////////////////////////
							//		めり込んだ分座標を引き戻す		//
							/////////////////////////////////////////
							D3DXVECTOR3 vec1=objPos-m_posOld;	//前座標とオブジェクトの方向ベクトル
							D3DXVECTOR3 vec2=objPos-m_pos;		//現在とオブジェクトのの方向ベクトル
							D3DXVECTOR3 distVec=vec1-vec2;		//２つの方向ベクトルの差分
						
							//プレイヤーのX座標だけ差し引いた座標を用意
							D3DXVECTOR3 posDist=D3DXVECTOR3(m_pos.x-distVec.x,m_pos.y,m_pos.z);
						
							//差し引いた結果オブジェクトと当たる場合
							if(CubeCheck(objPos,posDist,fSizeX,0.0f,fSizeZ,PLAYER_SIZE_X,0.0f,PLAYER_SIZE_Z))
							{
								//プレイヤーのZ座標を差分だけ戻す
								m_pos.z-=distVec.z;
							}
							else
							{
								//X座標差し引いた結果をプレイヤーの座標へ代入
								m_pos=posDist;
							}
						}//当たり判定チェック if 終端
					}//表示フラグチェック if 終端

					break;
				}

				///////////////////////////////////////////////////////
				//		アイテム(パーティクル)との当たり判定		//
				/////////////////////////////////////////////////////
				case OBJECT_TYPE_PARTICLE:
				{
					//パーティクルインスタンスにキャスト変換
					CParticle *pParticle=(CParticle*)scene;

					//パーティクルタイプがアイテムの場合のみ当たり判定をする
					if(pParticle->GetType()==TYPE_ITEM)
					{
						//パーティクル数取得
						int nNumParticle=pParticle->GetNum();

						//パーティクル数分ループ
						for(int i=0;i<nNumParticle;i++)
						{
							//パーティクルの座標取得
							D3DXVECTOR3 parPos=pParticle->GetParticlePos(i);

							//パーティクルがフィールドに着地してるかフラグを取得
							bool bLand=pParticle->CheckLand(i);
							//パーティクルの使用フラグ取得
							bool bUse=pParticle->GetUseFlag(i);

							//パーティクル座標がプレイヤーの有効範囲内の場合
							if(	CubeCheck(m_pos,parPos,PLAYER_USE_SIZE,0.0f,PLAYER_USE_SIZE,ITEM_SIZE,0.0f,ITEM_SIZE) &&
								bLand==true && m_animeType!=DOWN_ANIME)
							{
								//移動量がプレイヤーの方向へ向くように設定
								D3DXVECTOR3 velocity=(D3DXVECTOR3(m_pos.x,m_pos.y+PLAYER_CENTER,m_pos.z)-parPos)*PARTICLE_MOVETO_PLAYER_PROPORTION;

								//パーティクルの速度をセット
								pParticle->SetVelocity(i,velocity);
							}

							//パーティクルとプレイヤーが当たった場合
							if(	CubeCheck(parPos,m_pos,ITEM_SIZE,0.0f,ITEM_SIZE,PLAYER_SIZE_X,0.0f,PLAYER_SIZE_Z) &&
								bLand==true && bUse==true && m_animeType!=DOWN_ANIME)
							{
								//パーティクルを非表示にする
								pParticle->SetUseFlag(i,false);

								//アイテム数加算
								m_nNumItem++;

								//アイテム取得音再生カウントアップ
								m_nSoundCnt++;
							}

						}//パーティクル数分 for ループ 終端
					}//アイテムタイプチェック if 終端
					break;
				}

				///////////////////////////////////
				//		敵との当たり判定		//
				/////////////////////////////////
				case OBJECT_TYPE_ENEMY:
				{
					//表示されている敵だけ当たり判定させる
					if(scene->GetDisp())
					{
						//敵インスタンスへキャスト変換
						CEnemy *pEnemy=(CEnemy*)scene;

						//敵の座標を取得
						D3DXVECTOR3 enemyPos=pEnemy->GetPos();

						//敵のサイズを取得
						float fSizeX,fSizeY,fSizeZ;
						pEnemy->GetSize(&fSizeX,&fSizeY,&fSizeZ);

						//敵とプレイヤーとの当たり判定
						if(	CubeCheck(enemyPos,m_pos,fSizeX,0.0f,fSizeZ,PLAYER_SIZE_X,0.0f,PLAYER_SIZE_Z) &&
							m_pos.y+PLAYER_CENTER>enemyPos.y && m_pos.y+PLAYER_CENTER<enemyPos.y+fSizeY)
						{
							m_animeType=DOWN_ANIME;
						}
					}//表示フラグチェック if 終端

					break;
				}
			}//switch 終端

			//次ポインタへ移動
			scene=pNext;

		}//scene whileループ 終端
	}//priority forループ 終端

	///////////////////////////////////////////////
	//		フィールド範囲外(壁)当たり判定		//
	/////////////////////////////////////////////

	//フィールド横幅範囲外の場合
	if(	m_pos.x>(FIELD_NUM_X/2)*FIELDSIZE_X ||
		m_pos.x<-((FIELD_NUM_X/2)*FIELDSIZE_X))
	{
		//プレイヤー座標を元に戻す
		m_pos.x=m_posOld.x;
	}

	//フィールド奥行範囲外の場合
	if(	m_pos.z>(FIELD_NUM_Z/2)*FIELDSIZE_Z ||
		m_pos.z<-((FIELD_NUM_Z/2)*FIELDSIZE_Z))
	{
		//プレイヤー座標を元に戻す
		m_pos.z=m_posOld.z;
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
		m_posMove.y=0;
		m_bLandFlag=true;
	}

	//プレイヤーダウン後非表示にする
	if(	m_animeType==DOWN_ANIME && m_bStop==true &&
		m_bDisp==true)
	{
		//ライフを１つ減らす
		CLife::DestLife();

		//もっているアイテム数分パーティクルを発生させる
		CParticle::Create(TYPE_ITEM,m_nNumItem,m_pos,ITEM_SIZE,ITEM_SIZE);

		//アイテム数のリセット
		m_nNumItem=0;

		//プレイヤー非表示
		m_bDisp=false;

		//やられ顔をセットする
		CFace::SetFace(BEATEN_FACE);

		//影を非表示にする
		m_pShadow->SetDisp(false);

		//アイテム取得音停止
		CSound::StopSound(SOUND_LABEL_SE_ITEM);

		//ダメージ音再生
		CSound::PlaySoundA(SOUND_LABEL_SE_DAMAGE);
	}

	//プレイヤー復帰処理(非表示でかつダウンカウントが一定に達したら)
	if(m_bDisp==false && m_nDownCnt>PLAYER_RETURN_COUNT)
	{
		//初期の位置に戻す
		m_pos=D3DXVECTOR3(PLAYER_DEFAULT_POS_X,0.0f,0.0f);

		//アニメーション状態をポーズにする
		m_animeType=POSE_ANIME;

		//モデルを表示させる
		m_bDisp=true;

		//ダウンしている間のカウントリセット
		m_nDownCnt=0;

		//カメラ固定フラグをはずす
		m_pCamera->SetPosLock(false);

		//影を表示させる
		m_pShadow->SetDisp(true);

		//ゲーム内各要素のリセット
		CGame::Reset();
	}
	else if(m_bDisp==false)
	{
		//ダウン時(非表示)の間カウントアップ
		m_nDownCnt++;
	}

	//プレイヤーアニメーションの更新
	PlayAnimation();

	///////////////////////////////////////////////////
	//		アイテム数を各インスタンスへセット		//
	/////////////////////////////////////////////////

	//アイテム所持数のセット
	CPossessionNum::SetNum(m_nNumItem);

	//表示されている時のみ表情インスタンスに値をセット
	if(m_bDisp)
	{
		//アイテム数セット
		CFace::SetItemNum(m_nNumItem);
		//通常の顔をセットする
		CFace::SetFace(NORMAL_FACE);
	}

	//座標を影にセット
	m_pShadow->SetPos(m_pos);

	///////////////////////////////////////////
	//		アイテム取得効果音を再生		//
	/////////////////////////////////////////

	//取得効果音を鳴らす回数に上限を付ける
	if(m_nSoundCnt>LIMIT_SOUND_COUNT)
	{
		m_nSoundCnt=LIMIT_SOUND_COUNT;
	}

	//1フレームに1回再生させる
	if(m_nSoundCnt>0 && m_nElapsedCnt%ELAPSED_CERTAIN==0)
	{
		//アイテム取得音再生
		CSound::PlaySoundA(SOUND_LABEL_SE_ITEM);
		//再生回数を下げる
		m_nSoundCnt--;
	}

	//経過カウントアップ
	m_nElapsedCnt++;

//デバッグ
#ifdef _DEBUG
	//当たり判定用球体ポリゴンに自身の座標をセット
	m_pSphere->SetPos(D3DXVECTOR3(m_pos.x,m_pos.y+PLAYER_CENTER,m_pos.z));
#endif
}
//=============================================================================
//描画
//=============================================================================
void CPlayer::Draw()
{
	//表示フラグtrueのみ描画処理を開始
	if(m_bDisp==true)
	{
		//レンダラー情報取得
		CRenderer *pRenderer=CManager::GetRenderer();

		//デバイスのゲット
		LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

		D3DXMATRIX mtxScl,mtxRot,mtxTranslate;//サイズ,回転,位置
	
		//ワールドマトリックスを初期化
		D3DXMatrixIdentity(&m_mtxWorld);

		//サイズを設定
		D3DXMatrixScaling(	&mtxScl,
							m_scl.x,
							m_scl.y,
							m_scl.z);

		//サイズを反映
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

		//位置を設定
		D3DXMatrixTranslation(	&mtxTranslate,
								m_pos.x,
								m_pos.y,
								m_pos.z);

		//位置を反映
		D3DXMatrixMultiply(	&m_mtxWorld,&m_mtxWorld,
							&mtxTranslate);

		//ワールドマトリックスのセット
		pDevice->SetTransform(D3DTS_WORLD,&m_mtxWorld);

		//フレームのマトリクスを変換
		m_alloc.MatricesFrame(m_pFrameRoot,&m_mtxWorld);

		//トゥーンシェーダー開始
		CToonShader::Begin();

		//フレームの描画
		m_alloc.DrawFrame(pDevice,m_pFrameRoot);

		//トゥーンシェーダー終了
		CToonShader::End();
	}
}
//=============================================================================
//プレイヤーアニメーションの更新
//=============================================================================
void CPlayer::PlayAnimation(void)
{
	///////////////////////////////////////
	//		アニメーションの更新		//
	/////////////////////////////////////

	//アニメーションのタイプが変わってかつループ中の状態か再生終了時の場合
	if(((m_bLoop==false && m_bStop==true) || m_bLoop==true))
	{
		//走りとポーズ時はループさせる
		if(	m_animeType==RUN_ANIME ||
			m_animeType==POSE_ANIME)
		{
			CSceneX::SetAnimation(m_animeType,0.01,true);
		}//ループなしのアニメーションはは前の状態と違う場合のみ
		else if(m_animeType!=m_animePrevType)
		{
			//その他は終わりまで再生
			CSceneX::SetAnimation(m_animeType,0.03,false);
		}
	}

	//アニメーションの再生
	CSceneX::PlayAnimation();

	//前のアニメーションタイプを保存
	m_animePrevType=m_animeType;
}
//=============================================================================
//所持アイテム数取得
//=============================================================================
int CPlayer::GetItemNum(void)
{
	return m_nNumItem;
}
//=============================================================================
//プレイヤー角度取得
//=============================================================================
D3DXVECTOR3 CPlayer::GetRotation(void)
{
	return m_rot;
}
//=============================================================================
//プレイヤーインスタンス生成
//=============================================================================
CPlayer *CPlayer::Create()
{
	//プレイヤーインスタンス生成
	CPlayer *pPlayer=new CPlayer();

	//プレイヤー初期化
	pPlayer->Init();

	return pPlayer;
}
//EOF