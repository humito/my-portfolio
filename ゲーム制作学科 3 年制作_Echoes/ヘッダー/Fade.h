//=============================================================================
// フェード処理 [Fade.h]
// Author : 木村　文登
//=============================================================================
#ifndef _CFADE_H_
#define _CFADE_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "renderer.h"
#include "manager.h"
#include "scene2D.h"

//*****************************************************************************
//構造体
//*****************************************************************************
//フェードの状態
enum FADE_MODE
{
	FADE_NONE = 0,	//フェードなし
	FADE_IN,		//フェードイン
	FADE_OUT,		//フェードアウト
	FADE_MAX		//フェード状態数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//フェードクラス
class CFade
{
	//外部
	public:
		CFade();								//コンストラクタ
		~CFade(){}								//デストラクタ
		HRESULT Init();							//初期化
		void Uninit();							//終了
		void Update();							//更新
		void Draw();							//描画
		static void SetFade(MODE next);			//フェードセット
		static FADE_MODE GetFade();				//フェードゲット
		static CFade *Create();					//インスタンス生成

	//内部
	private:
		LPDIRECT3DTEXTURE9 m_pD3DTex;			//テクスチャへのポインタ
		LPDIRECT3DVERTEXBUFFER9 m_pD3DVtxBuff;	//頂点バッファへのポインタ
		float m_fWidth;							//横幅
		float m_fHeight;						//縦幅
		int m_nFadeCnt;							//フェードカウント
		int m_nAlpha;							//α値
		static FADE_MODE m_FadeMode;			//フェードの状態
		static MODE m_nextMode;					//次のモード
		void ChangeBuffer();					//頂点情報の変更
};
#endif
//EOF