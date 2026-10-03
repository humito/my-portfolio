//=============================================================================
//
// カメラクラス処理 [Camera.cpp]
// Author : 木村　文登
//
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Camera.h"
#include "Input/Input.h"
#include "Input/InputKeyboard.h"
#include "../main.h"
#include "../manager.h"
#include "../State/Game.h"
#include "../Object/Player.h"
#include <math.h>

//デバッグ用
#ifdef _DEBUG
	#include "DebugProc.h"
#endif

//*****************************************************************************
// 定数定義
//*****************************************************************************
#define MOVE_CAMERA_POS (5.5f)			//カメラ移動量
#define DISTANCE_CAMERA_POS_Y (330.0f)	//カメラの距離Y
#define DISTANCE_CAMERA_POS_XZ (120.0f)	//カメラの距離XZ
#define DISTANCE_PARSENT (0.06f)		//目的と現在との差分の割合
#define WAIT_COUNT_MAX (50)				//待ち時間最大数
#define DEBUG_MOVE_POS (10.0f)			//デバッグ時の視点移動量
#define DEBUG_TURN_POS (0.05f)			//デバッグ時の視点回転量
#define DEBUG_MOVE_POINT (10.0f)		//デバッグ時の注視点移動量
#define DEBUG_TURN_POINT (0.02f)		//デバッグ時の注視点回転量

//=============================================================================
//コンストラクタ
//=============================================================================
CCamera::CCamera()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CCamera::~CCamera()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CCamera::Init()
{
	m_fCam = DISTANCE_CAMERA_POS_XZ;	//注視点からの距離
	m_rotCamera.y = 0.00f;				//カメラの角度

	//カメラの視点
	m_posCameraP = D3DXVECTOR3(0.0f, 50.0f, -180.0f);

	//カメラの注視点
	m_posCameraR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//上方向ベクトル
	m_vecCameraU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	return S_OK;
}
//=============================================================================
//更新
//=============================================================================
void CCamera::Update()
{
	//モデルの座標取得
	D3DXVECTOR3 modelPos = CGame::GetPlayer()->GetPos();
	m_posCameraR = modelPos;

	///////////////////////////////////
	//		カメラの視点操作		//
	/////////////////////////////////

	//視点右旋回
	if (CInputKeyboard::GetKeyPress(DIK_RIGHT))
	{
		m_rotCamera.y -= DEBUG_TURN_POS;
	}
	//視点左旋回
	else if (CInputKeyboard::GetKeyPress(DIK_LEFT))
	{
		m_rotCamera.y += DEBUG_TURN_POS;
	}

	//カメラ座標更新
	m_posCameraP.x = m_posCameraR.x - sinf(m_rotCamera.y)*m_fCam;
	m_posCameraP.z = m_posCameraR.z - cosf(m_rotCamera.y)*m_fCam;

	//角度補正
	//カメラの角度が180°より上なら
	if (m_rotCamera.y > D3DX_PI)
	{
		m_rotCamera.y = -D3DX_PI;

	}//カメラの角度が-180°より下なら
	else if (m_rotCamera.y < -D3DX_PI)
	{
		m_rotCamera.y = D3DX_PI;
	}
}
//=============================================================================
//描画
//=============================================================================
void CCamera::Set()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	D3DXMatrixLookAtLH(	&m_mtxView,
						&m_posCameraP,
						&m_posCameraR,
						&m_vecCameraU);

	//ビューマトリックスの作成
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

	//ビューマトリックスの設定
	D3DXMatrixIdentity(&m_mtxProjection);

	//プロジェクションマトリックスの行列変換
	D3DXMatrixPerspectiveFovLH(	&m_mtxProjection,
								D3DX_PI / 4,//視野角(45度)
								(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,//アスペクト比(幅/高さ)
								10.0f,//near値(10.0f)
								10000.0f);//far値(10000.0f)

	//プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION,
		&m_mtxProjection);
}
//EOF