//=============================================================================
//敵処理[enemyB.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルード
//*****************************************************************************
#include "enemyB.h"
//*****************************************************************************
//プロトタイプ宣言(内部)
//*****************************************************************************
void SortEnemy(void);
void ChangeEneBBuff (int num);
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9		g_pD3DTextureEneB = NULL;				//テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffEneB=NULL;	//頂点バッファへのポインタ
static ENEMYB *g_enemy=NULL;									//エネミー構造体
static bool g_first=false;
//=============================================================================
//敵の初期化
//=============================================================================
HRESULT InitEnemyB (void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//NULLチェック
	if(g_enemy==NULL)
	{
		//動的確保
		g_enemy=new ENEMYB[ENEMYB_MAX];
	}

	///////////////////////////////////
	//	オブジェクト情報の初期化	//
	/////////////////////////////////

	//敵の個数分ループ
	for(int i=0;i<ENEMYB_MAX;i++)
	{
		g_enemy[i].fSize=130.0f;																//サイズ
		g_enemy[i].pos=D3DXVECTOR3((rand()%(int)(WALL_RIGHT_POS-WALL_LEFT_POS))+WALL_LEFT_POS,	//座標
								   g_enemy[i].fSize,
								   (rand()%(int)(WALL_AHEAD_POS-WALL_BACK_POS))+WALL_BACK_POS);

		g_enemy[i].posMove=D3DXVECTOR3(10.3f,0.0f,10.3f);										//移動量
		g_enemy[i].rot=D3DXVECTOR3(0.0f,0.0f,0.0f);												//角度
		g_enemy[i].scl=D3DXVECTOR3(1.0f,1.0f,1.0f);												//サイズ
		g_enemy[i].nCnt=0;																		//カウント
		g_enemy[i].bHit=false;																	//ヒットフラグ
		g_enemy[i].type=TYPE_ENEMY_NORMAL;														//状態
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
					&g_pD3DVtxBuffEneB,
					NULL)))
		{
			return E_FAIL;
		}

		//テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
									"data/TEXTURE/FireEnemy.png",
									&g_pD3DTextureEneB);
		g_first=true;
	}

	return S_OK;
}
//=============================================================================
//敵の更新
//=============================================================================
void UpdateEnemyB (void)
{
	//乱数使用
	srand((unsigned int)time(NULL));

	///////////////////////////////
	//	敵オブジェクトの更新	//
	/////////////////////////////

	//敵の個数分ループ
	for(int i=0;i<ENEMYB_MAX;i++)
	{
		///////////////////////////////////////
		//	敵の状態によって処理を分ける	//
		/////////////////////////////////////

		switch(g_enemy[i].type)
		{
			///////////////////////
			//	通常時の処理	//
			/////////////////////
			case TYPE_ENEMY_NORMAL:

				//カウントアップ
				g_enemy[i].nCnt++;

				//敵の移動
				g_enemy[i].pos.x+=sinf(g_enemy[i].rot.y+D3DX_PI/2)*g_enemy[i].posMove.x;//X座標	
			break;

			///////////////////////
			//	氷状態の処理	//
			//////////////////////
			case TYPE_ENEMY_ICE:
				//カウントアップ
				g_enemy[i].nCnt++;

				///////////////////////////////////
				//	カウント一定以上なら復帰	//
				/////////////////////////////////
				if(g_enemy[i].nCnt%30==0)
				{
					//通常タイプに変更
					g_enemy[i].type=TYPE_ENEMY_NORMAL;

					//座標を乱数によって変更する
					g_enemy[i].pos.x=(rand()%(int)(WALL_RIGHT_POS-WALL_LEFT_POS))+WALL_LEFT_POS;	//X座標
					g_enemy[i].pos.z=(rand()%(int)(WALL_AHEAD_POS-WALL_BACK_POS))+WALL_BACK_POS;	//Z座標					
				}
			break;

			///////////////////////
			//	爆発時の処理	//
			/////////////////////
			case TYPE_ENEMY_EXP:

				//爆発エフェクトを座標に設置
				//TODO:爆発エフェクトの作成

				//氷状態に変更
				g_enemy[i].type=TYPE_ENEMY_ICE;
				g_enemy[i].pos.x=0.0f;			//座標Xを画面外へ
				g_enemy[i].pos.z=0.0f;			//座標Zを画面外へ
			break;
		}

		///////////////////
		//	壁判定処理	//
		/////////////////

		//左壁
		if(g_enemy[i].pos.x-g_enemy[i].fSize<WALL_LEFT_POS)
		{
			//座標補正
			g_enemy[i].pos.x=WALL_LEFT_POS+g_enemy[i].fSize;
			//移動量反転
			g_enemy[i].posMove.x=-g_enemy[i].posMove.x;
		}

		//右壁
		if(g_enemy[i].pos.x+g_enemy[i].fSize>WALL_RIGHT_POS)
		{
			//座標補正
			g_enemy[i].pos.x=WALL_RIGHT_POS-g_enemy[i].fSize;
			//移動量反転
			g_enemy[i].posMove.x=-g_enemy[i].posMove.x;
		}

	}
}
//=============================================================================
//敵の描画
//=============================================================================
void DrawEnemyB (void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//アルファテストを行い透明色をなくす
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);		// αブレンドを行う
	pDevice->SetRenderState(D3DRS_ALPHAREF,0);					// α値変更
	pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);	// 任意の値より大きい値

	//Zバッファによる敵の並び替え
	SortEnemy();

	//弾の個数分ループ
	for(int i=0;i<ENEMYB_MAX;i++)
	{
		//頂点バッファ変更
		ChangeEneBBuff(i);

		//各情報のマトリックス変数
		D3DXMATRIX mtxScl,mtxRot,mtxTranslate,mtxView;

		//カメラ情報取得
		mtxView=GetMtxView();

		//アイデンティティ
		D3DXMatrixIdentity(&g_enemy[i].mtxWorld);
		//ビュー設定
		D3DXMatrixInverse(&g_enemy[i].mtxWorld,NULL,&mtxView);

		g_enemy[i].mtxWorld._41=0.0f;
		g_enemy[i].mtxWorld._42=0.0f;
		g_enemy[i].mtxWorld._43=0.0f;

		//大きさを設定
		D3DXMatrixScaling(&mtxScl,g_enemy[i].scl.x,
								  g_enemy[i].scl.y,
								  g_enemy[i].scl.z);
		//大きさを反映
		D3DXMatrixMultiply(&g_enemy[i].mtxWorld,
						   &g_enemy[i].mtxWorld,
						   &mtxScl);

		//回転を設定
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
										0.0f,
										0.0f,
										0.0f);

		//回転を反映
		D3DXMatrixMultiply(&g_enemy[i].mtxWorld,&g_enemy[i].mtxWorld,
						   &mtxRot);


		//位置を設定
		D3DXMatrixTranslation(&mtxTranslate,
							  g_enemy[i].pos.x,
							  g_enemy[i].pos.y,
							  g_enemy[i].pos.z);

		//位置を反映
		D3DXMatrixMultiply(&g_enemy[i].mtxWorld,&g_enemy[i].mtxWorld,
					 &mtxTranslate);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD,
							  &g_enemy[i].mtxWorld);


		//３Ｄポリゴンの描画
		pDevice->SetStreamSource(0,g_pD3DVtxBuffEneB, 0, sizeof(VERTEX_3D));
		
		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0,g_pD3DTextureEneB);

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
//敵頂点バッファ変更
//=============================================================================
void ChangeEneBBuff (int num)
{
	///////////////////////////
	//	頂点バッファの変更	//
	/////////////////////////
	VERTEX_3D *pVtx;

	g_pD3DVtxBuffEneB->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の設定
	pVtx[0].vtx=D3DXVECTOR3(-g_enemy[num].fSize,-g_enemy[num].fSize,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(-g_enemy[num].fSize,g_enemy[num].fSize,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(g_enemy[num].fSize,-g_enemy[num].fSize,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(g_enemy[num].fSize,g_enemy[num].fSize,0.0f);

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
	pVtx[0].tex=D3DXVECTOR2(0.0f+(0.5f*(g_enemy[num].nCnt/9%2)),1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f+(0.5f*(g_enemy[num].nCnt/9%2)),0.0f);
	pVtx[2].tex=D3DXVECTOR2(0.5f+(0.5f*(g_enemy[num].nCnt/9%2)),1.0f);
	pVtx[3].tex=D3DXVECTOR2(0.5f+(0.5f*(g_enemy[num].nCnt/9%2)),0.0f);

	g_pD3DVtxBuffEneB->Unlock();
}
//=============================================================================
//敵の終了
//=============================================================================
void UninitEnemyB (void)
{
	if(g_enemy!=NULL)
	{
		delete []g_enemy;
		g_enemy=NULL;
	}

	//テクスチャへのポインタ終了
	if(g_pD3DTextureEneB!=NULL)
	{
		g_pD3DTextureEneB->Release();
		g_pD3DTextureEneB=NULL;
	}

	//頂点バッファへのポインタ終了
	if(g_pD3DVtxBuffEneB!=NULL)
	{
		g_pD3DVtxBuffEneB->Release();
		g_pD3DVtxBuffEneB=NULL;
	}

}
//=============================================================================
//敵情報の取得
//=============================================================================
ENEMYB GetEnemyB (int num)
{
	return g_enemy[num];
}
//=============================================================================
//敵のヒット状態変更
//=============================================================================
void HitEnemyB (int num,float fMoveX,float fMoveZ,int type)
{
	//エネミーのカウントリセット
	g_enemy[num].nCnt=0;

	///////////////////////////////////////////
	//	プレイヤーの攻撃種類によって分ける	//
	/////////////////////////////////////////
	switch(type)
	{
		///////////////////////////////////
		//	プレイヤーの弾によるヒット	//
		/////////////////////////////////
		case TYPE_SHOT:

			g_enemy[num].type=TYPE_ENEMY_ICE;	//氷状態に変更
			g_enemy[num].bHit=true;				//ヒットフラグtrue
			g_enemy[num].pos.x=0.0f;			//座標Xを画面外へ
			g_enemy[num].pos.z=0.0f;			//座標Zを画面外へ

		break;

		///////////////////////////////////////
		//	プレイヤーのバットによるヒット	//
		/////////////////////////////////////
		case TYPE_ATTACK:

			//爆発状態にする
			g_enemy[num].type=TYPE_ENEMY_EXP;
		break;

		//何もない場合は特になし
		default:
		break;
	}
}
//=============================================================================
//敵の並び替え
//=============================================================================
void SortEnemy(void)
{
	//並び替え用変数
	ENEMYB work;

	///////////////////////////////////////////////
	//		Z座標が近い順に配列を並び替える		//
	//////////////////////////////////////////////

	//ソート対象A
	for(int i=0;i<ENEMYB_MAX-1;i++)
	{
		//ソート対象B
		for(int j=i+1;j<ENEMYB_MAX;j++)
		{
			//ソート対象AのZ座標がソート対象Bより大きければ入れ替える
			if(g_enemy[i].pos.z>g_enemy[j].pos.z)
			{
				work=g_enemy[i];		//ワークにAを代入
				g_enemy[i]=g_enemy[j];	//AにBを代入
				g_enemy[j]=work;		//Bにワークを代入
			}
		}
	}

}
//EOF