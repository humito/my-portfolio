//=============================================================================
//背景処理
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルード
//*****************************************************************************
#include "bg.h"
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DBgTex=NULL;	//テクスチャへのポインタ
VERTEX_2D g_BackVtx[4];//2Dポリゴン
TYPEBG g_BgType;//背景の種類
TYPEBG g_BgPreType;//前の背景の種類
static int g_nAlpha;//ポリゴンのα値
//=============================================================================
//背景初期化
//=============================================================================
HRESULT InitBg (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice=GetDevice();

	g_BgType=TITLE_BG;
	g_BgPreType=TITLE_BG;

	g_nAlpha=255;

	//頂点座標の代入
	g_BackVtx[0].vtx=D3DXVECTOR3(0.0f,600.0f,0.0f);
	g_BackVtx[1].vtx=D3DXVECTOR3(0.0f,0.0f,0.0f);
	g_BackVtx[2].vtx=D3DXVECTOR3(800.0f,600.0f,0.0f);
	g_BackVtx[3].vtx=D3DXVECTOR3(800.0f,0.0f,0.0f);

	//中身
	g_BackVtx[0].rhw=1.0f;
	g_BackVtx[1].rhw=1.0f;
	g_BackVtx[2].rhw=1.0f;
	g_BackVtx[3].rhw=1.0f;
		
	//反射光
	g_BackVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
	g_BackVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
	g_BackVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
	g_BackVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);

	//テクスチャ
	g_BackVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	g_BackVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	g_BackVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	g_BackVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);
	

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/title.jpg",
							  &g_pD3DBgTex);
							  

	return S_OK;

}
//=============================================================================
//背景更新
//=============================================================================
void UpdateBg (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice=GetDevice();

	if(g_BgType!=g_BgPreType)
	{
		switch(g_BgType)
		{
			case TITLE_BG:
				g_nAlpha=255;

				//テクスチャの読み込み
				D3DXCreateTextureFromFile(pDevice,
										  "data/TEXTURE/title.jpg",
										  &g_pD3DBgTex);
			break;

			case TUTOREAL_BG:
				//テクスチャの読み込み
				D3DXCreateTextureFromFile(pDevice,
										  "data/TEXTURE/tutoreal000.jpg",
										  &g_pD3DBgTex);

			break;

			case GAME_BG:
				g_nAlpha=0;
			break;

		}

		g_BgPreType=g_BgType;
	}

	//頂点座標の代入
	g_BackVtx[0].vtx=D3DXVECTOR3(0.0f,600.0f,0.0f);
	g_BackVtx[1].vtx=D3DXVECTOR3(0.0f,0.0f,0.0f);
	g_BackVtx[2].vtx=D3DXVECTOR3(800.0f,600.0f,0.0f);
	g_BackVtx[3].vtx=D3DXVECTOR3(800.0f,0.0f,0.0f);

	//中身
	g_BackVtx[0].rhw=1.0f;
	g_BackVtx[1].rhw=1.0f;
	g_BackVtx[2].rhw=1.0f;
	g_BackVtx[3].rhw=1.0f;
		
	//反射光
	g_BackVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
	g_BackVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
	g_BackVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);
	g_BackVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,g_nAlpha);

	//テクスチャ
	g_BackVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	g_BackVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	g_BackVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	g_BackVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

}
//=============================================================================
//背景描画
//=============================================================================
void DrawBg (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice=GetDevice();

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0,g_pD3DBgTex);

	//背景ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
									2,//ポリゴンの数
									&g_BackVtx[0],
									sizeof(VERTEX_2D));

}
//=============================================================================
//背景終了
//=============================================================================
void UninitBg (void)
{
	//テクスチャの開放
	if(g_pD3DBgTex!=NULL)
	{
		g_pD3DBgTex->Release();
		g_pD3DBgTex=NULL;
	}
}
//=============================================================================
//背景のセット
//=============================================================================
void SetBg (TYPEBG type)
{
	//前の背景の種類を代入
	g_BgPreType=g_BgType;

	//背景の種類をセット
	g_BgType=type;
}
//EOF