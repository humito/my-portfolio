//=============================================================================
// ゲーム処理 [CGame.cpp]
// Author : 木村　文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CGame.h"
#include "scene.h"
#include "scene2D.h"
#include "CScene3D.h"
#include "CSceneX.h"
#include "CSceneBillboard.h"
#include "CScore.h"
#include "CPlayer.h"
#include "CEnemy.h"
#include "CObject.h"
#include "CLife.h"
#include "CItemPossession.h"
#include "CTree.h"
#include "CMeshField.h"
#include "CMeshCylinder.h"
#include "CMeshDoom.h"
#include "CLeftoverBullet.h"
#include "CPause.h"
#include "CFace.h"
#include "CInputKeyboard.h"
#include "CInputJoystick.h"
#include "manager.h"
#include "CFade.h"
#include "CSound.h"
#include <stdlib.h>
#include <time.h>

//*****************************************************************************
//定数定義
//*****************************************************************************
#define VERYHARD_ITEM_NUM (10000)	//ベリーハード時のアイテム数
#define HARD_ITEM_NUM (6000)		//ハード時のアイテム数
#define NORMAL_ITEM_NUM (4000)		//ノーマル時のアイテム数
#define EASY_ITEM_NUM (2000)		//イージー時のアイテム数
#define ENEMY_START_POS_MAX (3)		//敵の発進地数
#define ENEMY_NEXT_START (1)		//次の敵の発進地
#define ENEMY_START_COUNT (100)		//敵が発生するカウントの一定間隔

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
CMeshField *CGame::m_pMeshField=NULL;	//フィールドインスタンス
CPlayer *CGame::m_pPlayer=NULL;			//プレイヤーインスタンス
CPause *CGame::m_pPause=NULL;			//ポーズインスタンス
bool CGame::m_bPause=false;				//ポーズフラグ

