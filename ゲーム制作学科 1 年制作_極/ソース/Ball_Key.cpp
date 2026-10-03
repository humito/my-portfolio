//====================================
//ボールキー入力関数
//制作者:木村文登
//====================================

//====================================
//インクルード
//====================================
#include <stdio.h>
#include "CScreen.h"
#include "Ball_Key.h"
//====================================
//ブロック崩しキー入力関数
//引数:
//戻値:無
//====================================
void Ball_Key (STR_GAME *pgame,STR_BALL *pball,STR_ENEMI *penemi)
{
	int i;

		//右操作
		if(INP(PK_RIGHT) || GPRIGHT(PK_RIGHT))
		{

			LOCATE(pgame->rx,RAKETY);
			BACKCOLOR(WHITE);
			printf("　　　");
			pgame->rx+=IDOU;
		}

		//左操作
		if(INP(PK_LEFT))
		{
			LOCATE(pgame->rx,RAKETY);
			BACKCOLOR(WHITE);
			printf("　　　");
			pgame->rx-=IDOU;
		}


		//ボールを増やす
		if(INP(PK_Z))
		{
			if(pgame->L>ZERO && pgame->GAME==TWO)//ライフ１つ以上でかつゲームモード２の条件であるなら
			{
					//ゲームモード４(4は復帰)
					pgame->GAME=FOUR;

					//各ボールの移動を初期に戻す
					for(i=ZERO;i<THREE;i++)
					{
						(pball+i)->tate=TWO;
						(pball+i)->yoko=ONE;
					}
					
					//敵座標再表示させる
					penemi->enemiY_old--;
			}
			else if(pgame->L>ONE && pgame->GAME==ZERO)//残機2つ以上でかつゲームモード0が条件
			{
				pgame->Z++;//1.2.3
				pgame->L--;//3.2.3
			}
			else if(pgame->GAME==THREE)
			{
				//ゲームモード５(スタート)
				pgame->GAME=FIVE;
			}
		//キークリア
		INPCLEAR();
		}


		//時間の流れを早くする
		if(INP(PK_X))
		{
			pgame->WAIT=FIVE;
			pgame->s+=TEN;
		}
		else
		{
			pgame->WAIT=TIME;
		}


		//クイックステップ左
		if(INP(PK_A))
		{
			BACKCOLOR(WHITE);
			LOCATE(pgame->rx,RAKETY);
			printf("　　　");

			pgame->rx-=TWOENTEEN;

			pgame->s-=MINAS;

			//キークリア
			INPCLEAR();

		}

		//クイックステップ右
		if(INP(PK_S))
		{
			BACKCOLOR(WHITE);
			LOCATE(pgame->rx,RAKETY);
			printf("　　　");

			pgame->rx+=TWOENTEEN;

			pgame->s-=MINAS;

			//キークリア
			INPCLEAR();


		}

}