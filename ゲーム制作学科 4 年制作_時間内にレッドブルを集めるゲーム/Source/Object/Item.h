//=============================================================================
//アイテム処理[Item.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _ITEM_H_
#define _ITEM_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "../Scene/SceneX.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//アイテムクラス
class CItem : public CSceneX
{
	//外部
	public:
		CItem();									//コンストラクタ
		~CItem(){}									//デストラクタ

		//初期化
		HRESULT Init(D3DXVECTOR3 pos,
					D3DXVECTOR3 rot);

		void Uninit();								//終了
		void Update();								//更新
		void Draw();								//描画

		//テクスチャ配列解放
		static void DeleteTextureArray();

		//インスタンス生成
		static CItem *Create(D3DXVECTOR3 pos,
								D3DXVECTOR3 rot);

	//内部
	private:
		//処理効率をよくするためテクスチャ配列単体で持つ
		static LPDIRECT3DTEXTURE9	*m_pD3DTextureArray;
		static int					m_nNumMat;
};
#endif
//EOF