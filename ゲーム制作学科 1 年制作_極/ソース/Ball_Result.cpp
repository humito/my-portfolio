//====================================
//ブロック崩しリザルト
//制作者:木村文登
//====================================

//====================================
//インクルード
//====================================
#include <stdio.h>
#include "CScreen.h"
#include "Ball_Result.h"



//====================================
//ボールリザルト関数
//引数:*pscore,*pcount,*pR
//戻値:無
//====================================
void Ball_Result (STR_GAME *pgame,int *pMODE,int *pBallMode)
{

	//変数宣言
	int i;
	int clear=ZERO;
	int x,y;

	//画面初期化
	CLS(GRAY,GREEN);


	




	//スコアタイトル表示
	LOCATE(THIRTY,THREE);
	printf("SCORE");
	//罫線座標指定
	x=EITEEN;
	y=TEN;


	//罫線描画
	LOCATE(x,y);
	printf("∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮");

	for(y=EREVEN;y<SIXTEEN;y++)
	{
		LOCATE(x,y);
		printf("∮　　　　　　　　　　　　　　　　　　　　　　∮");
	}
	LOCATE(x,y);
	printf("∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮∮");
	
	//スコア結果座標指定
	x=SCOREX;
	y=EREVEN;
	
	//スコア表示
	LOCATE(x,y);
	printf("タイムボーナス:%d",TIMEB-pgame->c);
	y++;
		
	LOCATE(x,y);
	printf("ゲームスコア  :%d",pgame->s);
	y++;

	if(pgame->R==ONE)
	{
		//クリアボーナス10000スコアUP
		clear=TIMEB;
	}

	LOCATE(x,y);
	printf("クリアボーナス:%d",clear);
	y++;
		
	//区切り
	LOCATE(x,y);
	printf("-------------------------");
	y++;
		
	//ボタンが押されるとトータルを表示
	getch();
		
	for(i=0;i<=(TIMEB-pgame->c)+pgame->s+clear;i++)
	{
		LOCATE(x,y);
		printf("トータルスコア:%d",i);
		
		if(kbhit())
		{
			LOCATE(x,y);
			printf("トータルスコア:%d",(TIMEB-pgame->c)+pgame->s+clear);
			KEYCLEAR();
			break;
		}
	}


	getch();

}