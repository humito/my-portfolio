#include "Score.h"

LPDIRECT3DTEXTURE9 g_pD3DSTex=NULL;				//テクスチャへのポインタ
VERTEX_2D g_sVtx[DIGITS][4];							//頂点情報格納ワーク
static int g_Num=0;
static int g_Cut=0;
//スコアの初期化
HRESULT InitScore (void)
{
	//値をリセット
	g_Num=0;

	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pDevices;

	//ゲッターで返す
	pDevices=GetDevice();

	//ずらす桁数の初期化
	g_Cut=10000000;

	for(int i=0;i<DIGITS;i++)
	{
		//頂点座標の代入
		g_sVtx[i][0].vtx=D3DXVECTOR3(0.0f+i*25.0f,50.0f,0.0f);
		g_sVtx[i][1].vtx=D3DXVECTOR3(0.0f+i*25.0f,0.0f,0.0f);
		g_sVtx[i][2].vtx=D3DXVECTOR3(25.0f+i*25.0f,50.0f,0.0f);
		g_sVtx[i][3].vtx=D3DXVECTOR3(25.0f+i*25.0f,0.0f,0.0f);
		//225
		//中身
		g_sVtx[i][0].rhw=1.0f;
		g_sVtx[i][1].rhw=1.0f;
		g_sVtx[i][2].rhw=1.0f;
		g_sVtx[i][3].rhw=1.0f;

		//テクスチャ
		g_sVtx[i][0].tex=D3DXVECTOR2(0.0f,1.0f);
		g_sVtx[i][1].tex=D3DXVECTOR2(0.0f,0.0f);
		g_sVtx[i][2].tex=D3DXVECTOR2(0.1f,1.0f);
		g_sVtx[i][3].tex=D3DXVECTOR2(0.1f,0.0f);

		
		//反射光
		g_sVtx[i][0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_sVtx[i][1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_sVtx[i][2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_sVtx[i][3].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevices,
							  "data/TEXTURE/number000.png",
							  &g_pD3DSTex);

	return S_OK;
}
//スコアの描画
void DrawScore (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pEDevices;

	//ゲッターで返す
	pEDevices=GetDevice();

	//頂点フォーマットのセット
	pEDevices->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pEDevices->SetTexture(0,g_pD3DSTex);

	for(int i=0;i<DIGITS;i++)
	{
		//ポリゴンの描画
		pEDevices->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
										2,//ポリゴンの数
										&g_sVtx[i][0],
										sizeof(VERTEX_2D));
	}


}
//スコア加算
void AddScore (int num)
{
	//スコアを引数分加算
	g_Num+=num;

	//スコア１ケタずつポリゴンに割り当て
	for(int i=0;i<DIGITS;i++)
	{
		//１ケタ表示用変数
		int number=0;

		//スコアの１ケタを抽出
		number=(g_Num/g_Cut)%10;


		//テクスチャ
		g_sVtx[i][0].tex=D3DXVECTOR2(0.0f+0.1f*number,1.0f);
		g_sVtx[i][1].tex=D3DXVECTOR2(0.0f+0.1f*number,0.0f);
		g_sVtx[i][2].tex=D3DXVECTOR2(0.1f+0.1f*number,1.0f);
		g_sVtx[i][3].tex=D3DXVECTOR2(0.1f+0.1f*number,0.0f);

		//ケタをずらす
		g_Cut/=10;
	}

	//割り当てが終わったらずらす桁数をリセット
	g_Cut=10000000;
}
//リザルト用スコア表示
void ResultSPolygon (void)
{
	for(int i=0;i<DIGITS;i++)
	{
		//頂点座標の代入
		g_sVtx[i][0].vtx=D3DXVECTOR3(0.0f+i*123.0f,1000.0f,0.0f);
		g_sVtx[i][1].vtx=D3DXVECTOR3(0.0f+i*123.0f,800.0f,0.0f);
		g_sVtx[i][2].vtx=D3DXVECTOR3(123.0f+i*123.0f,1000.0f,0.0f);
		g_sVtx[i][3].vtx=D3DXVECTOR3(123.0f+i*123.0f,800.0f,0.0f);
	}
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
}