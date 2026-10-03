#include "CoinPolygon.h"


LPDIRECT3DTEXTURE9 g_pCD3DTex=NULL;			//テクスチャへのポインタ
VERTEX_2D g_cVtx[COIN_MAX][4];				//頂点情報格納ワーク
static COIN g_Coin[COIN_MAX];				//コイン構造体
static int i=0;
//=============================================================================
// コインポリゴン初期化
//=============================================================================
HRESULT InitCPolygon (void)
{
	//構造体初期化
	for(i=0;i<COIN_MAX;i++)
	{
		g_Coin[i].fX=0*i;
		g_Coin[i].fY=-100;
		g_Coin[i].fWidth=50;
		g_Coin[i].fHeight=50;
		g_Coin[i].bUse=false;
	}


	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pCDevice;

	//ゲッターで返す
	pCDevice=GetDevice();

	for(i=0;i<COIN_MAX;i++)
	{
		//頂点座標の代入
		g_cVtx[i][0].vtx=D3DXVECTOR3(g_Coin[i].fX,g_Coin[i].fY+g_Coin[i].fHeight,0.0f);
		g_cVtx[i][1].vtx=D3DXVECTOR3(g_Coin[i].fX,g_Coin[i].fY,0.0f);
		g_cVtx[i][2].vtx=D3DXVECTOR3(g_Coin[i].fX+g_Coin[i].fWidth,g_Coin[i].fY+g_Coin[i].fHeight,0.0f);
		g_cVtx[i][3].vtx=D3DXVECTOR3(g_Coin[i].fX+g_Coin[i].fWidth,g_Coin[i].fY,0.0f);

		
		//中身
		g_cVtx[i][0].rhw=1.0f;
		g_cVtx[i][1].rhw=1.0f;
		g_cVtx[i][2].rhw=1.0f;
		g_cVtx[i][3].rhw=1.0f;
		
		//反射光
		g_cVtx[i][0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_cVtx[i][1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_cVtx[i][2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_cVtx[i][3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

		//テクスチャ
		g_cVtx[i][0].tex=D3DXVECTOR2(1.0f,1.0f);
		g_cVtx[i][1].tex=D3DXVECTOR2(0.0f,1.0f);
		g_cVtx[i][2].tex=D3DXVECTOR2(1.0f,0.0f);
		g_cVtx[i][3].tex=D3DXVECTOR2(0.0f,0.0f);
	}


	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pCDevice,
								"data/TEXTURE/coin.png",
								&g_pCD3DTex);

	return S_OK;
}
//=============================================================================
//コインポリゴン更新 
//=============================================================================
void UpdateCPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pCDevice;

	//ゲッターで返す
	pCDevice=GetDevice();




	for(i=0;i<COIN_MAX;i++)
	{
		
		//反射光
		g_cVtx[i][0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_cVtx[i][1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_cVtx[i][2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_cVtx[i][3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

		//頂点座標の代入
		g_cVtx[i][0].vtx=D3DXVECTOR3(g_Coin[i].fX,g_Coin[i].fY+g_Coin[i].fHeight,0.0f);
		g_cVtx[i][1].vtx=D3DXVECTOR3(g_Coin[i].fX,g_Coin[i].fY,0.0f);
		g_cVtx[i][2].vtx=D3DXVECTOR3(g_Coin[i].fX+g_Coin[i].fWidth,g_Coin[i].fY+g_Coin[i].fHeight,0.0f);
		g_cVtx[i][3].vtx=D3DXVECTOR3(g_Coin[i].fX+g_Coin[i].fWidth,g_Coin[i].fY,0.0f);
	}



}
//=============================================================================
// コインポリゴン描画
//=============================================================================
void DrawCPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pCDevice;

	//ゲッターで返す
	pCDevice=GetDevice();

	//頂点フォーマットのセット
	pCDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pCDevice->SetTexture(0,g_pCD3DTex);



	//敵ポリゴン（複数）描画
	//ポリゴンの描画
	pCDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
									2,//ポリゴンの数
									&g_cVtx[0][0],
									sizeof(VERTEX_2D));

}
//=============================================================================
// コインポリゴン終了
//=============================================================================
void UninitCPolygon (void)
{
	//テクスチャの開放
	if(g_pCD3DTex!=NULL)
	{
		g_pCD3DTex->Release();
		g_pCD3DTex=NULL;
	}
}
