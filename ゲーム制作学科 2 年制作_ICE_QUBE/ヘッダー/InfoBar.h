//=============================================================================
//インフォメーション処理[InfoBar.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _INFOBAR_H_
#define _INFOBAR_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "game.h"
//*****************************************************************************
//構造体宣言
//*****************************************************************************
//列挙型情報種類
typedef enum
{
	TUTOREAL_MOVE=0,
	TUTOREAL_SHOT,
	TUTOREAL_ATTACK,
	TUTOREAL_ENEMY,
	BONUS_ITEM,
	BONUS_DAMEGE,
	BONUS_WALL,
	BONUS_GET,	BONUS_TIME,
	BONUS_COMBO,
	BONUS_CAOS,
	BONUS_REFRESH,
	INFO_ADD_ENEMY,
	INFO_ADD_LEVEL,
	INFO_PLAYER_ICE,
	INFO_SPEED_UP,
	INFO_SNOWSTORM,
	INFO_ROTATION,
	INFO_MAX
}INFOTYPE;

//情報表示構造体
typedef struct
{
	float fX;
	float fWidth;
	float fY;
	float fHeight;
	float fU;
	float fDestU;
	float fV;
	float fDestV;
	int   nCnt;
}INFO;
//*****************************************************************************
//プロトタイプ宣言
//*****************************************************************************
HRESULT InitInfo (void);
void UpdateInfo (void);
void DrawInfo (void);
void UninitInfo (void);
void SetInfo(INFOTYPE disp);
#endif
///EOF