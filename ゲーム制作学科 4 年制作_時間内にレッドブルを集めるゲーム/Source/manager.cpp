//=============================================================================
//
// マネージャー処理 [manager.cpp]
// Author : 木村 文登
//
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "manager.h"
#include "System/Common.h"
#include "System/Camera.h"
#include "System/Light.h"
#include "System/Input/InputKeyboard.h"
#include "Object/Mesh/MeshDoom.h"
#include "State/State.h"
#include "State/Title.h"
#include "State/Game.h"
#include "State/Result.h"

#ifdef _DEBUG
	#include "System/DebugProc.h"
#endif

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
CLight *CManager::m_pLight = NULL;					//ライトインスタンス
CCamera *CManager::m_pCamera = NULL;				//カメラインスタンス
CInputKeyboard *CManager::m_pInput = NULL;			//キーボードインスタンス
CRenderer *CManager::m_pRenderer = NULL;			//レンダラーインスタンス
CState *CManager::m_pState = NULL;					//ステートインスタンス
STATE_INDEX CManager::m_StateIndex = TITLE_STATE;	//ステート番号

#ifdef _DEBUG
	CDebug *CManager::m_pDebug = NULL;			//デバック用フォントインスタンス
#endif

//=============================================================================
//初期化
//=============================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	///////////////////////////////////
	//		各インスタンス生成		//
	/////////////////////////////////

	//レンダラーインスタンス生成
	m_pRenderer = new CRenderer();

	//レンダラー初期化チェック
	if(FAILED(m_pRenderer->Init(hInstance, hWnd, bWindow)))
		return E_FAIL;

	//キーボードインスタンス生成
	m_pInput = new CInputKeyboard();
	m_pInput->Init(hInstance, hWnd);

	//カメラインスタンス生成
	m_pCamera = new CCamera();
	m_pCamera->Init();

	//ライトインスタンス生成
	m_pLight = new CLight();
	m_pLight->Init();

	//ステートの生成
	SetState(m_StateIndex);

#ifdef _DEBUG
	//デバッグインスタンス生成
	m_pDebug = new CDebug();
	m_pDebug->Init();
#endif

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CManager::Uninit()
{
	//現在もってるステートを削除
	DELETE_OBJECT(m_pState);

	//レンダラーの終了
	DELETE_OBJECT(m_pRenderer);

	//キーボード終了
	DELETE_OBJECT(m_pInput);

	//カメラ終了
	DELETE_OBJECT(m_pCamera);

	//ライト終了
	DELETE_OBJECT(m_pLight);

#ifdef _DEBUG
	//デバッグ終了
	DELETE_OBJECT(m_pDebug);
#endif
}
//=============================================================================
//更新
//=============================================================================
void CManager::Update()
{
	//キーボードの更新
	m_pInput->Update();

	//ゲームステートのみ更新させる
	if (m_StateIndex == GAME_STATE)
	{
		//カメラ更新
		m_pCamera->Update();

		//ライト更新
		m_pLight->Update();
	}

	//レンダラーの更新
	m_pRenderer->Update();

	//ステートの更新
	m_pState->Update();
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

	//レンダラーの描画
	m_pRenderer->Draw();
}
//=============================================================================
//ステートのセット
//=============================================================================
void CManager::SetState(STATE_INDEX index)
{
	//現在もってるステートを削除
	DELETE_OBJECT(m_pState);

	//ステート番号によって生成するステートを分ける
	switch (index)
	{
		//タイトル
		case TITLE_STATE:
			m_pState = CTitle::Create();
		break;

		//ゲーム
		case GAME_STATE:
			m_pState = CGame::Create();
		break;

		//リザルト
		case RESULT_STATE:
			m_pState = CResult::Create();
		break;
	}

	//ステート番号保持
	m_StateIndex = index;

	//レンダラーにステート番号セット
	CRenderer::SetStateIndex((int)index);
}
//EOF