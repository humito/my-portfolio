#include "Fuel.h"

LPDIRECT3DTEXTURE9 g_pFuD3DTex=NULL;	//テクスチャへのポインタ
VERTEX_2D g_fuVtx[FUEL_MAX][4];			//頂点情報格納ワーク
static int g_nFuels=0;//燃料メーター
static int g_nFuelFlag[FUEL_MAX];//燃料フラグ
//燃料メーターの初期化
HRESULT InitFuPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pPDevice;

	//ゲッターで返す
	pPDevice=GetDevice();

	//燃料の最大値を代入
	g_nFuels=FUEL_MAX*100;

	//燃料フラグを初期化
	for(int i=0;i<FUEL_MAX;i++)
	{
		g_nFuelFlag[i]=0;
	}

	for(int i=0;i<FUEL_MAX;i++)
	{
		//頂点座標の代入
		g_fuVtx[i][0].vtx=D3DXVECTOR3(345.0f+22.0f*i,50.0f,0.0f);
		g_fuVtx[i][1].vtx=D3DXVECTOR3(345.0f+22.0f*i,0.0f,0.0f);
		g_fuVtx[i][2].vtx=D3DXVECTOR3(367.0f+22.0f*i,50.0f,0.0f);
		g_fuVtx[i][3].vtx=D3DXVECTOR3(367.0f+22.0f*i,0.0f,0.0f);

		
		//中身
		g_fuVtx[i][0].rhw=1.0f;
		g_fuVtx[i][1].rhw=1.0f;
		g_fuVtx[i][2].rhw=1.0f;
		g_fuVtx[i][3].rhw=1.0f;
		
		//反射光
		g_fuVtx[i][0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_fuVtx[i][1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_fuVtx[i][2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_fuVtx[i][3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

		//テクスチャ
		g_fuVtx[i][0].tex=D3DXVECTOR2(0.0f,1.0f);
		g_fuVtx[i][1].tex=D3DXVECTOR2(0.0f,0.0f);
		g_fuVtx[i][2].tex=D3DXVECTOR2(0.5f,1.0f);
		g_fuVtx[i][3].tex=D3DXVECTOR2(0.5f,0.0f);
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pPDevice,
								"data/TEXTURE/fuel.png",
								&g_pFuD3DTex);

	return S_OK;

}
//燃料メーターの更新
void UpdateFuPolygon (void)
{
	//燃料の量をメーターに振り分け
	for(int i=0;i<FUEL_MAX;i++)
	{
		if(g_nFuels>100*i)
		{
			g_nFuelFlag[i]=1;
		}
		else
		{
			g_nFuelFlag[i]=0;
		}
	}

	//テクスチャの更新
	for(int i=0;i<FUEL_MAX;i++)
	{
		//テクスチャ
		g_fuVtx[i][0].tex=D3DXVECTOR2(0.0f+(0.5f*g_nFuelFlag[i]),1.0f);
		g_fuVtx[i][1].tex=D3DXVECTOR2(0.0f+(0.5f*g_nFuelFlag[i]),0.0f);
		g_fuVtx[i][2].tex=D3DXVECTOR2(0.5f+(0.5f*g_nFuelFlag[i]),1.0f);
		g_fuVtx[i][3].tex=D3DXVECTOR2(0.5f+(0.5f*g_nFuelFlag[i]),0.0f);
	}

}
void AddSubFuel (int num)
{
	//燃料の値を加算
	g_nFuels+=num;

	//燃料の値が最大値を超えたら最大値までにする
	if(g_nFuels>FUEL_MAX*100)
	{
		g_nFuels=FUEL_MAX*100;
	}

	//燃料の値が０以下ならフェードアウト
	if(g_nFuels<=0)
	{
		SetFade(FADE_OUT);
	}
}
//燃料メーターの描画
void DrawFuPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pPDevice;

	//ゲッターで返す
	pPDevice=GetDevice();

	//頂点フォーマットのセット
	pPDevice->SetFVF(FVF_VERTEX_2D);
	//テクスチャの設定
	pPDevice->SetTexture(0,g_pFuD3DTex);

	for(int i=0;i<FUEL_MAX;i++)
	{
		//ポリゴンの描画
		pPDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
										2,//ポリゴンの数
										&g_fuVtx[i][0],
										sizeof(VERTEX_2D));
	}
}
//燃料メーターの終了
void UninitFuPolygon (void)
{
	//テクスチャの開放
	if(g_pFuD3DTex!=NULL)
	{
		g_pFuD3DTex->Release();
		g_pFuD3DTex=NULL;
	}
}
