//=============================================================================
//
// シーンマネージャー処理 [manager.cpp]
// Author : 木村　文登
//
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "manager.h"
#include "Ccamera.h"
#include "CLight.h"
#include "CInputKeyboard.h"
#include "CInputJoystick.h"
#include "CTitle.h"
#include "CGame.h"
#include "CResult.h"
#include "CFade.h"
#include "CSound.h"
#include "CToonShader.h"

//デバッグ用
#ifdef _DEBUG
#include "CDebugproc.h"
#endif
//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
CLight *CManager::m_pLight=NULL;				//ライトインスタンス
CCamera *CManager::m_pCamera=NULL;				//カメラインスタンス
CInputKeyboard *CManager::m_pInput=NULL;		//キーボードインスタンス
CInputJoystick *CManager::m_pJoyStick=NULL;		//ジョイステックインスタンス
CRenderer *CManager::m_pRenderer=NULL;			//レンダラーインスタンス
CSound *CManager::m_pSound=NULL;				//サウンドインスタンス
CToonShader *CManager::m_pToon=NULL;			//トゥーンシェーダーインスタンス
CTitle *CManager::m_pTitle=NULL;				//タイトルインスタンス
CGame *CManager::m_pGame=NULL;					//ゲームインスタンス
CResult *CManager::m_pResult=NULL;				//リザルトインスタンス
CFade *CManager::m_pFade=NULL;					//フェードインスタンス
MODE CManager::m_mode=MODE_TITLE;				//各モード

#ifdef _DEBUG
	CDebug *CManager::m_pDebug=NULL;			//デバック用フォントインスタンス
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

	//ジョイスティックインスタンス生成
	m_pJoyStick=new CInputJoystick();
	//ジョイスティック初期化
	m_pJoyStick->Init(hInstance,hWnd);

	//サウンドインスタンス生成
	m_pSound=new CSound();
	//サウンド初期化
	m_pSound->InitSound(hWnd);

	//カメラインスタンス生成
	m_pCamera=new CCamera();
	//カメラ初期化
	m_pCamera->Init();

	//ライトインスタンス生成
	m_pLight=new CLight();
	//ライト初期化
	m_pLight->Init();

	//トゥーンシェーダーインスタンス生成
	m_pToon=new CToonShader();
	//トゥーンシェーダー初期化
	m_pToon->Init();

	//フェードインスタンス生成
	m_pFade=CFade::Create();

	//モードのセット
	SetMode(m_mode);

//デバッグのみ
#ifdef _DEBUG
	//デバッグ用ポーズフラグ初期化
	m_bDebugPause=false;

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
	//レンダラーの終了
	m_pRenderer->Uninit();
	delete m_pRenderer;
	m_pRenderer=NULL;

	//キーボード終了
	m_pInput->Uninit();
	delete m_pInput;
	m_pInput=NULL;

	//ジョイスティック終了
	m_pJoyStick->Uninit();
	delete m_pJoyStick;
	m_pJoyStick=NULL;

	//サウンド終了
	m_pSound->UninitSound();
	delete m_pSound;
	m_pSound=NULL;

	//カメラ終了
	m_pCamera->Uninit();
	delete m_pCamera;
	m_pCamera=NULL;

	//ライト終了
	delete m_pLight;
	m_pLight=NULL;

	//トゥーンシェーダー終了
	m_pToon->Uninit();
	delete m_pToon;
	m_pToon=NULL;

	//フェード終了
	m_pFade->Uninit();

	//タイトル終了
	if(m_pTitle!=NULL)
	{
		m_pTitle->Uninit();
		delete m_pTitle;
		m_pTitle=NULL;
	}

	//ゲーム終了
	if(m_pGame!=NULL)
	{
		m_pGame->Uninit();
		delete m_pGame;
		m_pGame=NULL;
	}

	//リザルト終了
	if(m_pResult!=NULL)
	{
		m_pResult->Uninit();
		delete m_pResult;
		m_pResult=NULL;
	}

#ifdef _DEBUG
	//デバッグ終了
	m_pDebug->Uninit();
	delete m_pDebug;
	m_pDebug=NULL;
