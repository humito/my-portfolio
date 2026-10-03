//=============================================================================
//ジョイスティック入力処理[CInputJoyStick.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CINPUTJOYSTICK_H_
#define _CINPUTJOYSTICK_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Input.h"

//*****************************************************************************
//構造体定義
//*****************************************************************************
//ゲームパッド各ボタンのラベル
enum GAME_PAD_NAME
{
	STICK_L_LEFT=0,	//左スティック 左方向
	STICK_L_RIGHT,	//左スティック 右方向
	STICK_L_UP,		//左スティック 上方向
	STICK_L_DOWN,	//左スティック 下方向
	STICK_R_LEFT,	//右スティック 左方向
	STICK_R_RIGHT,	//右スティック 右方向
	STICK_R_UP,		//右スティック 上方向
	STICK_R_DOWN,	//右スティック 下方向
	ARROW_UP,		//十字キー 上方向
	ARROW_RIGHT,	//十字キー 右方向
	ARROW_DOWN,		//十字キー 下方向
	ARROW_LEFT,		//十字キー 左方向
	BUTTON_1,		//ボタン1
	BUTTON_2,		//ボタン2
	BUTTON_3,		//ボタン3
	BUTTON_4,		//ボタン4
	TRIGGER_L,		//左トリガー
	TRIGGER_R,		//右トリガー
	BUTTON_BACK,	//バックボタン
	BUTTON_START,	//スタートボタン
	PAD_MAX			//パッド最大数
};
//*****************************************************************************
//クラス定義
//*****************************************************************************
class CInputJoystick : public CInput
{
	//外部
	public:
		CInputJoystick();
		~CInputJoystick();
		HRESULT Init(HINSTANCE hInstance, HWND hWnd);
		void Uninit();
		void Update();
		static bool GetPadPress(GAME_PAD_NAME PadName);			//プレス判定
		static bool GetPadTrigger(GAME_PAD_NAME PadName);		//トリガー判定
		static bool GetPadRelease(GAME_PAD_NAME PadName);		//リリース判定

	//内部
	private:
		LPDIRECTINPUTDEVICE8	m_pDIDevJoystick;	//ジョイスティックデバイス
		DIDEVCAPS				m_diDevCaps;		//デバイス能力
		const DIDEVICEOBJECTINSTANCE	*m_pDidoi;	//デバイスオブジェクトのインスタンス
		static BYTE m_aPadState[PAD_MAX];			//パッド情報
		static BYTE m_pPadState[PAD_MAX];			//前パッド情報
		static BYTE m_tPadState[PAD_MAX];			//トリガー情報
		static BYTE m_rPadState[PAD_MAX];			//リリース情報

		BOOL CALLBACK EnumJoysticksCallback();		//ジョイスティック生成
		BOOL CALLBACK EnumAxesCallback();			//プロパティの設定
};

#endif
//EOF