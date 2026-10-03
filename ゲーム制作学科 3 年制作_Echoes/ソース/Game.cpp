//=============================================================================
// ゲーム管理処理 [Game.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Game.h"
#include "main.h"
#include "scene.h"
#include "MeshField.h"
#include "MeshCylinder.h"
#include "MeshDoom.h"
#include "InputKeyboard.h"
#include "InputJoystick.h"
#include "Fade.h"
#include "manager.h"
#include "Menu.h"
#include "Player.h"
#include "Pause.h"
#include "Enemy.h"
#include "Score.h"
#include "Time.h"
#include "Combo.h"
#include "Ranking.h"
#include "EraseArea.h"
#include "Particle.h"
#include "FeedBackBlur.h"
#include "Camera.h"
#include "FrustumCulling.h"
#include "Sound.h"
#include <stdio.h>
#include <time.h>

#ifdef _DEBUG
	#include "DebugProc.h"
#endif

//*****************************************************************************
//定数定義
//*****************************************************************************
#define DEFAULT_ENEMY_NUM (70)	//通常の敵の数 130
#define CREATE_ENEMY_COUNT (15)	//敵を生成するカウント 10
#define ADD_FIELD_Z (500)		//補正用のフィールドZ座標
#define STATUS_BEGIN (0)		//ゲーム開始ステータス
#define STATUS_GAME (1)			//ゲーム中ステータス
#define LENGTH_CREATE (2000.0f)	//生成を判定する距離
#define SMOKE_NUM (5)			//生成する煙の数
#define SMOKE_SIZE (600.0f)		//生成する煙のサイズ

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
CMeshField *CGame::m_pField = NULL;	//フィールド
CPlayer *CGame::m_pPlayer = NULL;	//プレイヤー
CEraseArea *CGame::m_pArea = NULL;	//消滅エリア
CPause *CGame::m_pPause = NULL;		//ポーズ
int CGame::m_nStageMenu = STAGE1;	//ステージメニュー
bool CGame::m_bPause = false;		//ポーズフラグ

