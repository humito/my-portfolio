#ifndef _COINPOLYGON_H_
#define _COINPOLYGON_H_

#include "main.h"

#define COIN_MAX (100)

typedef struct
{
	float fX;
	float fY;
	float fWidth;
	float fHeight;
	float fU;
	float fV;
	bool  bUse;
}COIN;

HRESULT InitCPolygon (void);		//ポリゴンの初期化
void UpdateCPolygon (void);			//ポリゴンの更新
void DrawCPolygon (void);			//ポリゴンの描画
void UninitCPolygon (void);			//ポリゴンの終了


#endif _COINPOLYGON_H_