//=============================================================================
// メッシュ筒の処理 [MeshCylinder.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _MESHCYLINDER_H_
#define _MESHCYLINDER_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "Scene3D.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define CYLINDER_BLOCK_X (8)	//シリンダーのブロック数X
#define CYLINDER_BLOCK_Y (1)	//シリンダーのブロック数Y
#define CYLINDER_SIZE (450.0f)	//シリンダーの高さ
#define CYLINDER_HALF (7000.0f)	//シリンダーの半径

//*****************************************************************************
//クラス定義
//*****************************************************************************
//メッシュシリンダークラス
class CMeshCylinder : public CScene3D
{
	//外部
	public:
		CMeshCylinder();									//コンストラクタ
		~CMeshCylinder();									//デストラクタ
		HRESULT Init(D3DXVECTOR3 pos,						//初期化
					int nNumBlockX,
					int nNumBlockY,
					float fSizeCylinderY,
					float fHalfSize);

		void Uninit();										//終了
		void Update();										//更新
		void Draw();										//描画

		static CMeshCylinder *Create(D3DXVECTOR3 pos,		//インスタンス生成
									int nNumBlockX,
									int nNumBlockY,
									float fSizeCylinderY,
									float fHalfSize);

	//内部
	private:
		float m_fCylinderRot;								//１つのブロックの角度
};
#endif
//EOF