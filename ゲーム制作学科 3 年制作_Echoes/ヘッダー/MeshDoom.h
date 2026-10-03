//=============================================================================
// メッシュドームの処理 [MeshDoom.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _MESHDOOM_H_
#define _MESHDOOM_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "Scene3D.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define DOOM_BLOCK_X (40)	//ドームのブロック数X
#define DOOM_BLOCK_Y (40)	//ドームのブロック数Y
#define DOOM_HALF (7500.0f)	//ドームの半径

//*****************************************************************************
//クラス定義
//*****************************************************************************
//メッシュドームクラス
class CMeshDoom : public CScene3D
{
	//外部
	public:
		CMeshDoom();								//コンストラクタ
		~CMeshDoom();								//デストラクタ
		HRESULT Init(	char *pFileName,			//初期化
						D3DXVECTOR3 pos,
						int nNumBlockX,
						int nNumBlockY,
						float fHalfSize);
		void Uninit();								//終了
		void Update();								//更新
		void Draw();								//描画
		static CMeshDoom *Create(char *pFileName,	//インスタンス生成
								D3DXVECTOR3 pos,
								int nNumBlockX,
								int nNumBlockY,
								float fHalfSize);
	//内部
	private:
		float	m_fSideRot;							// 頂点の角度(横)
		float	m_fLengthRot;						// 頂点の角度(縦)
};


#endif
//EOF