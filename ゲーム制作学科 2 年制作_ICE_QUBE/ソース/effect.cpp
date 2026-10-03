//弾エフェクト処理[effect.cpp]
//Author:HUMITO KIMURA

#include "effect.h"
//グローバル変数
LPDIRECT3DTEXTURE9		g_pD3DTextureEfe = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffEfe = NULL;	// 頂点バッファインターフェースへのポインタ

static EFFECT g_effect[MAX_EFFECT][BULLET_MAX];
//初期化
HRESULT InitEffect (void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();


	//ポリゴンの設定
	for(int i=0;i<BULLET_MAX;i++)
	{
		for(int j=0;j<MAX_EFFECT;j++)
		{
			g_effect[j][i].pos=D3DXVECTOR3(5000.0f,0.0f,0.0f);
			g_effect[j][i].scl=D3DXVECTOR3(1.0f-(0.2f*j),1.0f-(0.2f*j),1.0f-(0.2f*j));
			g_effect[j][i].rot=D3DXVECTOR3(0.0f,0.0f,0.0f);
		}
	}


	//頂点バッファの生成
	if(FAILED(pDevice->CreateVertexBuffer
				(sizeof(VERTEX_3D)*4,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_pD3DVtxBuffEfe,
				NULL)))
	{
		return E_FAIL;
	}


	VERTEX_3D *pVtx;

	g_pD3DVtxBuffEfe->Lock(0,0,(void**)&pVtx,0);

	pVtx[0].vtx=D3DXVECTOR3(-110.0f,-110.0f,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(-110.0f,110.0f,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(110.0f,-110.0f,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(110.0f,110.0f,0.0f);

	pVtx[0].nor=D3DXVECTOR3(1.0f,1.0f,-1.0f);
	pVtx[1].nor=D3DXVECTOR3(1.0f,1.0f,-1.0f);
	pVtx[2].nor=D3DXVECTOR3(1.0f,1.0f,-1.0f);
	pVtx[3].nor=D3DXVECTOR3(1.0f,1.0f,-1.0f);

	pVtx[0].diffuse=D3DCOLOR_RGBA(100,100,200,255);
	pVtx[1].diffuse=D3DCOLOR_RGBA(100,100,200,255);
	pVtx[2].diffuse=D3DCOLOR_RGBA(100,100,200,255);
	pVtx[3].diffuse=D3DCOLOR_RGBA(100,100,200,255);

	pVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	pVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	pVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);
	

	g_pD3DVtxBuffEfe->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/effect000.jpg",
							  &g_pD3DTextureEfe);

	return S_OK;
}
//エフェクト更新
void UpdateEffect(void)
{
	
}
//エフェクト描画
void DrawEffect (void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//加算合成
	pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

	pDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);

	//弾の個数分ループ
	for(int i=0;i<BULLET_MAX;i++)
	{
		//エフェクトの個数分ループ
		for(int j=0;j<MAX_EFFECT;j++)
		{
			D3DXMATRIX mtxScl,mtxRot,mtxTranslate,mtxView;


			//カメラ情報取得
			mtxView=GetMtxView();

			D3DXMatrixIdentity(&g_effect[j][i].mtxWorld);
			D3DXMatrixInverse(&g_effect[j][i].mtxWorld,NULL,&mtxView);


			g_effect[j][i].mtxWorld._41=0.0f;
			g_effect[j][i].mtxWorld._42=0.0f;
			g_effect[j][i].mtxWorld._43=0.0f;


			D3DXMatrixScaling(&mtxScl,
							  g_effect[j][i].scl.x,
							  g_effect[j][i].scl.y,
							  g_effect[j][i].scl.z);

			D3DXMatrixMultiply(&g_effect[j][i].mtxWorld,
							   &g_effect[j][i].mtxWorld,
							   &mtxScl);

			//回転を設定
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
											0,
											0,
											0);

			//回転を反映
			D3DXMatrixMultiply(&g_effect[j][i].mtxWorld,&g_effect[j][i].mtxWorld,
							   &mtxRot);


			//位置を反映
			D3DXMatrixTranslation(&mtxTranslate,
								  g_effect[j][i].pos.x,
								  g_effect[j][i].pos.y,
								  g_effect[j][i].pos.z);

			//ワールドマトリックスの設定
			D3DXMatrixMultiply(&g_effect[j][i].mtxWorld,&g_effect[j][i].mtxWorld,
						 &mtxTranslate);

			//位置をセット
			pDevice->SetTransform(D3DTS_WORLD,
								  &g_effect[j][i].mtxWorld);


			//３Ｄポリゴンの描画
			pDevice->SetStreamSource(0,g_pD3DVtxBuffEfe, 0, sizeof(VERTEX_3D));
			//頂点フォーマットのセット
			pDevice->SetFVF(FVF_VERTEX_3D);

			//テクスチャの設定
			pDevice->SetTexture(0,g_pD3DTextureEfe);

			//ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
											0,//ポリゴンの数
											2);
		}
	}
	//描画後元に戻す
	pDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);

	//元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

}
//エフェクト終了
void UninitEffect (void)
{
	//テクスチャへのポインタ終了
	if(g_pD3DTextureEfe!=NULL)
	{
		g_pD3DTextureEfe->Release();
		g_pD3DTextureEfe=NULL;
	}
	//頂点バッファへのポインタ終了
	if(g_pD3DVtxBuffEfe!=NULL)
	{
		g_pD3DVtxBuffEfe->Release();
		g_pD3DVtxBuffEfe=NULL;
	}
}
//座標セット
void SetEffect(D3DXVECTOR3 data,int num,float fMoveX,float fMoveZ,float rot)
{
	for(int i=MAX_EFFECT-1;i>0;i--)
	{
		g_effect[i][num].pos.x=g_effect[i-1][num].pos.x-cosf(rot+D3DX_PI/2)*fMoveX;
		g_effect[i][num].pos.z=g_effect[i-1][num].pos.z+sinf(rot+D3DX_PI/2)*fMoveZ;
		g_effect[i][num].pos.y=g_effect[i-1][num].pos.y;
	}

	g_effect[0][num].pos=data;
}
//EOF