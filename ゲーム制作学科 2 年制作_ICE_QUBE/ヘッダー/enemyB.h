//=============================================================================
//敵処理[enemyB.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _ENEMYB_H_
#define _ENEMYB_H_
//*****************************************************************************
//インクルード
//*****************************************************************************
#include "game.h"
#include "player.h"
//*****************************************************************************
//グローバル変数
//*****************************************************************************
#define ENEMYB_MAX (20)
//*****************************************************************************
//構造体
//*****************************************************************************
//列挙型　敵の状態
typedef enum
{
	TYPE_ENEMY_NORMAL=0,
	TYPE_ENEMY_ICE,
	TYPE_ENEMY_EXP,
	TYPE_ENEMY_MAX
}ENEMYTYPE;

//敵構造体
typedef struct
{
	D3DXVECTOR3			pos;				//モデルの位置
	D3DXVECTOR3			posMove;			//モデルの移動量
	D3DXVECTOR3			rot;				//モデルの向き
	D3DXVECTOR3			scl;				//モデルの大きさ
	float				fSize;				//モデルのサイズ
	int					nCnt;				//カウント
	bool				bHit;				//ヒットフラグ
	ENEMYTYPE			type;				//敵のタイプ
	D3DXMATRIX			mtxWorld;			//ワールドマトリックス
}ENEMYB;
//*****************************************************************************
//プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyB (void);
void UpdateEnemyB (void);
void DrawEnemyB (void);
void UninitEnemyB (void);
ENEMYB GetEnemyB (int num);
void HitEnemyB (int num,float fMoveX,float fMoveZ,int type);

#endif
//EOF