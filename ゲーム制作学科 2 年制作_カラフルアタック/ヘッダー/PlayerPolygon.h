#ifndef _PLAYERPOLYGON_H_
#define _PLAYERPOLYGON_H_

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "main.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define HALF (2)
#define NEWPOS_X (SCREEN_WIDTH/HALF)
#define NEWPOS_Y (SCREEN_HEIGHT/HALF)
#define NORMAL (0)
#define MISS (1)
#define RETRY (2)
//*****************************************************************************
// 構造体
//*****************************************************************************
typedef struct
{
	float fX;							//X座標
	float fY;							//Y座標
	float fWidth;						//テクスチャの幅
	float fHeight;						//テクスチャの高さ
	int   nCount;						//復帰カウント
	int   nStatus;						//ステータス
	int   nColor;						//プレイヤーの車の色
	int   nAlpha;						//プレイヤーのα値(透明度)
}PLAYER;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPolygon (void);//ポリゴンの初期化
void UpdatePolygon (void);//ポリゴンの更新
void DrawPolygon (void);//ポリゴンの描画
void UninitPolygon (void);//ポリゴンの終了
PLAYER GetPlayer (void);//プレイヤー情報取得
void SetPlayer (PLAYER data);//プレイヤー情報設置


#endif _PLAYERPOLYGON_H_
//EOF