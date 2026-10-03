//=============================================================================
// フラスタムカリング処理 [FrustumCulling.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _FRUSTUMCULLING_H_
#define _FRUSTUMCULLING_H_

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "renderer.h"

//*****************************************************************************
//構造体定義
//*****************************************************************************
//平面構造体
struct PLANE
{
	float a;
	float b;
	float c;
	float d;
};

//視錐台
struct FRUSTUM
{
	PLANE leftPlane;
	PLANE rightPlane;
	PLANE topPlane;
	PLANE bottomPlane;
	float fNear;
	float fFar;
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//フラスタムカリングクラス
class CFrustum
{
	//外部
	public:
		CFrustum(){}
		~CFrustum(){}
		//初期化
		static void Init(float fAngle,float fAspect,float fNear,float fFar);
		//視錐台当たり判定
		static bool MeshFOVCheck(D3DXVECTOR3 pos,float fRadius);

	//内部
	private:
		//視錐台
		static FRUSTUM m_frustum;

		//平面パラメータ作成
		static void PlaneFromPoints(D3DXVECTOR3 *pPos0,
									D3DXVECTOR3 *pPos1,
									D3DXVECTOR3 *pPos2,
									PLANE *pPlane);
};
#endif
//EOF