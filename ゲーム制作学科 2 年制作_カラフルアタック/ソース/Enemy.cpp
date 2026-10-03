#include "Enemy.h"
LPDIRECT3DTEXTURE9 g_pED3DTex=NULL;				//テクスチャへのポインタ
VERTEX_2D g_eVtx[ENEMY_MAX][4];					//頂点情報格納ワーク
ENEMY g_Enemy[ENEMY_MAX];
static int i=0;									//ループカウンタ
static int g_nEnemys=0;
static int g_nTime=0;
//メモ：敵の発進地を更新で発生させる
//プロトタイプ宣言
void StartEnemy (int x,int y, float xMove, float yMove);//敵の発進地決め
//=============================================================================
// 敵ポリゴン初期化
//=============================================================================
HRESULT InitEPolygon (void)
{
	//敵構造体初期化
	for(i=0;i<ENEMY_MAX;i++)
	{
		g_Enemy[i].fX=-100;
		g_Enemy[i].fY=-100;
		g_Enemy[i].fWidth=100;
		g_Enemy[i].fHeight=150;
		g_Enemy[i].fxMove=0.5f;
		g_Enemy[i].fyMove=0.5f;
		g_Enemy[i].fRocate=0;
		g_Enemy[i].nColor=RED;
		g_Enemy[i].nStart=i%8;
		g_Enemy[i].bUse=false;
	}
	//タイムリセット
	g_nTime=0;

	//最初の数は２体
	g_nEnemys=ENEMY_INIT;

	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pEDevice;

	//ゲッターで返す
	pEDevice=GetDevice();


	//敵テクスチャ（複数）初期化
	for(i=0;i<ENEMY_MAX;i++)
	{
		//頂点座標の代入
		g_eVtx[i][0].vtx=D3DXVECTOR3(g_Enemy[i].fX,g_Enemy[i].fY+g_Enemy[i].fHeight,0.0f);
		g_eVtx[i][1].vtx=D3DXVECTOR3(g_Enemy[i].fX,g_Enemy[i].fY,0.0f);
		g_eVtx[i][2].vtx=D3DXVECTOR3(g_Enemy[i].fX+g_Enemy[i].fWidth,g_Enemy[i].fY+g_Enemy[i].fHeight,0.0f);
		g_eVtx[i][3].vtx=D3DXVECTOR3(g_Enemy[i].fX+g_Enemy[i].fWidth,g_Enemy[i].fY,0.0f);

		
		//中身
		g_eVtx[i][0].rhw=1.0f;
		g_eVtx[i][1].rhw=1.0f;
		g_eVtx[i][2].rhw=1.0f;
		g_eVtx[i][3].rhw=1.0f;
		
		//反射光
		g_eVtx[i][0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_eVtx[i][1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_eVtx[i][2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_eVtx[i][3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

		//テクスチャ
		g_eVtx[i][0].tex=D3DXVECTOR2(1.0f,1.0f);
		g_eVtx[i][1].tex=D3DXVECTOR2(0.0f,1.0f);
		g_eVtx[i][2].tex=D3DXVECTOR2(1.0f,0.68f);
		g_eVtx[i][3].tex=D3DXVECTOR2(0.0f,0.68f);

		//RED V:0.68～1.0
		//YELLO V:0.34～0.67
		//BLUE V:0.0～0.33
		//増加値:0.33
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pEDevice,
								"data/TEXTURE/Enemy.png",
								&g_pED3DTex);

	return S_OK;
}
//=============================================================================
//敵ポリゴン更新 
//=============================================================================
void UpdateEPolygon (void)
{
	//乱数使用
	srand((unsigned float)time(NULL));

	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pEDevice;

	//ゲッターで返す
	pEDevice=GetDevice();

	//敵の移動
	for(i=0;i<g_nEnemys;i++)
	{
		if(g_Enemy[i].bUse==true)
		{
			g_Enemy[i].fX+=g_Enemy[i].fxMove;
			g_Enemy[i].fY+=g_Enemy[i].fyMove;
		}
	}


	//画面外へ出たらスイッチOFF
	for(i=0;i<g_nEnemys;i++)
	{
		if(g_Enemy[i].fY>SCREEN_HEIGHT+300 ||
		   g_Enemy[i].fY<-300 ||
		   g_Enemy[i].fX>SCREEN_WIDTH+300 ||
		   g_Enemy[i].fX<-300)
		{
			g_Enemy[i].bUse=false;
		}
	}
	

	for(i=0;i<g_nEnemys;i++)
	{
		//敵のスイッチがOFFなら
		if(g_Enemy[i].bUse==false)
		{
			//発進地番号を乱数で決める
			g_Enemy[i].nStart=rand()%8;
			//g_Enemy[i].nStart=0;

			g_Enemy[i].nColor=rand()%3;

			//敵の発進地番号によって位置を決める
			switch(g_Enemy[i].nStart)
			{
				//上からの発進
				case 0:
					//敵の発信地設定
					StartEnemy(400,0,rand()%1+1,rand()%2+0.2f);
				break;

				//下からの発進
				case 1:
					//敵の発信地設定
					StartEnemy(200,1000,rand()%2+0.1f,-(rand()%2+0.1f));
				break;

				//右からの発進
				case 2:
					//敵の発信地設定
					StartEnemy(1000,350,-((rand()%2)+0.1f),0.0f);
				break;

				//左からの発進
				case 3:
					//敵の発信地設定
					StartEnemy(0,250,0.2f,0.1f);
				break;

				//斜め左上
				case 4:
					//敵の発信地設定
					StartEnemy(0,0,0.5f,0.4f);
				break;

				//斜め右上
				case 5:
					//敵の発信地設定
					StartEnemy(1000,0,-0.5f,0.4f);
				break;

				//斜め左下
				case 6:
					//敵の発信地設定
					StartEnemy(0,1000,0.5f,-0.4f);
				break;

				//斜め右下
				case 7:
					//敵の発信地設定
					StartEnemy(1000,1000,-0.5f,-0.4f);
				break;
			}//case



			switch(g_Enemy[i].nColor)
			{
			case RED:
				g_eVtx[i][0].tex=D3DXVECTOR2(1.0f,1.0f);
				g_eVtx[i][1].tex=D3DXVECTOR2(0.0f,1.0f);
				g_eVtx[i][2].tex=D3DXVECTOR2(1.0f,0.68f);
				g_eVtx[i][3].tex=D3DXVECTOR2(0.0f,0.68f);
			break;

			case YELLOW:
				g_eVtx[i][0].tex=D3DXVECTOR2(1.0f,0.67f);
				g_eVtx[i][1].tex=D3DXVECTOR2(0.0f,0.67f);
				g_eVtx[i][2].tex=D3DXVECTOR2(1.0f,0.34f);
				g_eVtx[i][3].tex=D3DXVECTOR2(0.0f,0.34f);
			break;

			case BLUE:
				g_eVtx[i][0].tex=D3DXVECTOR2(1.0f,0.33f);
				g_eVtx[i][1].tex=D3DXVECTOR2(0.0f,0.33f);
				g_eVtx[i][2].tex=D3DXVECTOR2(1.0f,0.0f);
				g_eVtx[i][3].tex=D3DXVECTOR2(0.0f,0.0f);
			break;
			}
		}
	}

	//タイムカウントアップ
	g_nTime+=COUNTUP;


	//カウント500ごとにエネミーを増やす
	if(g_nTime%500==0)
	{
		//エネミー増やす
		g_nEnemys++;
	}

	//エネミー数最大値抑える
	if(g_nEnemys>ENEMY_MAX)
	{
		g_nEnemys=ENEMY_MAX;
	}

	for(i=0;i<g_nEnemys;i++)
	{
		//頂点座標の代入
		g_eVtx[i][0].vtx=D3DXVECTOR3(g_Enemy[i].fX,g_Enemy[i].fY+g_Enemy[i].fHeight,0.0f);
		g_eVtx[i][1].vtx=D3DXVECTOR3(g_Enemy[i].fX,g_Enemy[i].fY,0.0f);
		g_eVtx[i][2].vtx=D3DXVECTOR3(g_Enemy[i].fX+g_Enemy[i].fWidth,g_Enemy[i].fY+g_Enemy[i].fHeight,0.0f);
		g_eVtx[i][3].vtx=D3DXVECTOR3(g_Enemy[i].fX+g_Enemy[i].fWidth,g_Enemy[i].fY,0.0f);
	}
}
//=============================================================================
//リザルト表示用ポリゴン
//=============================================================================
void ResultEPolygon (void)
{
	for(int i=0;i<3;i++)
	{
		//敵３つの座標を設定
		g_Enemy[i].fY=210;
		g_Enemy[i].fX=0+(i*332);

		//テクスチャを設定
		g_eVtx[i][0].tex=D3DXVECTOR2(1.0f,1.0f-(i*0.33f));
		g_eVtx[i][1].tex=D3DXVECTOR2(0.0f,1.0f-(i*0.33f));
		g_eVtx[i][2].tex=D3DXVECTOR2(1.0f,0.68f-(i*0.33f));
		g_eVtx[i][3].tex=D3DXVECTOR2(0.0f,0.68f-(i*0.33f));

		//頂点座標の代入
		g_eVtx[i][0].vtx=D3DXVECTOR3(g_Enemy[i].fX,g_Enemy[i].fY+g_Enemy[i].fHeight,0.0f);
		g_eVtx[i][1].vtx=D3DXVECTOR3(g_Enemy[i].fX,g_Enemy[i].fY,0.0f);
		g_eVtx[i][2].vtx=D3DXVECTOR3(g_Enemy[i].fX+g_Enemy[i].fWidth,g_Enemy[i].fY+g_Enemy[i].fHeight,0.0f);
		g_eVtx[i][3].vtx=D3DXVECTOR3(g_Enemy[i].fX+g_Enemy[i].fWidth,g_Enemy[i].fY,0.0f);

	}

	//残りの敵を画面外へ
	for(int i=3;i<ENEMY_MAX;i++)
	{
		g_Enemy[i].fY=1000;

		//頂点座標の代入
		g_eVtx[i][0].vtx=D3DXVECTOR3(g_Enemy[i].fX,g_Enemy[i].fY+g_Enemy[i].fHeight,0.0f);
		g_eVtx[i][1].vtx=D3DXVECTOR3(g_Enemy[i].fX,g_Enemy[i].fY,0.0f);
		g_eVtx[i][2].vtx=D3DXVECTOR3(g_Enemy[i].fX+g_Enemy[i].fWidth,g_Enemy[i].fY+g_Enemy[i].fHeight,0.0f);
		g_eVtx[i][3].vtx=D3DXVECTOR3(g_Enemy[i].fX+g_Enemy[i].fWidth,g_Enemy[i].fY,0.0f);
	}
}
//=============================================================================
// 敵ポリゴン描画
//=============================================================================
void DrawEPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pEDevice;

	//ゲッターで返す
	pEDevice=GetDevice();

	//頂点フォーマットのセット
	pEDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pEDevice->SetTexture(0,g_pED3DTex);



	//敵ポリゴン（複数）描画
	for(i=0;i<g_nEnemys;i++)
	{
		//ポリゴンの描画
		pEDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
										2,//ポリゴンの数
										&g_eVtx[i][0],
										sizeof(VERTEX_2D));
	}

}
//=============================================================================
// 敵の発進地設定
//=============================================================================
void StartEnemy (int x,int y, float xMove, float yMove)
{
	//敵のスイッチがOFFなら

	//スイッチをONにして構造体各変数に引数を代入
	g_Enemy[i].fX=(float)x;		//X座標
	g_Enemy[i].fY=(float)y;		//Y座標
	g_Enemy[i].fxMove=xMove;	//移動量
	g_Enemy[i].fyMove=yMove;	//移動量
	g_Enemy[i].bUse=true;		//スイッチ
}
//=============================================================================
// 敵ポリゴン終了
//=============================================================================
void UninitEPolygon (void)
{
	//テクスチャの開放
	if(g_pED3DTex!=NULL)
	{
		g_pED3DTex->Release();
		g_pED3DTex=NULL;
	}

}
ENEMY GetEnemy (int i)
{
	return g_Enemy[i];
}
void SetEnemy (ENEMY enemy,int i)
{
	g_Enemy[i]=enemy;
}
//EOF 