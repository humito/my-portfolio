//========================================
//HEW作品メイン関数
//制作者：木村文登
//========================================
//========================================
//インクルード
//========================================
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "CScreen.h"
#include "title.h"
#include "ball.h"
//========================================
//HEW作品メイン関数
//引数:無
//戻値:無
//========================================
void main (void)
{
	//変数宣言
	int color;

	int W=ZERO,M=ZERO;


	//各モードの変数
	int iMODE=ZERO;
	int iTitleMode=ZERO;
	int iBallMode=ZERO;


	//音楽・効果音
	MUSIC bgm;
	

	//ランダム設定
	srand(unsigned(NULL));

	bgm.loop41=OPENWAVE("loop_41.wav");
	bgm.pui=OPENMP3("pui.mp3");
	bgm.manuke=OPENMP3("manuke.mp3");
	bgm.play=OPENMP3("play.mp3");

	CUROFF();

	//メインループ
	while(ONE)
	{
		//ランダム0～4
		//色用乱数
		color=rand()%FIVE;


		//フラグ0でかつタイトルモードなら繰り返しで再生
		if(W==ZERO && iMODE!=ONE)
		{
			PLAYWAVE(bgm.loop41, ONE);
		}

		if(M==ZERO && iMODE==ONE)
		{
			//ゲーム音楽再生
			PLAYMP3(bgm.play,ONE);
		}

		//再生中フラグ代入
			W=ISPLAYINGWAVE(bgm.loop41);

			M=ISPLAYINGMP3(bgm.play);

		switch(iMODE)
		{
			case 0:
				//タイトル
				title(&iMODE,&iTitleMode,&color);
			break;

			case 1:

				//タイトルの曲停止
				STOPWAVE(bgm.loop41);
				W=ZERO;


				
				
				//ゲーム(ball)
				ball(&iBallMode,&iMODE,&bgm);


				if(iMODE==ZERO)
				{
					STOPMP3(bgm.play);
				}

				//タイトルモード初期化
				iTitleMode=ZERO;
			break;


			//何もない場合抜けるだけ
			default:
			break;

		}//switch 終

	}//while 終
	
}