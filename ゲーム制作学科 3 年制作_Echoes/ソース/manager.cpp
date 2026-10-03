//=============================================================================
//
// マネージャー処理 [manager.cpp]
// Author : 木村　文登
//
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "manager.h"
#include "Camera.h"
#include "FrustumCulling.h"
#include "LightManager.h"
#include "Sound.h"
#include "InputKeyboard.h"
#include "InputJoystick.h"
#include "Title.h"
#include "Menu.h"
#include "Game.h"
#include "Result.h"
#include "MeshField.h"
#include "ShaderManager.h"

//デバッグ用
#ifdef _DEBUG
	#include "DebugProc.h"
#endif

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
CCamera *CManager::m_pCamera = NULL;			//カメラインスタンス
CSound *CManager::m_pSound = NULL;				//サウンド
CFrustum *CManager::m_pFrustum = NULL;			//フラスタム
CInputKeyboard *CManager::m_pInput = NULL;		//キーボードインスタンス
CInputJoystick *CManager::m_pJoyStick = NULL;	//ゲームパッドインスタンス
CRenderer *CManager::m_pRenderer = NULL;		//レンダラーインスタンス
CTitle *CManager::m_pTitle = NULL;				//タイトル管理
CMenu *CManager::m_pMenu = NULL;				//メニュー管理
CGame *CManager::m_pGame = NULL;				//ゲーム管理
CResult *CManager::m_pResult = NULL;			//リザルト管理
MODE CManager::m_Mode = TITLE_MODE;				//モード

#ifdef _DEBUG
	CDebug *CManager::m_pDebug=NULL;		//デバック用フォントインスタンス
#endif
//=============================================================================
//コンストラクタ
//=============================================================================
CManager::CManager()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CManager::~CManager()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	///////////////////////////////////
	//		各インスタンス生成		//
	/////////////////////////////////

	//レンダラーインスタンス生成
	m_pRenderer=new CRenderer();

	//レンダラー初期化チェック
	if(FAILED(m_pRenderer->Init(hInstance,hWnd,bWindow)))
	{
		return E_FAIL;
	}

	//キーボードインスタンス生成
	m_pInput=new CInputKeyboard();
	//キーボード初期化
	m_pInput->Init(hInstance,hWnd);

	//ゲームパッドインスタンス生成
	m_pJoyStick = new CInputJoystick();
	m_pJoyStick->Init(hInstance, hWnd);

	//フラスタムカリング生成
	m_pFrustum = new CFrustum();

	//カメラインスタンス生成
	m_pCamera = new CCamera();
	//カメラ初期化
	m_pCamera->Init();
	m_pCamera->SetMode(m_Mode);

	//ライトインスタンス生成
	CLightManager::GetInstance()->InitAll();

	//サウンドインスタンス生成
	m_pSound = new CSound();
	m_pSound->InitSound(hWnd);

	//シェーダーマネージャー生成
	CShaderManager::GetInstance()->Init();

	//モードセット
	SetMode(m_Mode);

//デバッグ用
#ifdef _DEBUG
	//停止フラグ
	m_bStop = false;

	//デバッグインスタンス生成
	m_pDebug=new CDebug();
	m_pDebug->Init();
