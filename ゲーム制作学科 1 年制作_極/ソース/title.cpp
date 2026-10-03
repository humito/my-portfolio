#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "CScreen.h"
#include "title.h"
#include "Title_Draw.h"

int get (void);
void set (int *count);

//メモ：タイトル・アウト時の描画関数を作成

static int vcount;

void title (int *iMODE,int *pTitleMode,int *pcolor)
{
	//変数宣言
	int MODE=*iMODE;
	int count=ZERO;


	//カウントゲッター
	count=get();



	//タイトルメインループ
		//待ち時間
		WAIT(STOP);


		//タイトル描画処理
		Title_Draw(&pTitleMode,&pcolor,&MODE);



		if(INP(PK_Z))
		{
			*pTitleMode=TWO;
			//MODE=ONE;
		}
		
		//カウントアップ
		count++;

		
		LOCATE(1,ONE);
		printf("%d",count);




		/*if(count==200)
		{
			//デモへ
		}*/




		//カウントセット
		set(&count);

		//キークリアー
		INPCLEAR();


		//ポインタ変数へ戻す
		*iMODE=MODE;

}


//タイトルカウントゲッター
//引数:無
//戻値:vcount
int get (void)
{
	return (vcount);
}

//タイトルカウントセッター
//引数:*c
//戻値:無
void set (int *c)
{
	vcount=*c;
}
