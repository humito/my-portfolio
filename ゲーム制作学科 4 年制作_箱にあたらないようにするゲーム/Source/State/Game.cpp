//=============================================================================
// タイトル管理処理 [Game.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Game.h"
#include "../main.h"
#include "../manager.h"
#include "../Scene/scene.h"
#include "../Object/Player.h"
#include "../Object/Cube.h"
#include "../Object/Mesh/MeshField.h"
#include "../Object/Mesh/MeshDoom.h"
#include "../Object/Time.h"
#include "../System/Input/InputKeyboard.h"
#include "../System/renderer.h"
#include "../Shader/Fade.h"
#include <stdio.h>
#include <time.h>

//*****************************************************************************
//定数定義
//*****************************************************************************
#define FIELD_LIMIT_SIZE (270.0f)			//フィールド範囲内のサイズ
#define FIELD_SIZE (FIELD_LIMIT_SIZE * 2)	//フィールド全体のサイズ

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
CMeshField *CGame::m_pField = NULL;	//フィールドインスタンス
CPlayer *CGame::m_pPlayer = NULL;	//プレイヤーインスタンス

//=============================================================================
//初期化
//=============================================================================
void CGame::Init()
{
	//乱数初期化
	srand((unsigned int)time(NULL));

	m_fTime = 0.0f;

	m_nDropCnt = 0;

	//プレイヤー生成
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//フィールド生成
	m_pField = CMeshField::Create(	NO_LOAD_FIELD,
									NULL,
									"data/TEXTURE/stone-blocks.jpg",
									D3DXVECTOR3(0.0f, 0.0f, 0.0f),
									D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//ドーム生成
	CMeshDoom::Create("data/TEXTURE/sky.jpg",
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					DOOM_BLOCK_X, DOOM_BLOCK_Y, DOOM_HALF);

	//タイム生成
	float fPosX = (SCREEN_WIDTH / 2) - (TIME_WIDTH / 2);
	CTime::Create(	D3DXVECTOR3(fPosX, 0.0f, 0.0f),
					TIME_WIDTH,
					TIME_HEIGHT,
					1,
					DISP_2DPOLYGON,
					TIME_DIGIT_MAX);
}
//=============================================================================
//終了
//=============================================================================
void CGame::Uninit()
{
	//全オブジェクト削除
	CScene::ReleaseAll();
}
//=============================================================================
//更新
//=============================================================================
void CGame::Update()
{
	m_fTime += 1.0f;

	if ((int)m_fTime % 60 == 0
	||	CInputKeyboard::GetKeyTrigger(DIK_SPACE))
	{
		//生成時の座標
		D3DXVECTOR3 createPos = D3DXVECTOR3(-FIELD_LIMIT_SIZE + (float)(rand() % (int)FIELD_SIZE),
											(float)(rand() % 300) + 600.0f,
											-FIELD_LIMIT_SIZE + (float)(rand() % (int)FIELD_SIZE));

		//生成時の角度
		D3DXVECTOR3 createRot = D3DXVECTOR3((float)(rand() % 6),
			(float)(rand() % 6),
			(float)(rand() % 6));
		//立方体の生成
		CCube::Create("data/MODEL/cube.x", createPos, createRot);
	}

	//全オブジェクト更新
	CScene::UpdateAll();

#ifdef _DEBUG
	if (CInputKeyboard::GetKeyTrigger(DIK_RETURN))
		CManager::GetRenderer()->GetFade()->StartFade(RESULT_STATE);
#endif
}
//=============================================================================
//インスタンス生成
//=============================================================================
CGame *CGame::Create()
{
	CGame *pInstance = new CGame();
	pInstance->Init();
	return pInstance;
}
//EOF