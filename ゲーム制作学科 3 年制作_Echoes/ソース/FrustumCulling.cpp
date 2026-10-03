//=============================================================================
// フラスタムカリング処理 [FrustumCulling.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "FrustumCulling.h"
#include "manager.h"
#include "Camera.h"
#include <math.h>

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
FRUSTUM CFrustum::m_frustum = {};

//=============================================================================
//初期化
//=============================================================================
void CFrustum::Init(float fAngle, float fAspect, float fNear, float fFar)
{
	D3DXVECTOR3 pos0, pos1, pos2;

	float fTan = tanf(fAngle * 0.5f);

	///////////////////////
	//		左平面		//
	/////////////////////
	pos0 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	
	pos1 = D3DXVECTOR3(	-fFar * (fTan / fAspect),
						-fFar * (fTan),
						fFar);

	pos2 = D3DXVECTOR3(pos1.x, -pos1.y, pos1.z);
	PlaneFromPoints(&pos0, &pos1, &pos2, &m_frustum.leftPlane);

	///////////////////////
	//		右平面		//
	/////////////////////
	pos0 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	pos1 = D3DXVECTOR3(	fFar * (fTan / fAspect),
						fFar * (fTan),
						fFar);

	pos2 = D3DXVECTOR3(pos1.x, -pos1.y, pos1.z);
	PlaneFromPoints(&pos0, &pos1, &pos2, &m_frustum.rightPlane);

	///////////////////////
	//		上平面		//
	/////////////////////
	pos0 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	pos1 = D3DXVECTOR3(	-fFar * (fTan / fAspect),
						fFar * (fTan),
						fFar);

	pos2 = D3DXVECTOR3(-pos1.x, pos1.y, pos1.z);
	PlaneFromPoints(&pos0, &pos1, &pos2, &m_frustum.topPlane);

	///////////////////////
	//		下平面		//
	/////////////////////
	pos0 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	pos1 = D3DXVECTOR3(	fFar * (fTan / fAspect),
						-fFar * (fTan),
						fFar);

	pos2 = D3DXVECTOR3(-pos1.x, pos1.y, pos1.z);
	PlaneFromPoints(&pos0, &pos1, &pos2, &m_frustum.bottomPlane);

	//ニアー、ファークリップ設定
	m_frustum.fNear = fNear;
	m_frustum.fFar = fFar;
}
//=============================================================================
//平面パラメータ作成
//=============================================================================
void CFrustum::PlaneFromPoints(	D3DXVECTOR3 *pPos0,D3DXVECTOR3 *pPos1,
								D3DXVECTOR3 *pPos2,PLANE *pPlane)
{
	D3DXVECTOR3 vec0, vec1, vec2;

	//原点からファーまでの距離1
	vec0 = D3DXVECTOR3(	pPos1->x - pPos0->x,
						pPos1->y - pPos0->y,
						pPos1->z - pPos0->z);

	//原点からファーまでの距離2
	vec1 = D3DXVECTOR3(pPos2->x - pPos0->x,
						pPos2->y - pPos0->y,
						pPos2->z - pPos0->z);

	//それぞれの距離の外積を求め正規化
	D3DXVec3Cross(&vec2, &vec0, &vec1);
	D3DXVec3Normalize(&vec2, &vec2);

	//外積結果を平面に設定
	pPlane->a = vec2.x;
	pPlane->b = vec2.y;
	pPlane->c = vec2.z;
	pPlane->d = -(vec2.x * pPos0->x + vec2.y * pPos0->y + vec2.z * pPos0->z);
}
//=============================================================================
//視錐台当たり判定
//=============================================================================
bool CFrustum::MeshFOVCheck(D3DXVECTOR3 pos, float fRadius)
{
	//カメラインスタンス取得
	CCamera *pCamera = CManager::GetCamera();

	//マトリックスのビュー行列取得
	D3DXMATRIX viewMtx = pCamera->GetMtxView();

	//距離
	float fDist;

	//ビュー空間の各座標を変換
	D3DXVECTOR3 viewPos = D3DXVECTOR3(	viewMtx._11 * pos.x +
										viewMtx._21 * pos.y +
										viewMtx._31 * pos.z + viewMtx._41,

										viewMtx._12 * pos.x +
										viewMtx._22 * pos.y +
										viewMtx._32 * pos.z + viewMtx._42,
										
										viewMtx._13 * pos.x +
										viewMtx._23 * pos.y +
										viewMtx._33 * pos.z + viewMtx._43);

	//ニアクリップ平面より手前の場合
	if ((viewPos.z + fRadius) < m_frustum.fNear)
	{
		return false;
	}

	//ファークリップ平面より奥の場合
	if ((viewPos.z - fRadius) > m_frustum.fFar)
	{
		return false;
	}

	//左側面の距離
	fDist = (viewPos.x * m_frustum.leftPlane.a) +
			(viewPos.z * m_frustum.leftPlane.c);

	//左側面より左側の場合
	if (fDist > fRadius)
	{
		return false;
	}

	//右側面の距離
	fDist = (viewPos.x * m_frustum.rightPlane.a) +
			(viewPos.z * m_frustum.rightPlane.c);

	//右側面より右側の場合
	if (fDist > fRadius)
	{
		return false;
	}

	//上側面の距離
	fDist = (viewPos.y * m_frustum.topPlane.b) + 
			(viewPos.z * m_frustum.topPlane.c);

	//上側面より上側の場合
	if (fDist > fRadius)
	{
		return false;
	}

	//下側面の距離
	fDist = (viewPos.y * m_frustum.bottomPlane.b) +
			(viewPos.z * m_frustum.bottomPlane.c);

	//下側面より下側の場合
	if (fDist > fRadius)
	{
		return false;
	}

	return true;
}
//EOF