//=============================================================================
//初期化
//=============================================================================
HRESULT CGame::Init()
{
	//乱数初期化
	srand((unsigned int)time(NULL));

	//経過カウントリセット
	m_nCount = 0;

	//ポーズフラグ
	m_bPause = false;

	//ゲームステータス
	m_nStatus = STATUS_BEGIN;

	///////////////////////////////////////////////////
	//		選択されたステージから読込を分ける		//
	/////////////////////////////////////////////////

	//ステージ１の情報を初期値として入れる

	//ドームのテクスチャファイル名
	char *pDoomTexName = "data/TEXTURE/sky002.jpg";
	//フィールドのテクスチャファイル名
	char *pFieldTexName = "data/TEXTURE/field002.jpg";
	//フィールドのバイナリファイル名
	char *pFieldName = "data/FIELD/field002.dat";

	//サウンドラベル
	SOUND_LABEL sound = SOUND_LABEL_BGM_GAME1;

	//ステージ2
	if (m_nStageMenu == STAGE2)
	{
		pDoomTexName = "data/TEXTURE/sky001.jpg";
		pFieldTexName = "data/TEXTURE/field003.jpg";
		pFieldName = "data/FIELD/field001.dat";
		sound = SOUND_LABEL_BGM_GAME2;
	}

	//ステージ3
	else if (m_nStageMenu == STAGE3)
	{
		pDoomTexName = "data/TEXTURE/sky000.jpg";
		pFieldTexName = "data/TEXTURE/field000.jpg";
		pFieldName = "data/FIELD/field000.dat";
		sound = SOUND_LABEL_BGM_GAME3;
	}

	///////////////////////////////////////
	//		各オブジェクトの生成		//
	/////////////////////////////////////

	//ドーム生成
	CMeshDoom::Create(	pDoomTexName,
						D3DXVECTOR3(0.0f, 0.0f, 0.0f),
						DOOM_BLOCK_X,
						DOOM_BLOCK_Y,
						DOOM_HALF);

	//シリンダー生成
	CMeshCylinder::Create(	D3DXVECTOR3(0.0f, 0.0f, 0.0f),
							CYLINDER_BLOCK_X,
							CYLINDER_BLOCK_Y,
							CYLINDER_SIZE,
							CYLINDER_HALF);

	//フィールド生成
	m_pField = CMeshField::Create(	LOAD_FIELD,
									pFieldName,
									pFieldTexName,
									D3DXVECTOR3(0.0f, 0.0f, 0.0f),
									D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//消滅エリア生成
	m_pArea = CEraseArea::Create();

	//プレイヤー生成
	m_pPlayer = CPlayer::Create();

	//スコア生成
	CScore::Create(D3DXVECTOR3(SCREEN_WIDTH - SCORE_WIDTH, 0.0f, 0.0f),
					SCORE_WIDTH,
					SCORE_HEIGHT,
					SOCRE_GAME_MODE,
					DISP_2DPOLYGON,
					SCORE_DIGIT_MAX);

	//タイム生成
	CTime::Create(	D3DXVECTOR3((SCREEN_WIDTH / 2) - (TIME_WIDTH/2), 0.0f, 0.0f),
					TIME_WIDTH,
					TIME_HEIGHT,
					SOCRE_GAME_MODE,
					DISP_2DPOLYGON,
					TIME_DIGIT_MAX);

	//コンボ生成
	CCombo::Create();

	//サウンドの再生
	CSound::PlaySoundA(sound);

	///////////////////////////////////////////////////////
	//		フィールドからサイズとブロック数取得		//
	/////////////////////////////////////////////////////

	//計算用ブロック数とサイズ
	int nBlockX = 0 , nBlockZ = 0;
	float fBlockSizeZ = 0.0f;

	//ブロック数とサイズ取得
	m_pField->GetBlockNum(&nBlockX, &nBlockZ);
	m_pField->GetBlockSize(&m_fBlockSizeX, &fBlockSizeZ);
	//フィールドの半分のサイズ計算
	m_fHalfFieldSizeX = (nBlockX * m_fBlockSizeX) / 2;
	m_fHalfFieldSizeZ = (nBlockZ * fBlockSizeZ) / 2;

	//空間分割スペース初期化
	m_pnSpace = new int[nBlockX];
	for (int i = 0; i < nBlockX; ++i)
	{
		m_pnSpace[i] = 1;
	}

	//敵の生成ループ
	for (int i = 0; i < DEFAULT_ENEMY_NUM; ++i)
	{
		//敵のスタート位置設定
		StartEnemy();
	}

	//ゲーム中ステータス変更
	m_nStatus = STATUS_GAME;

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CGame::Uninit()
{
	//空間分割解放
	delete[] m_pnSpace;
	m_pnSpace = NULL;

	//全サウンド停止
	CSound::StopSound();

	//フィードバック使用しない
	CFeedBackBlur::SetUse(false);

	//ランキングにステージ情報セット
	CRanking::SetStage(m_nStageMenu);

	//全シーン解放
	CScene::ReleaseAll();
}
//=============================================================================
//更新
//=============================================================================
void CGame::Update()
{
	//ポーズフラグない場合は通常の更新
	if (!m_bPause)
	{
		//ポーズインスタンスあるなら解放させる
		if (m_pPause)
		{
			m_pPause->Uninit();
			m_pPause = NULL;
			//エッジ描画させる
			CRenderer::EdgeFlagChange(true);
		}

		//敵の自動生成
		if (m_nCount % CREATE_ENEMY_COUNT == 0)
		{
			StartEnemy();
		}

		//経過カウントアップ
		m_nCount++;
		
		//シーンオブジェクトの更新
		CScene::UpdateAll();
	}
	else
	{
		//ポーズ中の更新処理
		//ポーズインスタンスないなら生成させる
		if (!m_pPause)
		{
			m_pPause = CPause::Create();
			//エッジ描画させない
			CRenderer::EdgeFlagChange(false);
		}

		//ポーズ更新
		m_pPause->Update();
	}

	//Pキーが押されたらポーズフラグ切替
	if (CInputKeyboard::GetKeyTrigger(DIK_P)
	||  CInputJoystick::GetPadTrigger(BUTTON_START))
	{
		if (m_bPause)
			m_bPause = false;
		else
			m_bPause = true;
	}

//デバッグ用
#ifdef _DEBUG

	//F2で敵の生成
	if (CInputKeyboard::GetKeyTrigger(DIK_F2))
	{
		StartEnemy();
	}

	//F3で遷移
	if (CInputKeyboard::GetKeyTrigger(DIK_F3))
	{
		CFade::SetFade(RESULT_MODE);
	}

	//経過カウント表示
	CDebug::Print("\nGameCount:%d",m_nCount);
#endif
}
//=============================================================================
//敵の生成開始
//=============================================================================
void CGame::StartEnemy()
{
	//X,Z座標・敵の色を乱数で決める
	float fPosX = -m_fHalfFieldSizeX + (float)(rand() % (int)(m_fHalfFieldSizeX * 2));
	//空間の番号
	int nSpaceIndex = (int)((fPosX + m_fHalfFieldSizeX) / m_fBlockSizeX);

	//各空間の設置した個数分の1の確率で生成位置を決める
	while (rand() % m_pnSpace[nSpaceIndex] != 0)
	{
		fPosX = -m_fHalfFieldSizeX + (float)(rand() % (int)(m_fHalfFieldSizeX * 2));
		nSpaceIndex = (int)((fPosX + m_fHalfFieldSizeX) / m_fBlockSizeX);
	}

	//空間に設置した数加算
	m_pnSpace[nSpaceIndex]++;

	//Z座標
	float fPosZ = m_fHalfFieldSizeZ - (float)(rand() % ((int)m_fHalfFieldSizeZ + ADD_FIELD_Z));

	//敵の色
	ENEMY_COLOR enemyCol = (ENEMY_COLOR)(rand() % COLOR_NUM);

	//生成する位置
	D3DXVECTOR3 createPos = D3DXVECTOR3(fPosX, 0.0f, fPosZ);

	//エネミー生成
	CEnemy::Create(createPos, enemyCol);

	//ゲーム開始時以外なら煙パーティクル生成チェック
	if (m_nStatus != STATUS_BEGIN
	&&	CFrustum::MeshFOVCheck(createPos, ENEMY_RADIUS))
	{
		//ビューマトリクス取得
		D3DXMATRIX mtxView = CManager::GetCamera()->GetMtxView();
		//カメラのZ距離計算
		D3DXVECTOR3 vi(mtxView._13, mtxView._23, mtxView._33);
		float fCameraLenZ = D3DXVec3Dot(&createPos, &vi) + mtxView._43;
		//一定の距離以内なら煙パーティクル生成
		if (fCameraLenZ < LENGTH_CREATE)
		{
			CParticle::Create(	TYPE_SMOKE,
								SMOKE_NUM,
								createPos,
								SMOKE_SIZE,
								SMOKE_SIZE);
		}
	}
}
//=============================================================================
//ポーズさせない
//=============================================================================
void CGame::NoPause()
{
	m_bPause = false;
}
//EOF