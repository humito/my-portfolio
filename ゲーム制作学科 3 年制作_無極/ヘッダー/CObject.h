//=============================================================================
//その他オブジェクト処理[CObject.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _COBJECT_H_
#define _COBJECT_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "CSceneX.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define OBJECT_RECT_VERTEX_MAX (4)		//オブジェクト短形の頂点座標数

//*****************************************************************************
//クラス定義
//*****************************************************************************
//その他オブジェクト
class CObject : public CSceneX
{
	//外部
	public:
		CObject();															//コンストラクタ
		~CObject();															//デストラクタ
		HRESULT Init(char *FileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot);		//初期化
		void Uninit();														//終了
		void Update();														//更新
		void Draw();														//描画
		static void Create(char *FileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot);	//インスタンス生成
		void GetSize(float *pfSizeX,float *fSizeY,float *pfSizeZ);						//サイズの取得
	//内部
	private:
		float m_fSizeX;														//サイズX
		float m_fSizeY;														//サイズY
		float m_fSizeZ;														//サイズZ
};

#endif
//EOF