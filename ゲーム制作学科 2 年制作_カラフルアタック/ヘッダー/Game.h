#ifndef _GAME_H_
#define _GAME_H_

//インクルード
#include "main.h"
//マクロ定義
#define COUNTUP (1)			//カウント増加量
#define FUELTIME_MAX (1000)	//燃料減量時間最大値
#define FUELTIME_MIN (100)	//燃料減量時間最小値

//構造体
typedef struct
{
	int nRed;	//赤の個数
	int nYellow;//黄色の個数
	int nBlue;	//青の個数

}CARCOUNT;

//プロトタイプ宣言
void InitGame (void);	//ゲームの初期化
void UpdateGame (void);	//ゲームの更新
CARCOUNT GetCars (void);//車の数の取得

#endif _GAME_H_
//EOF