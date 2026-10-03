//=============================================================================
//ゲームメイン処理[game.cpp]
//Author:HUMITO KIMURA
//TODO:アイテムボックスの作成
//TODO:パーティクルを作成
//TODO:氷を一気に消す処理追加
//TODO:サウンド実装
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "game.h"
//*****************************************************************************
//定数定義(内部)
//*****************************************************************************
#define ADD_QUBE_SPEED (500.3f)//加算する立方体の移動量
//*****************************************************************************
//グローバル変数
//*****************************************************************************
static PLAYER g_player;
static QUBE   g_qube[QUBE_MAX];
static ENEMYA g_naturalEnemy[ENEMYA_MAX];
static ENEMYB g_enemy[ENEMYB_MAX];
static D3DXVECTOR3 g_posCameraP;
static BULLET g_bullet[BULLET_MAX];
static int    g_nCnt=0;
static int    g_nAttack=0;
static bool	  g_bPause=false;
//=============================================================================
//ゲーム内の初期化
//=============================================================================
HRESULT InitGame()
{
	//カウントリセット
	g_nCnt=0;
	//ヒットカウント
	g_nAttack=0;

	//カメラの初期化
	InitCamera();

	//ライトの初期化
	InitLight();

	//プレイヤーの初期化
	InitPlayer();

	//天敵の初期化
	InitEnemyA();

	//敵の初期化
	InitEnemyB();

	//敵爆発エフェクト初期化
	IniEneExp();

	//弾の初期化
	InitBullet();

	//弾エフェクトの初期化
	InitEffect();

	//弾爆発エフェクトの初期化
	InitBulExp();

	//フィールドの初期化
	InitMeshField(2,2,500.0f,500.0f);

	//壁の初期化
	InitWall(-4000.0f,0.0f,4000.0f,0.0f,2,2,4000.0f,2000.0f);

	//立方体の初期化
	InitQube(0.0f,0.0f,0.0f,500.0f,500.0f);

	//時間の初期化
	InitTime();

	//インフォメーションの初期化
	InitInfo();

	//ベジエ曲線の初期化
	InitBezier();

	//ポーズ画面の初期化
	InitPPolygon();

	//カーソルの初期化
	InitCuPolygon();

	//スコアの初期化
	InitScore();

	return S_OK;
}
//=============================================================================
//ゲーム内の更新
//=============================================================================
void UpdateGame()
{
	///////////////////////////
	//	各オブジェクト更新	//
	/////////////////////////

	//Pが押されたらポーズ切替
	if(GetKeyboardRelease(DIK_P))
	{
		//ポーズフラグtrue
		g_bPause=true;
	}

	//ポーズフラグチェック
	if(g_bPause!=false)
	{
		///////////////////////
		//	ポーズ画面内	//
		/////////////////////

		//ポーズ更新
		UpdatePPolygon();

		//カーソル更新
		UpdateCuPolygon();
	}
	else
	{
		///////////////////////
		//	ゲーム画面内	//
		/////////////////////

		//カメラの更新処理
		UpdateCamera();

		//フィールドの更新
		UpdateMeshField();

		//壁の更新
		UpdateWall();

		//プレイヤーの更新
		UpdatePlayer();

		//天敵の更新
		UpdateEnemyA();

		//敵の更新
		UpdateEnemyB();

		//敵爆発エフェクト更新
		UpdateEneExp();

		//弾の更新
		UpdateBullet();

		//弾エフェクトの更新
		UpdateEffect();

		//弾爆発エフェクトの更新
		UpdateBulExp();

		//立方体の更新
		UpdateQube();

		//時間の更新
		UpdateTime();

		//インフォメーションの更新
		UpdateInfo();

		///////////////////////////////////
		//	各オブジェクト情報の取得	//
		/////////////////////////////////

		//カメラ座標の取得
		g_posCameraP=GetPosCamera();

		//プレイヤー構造体の取得
		g_player=GetPlayer();

		//弾の数分ループ
		for(int i=0;i<BULLET_MAX;i++)
		{
			//弾の取得
			g_bullet[i]=GetBullet(i);
		}

		//天敵の数分ループ
		for(int i=0;i<ENEMYA_MAX;i++)
		{
			//天敵情報の取得
			g_naturalEnemy[i]=GetEnemyA(i);
		}

		//敵の数分ループ
		for(int i=0;i<ENEMYB_MAX;i++)
		{
			//敵情報の取得
			g_enemy[i]=GetEnemyB(i);
		}

		//立方体の数分ループ
		for(int i=0;i<QUBE_MAX;i++)
		{
			//立方体構造体の取得
			GetQube(i,&g_qube[i]);
		}

		/////////////////////////////////////////////////////////////////////
		//							当たり判定処理						  //
		///////////////////////////////////////////////////////////////////

		///////////////////////////////
		//	天敵に関する当たり判定	//
		//////////////////////////////
		//天敵の数分ループ
		for(int i=0;i<ENEMYA_MAX;i++)
		{
			//弾の数分ループ
			for(int j=0;j<BULLET_MAX;j++)
			{
				//弾と天敵の当たり判定
				if(HitQube(g_bullet[j].pos.x-g_bullet[j].fSize,g_bullet[j].pos.x+g_bullet[j].fSize,
						   g_bullet[j].pos.y-g_bullet[j].fSize,g_bullet[j].pos.y+g_bullet[j].fSize,
						   g_bullet[j].pos.z-50.0f,g_bullet[j].pos.z+50.0f,
						   g_naturalEnemy[i].pos.x-g_naturalEnemy[i].fSize,g_naturalEnemy[i].pos.x+g_naturalEnemy[i].fSize,
						   g_naturalEnemy[i].pos.y-g_naturalEnemy[i].fSize,g_naturalEnemy[i].pos.y+g_naturalEnemy[i].fSize,
						   g_naturalEnemy[i].pos.z-50.0f,g_naturalEnemy[i].pos.z+50.0f
						   ))
				{
					//敵のヒットフラグtrueにする
					HitEnemyA(i);
					AddScore(1);
				}
			}
		}

		///////////////////////////////
		//	敵に関する当たり判定	//
		/////////////////////////////
		for(int i=0;i<ENEMYB_MAX;i++)
		{
			//敵とプレイヤーの当たり判定
			if(HitQube(g_player.pos.x-g_player.fSize,g_player.pos.x+g_player.fSize,
						g_player.pos.y-g_player.fSize,g_player.pos.y+g_player.fSize,
						g_player.pos.z-g_player.fSize,g_player.pos.z+g_player.fSize,
						g_enemy[i].pos.x-g_enemy[i].fSize,g_enemy[i].pos.x+g_enemy[i].fSize,
						g_enemy[i].pos.y-g_enemy[i].fSize,g_enemy[i].pos.y+g_enemy[i].fSize,
						g_enemy[i].pos.z-30.0f,g_enemy[i].pos.z+30.0f
						) && g_player.type==TYPE_ATTACK)
			{
				//爆発エフェクトの生成
				CreateEneExp(g_enemy[i].pos.x,g_enemy[i].pos.y,g_enemy[i].pos.z,g_enemy[i].fSize+10.0f);
				//敵の状態を変更し画面外へ
				HitEnemyB(i,NULL,NULL,TYPE_ATTACK);
				//爆発サウンド再生
				PlaySound(SOUND_LABEL_SE_EXPLOSION);
				//スコア加算
				AddScore(10);
			}

			//敵と弾の当たり判定
			for(int j=0;j<BULLET_MAX;j++)
			{
				if(HitQube(g_bullet[j].pos.x-g_bullet[j].fSize,g_bullet[j].pos.x+g_bullet[j].fSize,
						   g_bullet[j].pos.y-g_bullet[j].fSize,g_bullet[j].pos.y+g_bullet[j].fSize,
						   g_bullet[j].pos.z-50.0f,g_bullet[j].pos.z+50.0f,
						   g_enemy[i].pos.x-g_enemy[i].fSize,g_enemy[i].pos.x+g_enemy[i].fSize,
						   g_enemy[i].pos.y-g_enemy[i].fSize,g_enemy[i].pos.y+g_enemy[i].fSize,
						   g_enemy[i].pos.z-30.0f,g_enemy[i].pos.z+30.0f
						   ))
				{
					//氷の立方体を敵の座標に設置
					CreateQube(g_enemy[i].pos.x,g_enemy[i].pos.z,g_enemy[i].fSize*2,g_enemy[i].fSize*2);
					//敵の状態を変更し画面外へ
					HitEnemyB(i,NULL,NULL,TYPE_SHOT);
				}
			}

			//敵と立方体の当たり判定
			for(int j=0;j<QUBE_MAX;j++)
			{
				//敵と立方体の当たり判定
				for(int j=0;j<QUBE_MAX;j++)
				{
					if(HitQube(g_qube[j].pos.x-g_qube[j].fHalfWidth,g_qube[j].pos.x+g_qube[j].fHalfWidth,
							   g_qube[j].pos.y-g_qube[j].fHalfWidth,g_qube[j].pos.y+g_qube[j].fHalfWidth,
							   g_qube[j].pos.z-g_qube[j].fHalfWidth,g_qube[j].pos.z+g_qube[j].fHalfWidth,
							   g_enemy[i].pos.x-g_enemy[i].fSize,g_enemy[i].pos.x+g_enemy[i].fSize,
							   g_enemy[i].pos.y-g_enemy[i].fSize,g_enemy[i].pos.y+g_enemy[i].fSize,
							   g_enemy[i].pos.z-30.0f,g_enemy[i].pos.z+30.0f
							   ))
					{
						//爆発エフェクトの生成
						CreateEneExp(g_enemy[i].pos.x,g_enemy[i].pos.y,g_enemy[i].pos.z,g_enemy[i].fSize+10.0f);
						//敵の状態を変更し画面外へ
						HitEnemyB(i,NULL,NULL,TYPE_ATTACK);
						//スコア加算
						AddScore(50);
					}
				}
			}
		}

		///////////////////////////////////
		//	立方体に関する当たり判定	//
		/////////////////////////////////

		//立方体の数分ループ
		for(int i=0;i<QUBE_MAX;i++)
		{
			//移動量変数
			float fMoveX,fMoveZ;

			//立方体とプレイヤーの当たり判定
			if(HitQube(g_player.pos.x-g_player.fSize,g_player.pos.x+g_player.fSize,
				g_player.pos.y-g_player.fSize,g_player.pos.y+g_player.fSize,
				g_player.pos.z-g_player.fSize,g_player.pos.z+g_player.fSize,
				g_qube[i].pos.x-g_qube[i].fHalfWidth,g_qube[i].pos.x+g_qube[i].fHalfWidth,
				g_qube[i].pos.y-g_qube[i].fHalfHeight,g_qube[i].pos.y+g_qube[i].fHalfHeight,
				g_qube[i].pos.z-g_qube[i].fHalfWidth,g_qube[i].pos.z+g_qube[i].fHalfWidth) &&
				g_player.type==TYPE_ATTACK)
			{

				//ヒットした場合プレイヤーと立方体の距離の差分(X,Z)を求め、そこから移動量を決める
				if(g_player.pos.x<g_qube[i].pos.x)
				{
					fMoveX=ADD_QUBE_SPEED;
				}
				else
				{
					fMoveX=-ADD_QUBE_SPEED;
				}

				if(g_player.pos.z<g_qube[i].pos.z)
				{
					fMoveZ=ADD_QUBE_SPEED;
				}
				else
				{
					fMoveZ=-ADD_QUBE_SPEED;
				}

				//ヒット回数加算
				g_nAttack++;
				//氷移動量加算
 				AddMove(i,fMoveX,fMoveZ,0.58f);
			}

			for(int j=i+1;j<QUBE_MAX-1;j++)
			{
				//立方体と立方体の当たり判定
				if(HitQube(g_qube[j].pos.x-g_qube[j].fHalfWidth,g_qube[j].pos.x+g_qube[j].fHalfWidth,
					g_qube[j].pos.y-g_qube[j].fHalfWidth,g_qube[j].pos.y+g_qube[j].fHalfWidth,
					g_qube[j].pos.z-g_qube[j].fHalfWidth,g_qube[j].pos.z+g_qube[j].fHalfWidth,
					g_qube[i].pos.x-g_qube[i].fHalfWidth,g_qube[i].pos.x+g_qube[i].fHalfWidth,
					g_qube[i].pos.y-g_qube[i].fHalfHeight,g_qube[i].pos.y+g_qube[i].fHalfHeight,
					g_qube[i].pos.z-g_qube[i].fHalfWidth,g_qube[i].pos.z+g_qube[i].fHalfWidth) &&
					g_qube[i].bSet==true && g_qube[j].bSet==true)
				{
					//ヒットした場合立方体と立方体の距離の差分(X,Z)を求め、そこから移動量を決める
					if(g_qube[j].pos.x<g_qube[i].pos.x)
					{
						fMoveX=ADD_QUBE_SPEED;
					}
					else
					{
						fMoveX=-ADD_QUBE_SPEED;
					}

					if(g_qube[j].pos.z<g_qube[i].pos.z)
					{
						fMoveZ=ADD_QUBE_SPEED;
					}
					else
					{
						fMoveZ=-ADD_QUBE_SPEED;
					}

					g_nAttack++;								//ヒットカウント加算
 					AddMove(i,fMoveX,fMoveZ,0.58f);				//立方体Aの移動量加算
 					AddMove(j,-(fMoveX/5),-(fMoveZ/5),0.58f);	//立方体Bの移動量加算
				}
			}
		}

		//コンボボーナスチェック
		if(g_nAttack%30==0 &&
		   g_nAttack!=0)
		{
			//スコア加算
			AddScore(300);
			//インフォメーション切替
			SetInfo(BONUS_COMBO);
		}

		//カオスボーナスチェック
		if(g_nAttack%30==0 &&
		   g_nAttack!=0)
		{
			//スコア加算
			AddScore(200);
			//インフォメーション切替
			SetInfo(BONUS_CAOS);
		}

		//カウントアップ
		g_nCnt++;

		if(g_nCnt>0+(200*(g_nCnt%200))    &&
			g_nCnt<200+(200*(g_nCnt%200)) &&
			g_nCnt<800
			)
		{
			//インフォメーション切替(チュートリアル部分のみ)
			SetInfo((INFOTYPE)(TUTOREAL_MOVE+(g_nCnt%200)%BONUS_ITEM));
		}
		else if(g_nCnt>800)
		{
			//インフォメーション切替(フリー状態)
			SetInfo(INFO_ROTATION);
		}

	}
}
//=============================================================================
//ゲーム内の描画
//=============================================================================
void DrawGame()
{
	//ポーズフラグチェック
	if(g_bPause!=false)
	{
		///////////////////////
		//	ポーズ画面内	//
		/////////////////////

		//ポーズ画面の描画
		DrawPPolygon();

		//カーソルの描画
		DrawCuPolygon();
	}

	///////////////////////
	//	ゲーム画面内	//
	/////////////////////

	//カメラのセット
	SetCamera();

	//立方体の描画
	DrawQube();

	//プレイヤーの描画
	DrawPlayer();

	//フィールドの描画
	DrawMeshField();

	//壁の描画
	DrawWall();

	//敵の描画
	DrawEnemyB();

	//敵爆発エフェクトの描画
	DrawEneExp();

	//天敵の描画
	DrawEnemyA();

	//弾の描画
	DrawBullet();

	//弾エフェクトの描画
	DrawEffect();

	//弾爆発エフェクトの描画
	DrawBulExp();

	//時間の描画
	DrawTime();

	//インフォメーションの描画
	DrawInfo();

	//スコアの描画
	DrawScore();
}
//=============================================================================
//ゲーム内の終了
//=============================================================================
void UninitGame()
{
	//カメラの終了
	UninitCamera();

	//ライトの終了
	UninitLight();

	//プレイヤーの終了
	UninitPlayer();

	//天敵の終了
	UninitEnemyA();

	//敵の終了
	UninitEnemyB();

	//敵爆発エフェクトの終了
	UninitEneExp();

	//弾の終了
	UninitBullet();

	//弾エフェクトの終了
	UninitEffect();

	//弾爆発エフェクトの終了
	UninitBulExp();

	//フィールドの終了
	UninitMeshField();

	//壁の終了
	UninitWall();

	//立方体の終了
	UninitQube();

	//時間の終了
	UninitTime();

	//インフォメーションの終了
	UninitInfo();

	//ポーズ画面の終了
	UninitPPolygon();

	//カーソルの終了
	UninitCuPolygon();

	//スコアの終了
	UninitScore();
}
//=============================================================================
// ポーズフラグの取得
//=============================================================================
void SetPause (bool flag)
{
	g_bPause=flag;
}
//EOF