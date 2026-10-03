//=============================================================================
//2Dシーン処理[scene2D.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _SCENE2D_H_
#define _SCENE2D_H_

#include "../System/renderer.h"

//++++++++++++++++++++++++++++++++++
//サブクラス
//++++++++++++++++++++++++++++++++++
//2Dポリゴンクラス
class CScene2D : public CScene
{
	//外部
	public:
		//メンバ関数
		CScene2D(int priority=5);	//コンストラクタ
		~CScene2D();				//デストラクタ
		HRESULT Init(void);			//初期化
		void Uninit(void);			//終了
		void Update(void);			//更新
		void Draw(void);			//描画
		static void Create();		//インスタンス生成

	//派生クラスのみ外部
	protected:
		LPDIRECT3DTEXTURE9 m_pD3DTex;			//テクスチャへのポインタ
		LPDIRECT3DVERTEXBUFFER9 m_pD3DVtxBuff;	//頂点バッファへのポインタ
};


#endif
//EOF