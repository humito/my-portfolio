#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "main.h"
#define ROOL_SPEED (0.01f)
#define EFFECT_MAX (100)

typedef struct
{
	float fX;			//X座標
	float fY;			//Y座標
	float fWidth;		//横幅
	float fHeight;		//縦幅
	float fAlpha;		//α値
	float fLength;		//ポリゴン対角線の長さ
	float fAngle;		//ポリゴンの対角線の角度
	D3DXVECTOR3 pos;	//ポリゴンの位置
	int   nColor;		//色
	int   nCount;		//寿命
	bool  bUse;			//使用スイッチ
}EFFECT;

HRESULT InitEfPolygon (void);//ポリゴンの初期化
void UpdateEfPolygon (void);//ポリゴンの更新
void DrawEfPolygon (void);//ポリゴンの描画
void UninitEfPolygon (void);//ポリゴンの終了
void SetEffect (int x,int y,int color);//エフェクトのセット



#endif _EFFECT_H_