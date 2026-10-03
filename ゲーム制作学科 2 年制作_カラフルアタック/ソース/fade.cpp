#include "fade.h"
static FADE g_fade;
static int g_nFade;
LPDIRECT3DTEXTURE9 g_pD3DFTex=NULL;				//テクスチャへのポインタ
VERTEX_2D g_fVtx[4];							//頂点情報格納ワーク
//======================================================================
//フェード初期化
//======================================================================
HRESULT InitFade (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice2;

	//ゲッターで返す
	pDevice2=GetDevice();

	//頂点座標の代入
	g_fVtx[0].vtx=D3DXVECTOR3(0,1000,0.0f);
	g_fVtx[1].vtx=D3DXVECTOR3(0,0,0.0f);
	g_fVtx[2].vtx=D3DXVECTOR3(1000,1000,0.0f);
	g_fVtx[3].vtx=D3DXVECTOR3(1000,0,0.0f);

	//中身
	g_fVtx[0].rhw=1.0f;
	g_fVtx[1].rhw=1.0f;
	g_fVtx[2].rhw=1.0f;
	g_fVtx[3].rhw=1.0f;

	//テクスチャ
	g_fVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	g_fVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	g_fVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	g_fVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

		
	//反射光
	g_fVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,0);
	g_fVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,0);
	g_fVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,0);
	g_fVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,0);

	g_nFade=0;

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice2,
							  "data/TEXTURE/fade000.png",
							  &g_pD3DFTex);

	return S_OK;
}
//======================================================================
//フェード更新
//======================================================================
void UpdateFade (void)
{
	if(g_fade==FADE_IN)
	{
		g_nFade--;
	}

	if(g_fade==FADE_OUT)
	{
		g_nFade++;
	}

	if(g_nFade<0)
	{
		g_nFade=0;
		g_fade=FADE_NONE;
	}

	if(g_nFade>255)
	{
		g_nFade=255;
		g_fade=FADE_IN;
	}

	//反射光
	g_fVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,g_nFade);
	g_fVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,g_nFade);
	g_fVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,g_nFade);
	g_fVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,g_nFade);

}
//======================================================================
//フェード描画
//======================================================================
void DrawFade (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice2;

	//ゲッターで返す
	pDevice2=GetDevice();

	//頂点フォーマットのセット
	pDevice2->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice2->SetTexture(0,g_pD3DFTex);

	//背景ポリゴンの描画
	pDevice2->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
									2,//ポリゴンの数
									&g_fVtx[0],
									sizeof(VERTEX_2D));
}
//======================================================================
//フェード終了
//======================================================================
void UninitFade (void)
{
	//テクスチャの開放
	if(g_pD3DFTex!=NULL)
	{
		g_pD3DFTex->Release();
		g_pD3DFTex=NULL;
	}
}
//======================================================================
//フェードセット
//======================================================================
void SetFade (FADE fade)
{
	g_fade=fade;
}
//======================================================================
//フェードゲット
//======================================================================
FADE GetFade (void)
{
	return g_fade;
}
//EOF
