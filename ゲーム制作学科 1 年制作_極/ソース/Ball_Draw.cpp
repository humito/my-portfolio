//=============================
//		ブロック崩し描画関数
//		制作者:木村文登
//=============================
//=============================
//		インクルード
//=============================
#include <stdio.h>
#include "CScreen.h"
#include "Ball_Draw.h"
//=============================
//	ブロック崩し描画関数
//		引数
//		戻値
//=============================
void Ball_Draw (STR_BALL *pball,STR_ENEMI *penemi,STR_GAME *pgame)
{
	//変数宣言
	int i;
	int x,y;




	//壁表示
	for(i=ONE;i<=WALL_LAST;i++)
	{
		LOCATE(WALL1,i);
		COLOR(RED);
		BACKCOLOR(LIME);
		printf("Ж");
	}



	//操作説明縦座標指定
	y=INFO;


	//説明表示
	BACKCOLOR(GRAY);
	LOCATE(WALL3,y);
	printf("←　：　左移動");
	y+=TWO;

	LOCATE(WALL3,y);
	printf("→　：　右移動");
	y+=TWO;

	LOCATE(WALL2,y);
	printf("ＬＢ : 左クイックステップ");
	y+=TWO;

	LOCATE(WALL2,y);
	printf("ＲＢ : 右クイックステップ");
	y+=TWO;

	LOCATE(WALL3,y);
	printf("Ａ　：　ボールを増やす");
	y+=TWO;

	LOCATE(WALL3,y);
	printf("Ｘ　:　スピードアップ");
	y+=TWO;




	//ラケット表示
	COLOR(PINK);
	BACKCOLOR(WHITE);
	LOCATE(pgame->rx,RAKETY);

	printf("(");

	BACKCOLOR(YELLOW);
	printf("####");

	BACKCOLOR(WHITE);
	printf(")");






	//敵表示

	if(penemi->enemiY_old != penemi->enemiY)
	{
	LOCATE(ONE,penemi->enemiY);
	COLOR(RED);
	printf("  ◆◆■■    ●  ◆◎◎◎◎◎◆  ●    ■■◆◆  \n");
	printf("  ■    ●●◆◆◆◎∵∵∵∵∵◎◆◆◆●●    ■  \n");
	printf("  ◆    ■■  ●  ◎∵∵●∵∵◎  ●  ■■    ◆  \n");
	printf("■■■      ◆◆◆◎∵∵∵∵∵◎◆◆◆      ■■■\n");
	printf("■  ■        ●  ◆◎◎◎◎◎◆  ●        ■  ■\n");
	


	penemi->enemiY_old=penemi->enemiY;
	}


		//ミスしたときのZ入力待ち描画
		if(pgame->GAME==TWO)//ミスモード
		{
			x=ONE;
			y=MISS;
			COLOR(LIME);

			LOCATE(x,y);
			printf("◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆");
			y++;

			LOCATE(x,y);
			printf("◆");
			BACKCOLOR(DARK);
			printf("　 ミスをしてしまった。Ａを押して復活しよう　 ");
			BACKCOLOR(WHITE);
			printf("◆");
			y++;

			LOCATE(x,y);
			printf("◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆");
			y++;

			pball->tate=THREE;
			pball->yoko=THREE;

		
		}
		else if(pgame->GAME==FOUR)//復帰モード
		{
			//説明非表示
			for(y=ONE;y<=WALL_LAST;y++)
			{
				LOCATE(ONE,y);
				printf("　　　　　　　　　　　　　　　　　　　　　　　　　");
			}

			//敵座標再表示
			penemi->enemiY_old--;

			//ゲームモード0
			pgame->GAME=ZERO;

		}
		else if(pgame->GAME==THREE)
		{
			COLOR(LIME);
			LOCATE(ONE,MISS);
			printf("ффффффффффффффффффффффффф\n");
			printf("ф");
			BACKCOLOR(DARK);
			printf("　　　　ボタンを押してゲームを開始してね　　　");
			BACKCOLOR(WHITE);
			printf("ф\n");
			printf("ффффффффффффффффффффффффф");
			
			//ゲームモード0
			pgame->GAME=ZERO;
		}
		else//ゲームモード0
		{
			//待ち時間によりボールの形が変化
			if(pgame->WAIT==FIVE)
			{
				//
				//残像表示
				COLOR(CORAL);
				LOCATE(pball->x_old,pball->y_old);
				printf("☆");

				//ボール表示
				COLOR(CORAL);
				LOCATE(pball->x,pball->y);
				printf("★");
			}
			else
			{
				//残像表示
				COLOR(CYAN);
				LOCATE(pball->x_old,pball->y_old);
				printf("○");

				//ボール表示
				COLOR(BLUE);
				LOCATE(pball->x,pball->y);
				printf("●");

			}


		}


		//残機表示
		COLOR(BLUE);
		BACKCOLOR(GRAY);
		LOCATE(WALL3,RAKETY);
		if(pgame->L==THREE)
		{
			printf("のこり:●●");
		}
		else if(pgame->L==TWO)
		{
			printf("のこり:●");
		}
		else
		{
			printf("のこり：");
		}

		printf("　　　　　　");





		//スコア表示
		LOCATE(WALL3,TEN);
		COLOR(GREEN);
		printf("スコア : %d",pgame->s);


		//敵HP表示
		LOCATE(WALL3,ONE);
		printf("敵ライフ : %d",penemi->enemiHP);


}