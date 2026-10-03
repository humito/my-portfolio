//=============================================================================
//
// シーンマネージャー処理 [manager.h]
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
class CInputKeyboard;	//キーボード
class CInputJoystick;	//ジョイスティック
class CDebug;			//デバッグ
class CSound;			//サウンド
class CTitle;			//タイトル
class CGame;			//ゲーム
class CResult;			//リザルト
class CFade;			//フェード
class CToonShader;		//トゥーンシェーダー

//*****************************************************************************
//構造体定義
//*****************************************************************************
//モード構造体
enum MODE
{
	MODE_TITLE=0,	//タイトル
	MODE_GAME,		//ゲーム
	MODE_RESULT,	//リザルト
	MODE_MAX		//モード数
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
		static CLight *GetLight(void);								//ライトのゲット
		static MODE GetMode(void);									//モードのゲット
		static void SetMode(MODE mode);								//モードのセット
	//内部
	private:
		static CRenderer *m_pRenderer;								//レンダラーのインスタンス
		static CInputKeyboard *m_pInput;							//キーボードのインスタンス
		static CInputJoystick *m_pJoyStick;							//ジョイステックインスタンス
		static CCamera *m_pCamera;									//カメラのインスタンス
		static CLight *m_pLight;									//ライトのインスタンス
		static CToonShader *m_pToon;								//トゥーンシェーダーインスタンス
		static CSound *m_pSound;									//サウンドインスタンス
		static CTitle *m_pTitle;									//タイトルインスタンス
		static CGame *m_pGame;										//ゲームインスタンス
		static CResult *m_pResult;									//リザルトのインスタンス
		static CFade *m_pFade;										//フェードインスタンス
		static MODE m_mode;											//ゲームモード

//デバッグ時のみ
#ifdef _DEBUG
		static CDebug *m_pDebug;									//デバッグのインスタンス
		bool m_bDebugPause;											//デバッグ用一時停止

#endif

};
#endif
//EOF