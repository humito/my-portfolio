//=============================================================================
//弾爆発エフェクト処理[bulletExp.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "bulletExp.h"
//*****************************************************************************
//プロトタイプ宣言(内部)
//*****************************************************************************
void ReleaseBulExp (int num);	//爆発エフェクト解放
void ChangeBExpBuff (int num);
//*****************************************************************************
//グローバル変数
//*****************************************************************************
static BULEXP *g_pBulExp[BULLET_MAX] = {NULL,NULL,NULL,NULL,NULL};	//弾爆発エフェクトポインタ
LPDIRECT3DTEXTURE9		g_pD3DTextureBulExp = NULL;					// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffBulExp = NULL;					// 頂点バッファインターフェースへのポインタ
//=============================================================================
//弾爆発初期化
//=============================================================================
HRESULT InitBulExp(void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//弾の個数分ループ
	for(int i=0;i<BULLET_MAX;i++)
	{
		//NULLチェック
		if(g_pBulExp[i]!=NULL)
		{
			//解放
			ReleaseBulExp(i);
		}
	}

	//頂点バッファの生成
	if(FAILED(pDevice->CreateVertexBuffer
				(sizeof(VERTEX_3D)*4,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_pD3DVtxBuffBulExp,
				NULL)))
	{
		return E_FAIL;
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/LiteEffect000.png",
							  &g_pD3DTextureBulExp);

	return S_OK;
}
//=============================================================================
//弾爆発終了
//=============================================================================
void UninitBulExp(void)
{
	//弾の個数分ループ
	for(int i=0;i<BULLET_MAX;i++)
	{
		//NULLチェック
		if(g_pBulExp[i]!=NULL)
		{
			//解放
			ReleaseBulExp(i);
		}
	}

	//テクスチャへのポインタ終了
	if(g_pD3DTextureBulExp!=NULL)
	{
		g_pD3DTextureBulExp->Release();
		g_pD3DTextureBulExp=NULL;
	}

	//頂点バッファへのポインタ終了
	if(g_pD3DVtxBuffBulExp!=NULL)
	{
		g_pD3DVtxBuffBulExp->Release();
		g_pD3DVtxBuffBulExp=NULL;
	}
}
//=============================================================================
//弾爆発更新
//=============================================================================
void UpdateBulExp(void)
{
	//弾の個数分ループ
	for(int i=0;i<BULLET_MAX;i++)
	{
		//NULLチェック
		if(g_pBulExp[i]!=NULL)
		{
			//エフェクトの更新
			g_pBulExp[i]->nCnt++;

			//カウント一定以上達したら
			if(g_pBulExp[i]->nCnt>30)
			{
				g_pBulExp[i]->pos.x=0.0f;
				g_pBulExp[i]->pos.z=0.0f;
				ReleaseBulExp(i);
			}
		}
	}
}
//=============================================================================
//弾爆発描画
//=============================================================================
void DrawBulExp(void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//弾の個数分ループ
	for(int i=0;i<BULLET_MAX;i++)
	{
		//NULLチェック
		if(g_pBulExp[i]!=NULL)
		{
			//頂点バッファを変更
			ChangeBExpBuff(i);

			//確保されてるエフェクトのみ描画
			D3DXMATRIX mtxScl,mtxRot,mtxTranslate,mtxView;

			//描画後元に戻す
			/*pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
			pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);*/

			//カメラ情報取得
			mtxView=GetMtxView();

			D3DXMatrixIdentity(&g_pBulExp[i]->mtxWorld);
			D3DXMatrixInverse(&g_pBulExp[i]->mtxWorld,NULL,&mtxView);


			g_pBulExp[i]->mtxWorld._41=0.0f;
			g_pBulExp[i]->mtxWorld._42=0.0f;
			g_pBulExp[i]->mtxWorld._43=0.0f;


			D3DXMatrixScaling(&mtxScl,1.0f,1.0f,1.0f);

			D3DXMatrixMultiply(&g_pBulExp[i]->mtxWorld,
							   &g_pBulExp[i]->mtxWorld,
							   &mtxScl);

			//回転を設定
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
											0,
											0,
											0);

			//回転を反映
			D3DXMatrixMultiply(&g_pBulExp[i]->mtxWorld,&g_pBulExp[i]->mtxWorld,
							   &mtxRot);


			//位置を反映
			D3DXMatrixTranslation(&mtxTranslate,
								  g_pBulExp[i]->pos.x,
								  g_pBulExp[i]->pos.y,
								  g_pBulExp[i]->pos.z);

			//ワールドマトリックスの設定
			D3DXMatrixMultiply(&g_pBulExp[i]->mtxWorld,&g_pBulExp[i]->mtxWorld,
						 &mtxTranslate);

			//位置をセット
			pDevice->SetTransform(D3DTS_WORLD,
								  &g_pBulExp[i]->mtxWorld);


			//３Ｄポリゴンの描画
			pDevice->SetStreamSource(0,g_pD3DVtxBuffBulExp, 0, sizeof(VERTEX_3D));
		
			//頂点フォーマットのセット
			pDevice->SetFVF(FVF_VERTEX_3D);

			//テクスチャの設定
			pDevice->SetTexture(0,g_pD3DTextureBulExp);

			//ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
											0,//ポリゴンの数
											2);

		}
	}
}
//=============================================================================
//弾爆発頂点バッファ変更
//=============================================================================
void ChangeBExpBuff (int num)
{
	///////////////////////////
	//	頂点バッファの変更	//
	/////////////////////////
	VERTEX_3D *pVtx;

	g_pD3DVtxBuffBulExp->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の設定
	pVtx[0].vtx=D3DXVECTOR3(-g_pBulExp[num]->fSize,-g_pBulExp[num]->fSize,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(-g_pBulExp[num]->fSize,g_pBulExp[num]->fSize,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(g_pBulExp[num]->fSize,-g_pBulExp[num]->fSize,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(g_pBulExp[num]->fSize,g_pBulExp[num]->fSize,0.0f);
	
	//法線ベクトル
	pVtx[0].nor=D3DXVECTOR3(1.0f,1.0f,-1.0f);
	pVtx[1].nor=D3DXVECTOR3(1.0f,1.0f,-1.0f);
	pVtx[2].nor=D3DXVECTOR3(1.0f,1.0f,-1.0f);
	pVtx[3].nor=D3DXVECTOR3(1.0f,1.0f,-1.0f);

	//光源色
	pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

	//テクスチャ座標
	pVtx[0].tex=D3DXVECTOR2(0.0f+(0.2f*((g_pBulExp[num]->nCnt/3)%5)),0.5f+(0.5f*(g_pBulExp[num]->nCnt/15)));
	pVtx[1].tex=D3DXVECTOR2(0.0f+(0.2f*((g_pBulExp[num]->nCnt/3)%5)),0.0f+(0.5f*(g_pBulExp[num]->nCnt/15)));
	pVtx[2].tex=D3DXVECTOR2(0.2f+(0.2f*((g_pBulExp[num]->nCnt/3)%5)),0.5f+(0.5f*(g_pBulExp[num]->nCnt/15)));
	pVtx[3].tex=D3DXVECTOR2(0.2f+(0.2f*((g_pBulExp[num]->nCnt/3)%5)),0.0f+(0.5f*(g_pBulExp[num]->nCnt/15)));
	
	g_pD3DVtxBuffBulExp->Unlock();
}
//=============================================================================
//弾爆発設置
//=============================================================================
void CreateBulExp (float fPosX,float fPosY,float fPosZ,float fSize)
{
	//弾の個数分ループ
	for(int i=0;i<BULLET_MAX;i++)
	{
		//NULLチェック
		if(g_pBulExp[i]==NULL)
		{
			//NULLの場合動的確保して作成
			g_pBulExp[i]=new BULEXP;

			g_pBulExp[i]->nCnt=0;
			g_pBulExp[i]->pos.x=fPosX;
			g_pBulExp[i]->pos.y=fPosY;
			g_pBulExp[i]->pos.z=fPosZ;
			g_pBulExp[i]->fSize=fSize;

			break;
		}
	}
}
//=============================================================================
//弾爆発解放
//=============================================================================
void ReleaseBulExp (int num)
{
	//NULLチェック
	if(g_pBulExp[num]!=NULL)
	{
		delete g_pBulExp[num];
		g_pBulExp[num]=NULL;
	}
}
//EOF