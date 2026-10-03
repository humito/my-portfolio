#include "result.h"

LPDIRECT3DTEXTURE9 g_pD3DrTex=NULL;			//テクスチャへのポインタ
VERTEX_2D g_rVtx[4];						//頂点情報格納ワーク

static int g_nColor[3];
static int g_nColorMove[3];
//初期化
HRESULT InitrPolygon (void)
{
	for(int i=0;i<3;i++)
	{
		g_nColor[i]=rand()%255;
		g_nColorMove[i]=2;
	}

	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1;

	//ゲッターで返す
	pDevice1=GetDevice();

	//頂点座標の代入
	g_rVtx[0].vtx=D3DXVECTOR3(260,200,0.0f);
	g_rVtx[1].vtx=D3DXVECTOR3(260,0,0.0f);
	g_rVtx[2].vtx=D3DXVECTOR3(660,200,0.0f);
	g_rVtx[3].vtx=D3DXVECTOR3(660,0,0.0f);

	//中身
	g_rVtx[0].rhw=1.0f;
	g_rVtx[1].rhw=1.0f;
	g_rVtx[2].rhw=1.0f;
	g_rVtx[3].rhw=1.0f;
		
	//反射光
	g_rVtx[0].diffuse=D3DCOLOR_RGBA(g_nColor[0],g_nColor[1],g_nColor[2],255);
	g_rVtx[1].diffuse=D3DCOLOR_RGBA(g_nColor[2],g_nColor[1],g_nColor[0],255);
	g_rVtx[2].diffuse=D3DCOLOR_RGBA(g_nColor[1],g_nColor[2],g_nColor[0],255);
	g_rVtx[3].diffuse=D3DCOLOR_RGBA(g_nColor[0],g_nColor[2],g_nColor[1],255);


	//テクスチャ
	g_rVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	g_rVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	g_rVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	g_rVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice1,
							  "data/TEXTURE/Result000.png",
							  &g_pD3DrTex);

	return S_OK;
}
//リザルトの更新
void UpdaterPolygon (void)
{
	//タイトルポリゴンの色を更新
	for(int i=0;i<3;i++)
	{
		//RGB各色を増やす
		g_nColor[i]+=g_nColorMove[i];

		//最大値か最小値にいったら移動量切替
		if(g_nColor[i]>255 ||
		   g_nColor[i]<1)
		{
			g_nColorMove[i]=-g_nColorMove[i];
		}
	}

	//反射光
	g_rVtx[0].diffuse=D3DCOLOR_RGBA(g_nColor[0],g_nColor[1],g_nColor[2],255);
	g_rVtx[1].diffuse=D3DCOLOR_RGBA(g_nColor[2],g_nColor[1],g_nColor[0],255);
	g_rVtx[2].diffuse=D3DCOLOR_RGBA(g_nColor[1],g_nColor[2],g_nColor[0],255);
	g_rVtx[3].diffuse=D3DCOLOR_RGBA(g_nColor[0],g_nColor[2],g_nColor[1],255);
}
//描画
void DrawrPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1;

	//ゲッターで返す
	pDevice1=GetDevice();

	//頂点フォーマットのセット
	pDevice1->SetFVF(FVF_VERTEX_2D);


	//テクスチャの設定
	pDevice1->SetTexture(0,g_pD3DrTex);

	//背景ポリゴンの描画
	pDevice1->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
									2,//ポリゴンの数
									&g_rVtx[0],
									sizeof(VERTEX_2D));

}
//終了
void UninitrPolygon (void)
{
	//テクスチャの開放
	if(g_pD3DrTex!=NULL)
	{
		g_pD3DrTex->Release();
		g_pD3DrTex=NULL;
	}
}
//EOF