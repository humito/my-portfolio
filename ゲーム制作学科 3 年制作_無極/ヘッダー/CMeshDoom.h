//=============================================================================
//
// メッシュドームの処理 [CMeshDoom.h]
// Author : HUMITO KIMURA
//
//=============================================================================
#ifndef _CMESHDOOM_H_
#define _CMESHDOOM_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "CScene3D.h"
//*****************************************************************************
//クラス定義
//*****************************************************************************
//メッシュドームクラス
class CMeshDoom : public CScene3D
{
	//外部
	public:
		CMeshDoom();
		~CMeshDoom();
		HRESULT Init(D3DXVECTOR3 pos,int nNumBlockX, int nNumBlockY,float fHalfSize);
		void Uninit();
		void Update();
		void Draw();
		static void Create(D3DXVECTOR3 pos,int nNumBlockX, int nNumBlockY,float fHalfSize);
	//内部
	private:
		int		m_nNumBlockX,m_nNumBlockY;		// ブロック数
		int		m_nStartX;						//１行ごとのブロックの初め
		float	m_fSideRot;						// 頂点の角度(横)
		float	m_fLengthRot;					// 頂点の角度(縦)
		float	m_fMoveSize;					// ドームの半径をXブロック分分割した長さ
};

#endif
//EOF