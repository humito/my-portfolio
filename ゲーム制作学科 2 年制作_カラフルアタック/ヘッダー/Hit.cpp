#include "Hit.h"

//*****************************************************************************
//’ZŒ`“–‚½‚è”»’èˆ—
//*****************************************************************************
BOOL isRectHit(int ax,int ay,int aw,int ah,int bx,int by,int bw,int bh)
{
	return ax<bx+bw && bx<ax+aw && 
		   ay<by+bh && by<ay+ah ? TRUE : FALSE;
}
//*****************************************************************************
//‰~Œ`“–‚½‚è”»’èˆ—
//*****************************************************************************
BOOL isCircleHit(int ax,int ay,int ar,int bx,int by,int br)
{
	int l=(bx-ax)*(bx-ax)+(by-ay)*(by-ay);
	
	return (ar+br)*(ar+br)<l;
}
