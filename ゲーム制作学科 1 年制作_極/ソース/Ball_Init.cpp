//===================================
//ブロック崩し初期化
//制作者:木村文登
//===================================

#include <stdio.h>
#include "CScreen.h"
#include "Ball_Init.h"
#include "ball.h"

void Ball_Init (STR_BALL *pball,STR_GAME *pgame,STR_ENEMI *penemi)
{
	int i;


	//画面初期化
	CLS(RED,WHITE);


	//右表示色付け
	for(i=ONE;i<INFO_X;i++)
	{
		LOCATE(INFO_BACK,i);
		BACKCOLOR(GRAY);
		printf("　　　　　　　　　　　　　　");
	}

	BACKCOLOR(WHITE);

		//ボール座標指定
		pball->x=BALLS_X;
		pball->y=BALLS_Y;

		//ボール残像指定
		pball->x_old=pball->x;
		pball->y_old=pball->y;

		//ボール移動指定
		pball->tate=TWO;
		pball->yoko=ONE;


	//敵の体力指定
	penemi->enemiY=ONE;//敵座標
	penemi->enemiHP=HP;//敵ライフ
	penemi->enemiY_old=TWO;//敵バックアップ座標



	//ラケット座標位置
	pgame->rx=FIVE;


	//カウント初期化
	pgame->c=ZERO;

	//スコア初期化
	pgame->s=ZERO;

	//ボール数初期化
	pgame->Z=ONE;

	//リザルト初期化
	pgame->R=ZERO;

	//ライフ初期化
	pgame->L=THREE;
	
	//時間初期化
	pgame->time=ZERO;
	
	//ゲームモード初期化(3はゲームスタート待ち)
	pgame->GAME=THREE;

	/*ゲームモード
	0...ゲーム中
	1...ゲームオーバー
	2...ミス（復帰待ち）
	3...ゲーム開始待ち
	4...復帰
	*/

	//待ち時間初期化
	pgame->WAIT=TIME;


	//ラケット当たりフラグ
	pgame->RAKET=ZERO;






}