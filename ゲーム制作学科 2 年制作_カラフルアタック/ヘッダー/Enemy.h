#ifndef _ENEMY_H_
#define _ENEMY_H_
//*****************************************************************************
// インクルード
//*****************************************************************************
#include "main.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define RED (0)
#define YELLOW (1)
#define BLUE (2)
#define ENEMY_MAX (50)
#define ENEMY_INIT (2)
//*****************************************************************************
// 構造体
//*****************************************************************************
typedef struct
{
	float fX;
	float fY;
	float fWidth;
	float fHeight;
	float fxMove;
	float fyMove;
	float fRocate;
	int   nColor;
	int   nStart;
	bool  bUse;
}ENEMY;
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEPolygon (void);		//ポリゴンの初期化
void UpdateEPolygon (void);			//ポリゴンの更新
void ResultEPolygon (void);			//リザルト表示用ポリゴン
void DrawEPolygon (void);			//ポリゴンの描画
void UninitEPolygon (void);			//ポリゴンの終了
ENEMY GetEnemy (int i);				//敵構造体取得
void SetEnemy (ENEMY enemy,int i);	//敵構造体設置

#endif _ENEMY_H_
//EOF