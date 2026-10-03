//=============================================================================
//
//ポリゴン処理 [PlayerPolygon.cpp]
// Author : 木村　文登
//
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "PlayerPolygon.h"
//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DTex=NULL;			//テクスチャへのポインタ
VERTEX_2D g_aVtx[4];						//頂点情報格納ワーク
static PLAYER g_Player;						//プレイヤー構造体
//=============================================================================
// プレイヤーポリゴン初期化関数
//=============================================================================
HRESULT InitPolygon (void)
{
	//横・縦・幅の初期値代入
	g_Player.fX=NEWPOS_X;
	g_Player.fY=NEWPOS_Y;
	g_Player.fWidth=100;
	g_Player.fHeight=150;
	g_Player.nCount=0;
	g_Player.nStatus=NORMAL;
	g_Player.nColor=RED;
	g_Player.nAlpha=255;

	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice;

	//ゲッターで返す
	pDevice=GetDevice();
		
	//頂点座標の代入
	g_aVtx[0].vtx=D3DXVECTOR3(g_Player.fX,g_Player.fY+g_Player.fHeight,0.0f);
	g_aVtx[1].vtx=D3DXVECTOR3(g_Player.fX,g_Player.fY,0.0f);
	g_aVtx[2].vtx=D3DXVECTOR3(g_Player.fX+g_Player.fWidth,g_Player.fY+g_Player.fHeight,0.0f);
	g_aVtx[3].vtx=D3DXVECTOR3(g_Player.fX+g_Player.fWidth,g_Player.fY,0.0f);

		
	//中身
	g_aVtx[0].rhw=1.0f;
	g_aVtx[1].rhw=1.0f;
	g_aVtx[2].rhw=1.0f;
	g_aVtx[3].rhw=1.0f;
		
	//反射光
	g_aVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,g_Player.nAlpha);
	g_aVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,g_Player.nAlpha);
	g_aVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,g_Player.nAlpha);
	g_aVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,g_Player.nAlpha);

	//テクスチャ
	g_aVtx[0].tex=D3DXVECTOR2(1.0f,1.0f);
	g_aVtx[1].tex=D3DXVECTOR2(0.0f,1.0f);
	g_aVtx[2].tex=D3DXVECTOR2(1.0f,0.0f);
	g_aVtx[3].tex=D3DXVECTOR2(0.0f,0.0f);
	
	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/sample000.png",
							  &g_pD3DTex);
	return S_OK;
}
//=============================================================================
// プレイヤーポリゴン描画関数
//=============================================================================
void DrawPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice;

	//ゲッターで返す
	pDevice=GetDevice();

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);
	//テクスチャの設定
	pDevice->SetTexture(0,g_pD3DTex);

	//これを使うと重なる時透明になる
	//pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

	//ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
									2,//ポリゴンの数
									&g_aVtx[0],
									sizeof(VERTEX_2D));
}
//=============================================================================
// プレイヤーポリゴン更新関数
//=============================================================================
void UpdatePolygon (void)
{
	//キー押したかのフラグ（カウントの整合性のため）
	static bool lPush=false;
	static bool rPush=false;
	static float fBackUpX;				//復帰処理に使うプレイヤーX座標バックアップ

	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice;

	//ゲッターで返す
	pDevice=GetDevice();


	/////////////////////////////////////
	//////		 入力処理		  //////
	///////////////////////////////////

	//////////////
	//	移動	//
	/////////////

	//左移動
	if(GetKeyboardPress(DIK_A)==true)
	{
		g_Player.fX-=1.0f;
	}

	//右移動
	if(GetKeyboardPress(DIK_D)==true)
	{
		g_Player.fX+=1.0f;
	}

	//上移動
	if(GetKeyboardPress(DIK_W)==true)
	{
		g_Player.fY-=1.0f;
	}

	//下移動
	if(GetKeyboardPress(DIK_S)==true)
	{
		g_Player.fY+=1.0f;
	}


	//////////////////////
	//	車の切り替え	//
	/////////////////////
	//赤の車
	if(GetKeyboardTrigger(DIK_LEFT)==true)
	{
		//テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
								  "data/TEXTURE/sample000.png",
								  &g_pD3DTex);
		g_Player.nColor=RED;
	}

	//黄色の車
	if(GetKeyboardTrigger(DIK_DOWN)==true)
	{
		//テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
								  "data/TEXTURE/sample001.png",
								  &g_pD3DTex);
		g_Player.nColor=YELLOW;
	}

	//青の車
	if(GetKeyboardTrigger(DIK_RIGHT)==true)
	{
		//テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
								  "data/TEXTURE/sample002.png",
								  &g_pD3DTex);
		g_Player.nColor=BLUE;
	}
	


	//////////////////////////
	//		壁制御			//
	/////////////////////////
	
	//右壁
	if(g_Player.fX > SCREEN_WIDTH-g_Player.fWidth)
	{
		g_Player.fX=SCREEN_WIDTH-g_Player.fWidth;
	}

	//左壁
	if(g_Player.fX < 0 &&
	   g_Player.nStatus==NORMAL)
	{
		g_Player.fX=0;
	}

	//上壁
	if(g_Player.fY < 0)
	{
		g_Player.fY=0;
	}

	//下壁
	if(g_Player.fY > SCREEN_HEIGHT-g_Player.fHeight)
	{
		g_Player.fY=SCREEN_HEIGHT-g_Player.fHeight;
	}

	//////////////////////////
	//	プレイヤー復帰処理	//
	/////////////////////////
	for(int i=0;i<3;i++)
	{
	//プレイヤー画面外（一定時間）
	if(g_Player.nCount<150 &&
	   g_Player.nStatus==MISS)
	{
		g_Player.fX=-100;
		g_Player.nCount++;
	}//一定時間を越した場合プレイヤーの状態をリトライへ
	else if(g_Player.nStatus==MISS)
	{
		g_Player.nStatus=RETRY;
		g_Player.fX=NEWPOS_X;
		//fBackUpX=g_Player.fX;
	}

	//プレイヤー点滅処理（一定時間）
	if(g_Player.nStatus==RETRY &&
	   g_Player.nCount<1466)
	{
		//g_Player.fX=fBackUpX;
		g_Player.nAlpha=0;
		g_Player.nCount++;
	}//一定時間を越した場合プレイヤーの状態をノーマルへ
	else if(g_Player.nStatus==RETRY)
	{
		//g_Player.fX=fBackUpX;
		g_Player.nAlpha=255;
		g_Player.nStatus=NORMAL;
	}

	//点滅をするために定期的に画面外へ移動
	if(g_Player.nStatus==RETRY &&
	   g_Player.nCount%2==0)
	{
		//fBackUpX=g_Player.fX;//復帰処理用バックアップ
		//g_Player.fX=-100;
		g_Player.nAlpha=255;
	}
	}

	////////////////////////////
	//プレイヤーポリゴンの更新//
	////////////////////////////
	//反射光
	g_aVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,g_Player.nAlpha);
	g_aVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,g_Player.nAlpha);
	g_aVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,g_Player.nAlpha);
	g_aVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,g_Player.nAlpha);

	//頂点座標の代入
	g_aVtx[0].vtx=D3DXVECTOR3(g_Player.fX,g_Player.fY+g_Player.fHeight,0.0f);
	g_aVtx[1].vtx=D3DXVECTOR3(g_Player.fX,g_Player.fY,0.0f);
	g_aVtx[2].vtx=D3DXVECTOR3(g_Player.fX+g_Player.fWidth,g_Player.fY+g_Player.fHeight,0.0f);
	g_aVtx[3].vtx=D3DXVECTOR3(g_Player.fX+g_Player.fWidth,g_Player.fY,0.0f);
}
//=============================================================================
// ポリゴン終了関数
//=============================================================================
void UninitPolygon (void)
{
	//テクスチャの開放
	if(g_pD3DTex!=NULL)
	{
		g_pD3DTex->Release();
		g_pD3DTex=NULL;
	}
}
//=============================================================================
// プレイヤー構造体ゲット
//=============================================================================
PLAYER GetPlayer (void)
{
	return g_Player;
}
//=============================================================================
// プレイヤー構造体セット
//=============================================================================
void SetPlayer (PLAYER data)
{
	g_Player=data;
}
//EOF