//=============================================================================
//
// カメラクラス処理 [Camera.cpp]
// Author : 木村　文登
//
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <math.h>
#include "Camera.h"
#include "FrustumCulling.h"
#include "Game.h"
#include "Player.h"
#include "MeshField.h"
#include "Input.h"
#include "InputKeyboard.h"
#include "InputJoystick.h"
#include "main.h"

//デバッグ用
#ifdef _DEBUG
#include "DebugProc.h"
#endif
//*****************************************************************************
// 定数定義
//*****************************************************************************
#define HALF_PI (D3DX_PI / 1.5f)							//半分の角度
#define ANGLE (D3DX_PI / 4)									//カメラアングル
#define NEAR_CLIP (10.0f)									//ニアークリップ
#define FAR_CLIP (19000.0f)									//ファークリップ
#define ASPECT ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	//アスペクト比
#define MOVE_CAMERA_POS (15.5f)								//カメラ移動量
#define ROTATION_CAMERA (0.05f)								//カメラ回転量
#define DISTANCE_CAMERA_POS_Y (330.0f)						//カメラの距離Y
#define DISTANCE_CAMERA_POS_XZ (400.0f)						//カメラの距離XZ
#define DISTANCE_PARSENT (0.06f)							//目的と現在との差分の割合
#define WAIT_COUNT_MAX (50)									//待ち時間最大数
#define TITLE_CAMERA_P (D3DXVECTOR3(91.5f, 40.0f, -30.0f))	//タイトル時の視点
#define TITLE_CAMERA_R (D3DXVECTOR3(50.0f, 40.0f, 4.0f))	//タイトル時の注視点
#define DEBUG_MOVE_POS (10.0f)								//デバッグ時の視点移動量
#define DEBUG_TURN_POS (0.01f)								//デバッグ時の視点回転量
#define DEBUG_MOVE_POINT (10.0f)							//デバッグ時の注視点移動量
#define DEBUG_TURN_POINT (0.02f)							//デバッグ時の注視点回転量

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
	///////////////////////
	//	カメラの初期化	//
	/////////////////////
	m_nWaitCnt = 0;					//カメラ待ち時間
	m_fCam = DISTANCE_CAMERA_POS_XZ;//注視点からの距離
	//カメラの角度
	m_rotCamera = D3DXVECTOR3(0.5f, 0.0f, 0.0f);

	//カメラの視点
	m_posCameraP = D3DXVECTOR3(0.0f, 100.0f, -180.0f);

	//カメラの注視点
	m_posCameraR = D3DXVECTOR3(	m_posCameraP.x + sinf(m_rotCamera.y + 0.00f)*m_fCam, //X
								0.0f,												 //Y
								m_posCameraP.z + cosf(m_rotCamera.y + 0.00f)*m_fCam);//Z
	//上方向ベクトル
	m_vecCameraU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//フラスタムカリング初期化 (※フルスクリーン時はHALF_PI)
	CFrustum::Init(HALF_PI, ASPECT, NEAR_CLIP, FAR_CLIP);

//デバッグ用
#ifdef _DEBUG
	//カメラ操作フラグ
	m_bCameraOperate = false;
