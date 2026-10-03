//===================================
//		ブロック崩しメイン関数
//			制作者:木村文登
//===================================

//===================================
//			インクルード
//===================================
#include <stdio.h>
#include "CScreen.h"
#include "main.h"
#include "Ball.h"
#include "Ball_Init.h"
#include "Ball_Draw.h"
#include "Ball_Update.h"
#include "Ball_Wall.h"
#include "Ball_Result.h"
#include "Ball_Key.h"


//=============================
//		ボールが動くゲーム（β）
//=============================

//スタティック変数
static STR_BALL vball[THREE];
static STR_ENEMI venemi;
static STR_GAME vgame;

//=============================
//ブロック崩し(ボス)
//引数:*pBallMode
//戻値:*pBallMode
//=============================
void ball (int *pBallMode,int *pMODE,MUSIC *pbgm)
{

	//変数宣言
	int BallMode=*pBallMode;
	int MODE=*pMODE;
	int i;//ループカウンタ

	//構造体変数宣言
	STR_BALL ball[THREE];
	STR_ENEMI enemi;
	STR_GAME game;




//メモ:関数分けをする。倒した時と負けた時のアニメをつける
	//ボタン開始待ちを作ってgetch()をなくす



	//構造体ゲッター
	for(i=ZERO;i<THREE;i++)
	{
		ball[i]=Getter(i);
	}

	//敵構造体ゲット
	enemi=Getenemi();

	//ゲーム構造体ゲット
	game=Getgame();





	/*=======================================
					ゲームモード
	=========================================*/
	switch(BallMode)
	{
		/*======================================
						ゲーム準備
		======================================*/

		//初期化
		case 0:

			for(i=ZERO;i<THREE;i++)
			{
				//ブロック崩し初期化関数処理
				Ball_Init(&ball[i],&game,&enemi);
			}
			

			//描画へ
			BallMode=ONE;
		break;


		//描画
		case 1:

			//画面描画関数処理
			for(i=0;i<game.Z;i++)
			{
				Ball_Draw(&ball[i],&enemi,&game);
			}


			//ライフがはじめなら
			if(game.L==THREE && game.c==ONE && game.s==ZERO)
			{
				//開始待ち
				BallMode=TWO;
			}
			else
			{
				//それ以外はゲーム内(ラケット操作)へ
				BallMode=THREE;
			}

		break;


		//開始待ち
		case 2:


		//敵再表示
		enemi.enemiY_old--;

		//ボタン待ち
		getch();


		//復帰モードで非表示にさせる
		game.GAME=FOUR;

		//ゲームモード3へ
		BallMode=3;


		break;
		/*======================================
						ゲーム開始
		======================================*/
		//ラケット操作
		case 3:
			//待ち時間（初期0.05）
			WAIT(game.WAIT);
			
			
			//ラケット操作関数処理
			Ball_Key(&game,&ball[ZERO],&enemi);

			//更新へ
			BallMode=FOUR;
			

			
		break;


		//更新
		case 4:

			//更新関数処理
			for(i=ZERO;i<game.Z;i++)
			{
				Ball_Update(&ball[i],&game,&enemi);
			}

			//ラケットの当たりが入ったら効果音を鳴らす
			if(game.RAKET != ZERO)
			{
				PLAYMP3(pbgm->pui,ZERO);
				game.RAKET=ZERO;
			}
			//壁当たり判定
			BallMode=FIVE;
		break;


		//壁当たり判定
		case 5:

			//壁当たり判定
			for(i=ZERO;i<game.Z;i++)
			{
				Ball_Wall(&ball[i],&game,&enemi);
			}

			if(game.out!=ZERO)
			{
				PLAYMP3(pbgm->manuke,ZERO);
				game.out=ZERO;
			}
			
			if(game.GAME==ONE)
			{
				//リザルトへ
				BallMode=SIX;
			}
			else
			{
				//描画へ
				BallMode=ONE;
			}


		break;


		//リザルト
		case 6:
			//ブロック崩しリザルトへ
			Ball_Result(&game,&MODE,&BallMode);

			//初期化へ戻る
			BallMode=ZERO;
			MODE=ZERO;
		break;

	}//switch終
	

	
	//カウントアップ
	game.c++;


	//構造体セッター
	for(i=ZERO;i<THREE;i++)
	{
		Setter(&ball[i],&enemi,&game,i);
	}



	//ポインタ変数へ戻す
	*pBallMode=BallMode;
	*pMODE=MODE;
}

//===================================
		//ボール構造体ゲッター
		//引数:i
		//戻値:vball[i]
//===================================
STR_BALL Getter (int i)
{
	return (vball[i]);
}
//===================================
		//敵構造体ゲッター
		//引数:無
		//戻値:venemi
//===================================
STR_ENEMI Getenemi (void)
{
	return (venemi);
}
//===================================
		//ゲーム構造体ゲッター
		//引数:無
		//戻値:vgame
//===================================
STR_GAME Getgame (void)
{
	return (vgame);
}
//===================================
		//全構造体セッター
		//引数:*pball,*penemi,*pgame,i
		//戻値:無
//===================================
void Setter (STR_BALL *pball,STR_ENEMI *penemi,STR_GAME *pgame,int i)
{
	vball[i]=*pball;

	venemi=*penemi;

	vgame=*pgame;
}