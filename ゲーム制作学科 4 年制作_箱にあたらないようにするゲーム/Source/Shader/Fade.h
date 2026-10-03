//=============================================================================
// フェードシェーダー[Fade.h]
// Author : 木村 文登
//=============================================================================
#ifndef _FADE_H_
#define _FADE_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Filter.h"
#include "../manager.h"

//*****************************************************************************
//構造体定義
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
//フェードフィルタークラス
class CFade : public CFilter
{
	//外部
	public:
		CFade(){}
		~CFade(){}
		void Load();								//読込
		void Uninit();								//終了
		void Update();								//更新
		void Draw(LPDIRECT3DDEVICE9 pDevice);		//描画

		//フェードの状態取得
		static FADE_MODE GetFade()
		{ return m_FadeMode; }

		//フェード開始
		static void StartFade(STATE_INDEX nextState)
		{
			m_nextState = nextState;

			if (m_FadeMode == FADE_NONE)
				m_FadeMode = FADE_IN;
		}

	//内部
	private:
		float m_fFadeTime;						//フェードを掛ける時間
		static FADE_MODE m_FadeMode;			//フェードの状態
		static STATE_INDEX m_nextState;			//次のステート
};
#endif
//EOF