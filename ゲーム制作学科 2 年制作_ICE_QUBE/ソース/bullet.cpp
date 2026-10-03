//=============================================================================
//弾処理[bullet.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "bullet.h"
//*****************************************************************************
//定数定義
//*****************************************************************************
#define CHARGE_MAX (100)
//*****************************************************************************
//プロトタイプ宣言(内部)
//*****************************************************************************
void ChangeBulBuff (int num);
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9		g_pD3DTextureBullet = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffBullet = NULL;	// 頂点バッファインターフェースへのポインタ
static BULLET g_bullet[BULLET_MAX];
static bool g_first=false;
//=============================================================================
//弾の初期化
//=============================================================================
HRESULT InitBullet (void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	///////////////////////////////////
	//	オブジェクト情報の初期化	//
	/////////////////////////////////

	//弾の初期化
	for(int i=0;i<BULLET_MAX;i++)
	{
		//弾構造体内変数初期化
		g_bullet[i].fSize=80.0f;												//半分のサイズ
		g_bullet[i].pos=D3DXVECTOR3(0.0f,g_bullet[i].fSize+10.0f,0.0f);	//座標
		g_bullet[i].rot=D3DXVECTOR3(0.0f,0.0f,0.0f);							//角度
		g_bullet[i].scl=D3DXVECTOR3(1.0f,1.0f,1.0f);							//サイズ
		g_bullet[i].fMoveX=50.5f;												//移動量
		g_bullet[i].fMoveZ=50.5f;												//移動量
		g_bullet[i].bUse=false;													//使用フラグ
		g_bullet[i].bCurve=false;
	
	}

	if(g_first==false)
	{
		//頂点バッファの生成
		if(FAILED(pDevice->CreateVertexBuffer
				 (sizeof(VERTEX_3D)*4,
				 D3DUSAGE_WRITEONLY,
				 FVF_VERTEX_3D,
				 D3DPOOL_MANAGED,
				 &g_pD3DVtxBuffBullet,
				 NULL)))
		{
			return E_FAIL;
		}
	

		//テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
									"data/TEXTURE/bullet000.png",
									&g_pD3DTextureBullet);

		g_first=true;
	}


	return S_OK;
}
//=============================================================================
//弾の更新
//=============================================================================
void UpdateBullet (void)
{
	//弾の個数分ループ
	for(int i=0;i<BULLET_MAX;i++)
	{
		//弾のスイッチONなら
		if(g_bullet[i].bUse==true)
		{
			//ベジエカーブ実装
			if(g_bullet[i].bCurve)
			{
				BezierCurve(i,&g_bullet[i].pos,&g_bullet[i].bCurve);
			}
			else
			{
				//弾の直進移動
				g_bullet[i].pos.x+=cosf(g_bullet[i].rot.y+D3DX_PI/2)*g_bullet[i].fMoveX;
				g_bullet[i].pos.z-=sinf(g_bullet[i].rot.y+D3DX_PI/2)*g_bullet[i].fMoveZ;
			}
		}
	}

	///////////////
	//	壁制御	//
	/////////////

	//弾の個数分ループ
	for(int i=0;i<BULLET_MAX;i++)
	{
		//弾が壁に当ったら
		if((g_bullet[i].pos.x+g_bullet[i].fSize>WALL_RIGHT_POS ||
		   g_bullet[i].pos.x-g_bullet[i].fSize<WALL_LEFT_POS ||
		   g_bullet[i].pos.z+g_bullet[i].fSize>WALL_AHEAD_POS ||
		   g_bullet[i].pos.z-g_bullet[i].fSize<WALL_BACK_POS )&&
		   g_bullet[i].bUse==true)
		{
			CreateBulExp(g_bullet[i].pos.x,g_bullet[i].pos.y,g_bullet[i].pos.z,g_bullet[i].fSize+30.0f);
			OutBullet(i);
			//AddScore(5);
		}
	}

	//エフェクトに座標をセット
	for(int i=0;i<BULLET_MAX;i++)
	{
		//エフェクトのセット
		SetEffect(g_bullet[i].pos,i,g_bullet[i].fMoveX,g_bullet[i].fMoveZ,g_bullet[i].rot.y);
	}
}
//=============================================================================
//弾の描画
//=============================================================================
void DrawBullet (void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//弾の個数分ループ
	for(int i=0;i<BULLET_MAX;i++)
	{
		//頂点バッファの変更
		ChangeBulBuff(i);

		//各情報のマトリックス変数
		D3DXMATRIX mtxScl,mtxRot,mtxTranslate,mtxView;

		//カメラ情報取得
		mtxView=GetMtxView();

		//アイデンティティ
		D3DXMatrixIdentity(&g_bullet[i].mtxWorld);
		//ビュー設定
		D3DXMatrixInverse(&g_bullet[i].mtxWorld,NULL,&mtxView);

		g_bullet[i].mtxWorld._41=0.0f;
		g_bullet[i].mtxWorld._42=0.0f;
		g_bullet[i].mtxWorld._43=0.0f;

		//大きさを設定
		D3DXMatrixScaling(&mtxScl,g_bullet[i].scl.x,
								  g_bullet[i].scl.y,
								  g_bullet[i].scl.z);
		//大きさを反映
		D3DXMatrixMultiply(&g_bullet[i].mtxWorld,
						   &g_bullet[i].mtxWorld,
						   &mtxScl);

		//回転を設定
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
										0.0f,
										0.0f,
										0.0f);

		//回転を反映
		D3DXMatrixMultiply(&g_bullet[i].mtxWorld,&g_bullet[i].mtxWorld,
						   &mtxRot);


		//位置を設定
		D3DXMatrixTranslation(&mtxTranslate,
							  g_bullet[i].pos.x,
							  g_bullet[i].pos.y,
							  g_bullet[i].pos.z);

		//位置を反映
		D3DXMatrixMultiply(&g_bullet[i].mtxWorld,&g_bullet[i].mtxWorld,
					 &mtxTranslate);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD,
							  &g_bullet[i].mtxWorld);


		//３Ｄポリゴンの描画
		pDevice->SetStreamSource(0,g_pD3DVtxBuffBullet, 0, sizeof(VERTEX_3D));
		
		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0,g_pD3DTextureBullet);

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
										0,//ポリゴンの数
										2);
	}
}
//=============================================================================
//弾の頂点バッファ変更
//=============================================================================
void ChangeBulBuff (int num)
{
	//バッファの更新

	VERTEX_3D *pVtx;

	g_pD3DVtxBuffBullet->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の設定
	pVtx[0].vtx=D3DXVECTOR3(-g_bullet[num].fSize,-g_bullet[num].fSize,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(-g_bullet[num].fSize,g_bullet[num].fSize,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(g_bullet[num].fSize,-g_bullet[num].fSize,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(g_bullet[num].fSize,g_bullet[num].fSize,0.0f);

	//法線ベクトル
	pVtx[0].nor=D3DXVECTOR3(-1.0f,1.0f,-1.0f);
	pVtx[1].nor=D3DXVECTOR3(-1.0f,1.0f,-1.0f);
	pVtx[2].nor=D3DXVECTOR3(-1.0f,1.0f,-1.0f);
	pVtx[3].nor=D3DXVECTOR3(-1.0f,1.0f,-1.0f);

	//ポリゴンの色設定
	pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

	//テクスチャ座標
	pVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	pVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	pVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	g_pD3DVtxBuffBullet->Unlock();
}
//=============================================================================
//弾の終了
//=============================================================================
void UninitBullet (void)
{
	//テクスチャへのポインタ終了
	if(g_pD3DTextureBullet!=NULL)
	{
		g_pD3DTextureBullet->Release();
		g_pD3DTextureBullet=NULL;
	}

	//頂点バッファへのポインタ終了
	if(g_pD3DVtxBuffBullet!=NULL)
	{
		g_pD3DVtxBuffBullet->Release();
		g_pD3DVtxBuffBullet=NULL;
	}
}
//=============================================================================
//弾のチャージ
//=============================================================================
void ChargeBullet (void)
{
	for(int i=0;i<BULLET_MAX;i++)
	{
		//弾がOFFならチャージする
		if(g_bullet[i].bUse!=true)
		{
			//チャージアップ
			g_bullet[i].nCharge++;
		
			//チャージ数によってサイズを変える
			if(g_bullet[i].nCharge>CHARGE_MAX)
			{
				g_bullet[i].fSize=250.5f;
			}
			else if(g_bullet[i].nCharge>50)
			{
				g_bullet[i].fSize=150.0f;
			}
			else
			{
				g_bullet[i].fSize=80.0f;
			}

			break;
		}
	}
}
//=============================================================================
//弾座標設置
//=============================================================================
void SetPosBullet (float fX,float fZ)
{
	//弾の個数分ループ
	for(int i=0;i<BULLET_MAX;i++)
	{
		//弾スイッチがOFFなら
		if(!g_bullet[i].bUse)
		{
			///////////////////
			//	弾発射準備	//
			//////////////////

			//モデルから角度を取得
			g_bullet[i].rot=GetRotModel();

			//モデルから座標を取得
			g_bullet[i].pos.x=fX+cosf(g_bullet[i].rot.y+D3DX_PI/2)*300.0f;	//X座標
			g_bullet[i].pos.y=g_bullet[i].fSize+10.0f;						//Y座標
			g_bullet[i].pos.z=fZ-sinf(g_bullet[i].rot.y+D3DX_PI/2)*300.0f;	//Z座標

			//ベジエ出発点のセット
			SetBezier(i,g_bullet[i].pos,g_bullet[i].rot,800.0f,800.0f);

			g_bullet[i].bUse=true;		//弾スイッチON
			g_bullet[i].bCurve=true;	//ベジエカーブON

			//連続を防ぐためループを抜ける
			break;
		}
	}

}
//=============================================================================
//弾座標画面外
//=============================================================================
void OutBullet (int i)
{
	g_bullet[i].pos.x=1000.0f;
	g_bullet[i].bUse=false;
	g_bullet[i].fSize=80.0f;
	g_bullet[i].nCharge=0;
}
//=============================================================================
//弾構造体取得
//=============================================================================
BULLET GetBullet (int num)
{
	return g_bullet[num];
}
//EOF