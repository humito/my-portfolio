#include <stdio.h>
#include "CScreen.h"
#include "Ball_Wall.h"

void Ball_Wall (STR_BALL *pball,STR_GAME *pgame,STR_ENEMI *penemi)
{
	int i;//ループカウンタ


	//下へはみでたらライフ減算
	if(pball->y>=BOTTOM)
	{

		pgame->s-=MISS1;//スコア800減算
		pgame->L--;//ライフ1つ減る

		//あやしい
		pball->x=INITX;//ボールをラケットの位置に配置
		pball->y=INITY;//ボールを敵の直前に配置
		pball->tate=TWO;//ボールの移動切替
		pgame->Z=ONE;//ボール数を1へ
		penemi->enemiY_old=MISS1;
		


		if(pgame->L<ONE)//ライフが０になったらゲームから抜ける
		{
			pgame->R=ZERO;//結果判定を0にする
			pgame->GAME=ONE;//ゲームモード1（ゲームオーバー）にする
		}
		else//違う場合はＺが押されるまでゲームを止めるようにする
		{
			//GAME変数を2（2はミス）にしてモード切替に変更
			pgame->GAME=TWO;
			pgame->out=ONE;
		}
	}
	else if(pball->y>penemi->enemiY && pball->y<penemi->enemiY+6)//敵に当ったら移動切替
	{
		//切替
		pball->tate=ONE;

		//点滅処理
		for(i=penemi->enemiY;i<=penemi->enemiY+FIVE;i++)
		{
			WAIT(TEN);
			LOCATE(ONE,i);
			printf("　　　　　　　　　　　　　　　　　　　　　　　　　");

			penemi->enemiY_old--;
		}

		//敵にダメージ与える
		penemi->enemiHP-=ONE;

		//敵の体力が０なら勝ちでループから抜ける
		if(penemi->enemiHP<ZERO)
		{
			pgame->R=ONE;//リザルト１
			pgame->GAME=ONE;//ゲームモード1（1はゲームオーバー）
		}


		//中心なら獲得スコアが変わる
		if(pball->x>EIGHTY && pball->x<THIRTYTWO)
		{
			//スコア500UP
			pgame->s+=UP1;
		}
		else
		{
			//スコア100UP
			pgame->s+=UP2;
		}
				
	}

	//左に当ったら移動切替
	if(pball->x<=ONE)
	{
		pball->yoko=ONE;
	}
	else if(pball->x>=FIFTY)//右なら移動切替
	{
		pball->yoko=TWO;
	}




}