#endif

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CCamera::Uninit()
{

}
//=============================================================================
//更新
//=============================================================================
void CCamera::Update()
{

//デバッグ用
#ifdef _DEBUG

	//カメラ操作フラグ切替
	if (CInputKeyboard::GetKeyTrigger(DIK_F4))
	{
		if (m_bCameraOperate)
		{
			m_bCameraOperate = false;
		}
		else
		{
			m_bCameraOperate = true;
		}
	}

	//カメラ操作フラグtrueなら操作可能
	if (m_bCameraOperate)
	{
		//カメラの操作
		FreeOperate();
	}

	//タイトル時の更新
	else if (m_cameraMode == TITLE_MODE)
	{
		m_posCameraP = TITLE_CAMERA_P;
		m_posCameraR = TITLE_CAMERA_R;
	}

	//ゲーム時のカメラ更新
	else if (m_cameraMode == GAME_MODE)
	{
		GameUpdate();
	}

//リリース時
#else

	//タイトル時の更新
	if (m_cameraMode == TITLE_MODE)
	{
		m_posCameraP = TITLE_CAMERA_P;
		m_posCameraR = TITLE_CAMERA_R;
	}

	//ゲームモードの場合ゲーム時の更新処理を行う
	else if (m_cameraMode == GAME_MODE)
	{
		GameUpdate();
	}

#endif
	///////////////////////////
	//		角度補正		//
	/////////////////////////
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
//カメラセット
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

	//プロジェクションマトリックスの設定
	D3DXMatrixIdentity(&m_mtxProjection);

	//プロジェクションマトリックスの初期化
	D3DXMatrixPerspectiveFovLH(	&m_mtxProjection,
								ANGLE,//視野角(45度)
								ASPECT,//アスペクト比(幅/高さ)
								NEAR_CLIP,//near値(10.0f)
								FAR_CLIP);//far値(10000.0f)
		

	//プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);
}
//=============================================================================
//カメラの操作
//=============================================================================
//デバッグ用
#ifdef _DEBUG
void CCamera::FreeOperate()
{
	//////////////////////////////////
	//		カメラの視点操作		//
	/////////////////////////////////
	//視点上移動
	if (CInputKeyboard::GetKeyPress(DIK_Y))
	{
		m_posCameraP.y += DEBUG_MOVE_POS;
	}
	//視点下移動
	if (CInputKeyboard::GetKeyPress(DIK_N))
	{
		m_posCameraP.y -= DEBUG_MOVE_POS;
	}

	//視点前移動
	if (CInputKeyboard::GetKeyPress(DIK_W))
	{
		m_posCameraP.x += sinf(m_rotCamera.y)*MOVE_CAMERA_POS;
		m_posCameraP.z += cosf(m_rotCamera.y)*MOVE_CAMERA_POS;
		m_posCameraR.x = m_posCameraP.x + sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z = m_posCameraP.z + cosf(m_rotCamera.y)*m_fCam;
	}
	//視点後移動
	if (CInputKeyboard::GetKeyPress(DIK_S))
	{
		m_posCameraP.x -= sinf(m_rotCamera.y)*MOVE_CAMERA_POS;
		m_posCameraP.z -= cosf(m_rotCamera.y)*MOVE_CAMERA_POS;
		m_posCameraR.x = m_posCameraP.x + sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z = m_posCameraP.z + cosf(m_rotCamera.y)*m_fCam;
	}
	//視点左移動
	if (CInputKeyboard::GetKeyPress(DIK_A))
	{
		m_posCameraP.x -= sinf(m_rotCamera.y + D3DX_PI / 2)*MOVE_CAMERA_POS;
		m_posCameraP.z -= cosf(m_rotCamera.y + D3DX_PI / 2)*MOVE_CAMERA_POS;
		m_posCameraR.x = m_posCameraP.x + sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z = m_posCameraP.z + cosf(m_rotCamera.y)*m_fCam;
	}
	//視点右移動
	if (CInputKeyboard::GetKeyPress(DIK_D))
	{
		m_posCameraP.x += sinf(m_rotCamera.y + D3DX_PI / 2)*MOVE_CAMERA_POS;
		m_posCameraP.z += cosf(m_rotCamera.y + D3DX_PI / 2)*MOVE_CAMERA_POS;
		m_posCameraR.x = m_posCameraP.x + sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z = m_posCameraP.z + cosf(m_rotCamera.y)*m_fCam;
	}
	//視点右旋回
	if (CInputKeyboard::GetKeyPress(DIK_C))
	{
		m_rotCamera.y -= DEBUG_TURN_POS;
		m_posCameraP.x = m_posCameraR.x - sinf(m_rotCamera.y)*m_fCam;
		m_posCameraP.z = m_posCameraR.z - cosf(m_rotCamera.y)*m_fCam;
	}
	//視点左旋回
	if (CInputKeyboard::GetKeyPress(DIK_Z))
	{
		m_rotCamera.y += DEBUG_TURN_POS;
		m_posCameraP.x = m_posCameraR.x - sinf(m_rotCamera.y)*m_fCam;
		m_posCameraP.z = m_posCameraR.z - cosf(m_rotCamera.y)*m_fCam;
	}

	//////////////////////////////////
	//		カメラの注視点操作		//
	/////////////////////////////////
	//右旋回
	if (CInputKeyboard::GetKeyPress(DIK_E))
	{
		m_rotCamera.y += DEBUG_TURN_POINT;
		m_posCameraR.x = m_posCameraP.x + sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z = m_posCameraP.z + cosf(m_rotCamera.y)*m_fCam;
	}
	//左旋回
	if (CInputKeyboard::GetKeyPress(DIK_Q))
	{
		m_rotCamera.y -= DEBUG_TURN_POINT;
		m_posCameraR.x = m_posCameraP.x + sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z = m_posCameraP.z + cosf(m_rotCamera.y)*m_fCam;
	}
	//上移動
	if (CInputKeyboard::GetKeyPress(DIK_T))
	{
		m_posCameraR.y += DEBUG_MOVE_POINT;
	}
	//下移動
	if (CInputKeyboard::GetKeyPress(DIK_B))
	{
		m_posCameraR.y -= DEBUG_MOVE_POINT;
	}
}
#endif
//=============================================================================
//ゲーム時の更新処理
//=============================================================================
void CCamera::GameUpdate()
{
	///////////////////////////////
	//		追従カメラ処理		//
	/////////////////////////////

	//プレイヤーインスタンス取得
	CPlayer *pPlayer = CGame::GetPlayer();
	//プレイヤーの座標取得
	D3DXVECTOR3 playerPos = pPlayer->GetPos();

	//プレイヤーの角度取得
	D3DXVECTOR3 playerRot = pPlayer->GetRot();

	//プレイヤーの座標をカメラの注視点に代入
	m_posCameraR = playerPos;

	//プレイヤーが静止状態かつカメラ静止状態なら
	if (playerPos == m_posPrevPlayer)
	{
		//カウントアップ
		m_nWaitCnt++;
	}
	else
	{
		//カウントリセット
		m_nWaitCnt = 0;
	}

	///////////////////////////////////////
	//		目的の角度、座標を代入		//
	/////////////////////////////////////

	//目的の角度をプレイヤーの背後にする
	m_fRotDestCameraY = playerRot.y + D3DX_PI;

	//プレイヤーの待ち時間が一定時間達したらカメラを自動で回転する
	if (m_nWaitCnt > WAIT_COUNT_MAX)
	{
		///////////////////////////////////////
		//		慣性によるカメラの回転		//
		/////////////////////////////////////

		//目的角度と現在の角度の差を求める
		float fDiffRotY = m_fRotDestCameraY - m_rotCamera.y;

		//差分補正
		//180°より上なら
		if (fDiffRotY > D3DX_PI)
		{
			fDiffRotY = fDiffRotY - D3DX_PI * 2;//360°減算
		}//-180°より下なら
		else if (fDiffRotY < -D3DX_PI)
		{
			fDiffRotY = fDiffRotY + D3DX_PI * 2;//360°加算
		}

		//カメラの角度を差分の割合分加算
		m_rotCamera.y += fDiffRotY*DISTANCE_PARSENT;

	}

	///////////////////////////////////////
	//		カメラ視点の移動操作		//
	/////////////////////////////////////
	//右旋回
	if (CInputKeyboard::GetKeyPress(DIK_L)
	||  CInputJoystick::GetPadPress(STICK_R_RIGHT))
	{
		m_rotCamera.y -= ROTATION_CAMERA;
	}

	//左旋回
	if (CInputKeyboard::GetKeyPress(DIK_J)
	||  CInputJoystick::GetPadPress(STICK_R_LEFT))
	{
		m_rotCamera.y += ROTATION_CAMERA;
	}

	//上旋回
	if (CInputKeyboard::GetKeyPress(DIK_I)
	||  CInputJoystick::GetPadPress(STICK_R_UP))
	{
		m_rotCamera.x += ROTATION_CAMERA;
	}

	//下旋回
	if (CInputKeyboard::GetKeyPress(DIK_K)
	||  CInputJoystick::GetPadPress(STICK_R_DOWN))
	{
		m_rotCamera.x -= ROTATION_CAMERA;
	}

	//上角度上限
	if (m_rotCamera.x > D3DX_PI / 2)
	{
		m_rotCamera.x = D3DX_PI / 2;
	}

	///////////////////////////////////
	//		カメラ視点の更新		//
	/////////////////////////////////

	//プレイヤーの位置から距離を置いて視点の移動
	m_posCameraP.x = playerPos.x - sinf(m_rotCamera.y)*DISTANCE_CAMERA_POS_XZ;
	m_posCameraP.y = playerPos.y + sinf(m_rotCamera.x)*DISTANCE_CAMERA_POS_Y;
	m_posCameraP.z = playerPos.z - cosf(m_rotCamera.y)*DISTANCE_CAMERA_POS_XZ;

	//フィールドの高さより下にしない
	float fHeight = CGame::GetField()->GetHeight(m_posCameraP);
	if (m_posCameraP.y < fHeight)
	{
		//カメラをフィールドの高さから一定距離にする
		m_rotCamera.x = m_rotCamera.x + 0.1f;
		m_posCameraP.y = playerPos.y + sinf(m_rotCamera.x)*DISTANCE_CAMERA_POS_Y;
	}

	//プレイヤー前座標更新
	m_posPrevPlayer = playerPos;
}
//=============================================================================
//ゲームモードのセット
//=============================================================================
void CCamera::SetMode(MODE mode)
{
	m_cameraMode = mode;
}
//=============================================================================
// カメラの向きを取得
//=============================================================================
D3DXVECTOR3 CCamera::GetRotCamera(void)
{
	return m_rotCamera;
}
//=============================================================================
// カメラのマトリックスを取得
//=============================================================================
D3DXMATRIX CCamera::GetMtxView(void)
{
	return m_mtxView;
}
//EOF