//=============================================================================
//コンストラクタ
//=============================================================================
CGame::CGame()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CGame::~CGame()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CGame::Init()
{
	//乱数の初期化
	srand((unsigned int) time(NULL));

	//経過カウント初期化
	m_nElapsedCnt=0;

	///////////////////////////////////////////
	//		ゲーム関連オブジェクト生成		//
	/////////////////////////////////////////

	//スコアインスタンス生成
	CScore::Create(D3DXVECTOR3(0.0f,50.0f,0.0f),200.0f,20.0f,SOCRE_GAME_MODE,DISP_2DPOLYGON,SCORE_DIGIT_MAX);

	//表情インスタンス生成
	CFace::Create();

	//アイテム所持数生成
	CPossessionNum::Create(D3DXVECTOR3(50.0f,600.0f,0.0f),DISP_2DPOLYGON);

	//残弾数インスタンス生成
	CLeftoverBullet::Create();

	//ライフインスタンス生成
	CLife::Create();

	//3Dオブジェクトインスタンス生成(メインフィールド起伏床)
	m_pMeshField=CMeshField::Create("data/TEXTURE/field007.jpg",D3DXVECTOR3(0.0f,0.0f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),FIELD_NUM_X,FIELD_NUM_Z,FIELDSIZE_X,FIELDSIZE_Z);

	//画面外のフィールド
	CMeshField::Create("data/TEXTURE/ground128.jpg",D3DXVECTOR3(0.0f,0.0f,4500.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),FIELD_NUM_X/2,FIELD_NUM_Z/2,FIELDSIZE_X*2,FIELDSIZE_Z);
	CMeshField::Create("data/TEXTURE/ground128.jpg",D3DXVECTOR3(0.0f,0.0f,-4500.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),FIELD_NUM_X/2,FIELD_NUM_Z/2,FIELDSIZE_X*2,FIELDSIZE_Z);
	CMeshField::Create("data/TEXTURE/ground128.jpg",D3DXVECTOR3(4500.0f,0.0f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),FIELD_NUM_X/2,FIELD_NUM_Z/2,FIELDSIZE_X,FIELDSIZE_Z*2.8f);
	CMeshField::Create("data/TEXTURE/ground128.jpg",D3DXVECTOR3(-4500.0f,0.0f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),FIELD_NUM_X/2,FIELD_NUM_Z/2,FIELDSIZE_X,FIELDSIZE_Z*2.8f);

	//3Dオブジェクト(起伏ドーム)
	CMeshDoom::Create(D3DXVECTOR3(0.0f,0.0f,0.0f),80,80,7000.0f);

	//3Dオブジェクト(起伏筒)
	CMeshCylinder::Create(D3DXVECTOR3(0.0f,0.0f,0.0f),10,1,1000.0f,6000.0f);

	//プレイヤーインスタンス生成
	m_pPlayer=CPlayer::Create();

	//敵インスタンス生成
	CEnemy::Create(TYPE_BOM,ENEMY_VERYEASY,D3DXVECTOR3(1000.0f,0.0f,450.0f));

	///////////////////////////////////////////////
	//		ゲーム関連その他オブジェクト		//
	/////////////////////////////////////////////
	
	for(int j=0;j<2;j++)
	{
		for(int i=0;i<5;i++)
		{
			//民家
			CObject::Create("data/MODEL/minka01.x",D3DXVECTOR3((1500.0f*j)-2200.0f,15.0f,2500.0f-(1300*(j%2))-(i*600.0f)),D3DXVECTOR3(0.0f,0.0f,0.0f));
			CObject::Create("data/MODEL/minka01.x",D3DXVECTOR3((1500.0f*j)-1400.0f,15.0f,2500.0f-(1300*(j%2))-(i*600.0f)),D3DXVECTOR3(0.0f,0.0f,0.0f));
		}
	}
	
	//民家
	CObject::Create("data/MODEL/minka00.x",D3DXVECTOR3(2200.0f,0.0f,2100.0f),D3DXVECTOR3(0.0f,0.0f,0.0f));
	CObject::Create("data/MODEL/minka00.x",D3DXVECTOR3(2200.0f,0.0f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f));

	//城
	CObject::Create("data/MODEL/siro.x",D3DXVECTOR3(2200.0f,15.0f,-1300.0f),D3DXVECTOR3(0.0f,D3DX_PI/2,0.0f));
	
	//塔
	CObject::Create("data/MODEL/sir.x",D3DXVECTOR3(800.0f,0.0f,-2700.0f),D3DXVECTOR3(0.0f,0.0f,0.0f));

	//塔
	CObject::Create("data/MODEL/sir.x",D3DXVECTOR3(-2500.0f,0.0f,-2700.0f),D3DXVECTOR3(0.0f,0.0f,0.0f));
	
	//法隆寺
	CObject::Create("data/MODEL/yume.x",D3DXVECTOR3(-1600.0f,250.0f,-2300.0f),D3DXVECTOR3(0.0f,0.0f,0.0f));

	//鳥居
	CObject::Create("data/MODEL/tori.x",D3DXVECTOR3(-800.0f,250.0f,-2200.0f),D3DXVECTOR3(0.0f,D3DX_PI/2,0.0f));

	//休憩所
	CObject::Create("data/MODEL/ste.x",D3DXVECTOR3(-100.0f,0.0f,2200.0f),D3DXVECTOR3(0.0f,-(D3DX_PI/2),0.0f));
	CObject::Create("data/MODEL/ste.x",D3DXVECTOR3(2300.0f,0.0f,1200.0f),D3DXVECTOR3(0.0f,D3DX_PI/2,0.0f));
	CObject::Create("data/MODEL/ste.x",D3DXVECTOR3(-2000.0f,0.0f,-600.0f),D3DXVECTOR3(0.0f,0.0f,0.0f));

	for(int j=0;j<2;j++)
	{
		for(int i=0;i<12;i++)
		{
			//木インスタンス生成
			CTree::Create("data/TEXTURE/tree001.png",D3DXVECTOR3(-2800.0f+(j*250.0f),0.0f,2000.0f-(i*300.0f)),D3DXVECTOR3(0.0f,0.0f,0.0f),200.0f,400.0f);
		}
	}
	
	//木インスタンス生成
	CTree::Create("data/TEXTURE/tree001.png",D3DXVECTOR3(-400.0f,0.0f,2100.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),200.0f,400.0f);
	CTree::Create("data/TEXTURE/tree001.png",D3DXVECTOR3(-400.0f,0.0f,1900.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),200.0f,400.0f);
	CTree::Create("data/TEXTURE/tree000.png",D3DXVECTOR3(2300.0f,0.0f,1500.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),200.0f,400.0f);

	//桜インスタンス生成
	CTree::Create("data/TEXTURE/tree002.png",D3DXVECTOR3(-400.0f,250.0f,-2100.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),400.0f,800.0f);
	CTree::Create("data/TEXTURE/tree002.png",D3DXVECTOR3(500.0f,0.0f,2100.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),400.0f,800.0f);
	CTree::Create("data/TEXTURE/tree002.png",D3DXVECTOR3(1500.0f,0.0f,-2300.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),400.0f,800.0f);

	//ゲームBGM再生
	CSound::PlaySoundA(SOUND_LABEL_BGM_GAME);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CGame::Uninit()
{
	//ゲーム関連の全シーンクラスを解放
	CScene::ReleaseAll();
}
//=============================================================================
//更新
//=============================================================================
void CGame::Update()
{
	//カウントアップ
	m_nElapsedCnt++;

	///////////////////////////////////////
	//		ポーズ画面への切替処理		//
	/////////////////////////////////////

	//Pキーを押した場合
	if(	CInputKeyboard::GetKeyTrigger(DIK_P) ||
		CInputJoystick::GetPadTrigger(BUTTON_START))
	{
		//ポーズフラグを切替
		if(m_bPause==false)
		{
			m_bPause=true;
		}
		else
		{
			m_bPause=false;
		}
	}

	///////////////////////////////////////////
	//		ゲーム内オブジェクトの更新		//
	/////////////////////////////////////////


	//ポーズフラグfalseならゲーム全体を更新、それ以外はポーズ画面のみ更新
	if(m_bPause==false)
	{
		//ポーズインスタンスに中身が入ってるなら
		if(m_pPause!=NULL)
		{
			m_pPause->Uninit();	//終了
			m_pPause=NULL;		//NULLセット
		}

		//一定のカウントで敵を発生させる
		if(m_nElapsedCnt%ENEMY_START_COUNT==0)
		{
			//ランダムの位置へ敵の生成
			StartCreateEnemy();
		}

		//シーンオブジェクトの更新
		CScene::UpdateAll();
	}
	else
	{
		//ポーズインスタンスNULLなら
		if(m_pPause==NULL)
		{
			//ポーズインスタンス生成
			m_pPause=CPause::Create();
		}

		//ポーズ画面更新
		m_pPause->Update();
	}
}
//=============================================================================
//描画
//=============================================================================
void CGame::Draw()
{
}
//=============================================================================
//敵の発生
//=============================================================================
void CGame::StartCreateEnemy(void)
{
	int type=rand()%ENEMY_START_POS_MAX;

	//プレイヤーのアイテム数から敵のレベルを設定
	int nNumItem=m_pPlayer->GetItemNum();
	//敵のレベル
	ENEMY_LEVEL level;

	//ベリーハード
	if(nNumItem>VERYHARD_ITEM_NUM)
	{
		level=ENEMY_VERYHARD;
	}//ハード
	else if(nNumItem>HARD_ITEM_NUM)
	{
		level=ENEMY_HARD;
	}//ノーマル
	else if(nNumItem>NORMAL_ITEM_NUM)
	{
		level=ENEMY_NORMAL;
	}//イージー
	else if(nNumItem>EASY_ITEM_NUM)
	{
		level=ENEMY_EASY;
	}//ベリーイージー
	else
	{
		level=ENEMY_VERYEASY;
	}

	//乱数の数値によって敵の生成する位置を決める
	if(type==0)
	{
		//1ヶ所目
		CEnemy::Create(TYPE_BOM,level,D3DXVECTOR3(1000.0f,1000.0f,450.0f));
	}
	else if(type==ENEMY_NEXT_START)
	{
		//2ヶ所目
		CEnemy::Create(TYPE_BOM,level,D3DXVECTOR3(-2600.0f,700.0f,-600.0f));
	}
	else
	{
		//3ヶ所目
		CEnemy::Create(TYPE_BOM,level,D3DXVECTOR3(1600.0f,700.0f,-2600.0f));
	}
}
//=============================================================================
//ゲーム内各要素リセット
//=============================================================================
void CGame::Reset(void)
{
	///////////////////////////////////////
	//		全敵インスタンス削除		//
	/////////////////////////////////////

	//プライオリティの先頭ポインタ取得
	CScene *scene=CScene::GetListTop(PRIORITY_SCENEX);

	//シーン終端までループ
	while(scene)
	{
		//次ポインタ取得
		CScene *pNext=scene->GetNextScene();

		//オブジェクトタイプが敵の場合
		if(scene->GetType()==OBJECT_TYPE_ENEMY)
		{
			//終了処理
			scene->Uninit();
		}

		//次ポインタへ移動
		scene=pNext;
	}
}
//=============================================================================
//プレイヤーの取得
//=============================================================================
CPlayer *CGame::GetPlayer(void)
{
	return m_pPlayer;
}
//=============================================================================
//フィールドの取得
//=============================================================================
CMeshField *CGame::GetField(void)
{
	return m_pMeshField;
}
//=============================================================================
//ポーズフラグセット
//=============================================================================
void CGame::SetPauseFlag(bool flag)
{
	m_bPause=flag;
}
//EOF