#include "Bezier.h"

static BEZIER g_bezier[BULLET_MAX];//ベジエ情報
//======================================================================
//ベジエ初期化
//======================================================================
void InitBezier (void)
{
	for(int i=0;i<BULLET_MAX;i++)
	{
		g_bezier[i].fT=0.0f;	//t

		g_bezier[i].fBA=0.0f;	//B0^3(t)
		g_bezier[i].fBB=0.0f;	//B1^3(t)
		g_bezier[i].fBC=0.0f;	//B2^3(t)
		g_bezier[i].fBD=0.0f;	//B3^3(t)

		g_bezier[i].pA.x=0.0f;	//P1
		g_bezier[i].pA.y=0.0f;
		g_bezier[i].pA.z=0.0f;

		g_bezier[i].pB.x=0.0f;	//P2
		g_bezier[i].pB.y=0.0f;
		g_bezier[i].pB.z=0.0f;

		g_bezier[i].pC.x=0.0f;	//P3
		g_bezier[i].pC.y=0.0f;
		g_bezier[i].pC.z=0.0f;

		g_bezier[i].pD.x=0.0f;	//P4
		g_bezier[i].pD.y=0.0f;
		g_bezier[i].pD.z=0.0f;

	}
}
//======================================================================
//ベジエカーブ実装
//======================================================================
void BezierCurve (int i,D3DXVECTOR3 *pos,bool *bUse)
{
	//(1-t)^3
	g_bezier[i].fBA=(1-g_bezier[i].fT)*(1-g_bezier[i].fT)*(1-g_bezier[i].fT);
	//3*t*((1-t)^2)
	g_bezier[i].fBB=3*g_bezier[i].fT*((1-g_bezier[i].fT)*(1-g_bezier[i].fT));
	//3*t^2*(1-t)
	g_bezier[i].fBC=3*g_bezier[i].fT*g_bezier[i].fT*(1-g_bezier[i].fT);
	//t^3
	g_bezier[i].fBD=g_bezier[i].fT*g_bezier[i].fT*g_bezier[i].fT;

	//座標変換
	pos->x=(g_bezier[i].pA.x*g_bezier[i].fBA)+(g_bezier[i].pB.x*g_bezier[i].fBB)+(g_bezier[i].pC.x*g_bezier[i].fBC)+(g_bezier[i].pD.x*g_bezier[i].fBD);
	pos->y=(g_bezier[i].pA.y*g_bezier[i].fBA)+(g_bezier[i].pB.y*g_bezier[i].fBB)+(g_bezier[i].pC.y*g_bezier[i].fBC)+(g_bezier[i].pD.y*g_bezier[i].fBD);
	pos->z=(g_bezier[i].pA.z*g_bezier[i].fBA)+(g_bezier[i].pB.z*g_bezier[i].fBB)+(g_bezier[i].pC.z*g_bezier[i].fBC)+(g_bezier[i].pD.z*g_bezier[i].fBD);


	//終了時にtを0.05ずつ増加する
	g_bezier[i].fT+=0.05f;

	//ベジエカーブが終着点に達したらフラグfalse
	if(g_bezier[i].fT>1.0f)
	{
		*bUse=false;
	}
}
void SetBezier(int i,D3DXVECTOR3 pos,D3DXVECTOR3 rot,float fWidth,float fHeight)
{
	//tの初期化
	g_bezier[i].fT=0.0f;

	//座標到着点の設定（ここでは一回転のカーブを目的とする）

	//最初の座標セット
	g_bezier[i].pA.x=pos.x;
	g_bezier[i].pA.y=pos.y;
	g_bezier[i].pA.z=pos.z;

	//２番目終着点の座標
	g_bezier[i].pB.x=pos.x+cosf(rot.y+D3DX_PI/2)*(fWidth*2);
	g_bezier[i].pB.y=pos.y+fHeight;
	g_bezier[i].pB.z=pos.z-sinf(rot.y+D3DX_PI/2)*(fWidth*2);

	//３番目終着点の座標
	g_bezier[i].pC.x=pos.x+cosf(rot.y+D3DX_PI/2)*-fWidth;
	g_bezier[i].pC.y=pos.y+fHeight;
	g_bezier[i].pC.z=pos.z-sinf(rot.y+D3DX_PI/2)*-fWidth;

	//最後の終着点の座標
	g_bezier[i].pD.x=pos.x+cosf(rot.y+D3DX_PI/2)*fWidth;
	g_bezier[i].pD.y=pos.y;
	g_bezier[i].pD.z=pos.z-sinf(rot.y+D3DX_PI/2)*fWidth;
}
//EOF