//=============================================================================
//タイトル内の処理[title.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "title.h"
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DtTex[2]={NULL,NULL};	//テクスチャへのポインタ
VERTEX_2D g_TitleVtx[2][4];						//2Dポリゴン
static int g_nAlpha;							//ポリゴンのα値
static int g_nCnt;								//経過カウント
//=============================================================================
//タイトルの初期化
//=============================================================================
HRESULT InitTitle (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice=GetDevice();

	g_nAlpha=255;	//α値初期化
	g_nCnt=0;		//経過カウント初期化

	//頂点座標の代入
	g_TitleVtx[0][0].vtx=D3DXVECTOR3(150.0f,550.0f,0.0f);
	g_TitleVtx[0][1].vtx=D3DXVECTOR3(150.0f,450.0f,0.0f);
	g_TitleVtx[0][2].vtx=D3DXVECTOR3(650.0f,550.0f,0.0f);
	g_TitleVtx[0][3].vtx=D3DXVECTOR3(650.0f,450.0f,0.0f);

	g_TitleVtx[1][0].vtx=D3DXVECTOR3(50.0f,250.0f,0.0f);
	g_TitleVtx[1][1].vtx=D3DXVECTOR3(50.0f,50.0f,0.0f);
	g_TitleVtx[1][2].vtx=D3DXVECTOR3(750.0f,250.0f,0.0f);
	g_TitleVtx[1][3].vtx=D3DXVECTOR3(750.0f,50.0f,0.0f);

	for(int i=0;i<2;i++)
	{
		//中身
		g_TitleVtx[i][0].rhw=1.0f;
		g_TitleVtx[i][1].rhw=1.0f;
		g_TitleVtx[i][2].rhw=1.0f;
		g_TitleVtx[i][3].rhw=1.0f;
		
		//反射光
		g_TitleVtx[i][0].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
		g_TitleVtx[i][1].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
		g_TitleVtx[i][2].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
		g_TitleVtx[i][3].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);

		//テクスチャ
		g_TitleVtx[i][0].tex=D3DXVECTOR2(0.0f,1.0f);
		g_TitleVtx[i][1].tex=D3DXVECTOR2(0.0f,0.0f);
		g_TitleVtx[i][2].tex=D3DXVECTOR2(1.0f,1.0f);
		g_TitleVtx[i][3].tex=D3DXVECTOR2(1.0f,0.0f);
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/start.png",
							  &g_pD3DtTex[0]);

	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/title_logo.png",
							  &g_pD3DtTex[1]);

	return S_OK;
}
//=============================================================================
//タイトルの更新
//=============================================================================
void UpdateTitle (void)
{
	//経過カウントアップ
	g_nCnt++;

	//一定カウントごとにα値を変更する
	if((g_nCnt%150)>=0 &&
		(g_nCnt%150)<=75)
	{
		g_nAlpha=0;
	}
	else
	{
		g_nAlpha=255;
	}

	//反射光更新
	g_TitleVtx[0][0].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
	g_TitleVtx[0][1].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
	g_TitleVtx[0][2].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
	g_TitleVtx[0][3].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);

	//エンターキーでワイプセット
	if(GetKeyboardTrigger(DIK_RETURN))
	{
		SetWipe(WIPE_IN);
	}
}
//=============================================================================
//タイトルの描画
//=============================================================================
void DrawTitle (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1=GetDevice();

	//頂点フォーマットのセット
	pDevice1->SetFVF(FVF_VERTEX_2D);

	for(int i=0;i<2;i++)
	{

		//テクスチャの設定
		pDevice1->SetTexture(0,g_pD3DtTex[i]);

		//背景ポリゴンの描画
		pDevice1->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
										2,//ポリゴンの数
										&g_TitleVtx[i][0],
										sizeof(VERTEX_2D));

	}
}
//=============================================================================
//タイトルの終了
//=============================================================================
void UninitTitle (void)
{
	for(int i=0;i<2;i++)
	{
		//テクスチャの開放
		if(g_pD3DtTex[i]!=NULL)
		{
			g_pD3DtTex[i]->Release();
			g_pD3DtTex[i]=NULL;
		}
	}
}
//EOF