//===============================================================
//3D当たり判定処理
//Author:HUMITO KIMURA
//===============================================================
#ifndef _HIT3D_H_
#define _HIT3D_H_
//インクルードファイル
#include "main.h"
//===============================================================
//立方体当たり判定チェック
//===============================================================
bool HitQube(float aX,float aXWidth,float aY,float aYWidth,float aZ,float aZWidth,
			 float bX,float bXWidth,float bY,float bYWidth,float bZ,float bZWidth);

#endif
//EOF