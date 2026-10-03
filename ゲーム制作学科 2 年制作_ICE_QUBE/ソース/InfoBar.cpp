//=============================================================================
//インフォメーション処理[InfoBar.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "InfoBar.h"
//*****************************************************************************
//定数定義
//*****************************************************************************
#define U_SIZE (0.33f)	//１つのU座標のサイズ
#define V_SIZE (0.165f)	//１つのV座標のサイズ
//*****************************************************************************
//プロトタイプ宣言(内部)
//*****************************************************************************
void ChangeInfoBuff (void);
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DITex=NULL;									//テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffInfo=NULL;						//頂点バッファへのポインタ
static INFO *g_pInfo=NULL;											//時間構造体
static bool g_first=false;
//=============================================================================
//インフォメーション初期化
//=============================================================================
HRESULT InitInfo (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice=GetDevice();

	//NULLチェック
	if(g_pInfo==NULL)
	{
		//動的確保
		g_pInfo=new INFO;
	}

	//オブジェクトの初期化
	g_pInfo->fX=0.0f;
	g_pInfo->fWidth=300.0f;
	g_pInfo->fY=555.0f;
	g_pInfo->fHeight=45.0f;
	g_pInfo->fU=0.0f;
	g_pInfo->fV=0.0f;
	g_pInfo->fDestU=0.0f;
	g_pInfo->fDestV=0.0f;
	g_pInfo->nCnt=0;


	///////////////////////////
	//	頂点バッファの設定	//
	/////////////////////////
	if(g_first==false)
	{
		//頂点バッファの生成
		if(FAILED(pDevice->CreateVertexBuffer
					(sizeof(VERTEX_2D)*4,
					D3DUSAGE_WRITEONLY,
					FVF_VERTEX_2D,
					D3DPOOL_MANAGED,
					&g_pD3DVtxBuffInfo,
					NULL)))
		{
			return E_FAIL;
		}

		//テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
								  "data/TEXTURE/InfoBar000.jpg",
								  &g_pD3DITex);
		g_first=true;
	}

	return S_OK;
}
//=============================================================================
//インフォメーション更新
//=============================================================================
void UpdateInfo (void)
{
	//慣性の法則によってUV座標を目的の位置へ移動
	g_pInfo->fU=g_pInfo->fU+(g_pInfo->fDestU-g_pInfo->fU)*0.05f;
	g_pInfo->fV=g_pInfo->fV+(g_pInfo->fDestV-g_pInfo->fV)*0.05f;

	//カウントアップ
	g_pInfo->nCnt++;
}
//=============================================================================
//インフォメーション描画
//=============================================================================
void DrawInfo (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1=GetDevice();

	//頂点バッファの変更
	ChangeInfoBuff();

	//頂点バッファのバインド
	pDevice1->SetStreamSource(0,g_pD3DVtxBuffInfo,0,sizeof(VERTEX_2D));

	//頂点フォーマットのセット
	pDevice1->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice1->SetTexture(0,g_pD3DITex);

	//ポリゴンの描画
	pDevice1->DrawPrimitive(D3DPT_TRIANGLESTRIP,
							0,//ポリゴンの数
							2);

}
//=============================================================================
//インフォメーション頂点バッファ書き換え
//=============================================================================
void ChangeInfoBuff (void)
{
	VERTEX_2D *pVtx;

	g_pD3DVtxBuffInfo->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の代入
	pVtx[0].vtx=D3DXVECTOR3(g_pInfo->fX,g_pInfo->fY+g_pInfo->fHeight,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(g_pInfo->fX,g_pInfo->fY,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(g_pInfo->fX+g_pInfo->fWidth,g_pInfo->fY+g_pInfo->fHeight,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(g_pInfo->fX+g_pInfo->fWidth,g_pInfo->fY,0.0f);

	//中身
	pVtx[0].rhw=1.0f;
	pVtx[1].rhw=1.0f;
	pVtx[2].rhw=1.0f;
	pVtx[3].rhw=1.0f;
		
	//反射光
	pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

	//テクスチャ
	pVtx[0].tex=D3DXVECTOR2(0.0f+(g_pInfo->fU),V_SIZE+(g_pInfo->fV));
	pVtx[1].tex=D3DXVECTOR2(0.0f+(g_pInfo->fU),0.0f+(g_pInfo->fV));
	pVtx[2].tex=D3DXVECTOR2(U_SIZE+(g_pInfo->fU),V_SIZE+(g_pInfo->fV));
	pVtx[3].tex=D3DXVECTOR2(U_SIZE+(g_pInfo->fU),0.0f+(g_pInfo->fV));

	g_pD3DVtxBuffInfo->Unlock();
}
//=============================================================================
//インフォメーションセット
//=============================================================================
void SetInfo(INFOTYPE disp)
{
	g_pInfo->fDestU=(U_SIZE*(disp%3));
	g_pInfo->fDestV=(V_SIZE*(disp/3));
}
//=============================================================================
//インフォメーション終了
//=============================================================================
void UninitInfo (void)
{
	//NULLチェック
	if(g_pInfo!=NULL)
	{
		delete g_pInfo;
		g_pInfo=NULL;
	}

	//頂点バッファの解放
	if(g_pD3DVtxBuffInfo!=NULL)
	{
		g_pD3DVtxBuffInfo->Release();
		g_pD3DVtxBuffInfo=NULL;
	}

	//テクスチャの開放
	if(g_pD3DITex!=NULL)
	{
		g_pD3DITex->Release();
		g_pD3DITex=NULL;
	}
}
//EOF