//=============================================================================
//ゲームメイン処理[game.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _GAME_H_
#define _GAME_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "main.h"
#include "player.h"
#include "meshfield.h"
#include "wall.h"
#include "qube.h"
#include "Hit3D.h"
#include "Bezier.h"
#include "bullet.h"
#include "effect.h"
#include "enemyA.h"
#include "enemyB.h"
#include"time.h"
#include "bulletExp.h"
#include "enemyExp.h"
#include "InfoBar.h"
#include "Pause.h"
#include "Cursor.h"
#include "Score.h"
#include "Sound.h"
//*****************************************************************************
//定数定義
//*****************************************************************************
//*****************************************************************************
//プロトタイプ宣言
//*****************************************************************************
HRESULT InitGame();								//ゲーム初期化
void UpdateGame();								//ゲーム更新
void DrawGame();								//ゲーム描画
void UninitGame();								//ゲーム終了
void SetPause (bool flag);

#endif
//EOF