#endif

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CManager::Uninit()
{
	//タイトル解放
	if (m_pTitle)
	{
		m_pTitle->Uninit();
		delete m_pTitle;
		m_pTitle = NULL;
	}

	//メニュー解放
	if (m_pMenu)
	{
		m_pMenu->Uninit();
		delete m_pMenu;
		m_pMenu = NULL;
	}

	//ゲーム解放
	if (m_pGame)
	{
		m_pGame->Uninit();
		delete m_pGame;
		m_pGame = NULL;
	}

	//リザルト解放
	if (m_pResult)
	{
		m_pResult->Uninit();
		delete m_pResult;
		m_pResult = NULL;
	}

	//レンダラーの終了
	m_pRenderer->Uninit();
	delete m_pRenderer;
	m_pRenderer = NULL;

	//キーボード終了
	m_pInput->Uninit();
	delete m_pInput;
	m_pInput = NULL;

	//ゲームパッド終了
	m_pJoyStick->Uninit();
	delete m_pJoyStick;
	m_pJoyStick = NULL;

	//フラスタムカリング終了
	delete m_pFrustum;
	m_pFrustum = NULL;

	//カメラ終了
	m_pCamera->Uninit();
	delete m_pCamera;
	m_pCamera = NULL;

	//ライト終了
	CLightManager::Delete();

	//サウンド終了
	m_pSound->UninitSound();
	delete m_pSound;
	m_pSound = NULL;

	//シェーダーマネージャー終了
	CShaderManager::Delete();

//デバッグ用
#ifdef _DEBUG
	//デバッグ終了
	m_pDebug->Uninit();
	delete m_pDebug;
#endif
}
//=============================================================================
//更新
//=============================================================================
void CManager::Update()
{
	//キーボードの更新
	m_pInput->Update();

	//ゲームパッド更新
	m_pJoyStick->Update();

	//カメラ更新
	m_pCamera->Update();

//デバッグ用
#ifdef _DEBUG

	//停止フラグ切替
	if (CInputKeyboard::GetKeyTrigger(DIK_F1))
	{
		if (m_bStop)
			m_bStop = false;
		else
			m_bStop = true;
	}

	//停止フラグfalse時のみ更新
	if (!m_bStop)
	{
		//タイトル更新
		if (m_pTitle)
		{
			m_pTitle->Update();
		}

		//メニュー更新
		if (m_pMenu)
		{
			m_pMenu->Update();
		}

		//ゲーム更新
		if (m_pGame)
		{
			m_pGame->Update();
		}

		//リザルト更新
		if (m_pResult)
		{
			m_pResult->Update();
		}
	}
//リリース時
#else
	//タイトル更新
	if (m_pTitle)
	{
		m_pTitle->Update();
	}

	//メニュー更新
	if (m_pMenu)
	{
		m_pMenu->Update();
	}

	//ゲーム更新
	if (m_pGame)
	{
		m_pGame->Update();
	}

	//リザルト更新
	if (m_pResult)
	{
		m_pResult->Update();
	}
#endif

	//レンダラーの更新
	m_pRenderer->Update();
}
//=============================================================================
//描画
//=============================================================================
void CManager::Draw()
{
	//カメラセット
	m_pCamera->Set();

	//レンダラーの描画
	m_pRenderer->Draw();
}
//=============================================================================
//モードセット
//=============================================================================
void CManager::SetMode(MODE mode)
{
	//モードセット
	m_Mode = mode;
	//カメラにゲームモードセット
	m_pCamera->SetMode(m_Mode);

	///////////////////////////////////////////
	//		現在のステートを終了させる		//
	/////////////////////////////////////////

	//タイトル解放
	if (m_pTitle)
	{
		m_pTitle->Uninit();
		delete m_pTitle;
		m_pTitle = NULL;
	}

	//メニュー解放
	if (m_pMenu)
	{
		m_pMenu->Uninit();
		delete m_pMenu;
		m_pMenu = NULL;
	}

	//ゲーム解放
	if (m_pGame)
	{
		m_pGame->Uninit();
		delete m_pGame;
		m_pGame = NULL;
	}

	//リザルト解放
	if (m_pResult)
	{
		m_pResult->Uninit();
		delete m_pResult;
		m_pResult = NULL;
	}

	///////////////////////////////////////////////////////
	//		モードによって生成するステートを用意		//
	/////////////////////////////////////////////////////
	switch (m_Mode)
	{
		//タイトル
		case TITLE_MODE:
			m_pTitle = new CTitle();
			m_pTitle->Init();
		break;

		//メニュー
		case MENU_MODE:
			m_pMenu = new CMenu();
			m_pMenu->Init();
		break;
		
		//ゲーム
		case GAME_MODE:
			m_pGame = new CGame();
			m_pGame->Init();
		break;

		//リザルト
		case RESULT_MODE:
			m_pResult = new CResult();
			m_pResult->Init();
		break;

		default:
		break;
	}

}
//=============================================================================
//レンダラーの取得
//=============================================================================
CRenderer *CManager::GetRenderer()
{
	return m_pRenderer;
}
//=============================================================================
//カメラの取得
//=============================================================================
CCamera *CManager::GetCamera()
{
	return m_pCamera;
}
//EOF