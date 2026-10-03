//=============================================================================
//
// 追従カメラ処理 [camera.cpp]
// Author : HUMITO KIMURA
//
//=============================================================================
#include "camera.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//*****************************************************************************
// グローバル変数
//*****************************************************************************
D3DXVECTOR3			g_posCameraP;			//カメラの視点
D3DXVECTOR3			g_posCameraR;			//カメラの注視点
D3DXVECTOR3			g_vecCameraU;			//カメラの上方向ベクトル
static D3DXMATRIX	g_mtxView;				//ビューマトリックス
D3DXMATRIX			g_mtxProjection;		//プロジェクションマトリックス
static float		g_fDis=2500.0f;			//カメラの一定距離
D3DXVECTOR3			g_rotCamera;			//カメラの向き（回転角）
D3DXVECTOR3			g_posCameraPDest;		//目的の視点
D3DXVECTOR3			g_posCameraRDest;		//目的の注視点
//=============================================================================
// カメラの初期化処理
//=============================================================================
HRESULT InitCamera(void)
{

	///////////////////////
	//	カメラの初期化	//
	/////////////////////

	g_rotCamera.y = 0.00f;//カメラの角度

	g_posCameraP=D3DXVECTOR3(10000.0f,9000.0f,10000.0f);	//カメラの視点

	//カメラの注視点
	g_posCameraR=D3DXVECTOR3(g_posCameraP.x+sinf(g_rotCamera.y+0.00f)*g_fDis,
		                     410.0f,
							 g_posCameraP.z+cosf(g_rotCamera.y+0.00f)*g_fDis);

	g_vecCameraU=D3DXVECTOR3(0.0f,1.0f,0.0f);		//上方向ベクトル

	return S_OK;
}
//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{
}
//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{
	//////////////////////////
	//	カメラの追従処理	//
	/////////////////////////

	//モデル取得
	D3DXVECTOR3 posModel=GetPosModel();
	D3DXVECTOR3 rotModel=GetRotModel();
	TYPE        type=GetPlayerType();

	//モデルの位置から一定距離
	g_posCameraPDest.x=posModel.x;							//視点X
	g_posCameraPDest.y=posModel.y+2000.0f;					//視点Y
	g_posCameraPDest.z=posModel.z-g_fDis;					//視点Z

	if(g_posCameraPDest.z<WALL_BACK_POS)
	{
		g_posCameraPDest.z=WALL_BACK_POS;
	}

	//モデルの前方の位置
	if(type==TYPE_ATTACK)
	{
		g_posCameraRDest.x=posModel.x;	//注視点X
		g_posCameraRDest.z=posModel.z;	//注視点Z
	}
	else
	{
		g_posCameraRDest.x=posModel.x-sinf(rotModel.y)*300.0f;	//注視点X
		g_posCameraRDest.z=posModel.z-cosf(rotModel.y)*300.0f;	//注視点Z
	}

	g_posCameraRDest.y=posModel.y;							//注視点Y
	

	g_posCameraP.x+=(g_posCameraPDest.x
					-g_posCameraP.x)*0.03f;
	g_posCameraP.y+=(g_posCameraPDest.y
					-g_posCameraP.y)*0.1f;
	g_posCameraP.z+=(g_posCameraPDest.z
					-g_posCameraP.z)*0.1f;

	g_posCameraR.x+=(g_posCameraRDest.x
					-g_posCameraR.x)*0.03f;
	g_posCameraR.y+=(g_posCameraRDest.y
					-g_posCameraR.y)*0.1f;
	g_posCameraR.z+=(g_posCameraRDest.z
					-g_posCameraR.z)*0.1f;
}
//=============================================================================
// カメラの設定処理
//=============================================================================
void SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice(); 
	D3DXMATRIX mtxScl,mtxRot,mtxTranslate;

	//ビューマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxView);

	D3DXMatrixLookAtLH(&g_mtxView,
					   &g_posCameraP,
					   &g_posCameraR,
					   &g_vecCameraU);

	//ビューマトリックスの作成
	pDevice->SetTransform(D3DTS_VIEW,
						  &g_mtxView);

	//ビューマトリックスの設定
	D3DXMatrixIdentity(&g_mtxProjection);

	//プロジェクションマトリックスの初期化
	D3DXMatrixPerspectiveFovLH(&g_mtxProjection,
							   D3DX_PI / 4,//視野角(45度)
							   600/400,//アスペクト比(幅/高さ)
							   10.0f,//near値(10.0f)
							   10000.0f//far値(10000.0f)
							   );

	//プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION,
						  &g_mtxProjection);
}
//=============================================================================
// カメラの座標を取得
//=============================================================================
D3DXVECTOR3 GetPosCamera (void)
{
	return g_posCameraP;
}
//=============================================================================
// カメラの向きを取得
//=============================================================================
D3DXVECTOR3 GetRotCamera(void)
{
	return g_rotCamera;
}
//=============================================================================
//マトリックスビューを取得
//=============================================================================
D3DXMATRIX GetMtxView(void)
{
	return g_mtxView;
}
//EOF