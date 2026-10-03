#include "Game.h"

static int g_nTime=0;				//経過タイム
static int i=0;						//ループカウンター
static int g_nSubFuelTime=0;		//燃料が減る一定時間
static PLAYER g_Player;				//ゲーム用プレイヤー構造体
static ENEMY g_Enemy[ENEMY_MAX];	//ゲーム用敵構造体
static CARCOUNT g_Cars;				//車の数計算
static MODE g_mode;					//モード番号
//ゲーム初期化
void InitGame (void)
{
	g_nTime=0;						//タイムリセット
	g_nSubFuelTime=FUELTIME_MAX;	//燃料の減る一定時間をリセット
	g_Cars.nRed=0;					//赤の個数
	g_Cars.nYellow=0;				//黄色の個数
	g_Cars.nBlue=0;					//青の個数
}
//ゲーム更新
void UpdateGame (void)
{
	g_nTime+=COUNTUP;//タイムカウントアップ

	////////////
	//ゲッター//
	///////////
	//プレイヤー構造体のゲッター
	g_Player=GetPlayer();

	//敵構造体のゲッター
	for(i=0;i<ENEMY_MAX;i++)
	{
		g_Enemy[i]=GetEnemy(i);
	}

	//////////////////////////////////
	//プレイヤーと敵との当たり判定敵//
	//////////////////////////////////
	for(i=0;i<ENEMY_MAX;i++)
	{
		//敵との衝突時同じ色なら
		if(isRectHit((int)g_Enemy[i].fX,(int)g_Enemy[i].fY,(int)g_Enemy[i].fWidth,(int)g_Enemy[i].fHeight,
					 (int)g_Player.fX,(int)g_Player.fY,(int)g_Player.fWidth,(int)g_Player.fHeight)==TRUE &&
					 g_Enemy[i].nColor==g_Player.nColor)
		{
			PlaySound(SOUND_LABEL_SE_LOCKON);									//ロックオン音鳴らす
			g_Enemy[i].bUse=false;												//敵のスイッチOFF
			SetEffect((int)g_Enemy[i].fX-100,(int)g_Enemy[i].fY-100,g_Enemy[i].nColor);	//エフェクトセット
			AddScore(100);														//スコア加算
			AddSubFuel(200);													//燃料の加算

			//敵の色によって倒した個数を分ける
			switch(g_Enemy[i].nColor)
			{
				//赤
				case RED:
					g_Cars.nRed+=COUNTUP;
				break;

				//黄色
				case YELLOW:
					g_Cars.nYellow+=COUNTUP;
				break;

				//青
				case BLUE:
					g_Cars.nBlue+=COUNTUP;
				break;
			}

		}//敵との衝突時プレイヤーの状態がNORMALで違う色ならミス
		else if(isRectHit((int)g_Enemy[i].fX,(int)g_Enemy[i].fY,(int)g_Enemy[i].fWidth,(int)g_Enemy[i].fHeight,
					 (int)g_Player.fX,(int)g_Player.fY,(int)g_Player.fWidth,(int)g_Player.fHeight)==TRUE &&
		   g_Player.nStatus==NORMAL)
		{
			PlaySound(SOUND_LABEL_SE_EXPLOSION);	//爆発効果音鳴らす
			g_Enemy[i].bUse=false;					//敵のスイッチOFF
			g_Player.nStatus=MISS;					//プレイヤーの状態MISS
			g_Player.nCount=0;						//プレイヤーのカウントリセット
			AddSubFuel(-((g_nTime/500)*50));		//燃料の減算
		}
	}

	/////////////////////////////////////////
	//			一定時間の処理			  //
	///////////////////////////////////////

	//一定時間ごとに燃料の減る量を増やす
	if(g_nTime%FUELTIME_MAX==0)
	{
		g_nSubFuelTime-=FUELTIME_MIN;
	}

	//減る量は100まで
	if(g_nSubFuelTime<FUELTIME_MIN)
	{
		g_nSubFuelTime=FUELTIME_MIN;
	}

	//一定時間ごとに燃料を減らす　(100：はやい　～　1000：遅い)
	if(g_nTime%g_nSubFuelTime==0)
	{
		//燃料１個分を減らす
		AddSubFuel(-100);
	}

	////////////////////
	//構造体のセッター//
	///////////////////
	//プレイヤーの構造体セット
	SetPlayer(g_Player);

	//敵の構造体セット
	for(i=0;i<ENEMY_MAX;i++)
	{
		SetEnemy(g_Enemy[i],i);
	}
}
CARCOUNT GetCars (void)
{
	return g_Cars;
}
//EOF