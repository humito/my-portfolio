//=============================================================================
//プレイヤー処理[player.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "player.h"
//*****************************************************************************
//定数定義
//*****************************************************************************
#define PLAYER_SPEED_X (5.5f)//プレイヤーのX移動量
#define PLAYER_SPEED_Z (5.5f)//プレイヤーのZ移動量
#define CUT_SPEED      (5)	 //プレイヤーの移動減算量
#define MODEL_MAX		(2)	//モデルの最大値
//*****************************************************************************
//プロトタイプ宣言(内部)
//*****************************************************************************
void PlayMove (void);	//プレイヤーの描画分け
//*****************************************************************************
//グローバル変数
//*****************************************************************************
LPD3DXMESH          g_pD3DXMeshModel[MODEL_MAX]={NULL,NULL};	//メッシュ情報へのポインタ
LPD3DXBUFFER		g_pD3DXBuffMatModel[MODEL_MAX]={NULL,NULL};	//マテリアル情報へのポインタ
DWORD				g_nNumMatModel[MODEL_MAX]={NULL,NULL};		//マテリアル情報の数
static 	D3DXMATRIX	g_mtxWorld[2];					//ワールドマトリックス
static int			g_model;						//モデルの種類
PLAYER              g_player;						//プレイヤー構造体
static bool g_first=false;
//=============================================================================
//プレイヤーの初期化
//=============================================================================
HRESULT InitPlayer()
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//各ポリゴン情報の設定
	g_player.pos=D3DXVECTOR3(10000.0f,0.0f,10000.0f);		//座標
	g_player.rot=D3DXVECTOR3(0.0f,0.0f,0.0f);		//角度
	g_player.scl=D3DXVECTOR3(1.0f,1.0f,1.0f);		//大きさ
	g_player.fSize=100.0f;							//幅
	g_player.posMove=D3DXVECTOR3(0.0f,0.0f,0.0f);	//移動量
	g_player.nMovCnt=0;								//移動時カウント
	g_player.type=TYPE_NUTRAL;						//プレイヤーのタイプ
	g_model=0;										//描画するモード

	///////////////////////////////////////////////
	//			Xファイル読み込みチェック		//
	/////////////////////////////////////////////
	//プレイヤーモデル4つ読み込む

	if(g_first==false)
	{
			//プレイヤーのニュートラル
		if(FAILED(D3DXLoadMeshFromX("data/MODEL/Player00.x",
									 D3DXMESH_SYSTEMMEM,
									 pDevice,
									 NULL,
									 &g_pD3DXBuffMatModel[0],
									 NULL,
									 &g_nNumMatModel[0],
									 &g_pD3DXMeshModel[0])) ||

			//プレイヤーの物理攻撃
			FAILED(D3DXLoadMeshFromX("data/MODEL/Player03.x",
									 D3DXMESH_SYSTEMMEM,
									 pDevice,
									 NULL,
									 &g_pD3DXBuffMatModel[1],
									 NULL,
									 &g_nNumMatModel[1],
									 &g_pD3DXMeshModel[1])))
		{
			//FAILを返す
			return E_FAIL;
		}
		g_first=true;
	}
	//OKを返す
	return S_OK;
}
//=============================================================================
//プレイヤーの更新
//=============================================================================
void UpdatePlayer()
{
	//旋回移動量
	float fDiffRotY=0.0f;
	float fDiffRotZ=0.0f;
	bool  bWall=false;

	//カメラの向きを取得
	D3DXVECTOR3 rot=GetRotCamera();

	//バックアップ座標
	g_player.posPre.x=g_player.pos.x;
	g_player.posPre.z=g_player.pos.z;

	//モデルの移動
	g_player.pos.x+=g_player.posMove.x;//X座標に移動量加算
	g_player.pos.z+=g_player.posMove.z;//Z座標に移動量加算

	//移動量減算
	g_player.posMove.x=g_player.posMove.x-(g_player.posMove.x/CUT_SPEED);//Xの移動量を減算
	g_player.posMove.z=g_player.posMove.z-(g_player.posMove.z/CUT_SPEED);//Zの移動量を減算


	//////////////////////////////////////////////////////////////////////////////
	//									入力処理							   //
	////////////////////////////////////////////////////////////////////////////

	/////////////////////////////
	//			攻撃		  //
	///////////////////////////

	//スペースキーを押した場合攻撃状態にする
	if(GetKeyboardPress(DIK_SPACE))
	{
		g_player.type=TYPE_ATTACK;		//攻撃状態にする
		g_player.rot.y++;				//プレイヤーのY軸を回転し続ける
		g_player.rotDestModel.z=0.0f;	//プレイヤーの目的のZ軸角度を初期化
		g_player.fSize=270.0f;			//プレイヤー攻撃時のサイズを変える
	}

	//スペースキーを離した場合通常状態にする
	if(GetKeyboardRelease(DIK_SPACE))
	{
		g_player.type=TYPE_NUTRAL;	//通常状態にする
		g_player.fSize=100.0f;		//通常時のサイズに変える
	}

	//Mキーを押したままで弾をチャージする
	if(GetKeyboardPress(DIK_M))
	{
		ChargeBullet();
	}
	//Mキーを離した時はショットする
	if(GetKeyboardRelease(DIK_M))
	{
		g_player.type=TYPE_SHOT;	//ショット状態にする
		SetPosBullet(g_player.pos.x,g_player.pos.z);//弾の座標をセット
	}

	///////////////////////////
	//	プレイヤーの移動	//
	//////////////////////////
	//上
	if(GetKeyboardPress(DIK_W)==true)
	{
		g_player.posMove.x-=cosf(rot.y+D3DX_PI/2)*PLAYER_SPEED_X;
		g_player.posMove.z+=sinf(rot.y+D3DX_PI/2)*PLAYER_SPEED_Z;

		if(g_player.type!=TYPE_ATTACK)
		{
			g_player.rotDestModel.y=D3DX_PI;//目的の向き
		}
	}

	//下
	if(GetKeyboardPress(DIK_S)==true)
	{
		g_player.posMove.x+=cosf(rot.y+D3DX_PI/2)*PLAYER_SPEED_X;
		g_player.posMove.z-=sinf(rot.y+D3DX_PI/2)*PLAYER_SPEED_Z;

		if(g_player.type!=TYPE_ATTACK)
		{
			g_player.rotDestModel.y=0.0f;//目的の向き
		}
	}

	//左
	if(GetKeyboardPress(DIK_A)==true)
	{
		g_player.posMove.x-=sinf(rot.y+D3DX_PI/2)*PLAYER_SPEED_X;
		g_player.posMove.z-=cosf(rot.y+D3DX_PI/2)*PLAYER_SPEED_Z;

		if(g_player.type!=TYPE_ATTACK)
		{
			g_player.rotDestModel.y=D3DX_PI/2;//目的の向き
		}
	}

	//右
	if(GetKeyboardPress(DIK_D)==true)
	{
		g_player.posMove.x+=sinf(rot.y+D3DX_PI/2)*PLAYER_SPEED_X;
		g_player.posMove.z+=cosf(rot.y+D3DX_PI/2)*PLAYER_SPEED_Z;

		if(g_player.type!=TYPE_ATTACK)
		{
			g_player.rotDestModel.y=-D3DX_PI/2;//目的の向き
		}
	}

	///////////////////////////////////////////////
	//			斜め入力の時は向きのみ			//
	//////////////////////////////////////////////

	//右上
	if(GetKeyboardPress(DIK_D)==true &&
	   GetKeyboardPress(DIK_W)==true && g_player.type!=TYPE_ATTACK)
	{
		g_player.rotDestModel.y=-(D3DX_PI/4)*3;//目的の向き
	}

	//左上
	if(GetKeyboardPress(DIK_A)==true &&
	   GetKeyboardPress(DIK_W)==true && g_player.type!=TYPE_ATTACK)
	{
		g_player.rotDestModel.y=(D3DX_PI/4)*3;//目的の向き
	}

	//右下
	if(GetKeyboardPress(DIK_D)==true &&
	   GetKeyboardPress(DIK_S)==true && g_player.type!=TYPE_ATTACK)
	{
		g_player.rotDestModel.y=-D3DX_PI/4;//目的の向き
	}

	//左下
	if(GetKeyboardPress(DIK_A)==true &&
	   GetKeyboardPress(DIK_S)==true && g_player.type!=TYPE_ATTACK)
	{
		g_player.rotDestModel.y=D3DX_PI/4;//目的の向き
	}


	//////////////////////////////////////////////////////////////////////////////
	//									向きの旋回処理							//
	//////////////////////////////////////////////////////////////////////////////

	//目的までの差分
	fDiffRotY=g_player.rotDestModel.y-g_player.rot.y;//Y軸の差分を求める
	fDiffRotZ=g_player.rotDestModel.z-g_player.rot.z;//Z軸の差分を求める

	///////////////////////
	//		差分補正	///
	//////////////////////
	//右上 270°
	if(fDiffRotY>D3DX_PI)
	{
		fDiffRotY=fDiffRotY-D3DX_PI*2;//360°減算
	}

	//上右-270°
	if(fDiffRotY<-D3DX_PI)
	{
		fDiffRotY=fDiffRotY+D3DX_PI*2;//360°加算
	}

	//プレイヤーが攻撃状態以外の場合
	if(g_player.type!=TYPE_ATTACK)
	{
		//回転加算
		g_player.rot.y+=fDiffRotY*0.1f;//Y軸
		g_player.rot.z+=fDiffRotZ*0.03f;//Z軸
	}


	///////////////////////
	//		角度補正	//
	/////////////////////
	//3.14より上の場合
	if(g_player.rot.y>D3DX_PI)
	{
		g_player.rot.y=-D3DX_PI;
	}
	//-3.14より下の場合
	if(g_player.rot.y<-D3DX_PI)
	{
		g_player.rot.y=D3DX_PI;
	}
	
	///////////////////
	//	壁衝突制御	//
	/////////////////

	//地面
	if(g_player.pos.y<0)
	{
		g_player.pos.y=0.0f;
	}

	//右壁
	if(g_player.pos.x>WALL_RIGHT_POS)
	{
		g_player.pos.x=WALL_RIGHT_POS;
		bWall=true;
	}
	//左壁
	if(g_player.pos.x<WALL_LEFT_POS)
	{
		g_player.pos.x=WALL_LEFT_POS;
		bWall=true;
	}
	//前壁
	if(g_player.pos.z>WALL_AHEAD_POS)
	{
		g_player.pos.z=WALL_AHEAD_POS;
		bWall=true;
	}
	//後壁
	if(g_player.pos.z<WALL_BACK_POS)
	{
		g_player.pos.z=WALL_BACK_POS;
		bWall=true;
	}
	
	if(bWall==true && g_player.type==TYPE_ATTACK)
	{
		AddScore(5);
		SetInfo(BONUS_WALL);
	}
	//=========================
	//プレイヤーアニメの再生
	//=========================
	PlayMove();
}
//=============================================================================
//プレイヤーの描画分け
//=============================================================================
void PlayMove (void)
{

	///////////////////////////////////
	//	プレイヤーの移動チェック	//
	/////////////////////////////////
	if(g_player.pos.x!=g_player.posPre.x ||
	   g_player.pos.z!=g_player.posPre.z)
	{
		if(g_player.type!=TYPE_ATTACK)
		{
			//モデルパターン移動
			g_player.type=TYPE_MOVE;
		}

		//移動カウントアップ
		g_player.nMovCnt++;

		//移動時の目的のZ軸角度をカウントによって分ける
		g_player.rotDestModel.z=(D3DX_PI)*(-1+(2*(g_player.nMovCnt%2)));
	}
	else
	{
		g_player.rotDestModel.z=0.0f;
		g_player.rot.z=0.0f;
	}

	////////////////////////////////////
	//	プレイヤーの攻撃チェック	 //
	///////////////////////////////////
	if(g_player.type==TYPE_ATTACK ||
	   g_player.type==TYPE_SHOT)
	{
		g_model=1;
	}
	else
	{
		g_model=0;
	}

}
//=============================================================================
//プレイヤーの描画
//=============================================================================
void DrawPlayer()
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXMATERIAL *pD3DXMatModel;
	D3DMATERIAL9 matDef;
	D3DXMATRIX mtxScl,mtxRot,mtxTranslate;//サイズ,回転,位置
	
	//プロジェクションマトリックスを反映
	D3DXMatrixIdentity(&g_mtxWorld[g_model]);

	//サイズを反映
	D3DXMatrixScaling(&mtxScl,1.0f,1.0f,1.0f);

	//位置を反映
	D3DXMatrixMultiply(&g_mtxWorld[g_model],
					   &g_mtxWorld[g_model],
					   &mtxScl);
	//回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
									g_player.rot.y,
									g_player.rot.x,
									g_player.rot.z);

	
	D3DXMatrixMultiply(&g_mtxWorld[g_model],&g_mtxWorld[g_model],
					   &mtxRot);


	//位置を反映
	D3DXMatrixTranslation(&mtxTranslate,
						  g_player.pos.x,
						  g_player.pos.y,
						  g_player.pos.z);

	//ワールドマトリックスの設定
	D3DXMatrixMultiply(&g_mtxWorld[g_model],&g_mtxWorld[g_model],
				 &mtxTranslate);

	pDevice->GetMaterial(&matDef);

	pD3DXMatModel=(D3DXMATERIAL*)g_pD3DXBuffMatModel[g_model]->GetBufferPointer();

	//座標のセット
	pDevice->SetTransform(D3DTS_WORLD,
							&g_mtxWorld[g_model]);

	for(int nCntMat=0;nCntMat<(int)g_nNumMatModel[g_model];nCntMat++)
	{
		pDevice->SetMaterial(&pD3DXMatModel[nCntMat].MatD3D);
		pDevice->SetTexture(0,NULL);
		g_pD3DXMeshModel[g_model]->DrawSubset(nCntMat);
	}

	//マテリアルセット
	pDevice->SetMaterial(&matDef);

}
//=============================================================================
//プレイヤーの終了
//=============================================================================
void UninitPlayer()
{

	for(int i=0;i<MODEL_MAX;i++)
	{
		//マテリアル情報の終了
		if(g_pD3DXBuffMatModel[i]!=NULL)
		{
			g_pD3DXBuffMatModel[i]->Release();
			g_pD3DXBuffMatModel[i]=NULL;
		}

		//メッシュ情報の終了
		if(g_pD3DXMeshModel[i]!=NULL)
		{
			g_pD3DXMeshModel[i]->Release();
			g_pD3DXMeshModel[i]=NULL;
		}
	}
}
//=============================================================================
//プレイヤー情報取得
//=============================================================================
PLAYER GetPlayer (void)
{
	return g_player;
}
//=============================================================================
//モデルの座標を取得
//=============================================================================
D3DXVECTOR3 GetPosModel (void)
{
	return g_player.pos;
}
//=============================================================================
//モデルの角度を取得
//=============================================================================
D3DXVECTOR3 GetRotModel(void)
{
	return g_player.rot;
}
//=============================================================================
//モデルのタイプ取得
//=============================================================================
TYPE GetPlayerType(void)
{
	return g_player.type;
}
//EOF