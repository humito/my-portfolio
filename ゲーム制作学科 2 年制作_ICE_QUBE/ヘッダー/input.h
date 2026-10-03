//=============================================================================
//
// 入力処理 [input.h]
// Author : 木村　文登
//
//=============================================================================
#ifndef _INPUT_H_
#define _INPUT_H_
//*****************************************************************************
// インクルード
//*****************************************************************************
#include "main.h"
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd);
void UninitKeyboard(void);
void UpdateKeyboard(void);

bool GetKeyboardPress(int nKey);
bool GetKeyboardTrigger(int nKey);
bool GetKeyboardRepeat(int nKey,bool push);
bool GetKeyboardRelease(int nKey);

#endif _INPUT_H_
//EOF