#endif
}
//=============================================================================
//更新
//=============================================================================
void CManager::Update()
{
	//キーボードの更新
	m_pInput->Update();

	//ジョイスティック更新
	m_pJoyStick->Update();

	//カメラ更新
	m_pCamera->Update();

	//ライト更新
	m_pLight->Update();

//デバッグ時のみの処理
#ifdef _DEBUG

	//デバッグ用ポーズフラグがfalseなら各ステートの通常の更新処理をする
	if(m_bDebugPause==false)
	{
		//タイトル更新
		if(m_pTitle!=NULL)
		{
			m_pTitle->Update();
		}

		//ゲーム更新
		if(m_pGame!=NULL)
		{
			m_pGame->Update();
		}

		//リザルト更新
		if(m_pResult!=NULL)
		{
			m_pResult->Update();
		}
	}

	//0キーが押されたらデバッグ用ポーズフラグを切り替える
	if(CInputKeyboard::GetKeyTrigger(DIK_0))
	{
		//フラグの現在の状態によって切替
		if(m_bDebugPause==false)
		{
			m_bDebugPause=true;
		}
		else
		{
			m_bDebugPause=false;
		}

		//カメラの操作フラグにセットする
		m_pCamera->SetDebugFlag(m_bDebugPause);
	}

	//デバッグ機能の表記
	CDebug::Print("\n////////////////デバッグ機能////////////////");
	CDebug::Print("\n0キー:一時停止			ON/OFF");
	CDebug::Print("\n1キー:当たり判定表示	ON/OFF");

#else
	//リリース時は各ステート処理のみ行う
	//タイトル更新
	if(m_pTitle!=NULL)
	{
		m_pTitle->Update();
	}

	//ゲーム更新
	if(m_pGame!=NULL)
	{
		m_pGame->Update();
	}

	//リザルト更新
	if(m_pResult!=NULL)
	{
		m_pResult->Update();
	}
#endif

}
//=============================================================================
//描画
//=============================================================================
void CManager::Draw()
{
	//カメラセット
	m_pCamera->Set();

	//ライトセット
	m_pLight->Set();

	//タイトルの描画
	if(m_pTitle!=NULL)
	{
		m_pTitle->Draw();
	}

	//ゲームの描画
	if(m_pGame!=NULL)
	{
		m_pGame->Draw();
	}

	//リザルトの描画
	if(m_pResult!=NULL)
	{
		m_pResult->Draw();
	}

	//レンダラーの描画
	m_pRenderer->Draw();
}
//=============================================================================
//モードのセット
//=============================================================================
void CManager::SetMode(MODE mode)
{
	///////////////////////////////////////////////////
	//		各モードのインスタンスを解放する		//
	/////////////////////////////////////////////////

	//タイトル解放
	if(m_pTitle!=NULL)
	{
		m_pTitle->Uninit();
		delete m_pTitle;
		m_pTitle=NULL;
	}

	//ゲーム解放
	if(m_pGame!=NULL)
	{
		m_pGame->Uninit();
		delete m_pGame;
		m_pGame=NULL;
	}

	//リザルト解放
	if(m_pResult!=NULL)
	{
		m_pResult->Uninit();
		delete m_pResult;
		m_pResult=NULL;
	}

	//サウンドの停止
	CSound::StopSound();
	
	///////////////////////////////////////////////////////
	//		セットしたモードからインスタンスを生成		//
	/////////////////////////////////////////////////////

	switch(mode)
	{
		//タイトル
		case MODE_TITLE:
		{
			//タイトルインスタンス生成
			m_pTitle=new CTitle();
			m_pTitle->Init();
			break;
		}

		//ゲーム
		case MODE_GAME:
		{
			//ゲームインスタンス生成
			m_pGame=new CGame();
			m_pGame->Init();
			break;
		}

		//リザルト
		case MODE_RESULT:
		{
			//リザルトインスタンス生成
			m_pResult=new CResult();
			m_pResult->Init();
			break;
		}
	}

	//モード変数にセット
	m_mode=mode;
}
//=============================================================================
//モードの取得
//=============================================================================
MODE CManager::GetMode(void)
{
	return m_mode;
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
//=============================================================================
//ライトの取得
//=============================================================================
CLight *CManager::GetLight(void)
{
	return m_pLight;
}
//EOF