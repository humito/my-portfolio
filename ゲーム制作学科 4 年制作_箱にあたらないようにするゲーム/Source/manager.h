//=============================================================================
//
// マネージャー処理 [manager.h]
// Author : 木村　文登
//
//=============================================================================
#ifndef _MANAGER_H_
#define _MANAGER_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <windows.h>
#include "System/renderer.h"

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CRenderer;		//レンダラークラス
class CCamera;			//カメラ
class CLight;			//ライト
class CInput;			//入力
class CInputKeyboard;	//キーボード
class CInputMouse;		//マウス
class CDebug;			//デバッグ
class CState;			//ステート

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//ステート番号
enum STATE_INDEX
{
	TITLE_STATE = 0,//タイトル
	GAME_STATE,		//ゲーム
	RESULT_STATE,	//リザルト
	STATE_MAX		//ステート数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//マネージャークラス
class CManager
{
	//外部
	public:
		CManager(){}												//コンストラクタ
		~CManager(){}												//デストラクタ
		HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);	//初期化
		void Uninit();												//終了
		void Update();												//更新
		void Draw();												//描画

		//カメラ取得
		static CCamera *GetCamera()
		{ return m_pCamera; }

		//ライト取得
		static CLight *GetLight()
		{ return m_pLight; }

		//レンダラー取得
		static CRenderer *GetRenderer()
		{ return m_pRenderer; }

		//ステート取得
		static CState *GetState()
		{ return m_pState; }

		//ステート番号のセット
		static void SetState(STATE_INDEX index);

		//ステート番号取得
		static STATE_INDEX GetStateIndex()
		{ return m_StateIndex; }

		//内部
	private:
		static CRenderer		*m_pRenderer;	//レンダラーのインスタンス
		static CInputKeyboard	*m_pInput;		//キーボードのインスタンス
		static CInputMouse		*m_pMouse;		//マウスインスタンス
		static CCamera			*m_pCamera;		//カメラのインスタンス
		static CLight			*m_pLight;		//ライトのインスタンス
		static CState			*m_pState;		//ステート
		static STATE_INDEX		m_StateIndex;	//ステート番号

#ifdef _DEBUG
		static CDebug *m_pDebug;				//デバッグのインスタンス
#endif
};
#endif
//EOF