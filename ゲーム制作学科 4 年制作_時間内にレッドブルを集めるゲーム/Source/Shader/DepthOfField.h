//=============================================================================
//被写界深度[DepthOfField.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _DEPTHOFFIELD_H_
#define _DEPTHOFFIELD_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Filter.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//被写界深度クラス
class CDepthOfField : public CFilter
{
	//外部
	public:
		CDepthOfField(){}							//コンストラクタ
		~CDepthOfField(){}							//デストラクタ
		void Load();								//読込
		void Uninit();								//終了
		void Draw(LPDIRECT3DDEVICE9 pDevice,		//描画
			LPDIRECT3DTEXTURE9 pBlurTexture,
			LPDIRECT3DTEXTURE9 pRenderTexture,
			LPDIRECT3DTEXTURE9 pZBuffTexture);

	//内部
	private:
		//バックバッファ用テクスチャ
		LPDIRECT3DTEXTURE9 m_pD3DZBuffTexture;
};
#endif
//EOF