//=============================================================================
//
// カメラクラス処理 [CCamera.cpp]
// Author : 木村　文登
//
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <math.h>
#include "main.h"
#include "manager.h"
#include "CGame.h"
#include "Ccamera.h"
#include "CPlayer.h"
#include "CMeshField.h"
#include "CInput.h"
#include "CInputKeyboard.h"
#include "CInputJoystick.h"

#ifdef _DEBUG
#include "CDebugproc.h"
#endif

//*****************************************************************************
// 定数定義
//*****************************************************************************
#define MOVE_CAMERA_POS (5.5f)			//カメラ移動量
#define ROTATION_CAMERA (0.05f)			//カメラ回転量
#define TITLE_ROTATION_CAMERA (0.002f)	//タイトル時のカメラ回転量
#define TITLE_DISTANCE_POS_Y (1040.0f)	//タイトル時のカメラ距離Y
#define TITLE_DISTANCE_POS_XZ (1800.0f)	//タイトル時のカメラ距離XZ
#define DISTANCE_CAMERA_POS_Y (330.0f)	//カメラの距離Y
#define DISTANCE_CAMERA_POS_XZ (600.0f)	//カメラの距離XZ
#define DISTANCE_PARSENT (0.04f)		//目的と現在との差分の割合
#define WAIT_COUNT_MAX (80)				//待ち時間最大数
#define CAMERA_SIZE_WIDTH (1900.0f)		//カメラ描画範囲の幅
#define CAMERA_SIZE_DEPTH (5900.0f)		//カメラ描画範囲の奥行

#ifdef _DEBUG
	#define DEBUG_MOVE_POS (10.0f)		//デバッグ時の視点移動量
	#define DEBUG_TURN_POS (0.01f)		//デバッグ時の視点回転量
	#define DEBUG_MOVE_POINT (10.0f)	//デバッグ時の注視点移動量
	#define DEBUG_TURN_POINT (0.02f)	//デバッグ時の注視点回転量
