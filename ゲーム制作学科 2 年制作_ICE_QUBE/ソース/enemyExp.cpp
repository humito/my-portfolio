//=============================================================================
//敵爆発エフェクト処理[enemyExp.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルード
//*****************************************************************************
#include "enemyExp.h"
//*****************************************************************************
//定数定義(内部)
//*****************************************************************************
#define ENEMY_EXP_MAX (ENEMYB_MAX*5)
//*****************************************************************************
//プロトタイプ宣言(内部)
//*****************************************************************************
void ReleaseEneExp (int num);	//爆発エフェクト解放
void ChangeEExpBuff (int num);	//頂点バッファ変更
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9		g_pD3DTextureEneExp = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffEneExp = NULL;	// 頂点バッファインターフェースへのポインタ
static ENEEXP *g_pEneExp[ENEMY_EXP_MAX];//敵爆発エフェクトポインタ
static bool g_first=false;
//=============================================================================
//敵爆発初期化
//=============================================================================
HRESULT IniEneExp(void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//弾の個数分ループ
	for(int i=0;i<ENEMY_EXP_MAX;i++)
	{
		//NULLチェック
		if(g_pEneExp[i]!=NULL)
		{
			//解放
			ReleaseEneExp(i);
		}
	}
	if(g_first==false)
	{
		//頂点バッファの生成
		if(FAILED(pDevice->CreateVertexBuffer
					(sizeof(VERTEX_3D)*4,
					D3DUSAGE_WRITEONLY,
					FVF_VERTEX_3D,
					D3DPOOL_MANAGED,
					&g_pD3DVtxBuffEneExp,
					NULL)))
		{
			return E_FAIL;
		}

		//テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
								  "data/TEXTURE/explosion001.png",
								  &g_pD3DTextureEneExp);
		g_first=true;
	}
	return S_OK;
}		
//=============================================================================
//敵爆発終了
//=============================================================================
void UninitEneExp(void)
{
	//弾の個数分ループ
	for(int i=0;i<ENEMY_EXP_MAX;i++)
	{
		//NULLチェック
		if(g_pEneExp[i]!=NULL)
		{
			//解放
			ReleaseEneExp(i);
		}
	}

	//テクスチャへのポインタ終了
	if(g_pD3DTextureEneExp!=NULL)
	{
		g_pD3DTextureEneExp->Release();
		g_pD3DTextureEneExp=NULL;
	}

	//頂点バッファへのポインタ終了
	if(g_pD3DVtxBuffEneExp!=NULL)
	{
		g_pD3DVtxBuffEneExp->Release();
		g_pD3DVtxBuffEneExp=NULL;
	}
}
//=============================================================================
//敵爆発更新
//=============================================================================
void UpdateEneExp(void)
{
	//弾の個数分ループ
	for(int i=0;i<ENEMY_EXP_MAX;i++)
	{
		//NULLチェック
		if(g_pEneExp[i]!=NULL)
		{
			//エフェクトの更新
			g_pEneExp[i]->nCnt++;

			//カウント一定以上達したら
			if(g_pEneExp[i]->nCnt>10)
			{
				//座標画面外
				g_pEneExp[i]->pos.x=0.0f;
				g_pEneExp[i]->pos.z=0.0f;
				//解放
				ReleaseEneExp(i);
			}
		}
	}
}
//=============================================================================
//敵爆発描画
//=============================================================================
void DrawEneExp(void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//弾の個数分ループ
	for(int i=0;i<ENEMY_EXP_MAX;i++)
	{
		//NULLチェック
		if(g_pEneExp[i]!=NULL)
		{
			//頂点バッファを変更
			ChangeEExpBuff(i);

			//確保されてるエフェクトのみ描画
			D3DXMATRIX mtxScl,mtxRot,mtxTranslate,mtxView;

			//描画後元に戻す
			/*pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
			pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);*/

			//カメラ情報取得
			mtxView=GetMtxView();

			D3DXMatrixIdentity(&g_pEneExp[i]->mtxWorld);
			D3DXMatrixInverse(&g_pEneExp[i]->mtxWorld,NULL,&mtxView);


			g_pEneExp[i]->mtxWorld._41=0.0f;
			g_pEneExp[i]->mtxWorld._42=0.0f;
			g_pEneExp[i]->mtxWorld._43=0.0f;


			D3DXMatrixScaling(&mtxScl,1.0f,1.0f,1.0f);

			D3DXMatrixMultiply(&g_pEneExp[i]->mtxWorld,
							   &g_pEneExp[i]->mtxWorld,
							   &mtxScl);

			//回転を設定
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
											0,
											0,
											0);

			//回転を反映
			D3DXMatrixMultiply(&g_pEneExp[i]->mtxWorld,&g_pEneExp[i]->mtxWorld,
							   &mtxRot);


			//位置を反映
			D3DXMatrixTranslation(&mtxTranslate,
								  g_pEneExp[i]->pos.x,
								  g_pEneExp[i]->pos.y,
								  g_pEneExp[i]->pos.z);

			//ワールドマトリックスの設定
			D3DXMatrixMultiply(&g_pEneExp[i]->mtxWorld,&g_pEneExp[i]->mtxWorld,
						 &mtxTranslate);

			//位置をセット
			pDevice->SetTransform(D3DTS_WORLD,
								  &g_pEneExp[i]->mtxWorld);


			//３Ｄポリゴンの描画
			pDevice->SetStreamSource(0,g_pD3DVtxBuffEneExp, 0, sizeof(VERTEX_3D));
		
			//頂点フォーマットのセット
			pDevice->SetFVF(FVF_VERTEX_3D);

			//テクスチャの設定
			pDevice->SetTexture(0,g_pD3DTextureEneExp);

			//ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
											0,//ポリゴンの数
											2);

		}
	}
}
//=============================================================================
//頂点バッファ変更
//=============================================================================
void ChangeEExpBuff (int num)
{
	///////////////////////////
	//	頂点バッファの変更	//
	/////////////////////////
	VERTEX_3D *pVtx;

	g_pD3DVtxBuffEneExp->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の設定
	pVtx[0].vtx=D3DXVECTOR3(-g_pEneExp[num]->fSize,-g_pEneExp[num]->fSize,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(-g_pEneExp[num]->fSize,g_pEneExp[num]->fSize,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(g_pEneExp[num]->fSize,-g_pEneExp[num]->fSize,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(g_pEneExp[num]->fSize,g_pEneExp[num]->fSize,0.0f);
	
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
	pVtx[0].tex=D3DXVECTOR2(0.0f+(0.2f*((g_pEneExp[num]->nCnt)%5)),0.5f+(0.5f*(g_pEneExp[num]->nCnt/5)));
	pVtx[1].tex=D3DXVECTOR2(0.0f+(0.2f*((g_pEneExp[num]->nCnt)%5)),0.0f+(0.5f*(g_pEneExp[num]->nCnt/5)));
	pVtx[2].tex=D3DXVECTOR2(0.2f+(0.2f*((g_pEneExp[num]->nCnt)%5)),0.5f+(0.5f*(g_pEneExp[num]->nCnt/5)));
	pVtx[3].tex=D3DXVECTOR2(0.2f+(0.2f*((g_pEneExp[num]->nCnt)%5)),0.0f+(0.5f*(g_pEneExp[num]->nCnt/5)));
	
	g_pD3DVtxBuffEneExp->Unlock();
}
//=============================================================================
//敵爆発作成
//=============================================================================
void CreateEneExp (float fPosX,float fPosY,float fPosZ,float fSize)
{
	//弾の個数分ループ
	for(int i=0;i<ENEMY_EXP_MAX;i++)
	{
		//NULLチェック
		if(g_pEneExp[i]==NULL)
		{
			//NULLの場合動的確保して作成
  			g_pEneExp[i]=new ENEEXP;

			g_pEneExp[i]->nCnt=0;
			g_pEneExp[i]->pos.x=fPosX;
			g_pEneExp[i]->pos.y=fPosY;
			g_pEneExp[i]->pos.z=fPosZ;
			g_pEneExp[i]->fSize=fSize;

			break;
		}
	}
}
//=============================================================================
//敵爆発解放
//=============================================================================
void ReleaseEneExp (int num)
{
	//NULLチェック
	if(g_pEneExp[num]!=NULL)
	{
		delete g_pEneExp[num];
		g_pEneExp[num]=NULL;
	}
}
//EOF