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
#include "renderer.h"

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CRenderer;		//レンダラークラス
class CCamera;			//カメラ
class CLight;			//ライト
class CInput;			//入力
class CFrustum;			//視錐台
class CInputKeyboard;	//キーボード
class CInputJoystick;	//ゲームパッド
class CSound;			//サウンド
class CDebug;			//デバッグ
class CTitle;			//タイトル
class CGame;			//ゲーム
class CResult;			//リザルト
class CMenu;			//メニュー画面

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//モード(ステート)
enum MODE
{
	TITLE_MODE=0,	//タイトル
	MENU_MODE,		//メニュー
	GAME_MODE,		//ゲーム
	RESULT_MODE,	//リザルト
	MODE_NUM		//モード数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//マネージャークラス
class CManager
{
	//外部
	public:
		CManager();													//コンストラクタ
		~CManager();												//デストラクタ
		HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);	//初期化
		void Uninit();												//終了
		void Update();												//更新
		void Draw();												//描画
		static CRenderer *GetRenderer(void);						//レンダラーのゲット
		static CCamera *GetCamera(void);							//カメラのゲット
		static void SetMode(MODE mode);								//モードセット

	//内部
	private:
		static CRenderer *m_pRenderer;								//レンダラーのインスタンス
		static CInputKeyboard *m_pInput;							//キーボードのインスタンス
		static CInputJoystick *m_pJoyStick;							//ゲームパッド
		static CCamera *m_pCamera;									//カメラのインスタンス
		static CFrustum *m_pFrustum;								//視錐台
		static CSound *m_pSound;									//サウンド
		static CTitle *m_pTitle;									//タイトル管理
		static CMenu *m_pMenu;										//メニュー管理
		static CGame *m_pGame;										//ゲーム管理
		static CResult *m_pResult;									//リザルト管理
		static MODE m_Mode;											//モード

//デバッグ時のみ
#ifdef _DEBUG
		static CDebug *m_pDebug;									//デバッグのインスタンス
		bool m_bStop;												//停止フラグ
#endif
};
#endif
//EOF