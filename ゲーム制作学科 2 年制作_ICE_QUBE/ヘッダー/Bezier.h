#ifndef _BEZIER_H_
#define _BEZIER_H_

#include "game.h"

//ベジエ曲線構造体
typedef struct
{
	float fT;			//t
	float fBA;			//B0^3(t)
	float fBB;			//B1^3(t)
	float fBC;			//B2^3(t)
	float fBD;			//B3^3(t)
	D3DXVECTOR3 pA;		//P1
	D3DXVECTOR3 pB;		//P2
	D3DXVECTOR3 pC;		//P3
	D3DXVECTOR3 pD;		//P4

}BEZIER;
void InitBezier (void);
void BezierCurve (int i,D3DXVECTOR3 *pos,bool *bUse);
void SetBezier(int i,D3DXVECTOR3 pos,D3DXVECTOR3 rot,float fWidth,float fHeight);
#endif
//EOF