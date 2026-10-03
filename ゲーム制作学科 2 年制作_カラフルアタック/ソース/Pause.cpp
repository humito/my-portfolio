#include "Pause.h"

LPDIRECT3DTEXTURE9 g_pPD3DTex=NULL;	//テクスチャへのポインタ
VERTEX_2D g_pVtx[4];				//頂点情報格納ワーク
static int g_nPause=0;				//ポーズ情報
static int g_alpha=0;				//α値
HRESULT InitPPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pPDevice;

	//ゲッターで返す
	pPDevice=GetDevice();

	//α値最少
	g_alpha=0;

	//頂点座標の代入
	g_pVtx[0].vtx=D3DXVECTOR3(350,700,0.0f);
	g_pVtx[1].vtx=D3DXVECTOR3(350,300,0.0f);
	g_pVtx[2].vtx=D3DXVECTOR3(650,700,0.0f);
	g_pVtx[3].vtx=D3DXVECTOR3(650,300,0.0f);

		
	//中身
	g_pVtx[0].rhw=1.0f;
	g_pVtx[1].rhw=1.0f;
	g_pVtx[2].rhw=1.0f;
	g_pVtx[3].rhw=1.0f;
		
	//反射光
	g_pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);

	//テクスチャ
	g_pVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	g_pVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	g_pVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	g_pVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pPDevice,
								"data/TEXTURE/pause.png",
								&g_pPD3DTex);

	return S_OK;
}
void UpdatePPolygon (void)
{
	g_nPause=GetPause();

	if(g_nPause==0)
	{
		g_alpha=0;
	}
	else
	{
		g_alpha=255;
	}

	//反射光
	g_pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
}
void DrawPPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pPDevice;

	//ゲッターで返す
	pPDevice=GetDevice();

	//頂点フォーマットのセット
	pPDevice->SetFVF(FVF_VERTEX_2D);
	//テクスチャの設定
	pPDevice->SetTexture(0,g_pPD3DTex);

	//ポリゴンの描画
	pPDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
									2,//ポリゴンの数
									&g_pVtx[0],
									sizeof(VERTEX_2D));
}
void UninitPPolygon (void)
{
	//テクスチャの開放
	if(g_pPD3DTex!=NULL)
	{
		g_pPD3DTex->Release();
		g_pPD3DTex=NULL;
	}
}
