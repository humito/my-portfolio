//=============================================================================
// キーボード処理 [InputKeyboard.h]
// Author : 木村　文登
//=============================================================================
#ifndef _CINPUTKEYBOARD_H_
#define _CINPUTKEYBOARD_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Input.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define KEY_MAX (256)//キーの数

//*****************************************************************************
//クラス定義
//*****************************************************************************
//キーボード入力クラス
class CInputKeyboard : public CInput
{
	//外部
	public:
		CInputKeyboard();								//コンストラクタ
		~CInputKeyboard();								//デストラクタ
		HRESULT Init(HINSTANCE hInstance, HWND hWnd);	//初期化
		void Uninit(void);								//終了
		void Update(void);								//更新

		static bool GetKeyPress(int nKey);				//プレス判定
		static bool GetKeyTrigger(int nKey);			//トリガー判定
		static bool GetKeyRepeat(int nKey);				//リピート判定
		static bool GetKeyRelease(int nKey);			//リリース判定

	//内部
	private:
		LPDIRECTINPUTDEVICE8 m_pDIDevKeyboard;			//キーボードデバイス
		static BYTE m_aKeyState[KEY_MAX];				//リピート情報
		static BYTE m_pKeyState[KEY_MAX];				//キー情報
		static BYTE m_tKeyState[KEY_MAX];				//トリガー情報
		static BYTE m_rKeyState[KEY_MAX];				//リリース情報
		static int m_nCount;							//カウント

};

#endif
//EOF