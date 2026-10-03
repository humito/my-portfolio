//=============================================================================
//
// メッシュ筒の処理 [CMeshCylinder.cpp]
// Author : HUMITO KIMURA
//
//=============================================================================
#ifndef _CMESHCYLINDER_H_
#define _CMESHCYLINDER_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "CScene3D.h"
//*****************************************************************************
//クラス定義
//*****************************************************************************
//メッシュシリンダークラス
class CMeshCylinder : public CScene3D
{
	//外部
	public:
	CMeshCylinder();
	~CMeshCylinder();
	HRESULT Init(D3DXVECTOR3 pos,int nNumBlockX, int nNumBlockY,float fSizeCylinderY,float fHalfSize);
	void Uninit();
	void Update();
	void Draw();
	static void Create(D3DXVECTOR3 pos,int nNumBlockX, int nNumBlockY,float fSizeCylinderY,float fHalfSize);
	
	//内部
	private:
	int m_nNumBlockX, m_nNumBlockY;					// ブロック数
	float m_fSizeBlockX, m_fSizeBlockY;				// ブロックサイズ
	float m_fCylinderRot;							//１つのブロックの角度
};

#endif
//EOF