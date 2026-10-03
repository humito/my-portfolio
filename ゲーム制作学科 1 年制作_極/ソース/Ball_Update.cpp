#include <stdio.h>
#include "main.h"
#include "CScreen.h"
#include "Ball_Update.h"


void Ball_Update(STR_BALL *pball,STR_GAME *pgame,STR_ENEMI *penemi)
{



	//バックアップ更新前に残像削除
	BACKCOLOR(WHITE);
	LOCATE(pball->x_old,pball->y_old);
	printf("　");


	//ボール座標バックアップ更新
	pball->x_old=pball->x;
	pball->y_old=pball->y;

	//ボール縦移動の更新
	if(pball->tate==ONE)
	{
		pball->y++;
	}
	else if(pball->tate==THREE)
	{
		pball->y+=ZERO;
	}
	else
	{
		pball->y--;
	}

	//ボール横移動の更新
	if(pball->yoko==ONE)
	{
		pball->x++;
	}
	else if(pball->yoko==THREE)
	{
		pball->yoko+=ZERO;
	}
	else
	{
		pball->x--;
	}

	//ラケットの動く範囲
	//左端制御
	if(pgame->rx<=ONE)
	{
		pgame->rx=ONE;
	}
	//右端制御
	if(pgame->rx>=RAKETX)
	{
		pgame->rx=RAKETX;
	}

	//ラケットの範囲内に入ったら移動切替更新
	if(pball->x<=pgame->rx+SIX && pball->x>=pgame->rx-ONE)
	{
		//ラケットの縦位置に入ってるか
		if(pball->y==RAKETY1 || pball->y==RAKETY2)
		{
			//ボール移動切替
			//ラケット右側
			if(pball->x<=pgame->rx+SIX && pball->x>=pgame->rx+FOUR)
			{
				pball->tate=TWO;
				pball->yoko=ONE;
			}
			else if(pball->x<=pgame->rx+TWO && pball->x>=pgame->rx)//ラケット左側
			{
				pball->tate=TWO;
				pball->yoko=ZERO;
			}
			else//中央
			{
				pball->tate=TWO;
			}


		pgame->RAKET=ONE;
		}
		




	}


	//敵を下へずらす
	if(pgame->c%ENEMIB==ZERO)
	{
		LOCATE(ONE,penemi->enemiY);
		printf("　　　　　　　　　　　　　　　　　　　　　　　　\n");
		printf("　　　　　　　　　　　　　　　　　　　　　　　　\n");
		printf("　　　　　　　　　　　　　　　　　　　　　　　　\n");
		printf("　　　　　　　　　　　　　　　　　　　　　　　　\n");
		printf("　　　　　　　　　　　　　　　　　　　　　　　　\n");

		penemi->enemiY++;//敵の座標を１つ下げる
		pgame->time=pgame->c;//カウントをタイムへ代入
		pgame->c=ZERO;//カウント初期化
	}




}