#ifndef _HIT_H_
#define _HIT_H_

#include "main.h"
//--------------------------------------------------------------
// プロトタイプ宣言
//--------------------------------------------------------------
BOOL isRectHit(int ax,int ay,int aw,int ah,int bx,int by,int bw,int bh);	//当たり判定(短形)
BOOL isCircleHit(int ax,int ay,int ar,int bx,int by,int br);				//当たり判定（円）

#endif _HIT_H_