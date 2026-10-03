//=============================================================================
//スコア処理[Score.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Score.h"
//*****************************************************************************
//プロトタイプ宣言(内部)
//*****************************************************************************
void ChangeScoreBuff (int num);
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DSTex=NULL;						//テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffScore=NULL;			//頂点バッファへのポインタ
VERTEX_2D g_sVtx[DIGITS][4];							//頂点情報格納ワーク
static int g_Num=0;
static int g_Cut=0;
static int g_plus=0;
//スコアの初期化
HRESULT InitScore (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice=GetDevice();

	g_plus=0;

	//値をリセット
	g_Num=0;

	//ずらす桁数の初期化
	g_Cut=10000000;

	///////////////////////////
	//	頂点バッファの設定	//
	/////////////////////////

	//頂点バッファの生成
	if(FAILED(pDevice->CreateVertexBuffer
				(sizeof(VERTEX_2D)*4,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_2D,
				D3DPOOL_MANAGED,
				&g_pD3DVtxBuffScore,
				NULL)))
	{
		return E_FAIL;
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/number000.png",
							  &g_pD3DSTex);

	return S_OK;
}
//スコアの描画
void DrawScore (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice=GetDevice();

	//ケタの数分ループ
	for(int i=0;i<DIGITS;i++)
	{
		//頂点バッファの変更
		ChangeScoreBuff(i);

		//頂点バッファのバインド
		pDevice->SetStreamSource(0,g_pD3DVtxBuffScore,0,sizeof(VERTEX_2D));

		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_2D);

		//テクスチャの設定
		pDevice->SetTexture(0,g_pD3DSTex);

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
								0,//ポリゴンの数
								2);
	}
}
//頂点バッファの変更
void ChangeScoreBuff (int num)
{
	//１ケタ表示用変数
	int number=0;

	//スコアの１ケタを抽出
	number=(g_Num/g_Cut)%10;

	VERTEX_2D *pVtx;

	g_pD3DVtxBuffScore->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の代入
	pVtx[0].vtx=D3DXVECTOR3(0.0f+num*25.0f,50.0f+g_plus,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(0.0f+num*25.0f,0.0f+g_plus,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(25.0f+num*25.0f,50.0f+g_plus,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(25.0f+num*25.0f,0.0f+g_plus,0.0f);

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
	pVtx[0].tex=D3DXVECTOR2(0.0f+0.1f*number,1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f+0.1f*number,0.0f);
	pVtx[2].tex=D3DXVECTOR2(0.1f+0.1f*number,1.0f);
	pVtx[3].tex=D3DXVECTOR2(0.1f+0.1f*number,0.0f);

	g_pD3DVtxBuffScore->Unlock();

	//ケタをずらす
	g_Cut/=10;

	//ケタ最大なら
	if(num==DIGITS-1)
	{
		//割り当てが終わったらずらす桁数をリセット
		g_Cut=10000000;
	}
}
//スコア加算
void AddScore (int num)
{
	//スコアを引数分加算
	g_Num+=num;

	//スコア最大数
	if(g_Num>99999999)
	{
		g_Num=99999999;
	}
}
//リザルト用スコア表示
void ResultSPolygon (void)
{
	g_plus=550.0f;
}
//スコアポリゴンの終了
void UninitScore (void)
{
	//テクスチャの開放
	if(g_pD3DSTex!=NULL)
	{
		g_pD3DSTex->Release();
		g_pD3DSTex=NULL;
	}

	//頂点バッファ開放
	if(g_pD3DVtxBuffScore!=NULL)
	{
		g_pD3DVtxBuffScore->Release();
		g_pD3DVtxBuffScore=NULL;
	}
}
//EOF