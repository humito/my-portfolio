//=============================================================================
//天敵処理[enemyA.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "enemyA.h"
//*****************************************************************************
//プロトタイプ宣言(内部)
//*****************************************************************************
void SortNaturalEnemy (void);
void ChangeEneABuff (int num);
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9		g_pD3DTextureEneA = NULL;					// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffEneA=NULL;	//頂点バッファへのポインタ
static ENEMYA	g_naturalEnemy[ENEMYA_MAX];							//天敵
static bool g_first=false;
//=============================================================================
//初期化処理
//=============================================================================
HRESULT InitEnemyA (void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	///////////////////////////////////
	//	オブジェクト情報の初期化	//
	/////////////////////////////////

	//天敵の数分ループ
	for(int i=0;i<ENEMYA_MAX;i++)
	{
		g_naturalEnemy[i].fSize=400.0f;								//半分サイズ

		g_naturalEnemy[i].pos=D3DXVECTOR3(11000.0f-(2000.0f*i),					//座標
										  g_naturalEnemy[i].fSize,
										  10800.0f);

		g_naturalEnemy[i].posMove=D3DXVECTOR3(10.3f,0.0f,10.3f);	//移動量
		g_naturalEnemy[i].rot=D3DXVECTOR3(0.0f,0.0f,0.0f);			//角度
		g_naturalEnemy[i].scl=D3DXVECTOR3(1.0f,1.0f,1.0f);			//サイズ
		g_naturalEnemy[i].nColor=255;								//色値
		g_naturalEnemy[i].bHit=false;								//ヒットフラグ
		g_naturalEnemy[i].nHitTime=0;								//ヒット時の経過時間
		g_naturalEnemy[i].nCnt=0;									//カウント
	}

	///////////////////////////
	//	頂点バッファの生成	//
	/////////////////////////
	if(g_first==false)
	{
		//頂点バッファの生成
		if(FAILED(pDevice->CreateVertexBuffer
					(sizeof(VERTEX_3D)*4,
					D3DUSAGE_WRITEONLY,
					FVF_VERTEX_3D,
					D3DPOOL_MANAGED,
					&g_pD3DVtxBuffEneA,
					NULL)))
		{
			return E_FAIL;
		}

		//テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
									"data/TEXTURE/NaturalEnemy000.png",
									&g_pD3DTextureEneA);
		g_first=true;
	}
	return S_OK;
}
//=============================================================================
//更新処理
//=============================================================================
void UpdateEnemyA (void)
{
	///////////////////////////////
	//	天敵オブジェクトの更新	//
	/////////////////////////////

	//天敵の数分ループ
	for(int i=0;i<ENEMYA_MAX;i++)
	{
		//カウントアップ
		g_naturalEnemy[i].nCnt++;

		//天敵移動
		g_naturalEnemy[i].pos.x+=sinf(g_naturalEnemy[i].rot.y+D3DX_PI/2)*g_naturalEnemy[i].posMove.x;//X座標
		g_naturalEnemy[i].pos.z+=sinf(g_naturalEnemy[i].rot.y+D3DX_PI/2)*g_naturalEnemy[i].posMove.z;//Z座標

		//ヒットフラグtrueなら
		if(g_naturalEnemy[i].bHit==true)
		{
			//ヒット時のカウントアップ
			g_naturalEnemy[i].nHitTime++;
		}

		//ヒット時の経過カウントが一定なら
		if(g_naturalEnemy[i].nHitTime>3)
		{
			g_naturalEnemy[i].bHit=false;//ヒットフラグfalse
			g_naturalEnemy[i].nColor=255;//色値を戻す
		}
	}

	///////////////
	//	壁制御	//
	/////////////

	//天敵の個数分ループ
	for(int i=0;i<ENEMYA_MAX;i++)
	{
		//敵がX座標の壁に当ったら
		if((g_naturalEnemy[i].pos.x>WALL_RIGHT_POS ||
		   g_naturalEnemy[i].pos.x<WALL_LEFT_POS))
		{
			//移動量の反転
			g_naturalEnemy[i].posMove.x=-g_naturalEnemy[i].posMove.x;

		}

		//敵がZ座標の壁に当ったら
		if((g_naturalEnemy[i].pos.z>WALL_AHEAD_POS ||
		   g_naturalEnemy[i].pos.z<WALL_BACK_POS))
		{
			//移動量の反転
			g_naturalEnemy[i].posMove.z=-g_naturalEnemy[i].posMove.z;
		}

	}

}
//=============================================================================
//描画処理
//=============================================================================
void DrawEnemyA (void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//アルファテストを行い透明色をなくす
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);		// αブレンドを行う
	pDevice->SetRenderState(D3DRS_ALPHAREF,100);				// α値変更
	pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);	// 任意の値より大きい値
	
	//Zバッファによる天敵の並び替え
	SortNaturalEnemy();

	//弾の個数分ループ
	for(int i=0;i<ENEMYA_MAX;i++)
	{
		//頂点バッファの変更
		ChangeEneABuff(i);

		//各情報のマトリックス変数
		D3DXMATRIX mtxScl,mtxRot,mtxTranslate,mtxView;

		//カメラ情報取得
		mtxView=GetMtxView();

		//アイデンティティ
		D3DXMatrixIdentity(&g_naturalEnemy[i].mtxWorld);
		//ビュー設定
		D3DXMatrixInverse(&g_naturalEnemy[i].mtxWorld,NULL,&mtxView);

		g_naturalEnemy[i].mtxWorld._41=0.0f;
		g_naturalEnemy[i].mtxWorld._42=0.0f;
		g_naturalEnemy[i].mtxWorld._43=0.0f;

		//大きさを設定
		D3DXMatrixScaling(&mtxScl,g_naturalEnemy[i].scl.x,
								  g_naturalEnemy[i].scl.y,
								  g_naturalEnemy[i].scl.z);
		//大きさを反映
		D3DXMatrixMultiply(&g_naturalEnemy[i].mtxWorld,
						   &g_naturalEnemy[i].mtxWorld,
						   &mtxScl);

		//回転を設定
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
										0.0f,
										0.0f,
										0.0f);

		//回転を反映
		D3DXMatrixMultiply(&g_naturalEnemy[i].mtxWorld,&g_naturalEnemy[i].mtxWorld,
						   &mtxRot);


		//位置を設定
		D3DXMatrixTranslation(&mtxTranslate,
							  g_naturalEnemy[i].pos.x,
							  g_naturalEnemy[i].pos.y,
							  g_naturalEnemy[i].pos.z);

		//位置を反映
		D3DXMatrixMultiply(&g_naturalEnemy[i].mtxWorld,&g_naturalEnemy[i].mtxWorld,
					 &mtxTranslate);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD,
							  &g_naturalEnemy[i].mtxWorld);


		//３Ｄポリゴンの描画
		pDevice->SetStreamSource(0,g_pD3DVtxBuffEneA, 0, sizeof(VERTEX_3D));
		
		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0,g_pD3DTextureEneA);

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
										0,//ポリゴンの数
										2);
	}
	//元に戻す
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);		// αブレンドを行う
	pDevice->SetRenderState(D3DRS_ALPHAREF,255);				// α値変更
	pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);	// 任意の値より大きい値
}
//=============================================================================
//天敵頂点バッファ情報の変更
//=============================================================================
void ChangeEneABuff (int num)
{
	VERTEX_3D *pVtx;

	g_pD3DVtxBuffEneA->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の設定
	pVtx[0].vtx=D3DXVECTOR3(-g_naturalEnemy[num].fSize,-g_naturalEnemy[num].fSize,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(-g_naturalEnemy[num].fSize,g_naturalEnemy[num].fSize,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(g_naturalEnemy[num].fSize,-g_naturalEnemy[num].fSize,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(g_naturalEnemy[num].fSize,g_naturalEnemy[num].fSize,0.0f);

	//法線ベクトル
	pVtx[0].nor=D3DXVECTOR3(-1.0f,1.0f,-1.0f);
	pVtx[1].nor=D3DXVECTOR3(-1.0f,1.0f,-1.0f);
	pVtx[2].nor=D3DXVECTOR3(-1.0f,1.0f,-1.0f);
	pVtx[3].nor=D3DXVECTOR3(-1.0f,1.0f,-1.0f);

	//ポリゴンの色設定
	pVtx[0].diffuse=D3DCOLOR_RGBA(g_naturalEnemy[num].nColor,g_naturalEnemy[num].nColor,g_naturalEnemy[num].nColor,255);
	pVtx[1].diffuse=D3DCOLOR_RGBA(g_naturalEnemy[num].nColor,g_naturalEnemy[num].nColor,g_naturalEnemy[num].nColor,255);
	pVtx[2].diffuse=D3DCOLOR_RGBA(g_naturalEnemy[num].nColor,g_naturalEnemy[num].nColor,g_naturalEnemy[num].nColor,255);
	pVtx[3].diffuse=D3DCOLOR_RGBA(g_naturalEnemy[num].nColor,g_naturalEnemy[num].nColor,g_naturalEnemy[num].nColor,255);

	//テクスチャ座標
	pVtx[0].tex=D3DXVECTOR2(0.0f+(0.165f*((g_naturalEnemy[num].nCnt/5)%6)),1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f+(0.165f*((g_naturalEnemy[num].nCnt/5)%6)),0.0f);
	pVtx[2].tex=D3DXVECTOR2(0.165f+(0.165f*((g_naturalEnemy[num].nCnt/5)%6)),1.0f);
	pVtx[3].tex=D3DXVECTOR2(0.165f+(0.165f*((g_naturalEnemy[num].nCnt/5)%6)),0.0f);

	g_pD3DVtxBuffEneA->Unlock();
}
//=============================================================================
//終了処理
//=============================================================================
void UninitEnemyA (void)
{
	//テクスチャへのポインタ終了
	if(g_pD3DTextureEneA!=NULL)
	{
		g_pD3DTextureEneA->Release();
		g_pD3DTextureEneA=NULL;
	}

	//頂点バッファへのポインタ終了
	if(g_pD3DVtxBuffEneA!=NULL)
	{
		g_pD3DVtxBuffEneA->Release();
		g_pD3DVtxBuffEneA=NULL;
	}
}
//=============================================================================
//ヒットフラグtrue
//=============================================================================
void HitEnemyA (int num)
{
	g_naturalEnemy[num].bHit=true;		//フラグtrueに変更
	g_naturalEnemy[num].nHitTime=0;		//カウントリセット
	g_naturalEnemy[num].nColor=100;		//色値を最少に設定
	//TODO:鉄が当たった効果音を出す
}
//=============================================================================
//天敵構造体の取得
//=============================================================================
ENEMYA GetEnemyA (int num)
{
	return g_naturalEnemy[num];
}
//=============================================================================
//敵の並び替え
//=============================================================================
void SortNaturalEnemy (void)
{
	//ソート用変数
	ENEMYA work;

	for(int i=0;i<ENEMYA_MAX-1;i++)
	{
		for(int j=i+1;j<ENEMYA_MAX;j++)
		{
			if(g_naturalEnemy[i].pos.z<g_naturalEnemy[j].pos.z)
			{
				work=g_naturalEnemy[i];
				g_naturalEnemy[i]=g_naturalEnemy[j];
				g_naturalEnemy[j]=work;
			}
		}
	}
}
//EOF