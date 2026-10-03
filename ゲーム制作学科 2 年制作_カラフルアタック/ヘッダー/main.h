//=============================================================================
//
// プリミティブ表示処理 [main.h]
// Author : 木村　文登
//
//=============================================================================
#ifndef _MAIN_H_
#define _MAIN_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "d3dx9.h"
#include "input.h"
#include "dinput.h"
#include "PlayerPolygon.h"
#include "Game.h"
#include "Enemy.h"
#include "Hit.h"
#include "title.h"
#include "TitleBg.h"
#include "result.h"
#include "fade.h"
#include "Effect.h"
#include "Score.h"
#include "Pause.h"
#include "Cursor.h"
#include "Fuel.h"
#include "bg.h"
//#include "CoinPolygon.h"
#include "Sound.h"
#include "xaudio2.h"
/*#include "bg.h"
#include "bgt.h"
#include "bgr.h"*/
//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#pragma comment (lib,"d3d9.lib")
#pragma comment (lib,"d3dx9.lib")
#pragma comment (lib,"dxguid.lib")
#pragma comment (lib,"dinput8.lib")
#pragma comment (lib,"winmm.lib")
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"DirectX雛形"		// ウインドウのキャプション名
#define SCREEN_WIDTH (1000)//幅
#define SCREEN_HEIGHT (1000)//高さ
#define FVF_VERTEX_2D (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)//頂点フォーマット(２Ｄ用)
//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 vtx;	//頂点座標
	float       rhw;	//(中身は1.0f)
	D3DCOLOR    diffuse;//反射光
	D3DXVECTOR2 tex;	//テクスチャ座標
}VERTEX_2D;


typedef enum
{
	MODE_TITLE=0,	//タイトル画面
	MODE_GAME,		//ゲーム画面
	MODE_RESULT,	//リザルト画面
	MODE_MAX
}MODE;
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//メッセージ
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);						//ウィンドウ初期化
LPDIRECT3DDEVICE9 GetDevice(void);												//g_pD3DDeviceのゲッター
void Uninit(void);																//終了
void Update(void);																//更新
void Draw(void);																//描画
void SetMode (MODE mode);														//モードのセッター
MODE GetMode (void);															//モードのゲッター
int GetPause (void);
void SetPause (int data);

#endif _MAIN_H_
//EOF