#endif

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
	m_nWaitCnt=0;									//カメラ待ち時間
	m_rotCamera=D3DXVECTOR3(D3DX_PI/3,0.0f,0.0f);	//カメラの角度
	m_bOperation=false;								//デバッグ用カメラ操作フラグ
	m_bPosLock=false;								//カメラ座標固定フラグ
	m_fCam=DISTANCE_CAMERA_POS_XZ;					//カメラと注視点の距離

	//幅・奥行
	m_fWidth=CAMERA_SIZE_WIDTH;
	m_fDepth=CAMERA_SIZE_DEPTH;

	//外積計算用の頂点座標
	m_cameraVertexpos[0]=D3DXVECTOR3(-m_fWidth/2,0.0f,m_fDepth);
	m_cameraVertexpos[1]=D3DXVECTOR3(m_fWidth/2,0.0f,m_fDepth);
	m_cameraVertexpos[2]=D3DXVECTOR3(m_fWidth/2,0.0f,0.0f);
	m_cameraVertexpos[3]=D3DXVECTOR3(-m_fWidth/2,0.0f,0.0f);

	//カメラの視点
	m_posCameraP=D3DXVECTOR3(0.0f,0.0f,0.0f);

	//カメラの注視点
	m_posCameraR=D3DXVECTOR3(0.0f,0.0f,0.0f);

	//上方向ベクトル
	m_vecCameraU=D3DXVECTOR3(0.0f,1.0f,0.0f);

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
	//デバッグ用カメラ操作が可能ならキーボード操作をさせる
	if(m_bOperation==true)
	{
#ifdef _DEBUG
		//カメラのキーボード操作
		FreeOperate();
#endif
	}
	else
	{
		//そうでない場合は通常のカメラ更新処理をする
		//モードの取得
		MODE mode=CManager::GetMode();

		//各モードによりカメラ処理を分ける
		switch(mode)
		{
			//タイトル
			case MODE_TITLE:
			{
				///////////////////////////////////
				//		カメラ視点の更新		//
				/////////////////////////////////

				//カメラの回転量加算
				m_rotCamera.y+=TITLE_ROTATION_CAMERA;

				//注視点設定
				m_posCameraR=D3DXVECTOR3(0.0f,0.0f,0.0f);

				//プレイヤーの位置から距離を置いて視点の移動
				m_posCameraP.x=m_posCameraR.x-sinf(m_rotCamera.y)*TITLE_DISTANCE_POS_XZ;
				m_posCameraP.y=m_posCameraR.y+DISTANCE_CAMERA_POS_Y;
				m_posCameraP.z=m_posCameraR.z-cosf(m_rotCamera.y)*TITLE_DISTANCE_POS_XZ;

				break;
			}

			//ゲーム
			case MODE_GAME:
			{
				///////////////////////////////
				//		追従カメラ処理		//
				/////////////////////////////

				//カメラ固定フラグがない場合のみ追従処理を行う
				if(!m_bPosLock)
				{
					//プレイヤーインスタンス取得
					CPlayer *pPlayer=CGame::GetPlayer();
					//プレイヤーの座標取得
					D3DXVECTOR3 playerPos=pPlayer->GetPos();

					//プレイヤーの角度取得
					D3DXVECTOR3 playerRot=pPlayer->GetRotation();

					//プレイヤーの座標をカメラの注視点に代入
					m_posCameraR=playerPos;

					//プレイヤーが静止状態かつカメラ静止状態なら
					if(	playerPos==m_posPrevPlayer &&
						(!CInputKeyboard::GetKeyPress(DIK_J)		&&
						!CInputKeyboard::GetKeyPress(DIK_L))		&&
						(!CInputJoystick::GetPadPress(STICK_R_LEFT)	&&
						!CInputJoystick::GetPadPress(STICK_R_RIGHT)))
					{
						//カウントアップ
						m_nWaitCnt++;
					}
					else
					{
						//カウントリセット
						m_nWaitCnt=0;
					}

					///////////////////////////////////////
					//		目的の角度、座標を代入		//
					/////////////////////////////////////

					//目的の角度をプレイヤーの背後にする
					m_fRotDestCameraY=playerRot.y+D3DX_PI;

					//プレイヤーの待ち時間が一定時間達したらカメラを自動で回転する
					if(m_nWaitCnt>WAIT_COUNT_MAX)
					{
						///////////////////////////////////////
						//		慣性によるカメラの回転		//
						/////////////////////////////////////

						//目的角度と現在の角度の差を求める
						float fDiffRotY=m_fRotDestCameraY-m_rotCamera.y;

						//差分補正
						//180°より上なら
						if(fDiffRotY>D3DX_PI)
						{
							fDiffRotY=fDiffRotY-D3DX_PI*2;//360°減算
						}//-180°より下なら
						else if(fDiffRotY<-D3DX_PI)
						{
							fDiffRotY=fDiffRotY+D3DX_PI*2;//360°加算
						}

						//カメラの角度を差分の割合分加算
						m_rotCamera.y+=fDiffRotY*DISTANCE_PARSENT;

					}

					///////////////////////////////////////
					//		カメラ視点の移動操作		//
					/////////////////////////////////////
					//右旋回
					if(	CInputKeyboard::GetKeyPress(DIK_L) ||
						CInputJoystick::GetPadPress(STICK_R_RIGHT))
					{
						m_rotCamera.y-=ROTATION_CAMERA;
					}

					//左旋回
					if(	CInputKeyboard::GetKeyPress(DIK_J) ||
						CInputJoystick::GetPadPress(STICK_R_LEFT))
					{
						m_rotCamera.y+=ROTATION_CAMERA;
					}

					//上旋回
					if(	CInputKeyboard::GetKeyPress(DIK_I) ||
						CInputJoystick::GetPadPress(STICK_R_UP))
					{
						m_rotCamera.x+=ROTATION_CAMERA;
					}

					//下旋回
					if(	CInputKeyboard::GetKeyPress(DIK_K) ||
						CInputJoystick::GetPadPress(STICK_R_DOWN))
					{
						m_rotCamera.x-=ROTATION_CAMERA;
					}

					//上角度上限
					if(m_rotCamera.x>D3DX_PI/2)
					{
						m_rotCamera.x=D3DX_PI/2;
					}

					//角度補正
					//カメラの角度が180°より上なら
					if(m_rotCamera.y>D3DX_PI)
					{
						m_rotCamera.y=-D3DX_PI;
					}//カメラの角度が-180°より下なら
					else if(m_rotCamera.y<-D3DX_PI)
					{
						m_rotCamera.y=D3DX_PI;
					}

					///////////////////////////////////
					//		カメラ視点の更新		//
					/////////////////////////////////

					//プレイヤーの位置から距離を置いて視点の移動
					m_posCameraP.x=playerPos.x-sinf(m_rotCamera.y)*DISTANCE_CAMERA_POS_XZ;
					m_posCameraP.y=playerPos.y+sinf(m_rotCamera.x)*DISTANCE_CAMERA_POS_Y;
					m_posCameraP.z=playerPos.z-cosf(m_rotCamera.y)*DISTANCE_CAMERA_POS_XZ;

					///////////////////////////////////////////////////////////
					//		カメラをフィールドの高さに合わせてY座標取得		//
					/////////////////////////////////////////////////////////

					//フィールドインスタンスを取得
					CMeshField *pMeshField=CGame::GetField();

					//高さをフィールドから取得
					float fHeightField=pMeshField->GetHeight(m_posCameraP);

					//カメラのY座標がフィールドの中に入る場合
					if(m_posCameraP.y<fHeightField)
					{
						//カメラをフィールドの高さから一定距離にする
						m_rotCamera.x=m_rotCamera.x+0.1f;
						m_posCameraP.y=playerPos.y+sinf(m_rotCamera.x)*DISTANCE_CAMERA_POS_Y;
					}

					//プレイヤー前座標を代入
					m_posPrevPlayer=playerPos;
				}

				break;
			}

		}
	}
}
//=============================================================================
//セット
//=============================================================================
void CCamera::Set()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	D3DXMATRIX mtxScl,mtxRot,mtxTranslate;

	//ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	D3DXMatrixLookAtLH(	&m_mtxView,
						&m_posCameraP,
						&m_posCameraR,
						&m_vecCameraU);

	//ビューマトリックスの作成
	pDevice->SetTransform(	D3DTS_VIEW,
							&m_mtxView);

	//プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	//プロジェクションマトリックスの設定
	D3DXMatrixPerspectiveFovLH(	&m_mtxProjection,
								D3DX_PI / 4,								//視野角(45度)
								(float)SCREEN_WIDTH/(float)SCREEN_HEIGHT,	//アスペクト比(幅/高さ)
								10.0f,										//near値(10.0f)
								9500.0f										//far値(10000.0f)
								);

	//プロジェクションマトリックスのセット
	pDevice->SetTransform(	D3DTS_PROJECTION,
							&m_mtxProjection);
}
//=============================================================================
///デバッグ用カメラ操作
//=============================================================================
#ifdef _DEBUG
void CCamera::FreeOperate(void)
{
	///////////////////////////////////
	//		カメラの操作処理		//
	/////////////////////////////////

	//////////////////////////////////
	//		カメラの視点操作		//
	/////////////////////////////////
	//視点上移動
	if(CInputKeyboard::GetKeyPress(DIK_Y))
	{
		m_posCameraP.y+=DEBUG_MOVE_POS;
	}
	//視点下移動
	if(CInputKeyboard::GetKeyPress(DIK_N))
	{
		m_posCameraP.y-=DEBUG_MOVE_POS;
	}

	//視点前移動
	if(CInputKeyboard::GetKeyPress(DIK_W))
	{
		m_posCameraP.x+=sinf(m_rotCamera.y)*MOVE_CAMERA_POS;
		m_posCameraP.z+=cosf(m_rotCamera.y)*MOVE_CAMERA_POS;
		m_posCameraR.x=m_posCameraP.x+sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z=m_posCameraP.z+cosf(m_rotCamera.y)*m_fCam;
	}
	//視点後移動
	if(CInputKeyboard::GetKeyPress(DIK_S))
	{
		m_posCameraP.x-=sinf(m_rotCamera.y)*MOVE_CAMERA_POS;
		m_posCameraP.z-=cosf(m_rotCamera.y)*MOVE_CAMERA_POS;
		m_posCameraR.x=m_posCameraP.x+sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z=m_posCameraP.z+cosf(m_rotCamera.y)*m_fCam;
	}
	//視点左移動
	if(CInputKeyboard::GetKeyPress(DIK_A))
	{
		m_posCameraP.x-=sinf(m_rotCamera.y+D3DX_PI/2)*MOVE_CAMERA_POS;
		m_posCameraP.z-=cosf(m_rotCamera.y+D3DX_PI/2)*MOVE_CAMERA_POS;
		m_posCameraR.x=m_posCameraP.x+sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z=m_posCameraP.z+cosf(m_rotCamera.y)*m_fCam;
	}
	//視点右移動
	if(CInputKeyboard::GetKeyPress(DIK_D))
	{
		m_posCameraP.x+=sinf(m_rotCamera.y+D3DX_PI/2)*MOVE_CAMERA_POS;
		m_posCameraP.z+=cosf(m_rotCamera.y+D3DX_PI/2)*MOVE_CAMERA_POS;
		m_posCameraR.x=m_posCameraP.x+sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z=m_posCameraP.z+cosf(m_rotCamera.y)*m_fCam;
	}
	//視点右旋回
	if(CInputKeyboard::GetKeyPress(DIK_C))
	{
		m_rotCamera.y-=DEBUG_TURN_POS;
		m_posCameraP.x = m_posCameraR.x-sinf(m_rotCamera.y)*m_fCam;
		m_posCameraP.z = m_posCameraR.z-cosf(m_rotCamera.y)*m_fCam;
	}
	//視点左旋回
	if(CInputKeyboard::GetKeyPress(DIK_Z))
	{
		m_rotCamera.y+=DEBUG_TURN_POS;
		m_posCameraP.x = m_posCameraR.x-sinf(m_rotCamera.y)*m_fCam;
		m_posCameraP.z = m_posCameraR.z-cosf(m_rotCamera.y)*m_fCam;
	}

	//////////////////////////////////
	//		カメラの注視点操作		//
	/////////////////////////////////
	//右旋回
	if(CInputKeyboard::GetKeyPress(DIK_E))
	{
		m_rotCamera.y+=DEBUG_TURN_POINT;
		m_posCameraR.x=m_posCameraP.x+sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z=m_posCameraP.z+cosf(m_rotCamera.y)*m_fCam;
	}
	//左旋回
	if(CInputKeyboard::GetKeyPress(DIK_Q))
	{
		m_rotCamera.y-=DEBUG_TURN_POINT;
		m_posCameraR.x=m_posCameraP.x+sinf(m_rotCamera.y)*m_fCam;
		m_posCameraR.z=m_posCameraP.z+cosf(m_rotCamera.y)*m_fCam;
	}
	//上移動
	if(CInputKeyboard::GetKeyPress(DIK_T))
	{
		m_posCameraR.y+=DEBUG_MOVE_POINT;
	}
	//下移動
	if(CInputKeyboard::GetKeyPress(DIK_B))
	{
		m_posCameraR.y-=DEBUG_MOVE_POINT;
	}

	//カメラ操作方法表示
	CDebug::Print("\n////////////////カメラ操作ガイド////////////////");
	CDebug::Print("\nWSAD:視点移動");
	CDebug::Print("\n  YN:視点上下移動");
	CDebug::Print("\n  ZC:視点旋回");
	CDebug::Print("\n  TB:注視点上下移動");
	CDebug::Print("\n  QE:注視点旋回");
}
#endif
//=============================================================================
//カメラ固定フラグの設定
//=============================================================================
void CCamera::SetPosLock(bool flag)
{
	m_bPosLock=flag;
}
//=============================================================================
//カメラ範囲の頂点座標取得
//=============================================================================
void CCamera::GetVertexPos(D3DXVECTOR3 *vertexPos,int i)
{
	//頂点X座標
	vertexPos->x=cosf(-m_rotCamera.y)*m_cameraVertexpos[i].x
				-sinf(-m_rotCamera.y)*m_cameraVertexpos[i].z+m_posCameraP.x;
	//頂点Z座標
	vertexPos->z=sinf(-m_rotCamera.y)*m_cameraVertexpos[i].x
				+cosf(-m_rotCamera.y)*m_cameraVertexpos[i].z+m_posCameraP.z;
}
//=============================================================================
// カメラの向きを取得
//=============================================================================
D3DXVECTOR3 CCamera::GetRotCamera(void)
{
	return m_rotCamera;
}
//=============================================================================
//カメラ座標取得
//=============================================================================
D3DXVECTOR3 CCamera::GetPosCamera(void)
{
	return m_posCameraP;
}
//=============================================================================
// カメラのマトリックスを取得
//=============================================================================
D3DXMATRIX CCamera::GetMtxView(void)
{
	return m_mtxView;
}
//=============================================================================
//カメラデバッグ操作フラグのセット
//=============================================================================
#ifdef _DEBUG
void CCamera::SetDebugFlag (bool bFlag)
{
	m_bOperation=bFlag;
}
#endif
//EOF