//=============================================================================
//壁の処理[wall.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "wall.h"
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9		g_pD3DTextureWall = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffWall = NULL;	// 頂点バッファインターフェースへのポインタ
LPDIRECT3DINDEXBUFFER9 g_pD3DIndexBuffWall=NULL;	//インデックスバッファへのポインタ
static D3DXMATRIX		g_mtxWorld[4];				//ワールドマトリックス
D3DXVECTOR3				g_posWall;					//ポリゴンの位置
D3DXVECTOR3				g_rotWall;					//ポリゴンの向き(回転)
D3DXVECTOR3				g_sclWall;					//ポリゴンの大きさ(スケール)
static int				g_nNumVertexIndex;			//頂点の総インデックス数
static int				g_nNumPolygon;				// 総ポリゴン数
static int				g_nNumVertex;				// 総頂点数
static int			g_nNumBlockX, g_nNumBlockY;		// ブロック数
static float		g_fSizeBlockX, g_fSizeBlockY;	// ブロックサイズ
//=============================================================================
//壁初期化
//=============================================================================
HRESULT InitWall(float fX,float fY,float fZ,float fRot,int nNumBlockX, int nNumBlockY, float fSizeBlockX, float fSizeBlockY)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//ポリゴンの設定
	g_posWall=D3DXVECTOR3(10000.0f,0.0f,10000.0f);//座標
	g_rotWall=D3DXVECTOR3(0.0f,fRot,0.0f);//回転
	g_sclWall=D3DXVECTOR3(1.0f,1.0f,1.0f);//サイズ

	//ブロック数代入
	g_nNumBlockX=nNumBlockX;
	g_nNumBlockY=nNumBlockY;

	//ブロックサイズ代入
	g_fSizeBlockX=fSizeBlockX;
	g_fSizeBlockY=fSizeBlockY;

	g_nNumPolygon=12;
	g_nNumVertex=14;
	g_nNumVertexIndex=9;

	////////////////////////////////////////////////////////////////////////////
	//							頂点バッファの生成							 //
	//////////////////////////////////////////////////////////////////////////

	//頂点バッファの生成
	if(FAILED(pDevice->CreateVertexBuffer
			 (sizeof(VERTEX_3D)*g_nNumVertex,
			 D3DUSAGE_WRITEONLY,
			 FVF_VERTEX_3D,
			 D3DPOOL_MANAGED,
			 &g_pD3DVtxBuffWall,
			 NULL)))
	{
		return E_FAIL;
	}

	VERTEX_3D *pVtx;//3Dポリゴン用頂点

	g_pD3DVtxBuffWall->Lock(0,0,(void**)&pVtx,0);

	///////////////////////
	//	頂点設定ループ	//
	/////////////////////

	//2x2マス
	//行
	for(int y=0;y<2;y++)
	{
		//列
		for(int x=0;x<2+1;x++)
		{
			///////////////////////////
			//	頂点座標のセット	//
			/////////////////////////

			//下側
			pVtx[0+(y*8)+(x*2)].vtx=D3DXVECTOR3(fX+(x*g_fSizeBlockX),					//X
												(fY+g_fSizeBlockY)-(y*g_fSizeBlockY),	//Y
												fZ										//Z
												);
			//上側
			pVtx[1+(y*8)+(x*2)].vtx=D3DXVECTOR3(fX+(x*g_fSizeBlockX),					//X
												(g_fSizeBlockY*2)-(y*g_fSizeBlockY),	//Y
												fZ										//Z
												);

			//最初の行以外でかつ最初の列
			if(y!=0 && x==0)
			{
				//ひとつ前の頂点座標に下側の座標をセット
				pVtx[-1+(y*8)+(x*2)].vtx=pVtx[0+(y*8)+(x*2)].vtx;
			}

			//最後の行まで
			if(y<1)
			{
				//ひとつ後の頂点座標に上側の座標をセット
				pVtx[2+(y*8)+(x*2)].vtx=pVtx[1+(y*8)+(x*2)].vtx;
			}
		}
	}

	///////////////////////////////////////////
	//	法線ベクトル＆反射光　設定ループ	//
	/////////////////////////////////////////
	for(int i=0;i<14;i++)
	{
		pVtx[i].nor=D3DXVECTOR3(1.0f,1.0f,1.0f);		//法線ベクトル
		pVtx[i].diffuse=D3DCOLOR_RGBA(255,255,255,255);	//反射光
	}
	
	///////////////////////////////
	//	テクスチャ設定ループ	//
	/////////////////////////////
	//行
	for(int z=0;z<2;z++)
	{
		//列
		for(int x=0;x<2+1;x++)
		{
			//テクスチャのセット
			pVtx[0+(z*8)+(x*2)].tex=D3DXVECTOR2(0.0f+(x*1.0f),1.0f);//下側
			pVtx[1+(z*8)+(x*2)].tex=D3DXVECTOR2(0.0f+(x*1.0f),0.0f);//上側
		
			//最初の行以外でかつ最初の列
			if(z!=0 && x==0)
			{
				//ひとつ前の頂点座標に下側のテクスチャをセット
				pVtx[-1+(z*8)+(x*2)].tex=pVtx[0+(z*8)+(x*2)].tex;
			}

			//最後の行まで
			if(z<1)
			{
				//ひとつ後の頂点座標に上側のテクスチャをセット
				pVtx[2+(z*8)+(x*2)].tex=pVtx[1+(z*8)+(x*2)].tex;
			}

		}
	}

	g_pD3DVtxBuffWall->Unlock();


	////////////////////////////////////////////////////////////////////////////
	//					インデックスバッファの生成				              //
	///////////////////////////////////////////////////////////////////////////
	//インデックスバッファの生成
	if(FAILED(pDevice->CreateIndexBuffer
			 (sizeof(WORD)*g_nNumVertexIndex,
			 D3DUSAGE_WRITEONLY,
			 D3DFMT_INDEX16,
			 D3DPOOL_MANAGED,
			 &g_pD3DIndexBuffWall,
			 NULL)))
	{
		return E_FAIL;
	}

	//インデックスのポインタ
	WORD *pIndex;

	g_pD3DIndexBuffWall->Lock(0,0,(void**)&pIndex,0);
	
	pIndex[0]=3;
	pIndex[1]=0;
	pIndex[2]=4;
	pIndex[3]=1;
	pIndex[4]=5;
	pIndex[5]=2;
	pIndex[6]=2;
	pIndex[7]=6;
	pIndex[8]=6;
	pIndex[9]=3;
	pIndex[10]=7;
	pIndex[11]=4;
	pIndex[12]=8;
	pIndex[13]=5;
	


	g_pD3DIndexBuffWall->Unlock();


	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/wall005.jpg",
							  &g_pD3DTextureWall);
	return S_OK;
}
//=============================================================================
//壁終了
//=============================================================================
void UninitWall(void)
{
	//テクスチャへのポインタ終了
	if(g_pD3DTextureWall!=NULL)
	{
		g_pD3DTextureWall->Release();
		g_pD3DTextureWall=NULL;
	}
	//頂点バッファへのポインタ終了
	if(g_pD3DVtxBuffWall!=NULL)
	{
		g_pD3DVtxBuffWall->Release();
		g_pD3DVtxBuffWall=NULL;
	}

	//インデックスバッファへのポインタ終了
	if(g_pD3DIndexBuffWall!=NULL)
	{
		g_pD3DIndexBuffWall->Release();
		g_pD3DIndexBuffWall=NULL;
	}
}
//=============================================================================
//壁更新
//=============================================================================
void UpdateWall(void)
{

}
//=============================================================================
//壁描画
//=============================================================================
void DrawWall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	///////////////////////////////
	//	ポリゴンを４つ分描画	//
	/////////////////////////////
	for(int i=0;i<4;i++)
	{
		D3DXMATRIX mtxScl,mtxRot,mtxTranslate;
		D3DXMatrixIdentity(&g_mtxWorld[i]);

		D3DXMatrixScaling(&mtxScl,1.0f,1.0f,1.0f);

		D3DXMatrixMultiply(&g_mtxWorld[i],
							&g_mtxWorld[i],
							&mtxScl);

		//回転を設定
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
										g_rotWall.y+(D3DX_PI/2)*i,
										g_rotWall.x,
										g_rotWall.z);

		//回転を反映
		D3DXMatrixMultiply(&g_mtxWorld[i],&g_mtxWorld[i],
							&mtxRot);


		//位置を反映
		D3DXMatrixTranslation(&mtxTranslate,
								g_posWall.x,
								g_posWall.y,
								g_posWall.z);

		//ワールドマトリックスの設定
		D3DXMatrixMultiply(&g_mtxWorld[i],&g_mtxWorld[i],
						&mtxTranslate);

		//位置をセット
		pDevice->SetTransform(D3DTS_WORLD,
								&g_mtxWorld[i]);


		//３Ｄポリゴンの描画
		pDevice->SetStreamSource(0,g_pD3DVtxBuffWall, 0, sizeof(VERTEX_3D));
		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_3D);

		//インデックスをバインド
		pDevice->SetIndices(g_pD3DIndexBuffWall);


		//テクスチャの設定
		pDevice->SetTexture(0,g_pD3DTextureWall);

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
										0,//ポリゴンの数
										g_nNumPolygon);

			//ポリゴンの描画(インデックス)
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
										0,
										0,
										g_nNumVertex,
										0,
										g_nNumPolygon);
	}
}
//EOF