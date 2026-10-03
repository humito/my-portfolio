//===============================================================
//3D当たり判定処理
//Author:HUMITO KIMURA
//===============================================================
//インクルードファイル
#include "main.h"
//===============================================================
//立方体当たり判定チェック
//===============================================================
bool HitQube(float aX,float aXWidth,float aY,float aYWidth,float aZ,float aZWidth,
			 float bX,float bXWidth,float bY,float bYWidth,float bZ,float bZWidth)
{
	//Aの立方体がBの立方体の範囲内にある場合TRUEを返す
	if((((aX>=bX && aX<=bXWidth) || (aXWidth>=bX && aXWidth<=bXWidth))  &&
		((aY>=bY && aY<=bYWidth) || (aYWidth>=bY && aYWidth<=bYWidth)) &&
		((aZ>=bZ && aZ<=bZWidth) || (aZWidth>=bZ && aZWidth<=bZWidth))) ||

		(((bX>=aX && bX<=aXWidth) || (bXWidth>=aX && bXWidth<=aXWidth)) &&
		((bY>=aY && bY<=aYWidth) || (bYWidth>=aY && bYWidth<=aYWidth)) &&
		((bZ>=aZ && bZ<=aZWidth) || (bZWidth>=aZ && bZWidth<=aZWidth)))
	)
	{
		//当ったらTRUEを返す
		return true;
	}

	//違う場合FALSEを返す
	return false;
}
//EOF