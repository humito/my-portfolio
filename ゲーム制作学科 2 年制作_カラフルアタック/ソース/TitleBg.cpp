#include "TitleBg.h"

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DBg2Tex=NULL;		//テクスチャへのポインタ
VERTEX_2D g_btVtx[4];						//頂点情報格納ワーク
static float g_fU=0.0f,g_fV=0.0f;			//テクスチャのＵＶ座標
static float g_fMoveU=0.0f,g_fMoveV=0.0f;	//UV座標の移動量
//=============================================================================
// ポリゴン背景初期化関数
//=============================================================================
HRESULT InittBgPolygon (void)
{
	//テクスチャ座標初期化
	g_fU=0.0f;
	g_fV=0.0f;
	g_fMoveU=0.01f;
	g_fMoveV=0.003f;

	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1;

	//ゲッターで返す
	pDevice1=GetDevice();

	//頂点座標の代入
	g_btVtx[0].vtx=D3DXVECTOR3(0,1000,0.0f);
	g_btVtx[1].vtx=D3DXVECTOR3(0,0,0.0f);
	g_btVtx[2].vtx=D3DXVECTOR3(1000,1000,0.0f);
	g_btVtx[3].vtx=D3DXVECTOR3(1000,0,0.0f);

	//中身
	g_btVtx[0].rhw=1.0f;
	g_btVtx[1].rhw=1.0f;
	g_btVtx[2].rhw=1.0f;
	g_btVtx[3].rhw=1.0f;
		
	//反射光
	g_btVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	g_btVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	g_btVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	g_btVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,255);


	//テクスチャ
	g_btVtx[0].tex=D3DXVECTOR2(g_fU,g_fV+0.2f);
	g_btVtx[1].tex=D3DXVECTOR2(g_fU,g_fV);
	g_btVtx[2].tex=D3DXVECTOR2(g_fU+0.2f,g_fV+0.2f);
	g_btVtx[3].tex=D3DXVECTOR2(g_fU+0.2f,g_fV);

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice1,
							  "data/TEXTURE/TitleBg.png",
							  &g_pD3DBg2Tex);

	return S_OK;
}
void UpdatetBgPolygon (void)
{
	g_fU+=g_fMoveU;
	g_fV+=g_fMoveV;

	if(g_fU+0.2f>1.0f ||
	   g_fU<0)
	{
		g_fMoveU=-g_fMoveU;
	}

	if(g_fV+0.2f>1.0f ||
	   g_fV<0)
	{
		g_fMoveV=-g_fMoveV;
	}

	//テクスチャ
	g_btVtx[0].tex=D3DXVECTOR2(g_fU,g_fV+0.2f);
	g_btVtx[1].tex=D3DXVECTOR2(g_fU,g_fV);
	g_btVtx[2].tex=D3DXVECTOR2(g_fU+0.2f,g_fV+0.2f);
	g_btVtx[3].tex=D3DXVECTOR2(g_fU+0.2f,g_fV);
}
//=============================================================================
// ポリゴン背景描画関数
//=============================================================================
void DrawtBgPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1;

	//ゲッターで返す
	pDevice1=GetDevice();

	//頂点フォーマットのセット
	pDevice1->SetFVF(FVF_VERTEX_2D);


	//テクスチャの設定
	pDevice1->SetTexture(0,g_pD3DBg2Tex);

	//背景ポリゴンの描画
	pDevice1->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
									2,//ポリゴンの数
									&g_btVtx[0],
									sizeof(VERTEX_2D));
}
//=============================================================================
// ポリゴン背景終了関数
//=============================================================================
void UninittBgPolygon (void)
{
	//テクスチャの開放
	if(g_pD3DBg2Tex!=NULL)
	{
		g_pD3DBg2Tex->Release();
		g_pD3DBg2Tex=NULL;
	}
}
//EOF