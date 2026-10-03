//=============================================================================
//リザルト処理[result.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "result.h"
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DrTex=NULL;	//テクスチャへのポインタ
VERTEX_2D g_ResultVtx[4];			//2Dポリゴン
//=============================================================================
//リザルトの初期化
//=============================================================================
HRESULT InitResult (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice=GetDevice();

	//頂点座標の代入
	g_ResultVtx[0].vtx=D3DXVECTOR3(150.0f,150.0f,0.0f);
	g_ResultVtx[1].vtx=D3DXVECTOR3(150.0f,50.0f,0.0f);
	g_ResultVtx[2].vtx=D3DXVECTOR3(650.0f,150.0f,0.0f);
	g_ResultVtx[3].vtx=D3DXVECTOR3(650.0f,50.0f,0.0f);

	//中身
	g_ResultVtx[0].rhw=1.0f;
	g_ResultVtx[1].rhw=1.0f;
	g_ResultVtx[2].rhw=1.0f;
	g_ResultVtx[3].rhw=1.0f;
		
	//反射光
	g_ResultVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	g_ResultVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	g_ResultVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	g_ResultVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

	//テクスチャ
	g_ResultVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	g_ResultVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	g_ResultVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	g_ResultVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/result_logo.png",
							  &g_pD3DrTex);

	return S_OK;
}
//=============================================================================
//リザルトの更新
//=============================================================================
void UpdateResult (void)
{
	//エンターキーでワイプセット
	if(GetKeyboardTrigger(DIK_RETURN))
	{
		SetWipe(WIPE_IN);
	}
}
//=============================================================================
//リザルトの描画
//=============================================================================
void DrawResult (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevice1=GetDevice();

	//頂点フォーマットのセット
	pDevice1->SetFVF(FVF_VERTEX_2D);


	//テクスチャの設定
	pDevice1->SetTexture(0,g_pD3DrTex);

	//背景ポリゴンの描画
	pDevice1->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
									2,//ポリゴンの数
									&g_ResultVtx[0],
									sizeof(VERTEX_2D));

}
//=============================================================================
//リザルトの終了
//=============================================================================
void UninitResult (void)
{
	//テクスチャの開放
	if(g_pD3DrTex!=NULL)
	{
		g_pD3DrTex->Release();
		g_pD3DrTex=NULL;
	}
}
//EOF