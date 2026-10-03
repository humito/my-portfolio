//=============================================================================
// フェード処理 [CFade.h]
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
	FADE_NONE=0,//フェードなし
	FADE_IN,	//フェードイン
	FADE_OUT,	//フェードアウト
	FADE_MAX	//フェード状態数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//フェードクラス
class CFade : public CScene2D
{
	//外部
	public:
		CFade(int priority=6);							//コンストラクタ
		~CFade();										//デストラクタ
		HRESULT Init();									//初期化
		void Uninit();									//終了
		void Update();									//更新
		void Draw();									//描画
		static void SetFade(FADE_MODE mode,MODE next);	//フェードセット
		static FADE_MODE GetFade();						//フェードゲット
		static CFade *Create();							//インスタンス生成

	//内部
	private:
		void ChangeBuffer();								//頂点情報の変更
		float m_fWidth;										//横幅
		float m_fHeight;									//縦幅
		int m_nFadeCnt;										//フェードカウント
		int m_nAlpha;										//α値
		static FADE_MODE m_FadeMode;						//フェードの状態
		static MODE m_nextMode;								//次のモード
		static bool m_bChangeMode;							//モード変更フラグ
};
#endif
//EOF