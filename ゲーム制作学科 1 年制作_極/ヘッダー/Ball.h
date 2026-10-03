#ifndef BALL_H
#define BALL_H


#include "main.h"


#define ZERO (0)
#define ONE (1)
#define TWO (2)
#define THREE (3)
#define FOUR (4)
#define FIVE (5)
#define SIX (6)


typedef struct
{
	int x;//ボール座標
	int y;
	int x_old;//ボール残像座標
	int y_old;
	int tate;//ボール縦移動スタイル
	int yoko;//ボール横移動スタイル

}STR_BALL;

typedef struct
{
	int rx;//ラケット座標
	int c;//カウンタ
	int time;//タイム
	int s;//スコア
	int R;//勝敗結果(RESULT)
	int L;//残機(LIFE)
	int Z;//ボール増加回数
	int GAME;//メインループの条件
	int WAIT;//待ち時間変数
	int out;//落ちたかのフラグ
	int RAKET;//ラケットに当った時のフラグ
	int ATTACK1;//スコア100のフラグ
	int ATTACK2;//スコア500のフラグ
	int SHOOT;//ボールを放つフラグ
}STR_GAME;

typedef struct
{
	int enemiY;//敵座標
	int enemiHP;//敵ライフ
	int enemiY_old;//バックアップ座標
}STR_ENEMI;


void ball (int *,int *,MUSIC *);
STR_BALL Getter (int);
STR_ENEMI Getenemi ();
STR_GAME Getgame ();
void Setter (STR_BALL *,STR_ENEMI *,STR_GAME *,int);



#endif BALL_H