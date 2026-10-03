//=============================================================================
// 画像表示 [Image.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _IMAGE_H_
#define _IMAGE_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "../Scene/scene2D.h"
#include <Windows.h>

//*****************************************************************************
//クラス定義
//*****************************************************************************
//画像クラス
class CImage : public CScene2D
{
	//外部
	public:
		CImage();									//コンストラクタ
		~CImage();									//デストラクタ
		HRESULT Init(	char *pFileName,			//初期化
						D3DXVECTOR3 pos,
						float fWidth,
						float fHeight);

		void Uninit();								//終了
		void Draw();								//描画

		void ChangeTexture(char *pFileName);		//テクスチャの変更

		static CImage *Create(	char *pFileName,	//インスタンス生成
								D3DXVECTOR3 pos,
								float fWidth,
								float fHeight);

	//内部
	private:
		float m_fHalfWidth;							//半分の幅
		float m_fHalfHeight;						//半分の高さ

		void ChangeBuffer();						//頂点バッファの変更
};
#endif
//EOF