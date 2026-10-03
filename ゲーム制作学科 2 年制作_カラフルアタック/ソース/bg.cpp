#include "bg.h"
//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DBgTex=NULL;			//テクスチャへのポインタ
VERTEX_2D g_bVtx[4];						//頂点情報格納ワーク
//=============================================================================
// ポリゴン背景初期化関数
//=============================================================================
HRESULT InitBgPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1;

	//ゲッターで返す
	pDevice1=GetDevice();

	//頂点座標の代入
	g_bVtx[0].vtx=D3DXVECTOR3(0,1000,0.0f);
	g_bVtx[1].vtx=D3DXVECTOR3(0,0,0.0f);
	g_bVtx[2].vtx=D3DXVECTOR3(1000,1000,0.0f);
	g_bVtx[3].vtx=D3DXVECTOR3(1000,0,0.0f);

	//中身
	g_bVtx[0].rhw=0.2f;
	g_bVtx[1].rhw=0.2f;
	g_bVtx[2].rhw=0.2f;
	g_bVtx[3].rhw=0.2f;
		
	//反射光
	g_bVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,0);
	g_bVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,0);
	g_bVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,0);
	g_bVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,0);


	//テクスチャ
	g_bVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	g_bVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	g_bVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	g_bVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice1,
							  "data/TEXTURE/Bg.png",
							  &g_pD3DBgTex);

	return S_OK;
}
//=============================================================================
// ポリゴン背景描画関数
//=============================================================================
void DrawBgPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1;

	//ゲッターで返す
	pDevice1=GetDevice();

	//頂点フォーマットのセット
	pDevice1->SetFVF(FVF_VERTEX_2D);


	//テクスチャの設定
	pDevice1->SetTexture(0,g_pD3DBgTex);

	//背景ポリゴンの描画
	pDevice1->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
									2,//ポリゴンの数
									&g_bVtx[0],
									sizeof(VERTEX_2D));

}
//=============================================================================
// ポリゴン背景終了関数
//=============================================================================
void UninitBgPolygon (void)
{
	//テクスチャの開放
	if(g_pD3DBgTex!=NULL)
	{
		g_pD3DBgTex->Release();
		g_pD3DBgTex=NULL;
	}
}
//EOF