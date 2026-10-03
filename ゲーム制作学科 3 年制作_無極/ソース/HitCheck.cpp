//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "HitCheck.h"
//=============================================================================
//円形を使った当たり判定
//=============================================================================
bool EllipsCheck(D3DXVECTOR3 pos1,D3DXVECTOR3 pos2,float fHalfSize1,float fHalfSize2)
{
	//対象１から２への方向ベクトル
	D3DXVECTOR3 vec=pos1-pos2;

	//対象１から２への距離
	float fLength=D3DXVec3LengthSq(&vec);

	//距離を２乗する
	fLength=(float)pow(fLength,2);

	//対象の半径の合計を２乗する
	float fSizeSum=(float)pow((fHalfSize1+fHalfSize2),2);

	//距離が半径の合計以下ならば範囲に入っている
	if(fLength<=fSizeSum)
	{
		return true;
	}

	return false;
}
//=============================================================================
//立方体を使った当たり判定
//=============================================================================
bool CubeCheck(D3DXVECTOR3 pos1,D3DXVECTOR3 pos2,float fSizeX1,float fSizeY1,float fSizeZ1,float fSizeX2,float fSizeY2,float fSizeZ2)
{
	if(((pos1.x-fSizeX1<=pos2.x-fSizeX2 && pos1.x+fSizeX1>=pos2.x-fSizeX2) ||
		(pos1.x-fSizeX1<=pos2.x+fSizeX2 && pos1.x+fSizeX1>=pos2.x+fSizeX2)) &&
		((pos1.z-fSizeZ1<=pos2.z-fSizeZ2 && pos1.z+fSizeZ1>=pos2.z-fSizeZ2) ||
		(pos1.z-fSizeZ1<=pos2.z+fSizeZ2 && pos1.z+fSizeZ1>=pos2.z+fSizeZ2)))
	{
		return true;
	}

	return false;
}
//EOF