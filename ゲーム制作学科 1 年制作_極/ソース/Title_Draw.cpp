#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "CScreen.h"
#include "Title_Draw.h"


void Title_Draw (int **pTitleMode,int **pcolor,int *pMODE)
{
	//変数宣言
	int x=ONE,y=ONE;
	int x_old=ONE,y_old=ONE;
	int i;
	char ichar[STR];

	FILE *pfile;
	
	//タイトル表示
	switch(**pTitleMode)
	{

		case 0://タイトル表示

			//背景初期化
			CLS(YELLOW,RED);

			//テキストファイル読み込み
			pfile=fopen("title.txt","r");

			COLOR(YELLOW);

			//テキスト内容分ループ(極を表示)
			for(i=0;i<TITLE;i++)
			{
				//1行読み込み
				fgets(&ichar[ZERO],TITLEG,pfile);
				//読み込んだ内容を表示
				LOCATE(x,y);
				printf("%s",&ichar[ZERO]);
				y++;
			}
			//ファイルを閉じる
			fclose(pfile);

			//タイトルモードを1へ
			**pTitleMode=ONE;

			break;//case0終


		case 1://ループ2回目以降 文字色ランダム表示

			y=PLEASEY;
			x=PLEASEX;


			WAIT(STR);
			LOCATE(x,y);
			printf("　　　　　　　　　　　　　　　　");


			//各数値によって文字色を変える
			switch(**pcolor)
			{
				case 0:
					COLOR(BLUE);
				break;

				case 1:
					COLOR(YELLOW);
				break;

				case 2:
					COLOR(PINK);
				break;

				case 3:
					COLOR(SKYBLUE);
				break;
					
				case 4:
				COLOR(GREEN);

				default:
				break;

			}//文字色ランダムswitch終


			//キー入力要求表示
			LOCATE(x,y);
			printf("PLEASE PUSH  A BUTTON");

		break;//case1終



		//ゲームスタート描画
		case 2:



		//背景切替
		for(y=ONE;y<=BALLSD;y++)
		{
			WAIT(STR);

			BACKCOLOR(WHITE);
			LOCATE(x,y);
			printf("　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　");
		}

		y=ONE;

		//記号移動
		for(x=ONE;x<FIFTY+TWO;x++)
		{
			WAIT(FIFTY);

			

			LOCATE(x,y);
			printf("Ж");

		}


		//バックカラー描画
		for(y=ONE;y<=BALLSD;y++)
		{
			WAIT(STR);

			LOCATE(x,y);
			BACKCOLOR(LIME);
			COLOR(RED);
			printf("Ж");
		}
		
		//操作背景
		for(y=ONE;y<=BALLSD;y++)
		{
			WAIT(STR);
			LOCATE(x+TWO,y);
			BACKCOLOR(GRAY);
			printf("　　　　　　　　　　　　　 ");
		}

		y_old=BOSS;


		//敵移動描画
		for(y=y_old;y>=ONE;y--)
		{
			WAIT(FIFTY);

			BACKCOLOR(WHITE);
			LOCATE(ONE,y_old);
			printf("　　　　　　　　　　　　　　　　　　　　　　　　　\n");
			printf("　　　　　　　　　　　　　　　　　　　　　　　　　\n");
			printf("　　　　　　　　　　　　　　　　　　　　　　　　　\n");
			printf("　　　　　　　　　　　　　　　　　　　　　　　　　\n");
			printf("　　　　　　　　　　　　　　　　　　　　　　　　　\n");


			LOCATE(ONE,y);
			printf("  ◆◆■■    ●  ◆◎◎◎◎◎◆  ●    ■■◆◆  \n");
			printf("  ■    ●●◆◆◆◎∵∵∵∵∵◎◆◆◆●●    ■  \n");
			printf("  ◆    ■■  ●  ◎∵∵●∵∵◎  ●  ■■    ◆  \n");
			printf("■■■      ◆◆◆◎∵∵∵∵∵◎◆◆◆      ■■■\n");
			printf("■  ■        ●  ◆◎◎◎◎◎◆  ●        ■  ■\n");
		

			y_old=y;
		}


		//ラケット移動
		for(x=RAKETX;x>4;x--)
		{

			WAIT(FIFTY);

			LOCATE(x_old,RAKETY);
			printf("      ");

			COLOR(PINK);
			BACKCOLOR(WHITE);
			LOCATE(x,RAKETY);

			printf("(");
			BACKCOLOR(YELLOW);
			printf("####");
			BACKCOLOR(WHITE);
			printf(")");

			x_old=x;
		}

		y=BALLSD;
		y_old=y;
		x_old=BALLSS;

		for(x=x_old;x>TITLE;x--,y--)
		{
			WAIT(STR);

			COLOR(CYAN);
			LOCATE(x_old,y_old);
			printf("○");

			COLOR(BLUE);
			LOCATE(x,y);
			printf("●");

			LOCATE(x_old,y_old);
			printf("　");

			x_old=x;
			y_old=y;
		}



		*pMODE=ONE;


		break;


	}//switch終

	
	
}