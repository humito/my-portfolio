//=============================================================================
//残り時間処理[time.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include"time.h"
//*****************************************************************************
//定数定義
//*****************************************************************************
#define DIGIT_MAX (2)	//最大桁数
#define COUNT (60)		//一定のカウント
//*****************************************************************************
//プロトタイプ宣言(内部)
//*****************************************************************************
void ChangeTimeBuff (int num);
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DTTex=NULL;									//テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffTime=NULL;						//頂点バッファへのポインタ
static TIME *g_pTime=NULL;											//時間構造体
static int g_nCnt=0;												//カウント
static int g_nTime=0;												//残りタイム
//=============================================================================
//時間初期化
//=============================================================================
HRESULT InitTime (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice=GetDevice();

	//カウントの初期化
	g_nCnt=0;

	//タイムの初期化
	g_nTime=50;

	///////////////////////////////
	//	オブジェクトの初期化	//
	/////////////////////////////

	//NULLチェック
	if(g_pTime==NULL)
	{
		//動的確保
		g_pTime=new TIME[DIGIT_MAX];
	}

	//桁数分ループ
	for(int i=0;i<DIGIT_MAX;i++)
	{
		g_pTime[i].fX=350.0f+(50.0f*i);
		g_pTime[i].fWidth=50.0f;
		g_pTime[i].fY=0.0f;
		g_pTime[i].fHeight=80.0f;
		g_pTime[i].nNum=0;
		g_pTime[i].nColorR=0;
	}

	///////////////////////////
	//	頂点バッファの設定	//
	/////////////////////////

	//頂点バッファの生成
	if(FAILED(pDevice->CreateVertexBuffer
				(sizeof(VERTEX_2D)*4,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_2D,
				D3DPOOL_MANAGED,
				&g_pD3DVtxBuffTime,
				NULL)))
	{
		return E_FAIL;
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/number001.png",
							  &g_pD3DTTex);

	return S_OK;
}
//=============================================================================
//時間更新
//=============================================================================
void UpdateTime (void)
{
	//カウントアップ
	g_nCnt++;

	//カウント一定ごとにタイムを1つ減らす
	if(g_nCnt%COUNT==0)
	{
		g_nTime--;
	}

	//タイムを各ケタに代入
	g_pTime[1].nNum=g_nTime%10;
	g_pTime[0].nNum=g_nTime/10;

	//タイムが０より下ならワイプ
	if(g_nTime<1)
	{
		g_nTime=99;
		SetWipe(WIPE_IN);
	}

	//桁数分ループ
	for(int i=0;i<DIGIT_MAX;i++)
	{

		//カウント10以下なら文字色を赤にする
		if(g_nTime<11)
		{
			//ポリゴンのGとBの値を0にする
			g_pTime[i].nColorR=255;
		}
	}
}
//=============================================================================
//時間描画
//=============================================================================
void DrawTime (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1=GetDevice();

	//ケタの数分ループ
	for(int i=0;i<DIGIT_MAX;i++)
	{
		//頂点バッファの変更
		ChangeTimeBuff(i);

		//頂点バッファのバインド
		pDevice1->SetStreamSource(0,g_pD3DVtxBuffTime,0,sizeof(VERTEX_2D));

		//頂点フォーマットのセット
		pDevice1->SetFVF(FVF_VERTEX_2D);

		//テクスチャの設定
		pDevice1->SetTexture(0,g_pD3DTTex);

		//ポリゴンの描画
		pDevice1->DrawPrimitive(D3DPT_TRIANGLESTRIP,
								0,//ポリゴンの数
								2);
	}

}
//=============================================================================
//時間頂点バッファ変更
//=============================================================================
void ChangeTimeBuff (int num)
{
	VERTEX_2D *pVtx;

	g_pD3DVtxBuffTime->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の代入
	pVtx[0].vtx=D3DXVECTOR3(g_pTime[num].fX,g_pTime[num].fY+g_pTime[num].fHeight,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(g_pTime[num].fX,g_pTime[num].fY,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(g_pTime[num].fX+g_pTime[num].fWidth,g_pTime[num].fY+g_pTime[num].fHeight,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(g_pTime[num].fX+g_pTime[num].fWidth,g_pTime[num].fY,0.0f);

	//中身
	pVtx[0].rhw=1.0f;
	pVtx[1].rhw=1.0f;
	pVtx[2].rhw=1.0f;
	pVtx[3].rhw=1.0f;
		
	//反射光
	pVtx[0].diffuse=D3DCOLOR_RGBA(g_pTime[num].nColorR,0,0,255);
	pVtx[1].diffuse=D3DCOLOR_RGBA(g_pTime[num].nColorR,0,0,255);
	pVtx[2].diffuse=D3DCOLOR_RGBA(g_pTime[num].nColorR,0,0,255);
	pVtx[3].diffuse=D3DCOLOR_RGBA(g_pTime[num].nColorR,0,0,255);

	//テクスチャ
	pVtx[0].tex=D3DXVECTOR2(0.0f+(0.1f*g_pTime[num].nNum),1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f+(0.1f*g_pTime[num].nNum),0.0f);
	pVtx[2].tex=D3DXVECTOR2(0.1f+(0.1f*g_pTime[num].nNum),1.0f);
	pVtx[3].tex=D3DXVECTOR2(0.1f+(0.1f*g_pTime[num].nNum),0.0f);

	g_pD3DVtxBuffTime->Unlock();
}
//=============================================================================
//時間終了
//=============================================================================
void UninitTime (void)
{
	if(g_pTime!=NULL)
	{
		delete []g_pTime;
		g_pTime=NULL;
	}

	//頂点バッファの解放
	if(g_pD3DVtxBuffTime!=NULL)
	{
		g_pD3DVtxBuffTime->Release();
		g_pD3DVtxBuffTime=NULL;
	}

	//テクスチャの開放
	if(g_pD3DTTex!=NULL)
	{
		g_pD3DTTex->Release();
		g_pD3DTTex=NULL;
	}

}
//EOF