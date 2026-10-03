//=============================================================================
//立方体の処理[qube.cpp]
//Author:HUMITO KIMURA
//TODO:頂点バッファのポインタの配列をなくす。描画前に頂点バッファポインタ更新関数を呼ぶ
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "qube.h"
//*****************************************************************************
//定数定義(内部)
//*****************************************************************************
#define QUBE_CLEAR (200)			//立方体の透明な状態
#define QUBE_HIT (1)				//立方体が当たった時の色
#define COLOR_MAX (255)				//色の最大数
static const int g_nQubeVertex=4;	//頂点数
//*****************************************************************************
//プロトタイプ宣言(内部)
//*****************************************************************************
void ReleaseQube (int num);		//立方体解放
void ChangeQubeBuff (int num);	//頂点バッファ変更
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DTextureQube;					// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffQube;				// 頂点バッファへのポインタ
static D3DXMATRIX g_mtxWorld[6];						// ワールドマトリックス
static QUBE			  *g_pQube[QUBE_MAX];				//立方体構造体
static float		g_fRotY=0.0f;						//移動用の角度
static bool g_first=false;
//=============================================================================
//立方体初期化
//=============================================================================
HRESULT InitQube(float posX,float posY,float posZ,float Width,float Height)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//弾の個数分ループ
	for(int i=0;i<QUBE_MAX;i++)
	{
		//NULLチェック
		if(g_pQube[i]!=NULL)
		{
			//解放
			ReleaseQube(i);
		}
	}

	////////////////////////////////////////////////////////////////////////////
	//							頂点バッファの生成							 //
	//////////////////////////////////////////////////////////////////////////

	if(g_first==false)
	{
		if(FAILED(pDevice->CreateVertexBuffer
					(sizeof(VERTEX_3D)*g_nQubeVertex,
					D3DUSAGE_WRITEONLY,
					FVF_VERTEX_3D,
					D3DPOOL_MANAGED,
					&g_pD3DVtxBuffQube,
					NULL)))
		{
			return E_FAIL;
		}

		//テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
								  "data/TEXTURE/qube000.jpg",
								  &g_pD3DTextureQube);

		g_first=true;
	}
	return S_OK;
}
//=============================================================================
//立方体終了
//=============================================================================
void UninitQube(void)
{
	//立方体の個数分ループ
	for(int i=0;i<QUBE_MAX;i++)
	{
		//NULLチェック
		if(g_pQube[i]!=NULL)
		{
			//解放
			ReleaseQube(i);
		}
	}

	//テクスチャへのポインタ終了
	if(g_pD3DTextureQube!=NULL)
	{
		g_pD3DTextureQube->Release();
		g_pD3DTextureQube=NULL;
	}

	//頂点バッファへのポインタ終了
	if(g_pD3DVtxBuffQube!=NULL)
	{
		g_pD3DVtxBuffQube->Release();
		g_pD3DVtxBuffQube=NULL;
	}
}
//=============================================================================
//立方体更新
//=============================================================================
void UpdateQube(void)
{
	//立方体の数分ループ
	for(int num=0;num<QUBE_MAX;num++)
	{

		if(g_pQube[num]!=NULL)
		{
			///////////////////////////////////
			//	立方体オブジェクトの更新	//
			/////////////////////////////////

			//移動フラグtrueの場合
			if(g_pQube[num]->bMove==true)
			{
				//立方体の座標を移動量分加算
				g_pQube[num]->pos.x+=sinf(g_fRotY+D3DX_PI/2)*g_pQube[num]->posMove.x;
				g_pQube[num]->pos.z+=sinf(g_fRotY+D3DX_PI/2)*g_pQube[num]->posMove.z;

				//立方体のY軸角度を移動量分加算
				g_pQube[num]->rot.y+=g_pQube[num]->posMove.y;
		
				//慣性の法則により各移動量を減算する
				g_pQube[num]->posMove.x=g_pQube[num]->posMove.x-(g_pQube[num]->posMove.x/10);
				g_pQube[num]->posMove.y=g_pQube[num]->posMove.y-(g_pQube[num]->posMove.y/10);
				g_pQube[num]->posMove.z=g_pQube[num]->posMove.z-(g_pQube[num]->posMove.z/10);

				///////////////////
				//	壁判定処理	//
				/////////////////

				//右壁
				if(g_pQube[num]->pos.x+g_pQube[num]->fHalfWidth>WALL_RIGHT_POS)
				{
					g_pQube[num]->pos.x=WALL_RIGHT_POS-g_pQube[num]->fHalfWidth;
					g_pQube[num]->posMove.x=-g_pQube[num]->posMove.x;
				}

				//左壁
				if(g_pQube[num]->pos.x-g_pQube[num]->fHalfWidth<WALL_LEFT_POS)
				{
					g_pQube[num]->pos.x=WALL_LEFT_POS+g_pQube[num]->fHalfWidth;
					g_pQube[num]->posMove.x=-g_pQube[num]->posMove.x;
				}

				//前壁
				if(g_pQube[num]->pos.z+g_pQube[num]->fHalfWidth>WALL_AHEAD_POS)
				{
					g_pQube[num]->pos.z=WALL_AHEAD_POS-g_pQube[num]->fHalfWidth;
					g_pQube[num]->posMove.z=-g_pQube[num]->posMove.z;
				}

				//後壁
				if(g_pQube[num]->pos.z-g_pQube[num]->fHalfWidth<WALL_BACK_POS)
				{
					g_pQube[num]->pos.z=WALL_BACK_POS+g_pQube[num]->fHalfWidth;
					g_pQube[num]->posMove.z=-g_pQube[num]->posMove.z;
				}
			}
		}
	}
}
//=============================================================================
//立方体描画
//=============================================================================
void DrawQube(void)
{
	//デバイスを取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	int rotz=0;//上への回転(Z軸)

	//立方体の個数分ループ
	for(int i=0;i<QUBE_MAX;i++)
	{
		if(g_pQube[i]!=NULL)
		{
			//頂点バッファの変更
			ChangeQubeBuff(i);

			//６面分ループ
			for(int j=0;j<6;j++)
			{
				//マトリックス変数
				D3DXMATRIX mtxScl,mtxRot,mtxTranslate;

				//アイデンティティ
				D3DXMatrixIdentity(&g_mtxWorld[j]);

				//大きさを設定
				D3DXMatrixScaling(&mtxScl,1.0f,1.0f,1.0f);

				//大きさを反映
				D3DXMatrixMultiply(&g_mtxWorld[j],
									&g_mtxWorld[j],
									&mtxScl);
				//回転が4面までなら
				if(j<4)
				{
					//上への回転はなし
					rotz=0;
				}
				else
				{
					//5～6面からは上と下の回転をする
					rotz=-1+(2*(j%4));
				}

				//回転を設定
				D3DXMatrixRotationYawPitchRoll(&mtxRot,
												g_pQube[i]->rot.y+(D3DX_PI/2)*j,
												g_pQube[i]->rot.x+(D3DX_PI/2)*rotz,
												g_pQube[i]->rot.z);

				//回転を反映
				D3DXMatrixMultiply(&g_mtxWorld[j],&g_mtxWorld[j],
									&mtxRot);


				//位置を設定
				D3DXMatrixTranslation(&mtxTranslate,
										g_pQube[i]->pos.x,
										g_pQube[i]->pos.y,
										g_pQube[i]->pos.z);

				//位置を反映
				D3DXMatrixMultiply(&g_mtxWorld[j],&g_mtxWorld[j],
								&mtxTranslate);

				//ワールドマトリックスの設定
				pDevice->SetTransform(D3DTS_WORLD,
										&g_mtxWorld[j]);

				//３Ｄポリゴンの描画
				pDevice->SetStreamSource(0,g_pD3DVtxBuffQube, 0, sizeof(VERTEX_3D));

				//頂点フォーマットのセット
				pDevice->SetFVF(FVF_VERTEX_3D);

				//テクスチャの設定
				pDevice->SetTexture(0,g_pD3DTextureQube);

				//ポリゴンの描画(頂点)
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
												0,				//開始するポリゴン頂点
												2	//ポリゴン数
												);
			}
		}					
	}
}
//=============================================================================
//立方体頂点バッファ変更
//=============================================================================
void ChangeQubeBuff (int num)
{
	//3Dポリゴン用頂点
	VERTEX_3D *pVtx;

	//頂点バッファの変更
	g_pD3DVtxBuffQube->Lock(0,0,(void**)&pVtx,0);

	///////////////////////
	//	頂点設定ループ	//
	/////////////////////

	//立方体前面
	pVtx[0].vtx=D3DXVECTOR3(-g_pQube[num]->fHalfWidth,-g_pQube[num]->fHalfHeight,-g_pQube[num]->fHalfWidth);
	pVtx[1].vtx=D3DXVECTOR3(-g_pQube[num]->fHalfWidth,g_pQube[num]->fHalfHeight,-g_pQube[num]->fHalfWidth);
	pVtx[2].vtx=D3DXVECTOR3(g_pQube[num]->fHalfWidth,-g_pQube[num]->fHalfHeight,-g_pQube[num]->fHalfWidth);
	pVtx[3].vtx=D3DXVECTOR3(g_pQube[num]->fHalfWidth,g_pQube[num]->fHalfHeight,-g_pQube[num]->fHalfWidth);
		
	for(int i=0;i<4;i++)
	{
		//法線ベクトル
		pVtx[i].nor=D3DXVECTOR3(-1.0f,1.0f,-1.0f);

		//ポリゴンの色とα値
		pVtx[i].diffuse=D3DCOLOR_RGBA(g_pQube[num]->nColor,	
										g_pQube[num]->nColor,
										g_pQube[num]->nColor,
										g_pQube[num]->nAlpha
										);
	}

	//テクスチャ設定
	pVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	pVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	pVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	g_pD3DVtxBuffQube->Unlock();
}
//=============================================================================
//立方体構造体取得
//=============================================================================
void GetQube (int num,QUBE *data)
{
	if(g_pQube[num]!=NULL)
	{
		 *data=*g_pQube[num];
	}
	else
	{
		data->pos.x=NULL;
		data->pos.z=NULL;
		data->fHalfWidth=NULL;
		data->fHalfHeight=NULL;
		data->bMove=NULL;
	}
}
//=============================================================================
//立方体の生成
//=============================================================================
void CreateQube (float posX,float posZ,float Width,float Height)
{
	for(int num=0;num<QUBE_MAX;num++)
	{
		if(g_pQube[num]==NULL)
		{
			//動的確保
			g_pQube[num]=new QUBE;

			g_pQube[num]->bSet=true;
			g_pQube[num]->fHalfWidth=(Width/2.0f);									//半分の幅
			g_pQube[num]->fHalfHeight=(Height/2.0f);								//半分の高さ
			g_pQube[num]->pos=D3DXVECTOR3(posX,										//座標
										g_pQube[num]->fHalfHeight,
										posZ
										);			
			g_pQube[num]->nAlpha=200;												//α値
			g_pQube[num]->rot=D3DXVECTOR3(0.0f,0.0f,0.0f);							//回転
			g_pQube[num]->posMove=D3DXVECTOR3(0.0f,0.0f,0.0f);						//移動量
			g_pQube[num]->nColor=COLOR_MAX;											//全体の色
			g_pQube[num]->bMove=false;												//移動フラグ
			
			break;
		}
	}
}
//=============================================================================
//立方体の移動量加算
//=============================================================================
void AddMove (int num,float fMoveX,float fMoveZ,float fRotY)
{
	if(g_pQube[num]!=NULL)
	{
		//移動フラグtrue
		g_pQube[num]->bMove=true;

		//移動量加算
		g_pQube[num]->posMove.x+=fMoveX;
		g_pQube[num]->posMove.z+=fMoveZ;
		g_pQube[num]->posMove.y+=fRotY;
	}
}
//=============================================================================
//立方体の解放
//=============================================================================
void ReleaseQube (int num)
{
	if(g_pQube[num]!=NULL)
	{
		delete g_pQube[num];
		g_pQube[num]=NULL;
	}
}
//EOF