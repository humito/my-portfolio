//=============================================================================
//
// メッシュ地面の処理 [meshfield.cpp]
// Author : HUMITO KIMURA
//
//=============================================================================
#include "meshfield.h"
#include "input.h"
#include "camera.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DTextureField=NULL;		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffField=NULL;	// 頂点バッファへのポインタ
LPDIRECT3DINDEXBUFFER9 g_pD3DIndexBuffField=NULL;//インデックスバッファへのポインタ
static D3DXMATRIX g_mtxWorldField[9][9];			// ワールドマトリックス
D3DXVECTOR3 g_posField;						// 位置
D3DXVECTOR3 g_rotField;						// 向き
static int g_nNumVertexIndex;				//頂点の総インデックス数
int g_nNumBlockX, g_nNumBlockZ;				// ブロック数
static int g_nNumVertex;					// 総頂点数
static int g_nNumPolygon;					// 総ポリゴン数
float g_fSizeBlockX, g_fSizeBlockZ;			// ブロックサイズ
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshField(int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//ブロック数代入
	g_nNumBlockX=nNumBlockX;
	g_nNumBlockZ=nNumBlockZ;

	//ブロックサイズ代入
	g_fSizeBlockX=fSizeBlockX;
	g_fSizeBlockZ=fSizeBlockZ;

	//総ポリゴン数
	g_nNumPolygon=12;

	//総頂点数
	g_nNumVertex=2*(g_nNumBlockX+1)+2*(g_nNumBlockZ+1)+2*(g_nNumBlockX*g_nNumBlockZ);

	//インデックス数9
	g_nNumVertexIndex=(g_nNumBlockX+1)*(g_nNumBlockZ+1);

	//ポリゴンの設定
	g_posField=D3DXVECTOR3(WALL_LEFT_POS,0.0f,WALL_BACK_POS);//座標
	g_rotField=D3DXVECTOR3(0.0f,0.0f,0.0f);//回転
	


	////////////////////////////////////////////////////////////////////////////
	//					インデックスバッファの生成				              //
	///////////////////////////////////////////////////////////////////////////


	//インデックスバッファの生成
	if(FAILED(pDevice->CreateIndexBuffer
			 (sizeof(WORD)*g_nNumVertexIndex,
			 D3DUSAGE_WRITEONLY,
			 D3DFMT_INDEX16,
			 D3DPOOL_MANAGED,
			 &g_pD3DIndexBuffField,
			 NULL)))
	{
		return E_FAIL;
	}

	//インデックスのポインタ
	WORD *pIndex;

	g_pD3DIndexBuffField->Lock(0,0,(void**)&pIndex,0);
	
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

	
	//インデックス設定ループ
	
	/*for(int z=0;z<g_nNumBlockZ;z++)
	{
		for(int x=0;x<g_nNumBlockX+1;x++)
		{
			//下側
			pIndex[0+(z*8)+(x*2)]=(g_nNumVertexIndex/g_nNumBlockX+1)+z*(g_nNumVertexIndex/g_nNumBlockX+1)+x;

			//上側
			pIndex[1+(z*8)+(x*2)]=z*(g_nNumVertexIndex/g_nNumBlockX+1)+x;
		

			//最初の行以外でかつ最初の列
			if(z!=0 && x==0)
			{
				pIndex[-1+(z*8)+(x*2)]=pIndex[0+(z*8)+(x*2)];
			}

			//最後の行まで
			if(z<g_nNumBlockZ-1 && x==g_nNumBlockX)
			{
				pIndex[2+(z*8)+(x*2)]=pIndex[1+(z*8)+(x*2)];
			}
		}

	}*/
	
	
	g_pD3DIndexBuffField->Unlock();



	////////////////////////////////////////////////////////////////////////////
	//							頂点バッファの生成							 //
	////////////////////////////////////////////////////////////////////////////

	if(FAILED(pDevice->CreateVertexBuffer
			 (sizeof(VERTEX_3D)*g_nNumVertex,
			 D3DUSAGE_WRITEONLY,
			 FVF_VERTEX_3D,
			 D3DPOOL_MANAGED,
			 &g_pD3DVtxBuffField,
			 NULL)))
	{
		return E_FAIL;
	}

	
	VERTEX_3D *pVtx;//3Dポリゴン用頂点

	g_pD3DVtxBuffField->Lock(0,0,(void**)&pVtx,0);

	//頂点情報の設定(x:-1000～1000,y:0,z:-1000～1000)

	///////////////////////
	//	頂点設定ループ	//
	/////////////////////

	//2x2マス
	//行
	for(int z=0;z<g_nNumBlockZ;z++)
	{
		//列
		for(int x=0;x<g_nNumBlockX+1;x++)
		{

			pVtx[0+(z*8)+(x*2)].vtx=D3DXVECTOR3(-g_fSizeBlockX+(x*g_fSizeBlockX),
												0.0f,
												0.0f-(z*g_fSizeBlockZ)
												);

			pVtx[1+(z*8)+(x*2)].vtx=D3DXVECTOR3(-g_fSizeBlockX+(x*g_fSizeBlockX),
												0.0f,
												g_fSizeBlockZ-(z*g_fSizeBlockZ)
												);

			//最初の行以外でかつ最初の列
			if(z!=0 && x==0)
			{
				pVtx[-1+(z*8)+(x*2)].vtx=pVtx[0+(z*8)+(x*2)].vtx;
			}

			//最後の行まで
			if(z<g_nNumBlockZ-1)
			{
				pVtx[2+(z*8)+(x*2)].vtx=pVtx[1+(z*8)+(x*2)].vtx;
			}
		}
	}

	///////////////////////////////////////////
	//	法線ベクトル＆反射光　設定ループ	//
	/////////////////////////////////////////
	for(int i=0;i<g_nNumVertex;i++)
	{
		pVtx[i].nor=D3DXVECTOR3(0.0f,1.0f,0.0f);
		pVtx[i].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	}

	///////////////////////////////
	//	テクスチャ設定ループ	//
	/////////////////////////////
	//行
	for(int z=0;z<g_nNumBlockZ;z++)
	{
		//列
		for(int x=0;x<g_nNumBlockX+1;x++)
		{
			pVtx[0+(z*8)+(x*2)].tex=D3DXVECTOR2(0.0f+(x*1.0f),1.0f);
			pVtx[1+(z*8)+(x*2)].tex=D3DXVECTOR2(0.0f+(x*1.0f),0.0f);
		
			//最初の行以外でかつ最初の列
			if(z!=0 && x==0)
			{
				pVtx[-1+(z*8)+(x*2)].tex=pVtx[0+(z*8)+(x*2)].tex;
			}

			//最後の行まで
			if(z<g_nNumBlockZ-1)
			{
				pVtx[2+(z*8)+(x*2)].tex=pVtx[1+(z*8)+(x*2)].tex;
			}

		}
	}

	g_pD3DVtxBuffField->Unlock();


	//テクスチャの読み込み

	D3DXCreateTextureFromFile(pDevice,
							  "data/TEXTURE/ground001.jpg",
							  &g_pD3DTextureField);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
	//テクスチャの終了
	if(g_pD3DTextureField!=NULL)
	{
		g_pD3DTextureField->Release();
		g_pD3DTextureField=NULL;
	}
	//頂点バッファの終了
	if(g_pD3DVtxBuffField!=NULL)
	{
		g_pD3DVtxBuffField->Release();
		g_pD3DVtxBuffField=NULL;
	}

	//インデックスバッファの終了
/*	if(g_pD3DIndexBuffField!=NULL)
	{
		g_pD3DIndexBuffField->Release();
		g_pD3DIndexBuffField=NULL;
	}*/
	
}
//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{
}

