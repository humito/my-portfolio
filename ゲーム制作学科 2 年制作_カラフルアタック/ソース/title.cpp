#include "title.h"


LPDIRECT3DTEXTURE9 g_pD3DtTex=NULL;	//テクスチャへのポインタ
VERTEX_2D g_tVtx[TITLE_STR_MAX][4];		//頂点情報格納ワーク
static float g_fStrMoveY[TITLE_STR_MAX];
static float g_fY[TITLE_STR_MAX];
//タイトル初期化
HRESULT InittPolygon (void)
{

	
	for(int i=0;i<TITLE_STR_MAX;i++)
	{
		//Ｙ座標初期化
		g_fY[i]=100.0f;

		//文字移動量初期化
		//配列（文字列）の奇数偶数によって移動量を変更する
		if(i%2==0)
		{
			g_fStrMoveY[i]=0.2f;
		}
		else
		{
			g_fStrMoveY[i]=-0.2f;
		}
	}
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1;

	//ゲッターで返す
	pDevice1=GetDevice();

	for(int i=0;i<TITLE_STR_MAX;i++)
	{
		//頂点座標の代入
		g_tVtx[i][0].vtx=D3DXVECTOR3(50.0f+(112.5f*i),g_fY[i]+92.5f,0.0f);
		g_tVtx[i][1].vtx=D3DXVECTOR3(50.0f+(112.5f*i),g_fY[i],0.0f);
		g_tVtx[i][2].vtx=D3DXVECTOR3(142.5f+(112.5f*i),g_fY[i]+92.5f,0.0f);
		g_tVtx[i][3].vtx=D3DXVECTOR3(142.5f+(112.5f*i),g_fY[i],0.0f);

		//中身
		g_tVtx[i][0].rhw=1.0f;
		g_tVtx[i][1].rhw=1.0f;
		g_tVtx[i][2].rhw=1.0f;
		g_tVtx[i][3].rhw=1.0f;
		
		//反射光
		g_tVtx[i][0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_tVtx[i][1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_tVtx[i][2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_tVtx[i][3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

		//テクスチャ
		g_tVtx[i][0].tex=D3DXVECTOR2(0.0f+(i*0.125f),1.0f);
		g_tVtx[i][1].tex=D3DXVECTOR2(0.0f+(i*0.125f),0.0f);
		g_tVtx[i][2].tex=D3DXVECTOR2(0.125f+(i*0.125f),1.0f);
		g_tVtx[i][3].tex=D3DXVECTOR2(0.125f+(i*0.125f),0.0f);
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice1,
							  "data/TEXTURE/title000.png",
							  &g_pD3DtTex);

	return S_OK;
}
//タイトル更新
void UpdatetPolygon (void)
{
	for(int i=0;i<TITLE_STR_MAX;i++)
	{
		//各文字Y座標を移動領分加算
		g_fY[i]+=g_fStrMoveY[i];

		//一定の位置までいったら
		if(g_fY[i]<0 ||
		   g_fY[i]>200)
		{
			//移動量を反転
			g_fStrMoveY[i]=-g_fStrMoveY[i];
		}

		//頂点座標の代入
		g_tVtx[i][0].vtx=D3DXVECTOR3(50.0f+(112.5f*i),g_fY[i]+92.5f,0.0f);
		g_tVtx[i][1].vtx=D3DXVECTOR3(50.0f+(112.5f*i),g_fY[i],0.0f);
		g_tVtx[i][2].vtx=D3DXVECTOR3(142.5f+(112.5f*i),g_fY[i]+92.5f,0.0f);
		g_tVtx[i][3].vtx=D3DXVECTOR3(142.5f+(112.5f*i),g_fY[i],0.0f);
	}
}
//タイトル描画
void DrawtPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1;

	//ゲッターで返す
	pDevice1=GetDevice();

	//頂点フォーマットのセット
	pDevice1->SetFVF(FVF_VERTEX_2D);


	//テクスチャの設定
	pDevice1->SetTexture(0,g_pD3DtTex);

	for(int i=0;i<TITLE_STR_MAX;i++)
	{
		//背景ポリゴンの描画
		pDevice1->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
										2,//ポリゴンの数
										&g_tVtx[i][0],
										sizeof(VERTEX_2D));
	}

}
//タイトル終了
void UninittPolygon (void)
{
	//テクスチャの開放
	if(g_pD3DtTex!=NULL)
	{
		g_pD3DtTex->Release();
		g_pD3DtTex=NULL;
	}
}
//EOF