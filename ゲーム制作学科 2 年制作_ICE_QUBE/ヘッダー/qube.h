//=============================================================================
//立方体の処理[qube.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _QUBE_H_
#define _QUBE_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "game.h"
//*****************************************************************************
//定数定義
//*****************************************************************************
#define QUBE_MAX (80)				//立方体の最大値
//*****************************************************************************
//構造体宣言
//*****************************************************************************
//立方体
typedef struct
{
	D3DXVECTOR3 pos;			//座標
	D3DXVECTOR3 posMove;		//移動量
	D3DXVECTOR3 rot;			//角度
	int			nAlpha;			//α値
	int			nColor;			//全体の色
	float		fHalfWidth;		//半分の幅
	float		fHalfHeight;	//半分の高さ
	bool		bSet;			//設置可能フラグ
	bool		bMove;			//移動フラグ
}QUBE;
//*****************************************************************************
//プロトタイプ宣言
//*****************************************************************************
HRESULT InitQube(float posX,float posY,float posZ,float Width,float Height);	//立方体の初期化
void UninitQube(void);															//立方体の終了
void UpdateQube(void);															//立方体の更新
void DrawQube(void);															//立方体の描画
void GetQube (int num,QUBE *data);
void CreateQube (float posX,float posZ,float Width,float Height);
void AddMove (int num,float fMoveX,float fMoveZ,float fRotY);
#endif
//EOF