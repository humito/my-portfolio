//=============================================================================
//
// 入力処理 [input.cpp]
// Author : HUMITO KIMURA
//
//=============================================================================
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECTINPUT8 g_pDInput=NULL;
LPDIRECTINPUTDEVICE8 g_pDIDevKeyboard=NULL;
BYTE g_aKeyState[256];
BYTE g_pKeyState[256];
BYTE g_tKeyState[256];
BYTE g_rKeyState[256];
int nCount=0;
//=============================================================================
// キーボードの初期化
//=============================================================================
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT hr;
	
	// DirectInputオブジェクトの作成
	if(g_pDInput==NULL)
	{
		hr=DirectInput8Create(hInstance,DIRECTINPUT_VERSION,
							  IID_IDirectInput8,(void**)&g_pDInput,
							  NULL);
		if(FAILED(hr))
		{
			return hr;
		}
	}

	// デバイスオブジェクトを作成
	hr=g_pDInput->CreateDevice(GUID_SysKeyboard,
							   &g_pDIDevKeyboard,NULL);

	if(FAILED(hr))
	{
		return hr;
	}


	// データフォーマットを設定
	hr=g_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);

	if(FAILED(hr))
	{
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr=g_pDIDevKeyboard->SetCooperativeLevel(hWnd,
											(DISCL_FOREGROUND
											| DISCL_NONEXCLUSIVE));

	if(FAILED(hr))
	{
		return hr;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	g_pDIDevKeyboard->Acquire();

	return S_OK;
}

//=============================================================================
// キーボードの終了処理
//=============================================================================
void UninitKeyboard(void)
{
	// DirectInputオブジェクトの開放
	if(g_pDInput!=NULL)
	{
		g_pDInput->Release();
		g_pDInput=NULL;
	}

	// デバイスオブジェクトの開放
	if(g_pDIDevKeyboard!=NULL)
	{
		g_pDIDevKeyboard->Unacquire();
		g_pDIDevKeyboard->Release();
		g_pDIDevKeyboard=NULL;
	}
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
void UpdateKeyboard(void)
{
	//キー情報取得用
	BYTE aKeyState[256];

	//データを取得
	if(SUCCEEDED(g_pDIDevKeyboard->GetDeviceState(
									sizeof(aKeyState),
									&aKeyState[0])))
	{
		//キーの更新
		for(int nKey=0;nKey<256;nKey++)
		{
			g_pKeyState[nKey]=g_aKeyState[nKey];											//Prev
			g_aKeyState[nKey]=aKeyState[nKey];												//Read
			g_tKeyState[nKey]=(g_pKeyState[nKey] ^ g_aKeyState[nKey]) & g_aKeyState[nKey];	//trig
			g_rKeyState[nKey]=(g_pKeyState[nKey] ^ g_aKeyState[nKey]) & g_pKeyState[nKey];	//release
		}
	}
	else
	{
		//キーボードへのアクセス権取得（入力できる状態）
		g_pDIDevKeyboard->Acquire();
	}

	//g_ReleaseKey=(g_PrevKey ^ g_ReadKey) & g_PrevKey;
}
//=============================================================================
// キーボードのプレス状態を取得
//=============================================================================
bool GetKeyboardPress(int nKey)
{

	//引数と同じキーが押されたら
	if((g_aKeyState[nKey] & 0x80))
	{
		return true;
	}

	//何も押されない時はfalseで返す
	return false;
}
//=============================================================================
// キーボードのトリガー状態を取得
//=============================================================================
bool GetKeyboardTrigger(int nKey)
{
	if(g_tKeyState[nKey] & 0x80)
	{
		return true;
	}

	return false;
}

//=============================================================================
// キーボードのリピート状態を取得
//=============================================================================
bool GetKeyboardRepeat(int nKey,bool push)
{
	//入力判定
	if(g_aKeyState[nKey] & 0x80)
	{
		//カウント5～35入力不可
		if(nCount<5 || nCount>35)
		{
			nCount++;
			return true;
		}
		else
		{
			//5～35の間はカウントアップ
			nCount++;
		}
	}
	else if(push==true)
	{
		//何も押されてなくて押した判定がtrueならカウントリセット
		nCount=0;
	}

	return false;
}

//=============================================================================
// キーボードのリリ－ス状態を取得
//=============================================================================
bool GetKeyboardRelease(int nKey)
{
	if(g_rKeyState[nKey] & 0x80)
	{
		return true;
	}

	return false;
}
//EOF