//=============================================================================
// 描画初理
//=============================================================================
void DrawMeshField(void)
{
	//デバイスを取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	for(int i=0;i<9;i++)
	{
		for(int j=0;j<9;j++)
		{
			//マトリックス変数
			D3DXMATRIX mtxScl,mtxRot,mtxTranslate;

			//アイデンティティ
			D3DXMatrixIdentity(&g_mtxWorldField[i][j]);

			//大きさを設定
			D3DXMatrixScaling(&mtxScl,1.0f,1.0f,1.0f);

			//大きさを設定
			D3DXMatrixMultiply(&g_mtxWorldField[i][j],
							   &g_mtxWorldField[i][j],
							   &mtxScl);

			//回転を設定
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
											g_rotField.y,
											g_rotField.x,
											g_rotField.z);

			//回転を反映
			D3DXMatrixMultiply(&g_mtxWorldField[i][j],&g_mtxWorldField[i][j],
							   &mtxRot);


			//位置を反映
			D3DXMatrixTranslation(&mtxTranslate,
								  g_posField.x+(j*1000.0f),
								  g_posField.y,
								  g_posField.z+(i*1000.0f));

			//位置を設定
			D3DXMatrixMultiply(&g_mtxWorldField[i][j],&g_mtxWorldField[i][j],
						 &mtxTranslate);

			//位置をセット
			pDevice->SetTransform(D3DTS_WORLD,
								  &g_mtxWorldField[i][j]);

			//３Ｄポリゴンの描画
			pDevice->SetStreamSource(0,g_pD3DVtxBuffField, 0, sizeof(VERTEX_3D));

			//インデックスをバインド
			pDevice->SetIndices(g_pD3DIndexBuffField);

			//頂点フォーマットのセット
			pDevice->SetFVF(FVF_VERTEX_3D);

			//テクスチャの設定
			pDevice->SetTexture(0,g_pD3DTextureField);

			//ポリゴンの描画(頂点)
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
											0,				//開始するポリゴン頂点
											g_nNumPolygon	//ポリゴン数
											);
									
			//ポリゴンの描画(インデックス)
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
										  0,
										  0,
										  g_nNumVertex,
										  0,
										  g_nNumPolygon);
		}//for j end
	}//for i end
}
//EOF