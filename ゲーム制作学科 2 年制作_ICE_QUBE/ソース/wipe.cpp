//=============================================================================
//ワイプ処理[wipe.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "wipe.h"
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DWTex=NULL;		//テクスチャへのポインタ
VERTEX_2D g_BlockVtx[MAX_2DBLOCK][4];	//2Dポリゴン
BLOCK g_Block[MAX_2DBLOCK];				//ブロックの情報
static WIPE g_Wipe;						//ワイプの状態
static int g_WipeCnt;					//ワイプイン・アウト時のカウント
//=============================================================================
//ワイプ初期化
//=============================================================================
HRESULT InitWipe (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice=GetDevice();

	
	g_Wipe=WIPE_NONE;	//ワイプ初期化
	g_WipeCnt=0;		//ワイプカウント初期化

	//ブロックの個数分ループ
	for(int i=0;i<MAX_2DBLOCK;i++)
	{
		//各ブロックの座標設定
		g_Block[i].fX=0+(float)((i%5)*BLOCK_WIDTH);								//ブロックのX座標
		g_Block[i].fWidth=(float)(BLOCK_WIDTH+((i%5)*BLOCK_WIDTH));				//ブロックの幅
		g_Block[i].fY=-(float)BLOCK_HEIGHT;										//ブロックのY座標
		g_Block[i].fHeight=(float)BLOCK_HEIGHT;									//ブロックの高さ
		g_Block[i].fDestY=(float)((BLOCK_HEIGHT*4)-(((i/5)%5)*BLOCK_HEIGHT));	//ブロックの目的の位置
		g_Block[i].bSet=false;													//ブロックの目的位置達成フラグ
		
		//頂点座標の代入
		g_BlockVtx[i][0].vtx=D3DXVECTOR3(g_Block[i].fX,g_Block[i].fY+g_Block[i].fHeight,0.0f);
		g_BlockVtx[i][1].vtx=D3DXVECTOR3(g_Block[i].fX,g_Block[i].fY,0.0f);
		g_BlockVtx[i][2].vtx=D3DXVECTOR3(g_Block[i].fX+g_Block[i].fWidth,g_Block[i].fY+g_Block[i].fHeight,0.0f);
		g_BlockVtx[i][3].vtx=D3DXVECTOR3(g_Block[i].fX+g_Block[i].fWidth,g_Block[i].fY,0.0f);

		//中身
		g_BlockVtx[i][0].rhw=1.0f;
		g_BlockVtx[i][1].rhw=1.0f;
		g_BlockVtx[i][2].rhw=1.0f;
		g_BlockVtx[i][3].rhw=1.0f;
		
		//反射光
		g_BlockVtx[i][0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_BlockVtx[i][1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_BlockVtx[i][2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_BlockVtx[i][3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

		//テクスチャ
		g_BlockVtx[i][0].tex=D3DXVECTOR2(0.0f,1.0f);
		g_BlockVtx[i][1].tex=D3DXVECTOR2(0.0f,0.0f);
		g_BlockVtx[i][2].tex=D3DXVECTOR2(1.0f,1.0f);
		g_BlockVtx[i][3].tex=D3DXVECTOR2(1.0f,0.0f);
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/simbol.jpg",
							  &g_pD3DWTex);
							  

	return S_OK;

}
//=============================================================================
//ワイプ更新
//=============================================================================
void UpdateWipe (void)
{
	//ワイプの状態によって処理を分ける
	switch(g_Wipe)
	{
		//ワイプなし
		case WIPE_NONE:
			for(int i=0;i<MAX_2DBLOCK;i++)
			{
				g_Block[i].fX=0+(float)((i%5)*BLOCK_WIDTH);								//ブロックのX座標
				g_Block[i].fWidth=(float)(BLOCK_WIDTH+((i%5)*BLOCK_WIDTH));				//ブロックの幅
				g_Block[i].fY=-(float)BLOCK_HEIGHT;										//ブロックのY座標
				g_Block[i].fHeight=(float)BLOCK_HEIGHT;									//ブロックの高さ
				g_Block[i].fDestY=(float)((BLOCK_HEIGHT*4)-(((i/5)%5)*BLOCK_HEIGHT));	//ブロックの目的の位置
			}
		break;

		//ワイプイン
		case WIPE_IN:

			for(int i=0;i<MAX_2DBLOCK;i++)
			{
				//目的の位置にブロックが移動する
				g_Block[i].fY+=(g_Block[i].fDestY-g_Block[i].fY)*0.5f;

				//ブロックの位置が目的に達したらフラグTRUE
				if(g_Block[i].fY>=g_Block[i].fDestY)
				{
					g_Block[i].bSet=true;
				}
			}

			//最後のブロックまで達したらWIPE_OUTの状態に変更
			if(g_Block[24].bSet==true)
			{
				//ワイプの状態をOUTにする
				g_Wipe=WIPE_OUT;

				//ブロックの個数分ループ
				for(int i=0;i<MAX_2DBLOCK;i++)
				{
					//ブロックの達するフラグをFALSEにする
					g_Block[i].bSet=false;
					//ブロックの目的地を画面外下へ
					g_Block[i].fDestY=SCREEN_HEIGHT;
				}
			}


		break;

		//ワイプアウト
		case WIPE_OUT:

			//ブロックの個数分ループ
			for(int i=0;i<MAX_2DBLOCK;i++)
			{
				//目的の位置にブロックが移動する
				g_Block[i].fY+=(g_Block[i].fDestY-g_Block[i].fY)*0.5f;

				//ブロックの位置が目的に達したらフラグTRUE
				if(g_Block[i].fY>=g_Block[i].fDestY)
				{
					g_Block[i].bSet=true;
				}
			}

			//最後のブロックまで達したらWIPE_NONEの状態に変更
			if(g_Block[24].bSet==true)
			{
				//ワイプの状態をOUTにする
				g_Wipe=WIPE_NONE;

				//ブロックの個数分ループ
				for(int i=0;i<MAX_2DBLOCK;i++)
				{
					//ブロックの達するフラグをFALSEにする
					g_Block[i].bSet=false;
					//ブロックの目的地を画面外下へ
					g_Block[i].fDestY=(float)((BLOCK_HEIGHT*4)-(((i/5)%5)*BLOCK_HEIGHT));
				}
			}

		break;

		//それ以外
		default:
		break;
	}
	//ブロックの個数分ループ
	for(int i=0;i<MAX_2DBLOCK;i++)
	{
		//頂点座標の代入
		g_BlockVtx[i][0].vtx=D3DXVECTOR3(g_Block[i].fX,g_Block[i].fY+g_Block[i].fHeight,0.0f);
		g_BlockVtx[i][1].vtx=D3DXVECTOR3(g_Block[i].fX,g_Block[i].fY,0.0f);
		g_BlockVtx[i][2].vtx=D3DXVECTOR3(g_Block[i].fWidth,g_Block[i].fY+g_Block[i].fHeight,0.0f);
		g_BlockVtx[i][3].vtx=D3DXVECTOR3(g_Block[i].fWidth,g_Block[i].fY,0.0f);
	}
}
//=============================================================================
//ワイプ描画
//=============================================================================
void DrawWipe (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1=GetDevice();

	//頂点フォーマットのセット
	pDevice1->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice1->SetTexture(0,g_pD3DWTex);

	//ブロックの個数分ループ
	for(int i=0;i<MAX_2DBLOCK;i++)
	{
		//背景ポリゴンの描画
		pDevice1->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
										2,//ポリゴンの数
										&g_BlockVtx[i][0],
										sizeof(VERTEX_2D));
	}
}
//=============================================================================
//ワイプ終了
//=============================================================================
void UninitWipe (void)
{
	//テクスチャの開放
	if(g_pD3DWTex!=NULL)
	{
		g_pD3DWTex->Release();
		g_pD3DWTex=NULL;
	}

}
//=============================================================================
//ワイプ状態のゲット
//=============================================================================
WIPE GetWipe(void)
{
	return g_Wipe;
}
//=============================================================================
//ワイプ状態のセット
//=============================================================================
void SetWipe (WIPE type)
{
	g_Wipe=type;
}
//EOF