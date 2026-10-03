//=============================================================================
// タイトル処理 [CTitle.cpp]
// Author : 木村　文登
//=============================================================================
//*****************************************************************************
//定数定義
//*****************************************************************************
#define COUNT_MAX (80)				//花火を発生させる一定の経過時間
#define FIREWORKS_POSY (500.0f)		//パーティクルが発生するY座標
#define RANGE_PARTICLE_NUM (150+150)//パーティクル数の範囲（150～300）
#define RANGE_PARTICLE_SIZE (250+25)//パーティクルサイズの範囲（25～275）

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CTitle.h"
#include "CTitleLogo.h"
#include "CStartLogo.h"
#include "scene.h"
#include "manager.h"
#include "CMeshCylinder.h"
#include "CMeshDoom.h"
#include "CMeshField.h"
#include "CObject.h"
#include "CTree.h"
#include "CParticle.h"
#include "CTutorialBG.h"
#include "CInputKeyboard.h"
#include "CInputJoystick.h"
#include "CFade.h"
#include "CSound.h"
#include <stdlib.h>
#include <time.h>

//=============================================================================
//コンストラクタ
//=============================================================================
CTitle::CTitle()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CTitle::~CTitle()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CTitle::Init()
{
	//乱数の初期化
	srand((unsigned int) time(NULL));

	//経過カウント初期化
	m_nCnt=0;

	//チュートリアル背景表示フラグ
	m_bTutorial=false;

	///////////////////////////////////////////////
	//		タイトル関連オブジェクト生成		//
	/////////////////////////////////////////////

	//タイトルロゴインスタンス生成
	CTitleLogo::Create();

	//スタートロゴ
	CStartLogo::Create();

	//3Dオブジェクトインスタンス生成(起伏床)
	CMeshField::Create("data/TEXTURE/field007.jpg",D3DXVECTOR3(0.0f,0.0f,0.0f),D3DXVECTOR3(0.0f,/*D3DX_PI*0.25f*/0.0f,0.0f),FIELD_NUM_X,FIELD_NUM_Z,FIELDSIZE_X,FIELDSIZE_Z);

	//画面外のフィールド
	CMeshField::Create("data/TEXTURE/ground128.jpg",D3DXVECTOR3(0.0f,0.0f,4500.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),FIELD_NUM_X/2,FIELD_NUM_Z/2,FIELDSIZE_X*2,FIELDSIZE_Z);
	CMeshField::Create("data/TEXTURE/ground128.jpg",D3DXVECTOR3(0.0f,0.0f,-4500.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),FIELD_NUM_X/2,FIELD_NUM_Z/2,FIELDSIZE_X*2,FIELDSIZE_Z);
	CMeshField::Create("data/TEXTURE/ground128.jpg",D3DXVECTOR3(4500.0f,0.0f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),FIELD_NUM_X/2,FIELD_NUM_Z/2,FIELDSIZE_X,FIELDSIZE_Z*2.8f);
	CMeshField::Create("data/TEXTURE/ground128.jpg",D3DXVECTOR3(-4500.0f,0.0f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),FIELD_NUM_X/2,FIELD_NUM_Z/2,FIELDSIZE_X,FIELDSIZE_Z*2.8f);

	//3Dオブジェクト(起伏ドーム)
	CMeshDoom::Create(D3DXVECTOR3(0.0f,0.0f,0.0f),80,80,7000.0f);

	//3Dオブジェクト(起伏筒)
	CMeshCylinder::Create(D3DXVECTOR3(0.0f,0.0f,0.0f),10,1,1000.0f,6000.0f);

	///////////////////////////////////////////////
	//		タイトル関連その他オブジェクト		//
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
	
	//民家２
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

	//タイトルBGM再生
	CSound::PlaySoundA(SOUND_LABEL_BGM_TITLE);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CTitle::Uninit()
{
	//タイトル関連の全シーンを解放
	CScene::ReleaseAll();
}
//=============================================================================
//更新
//=============================================================================
void CTitle::Update()
{
	///////////////////////////////////////////////////
	//		パーティクルをランダムに発生させる		//
	/////////////////////////////////////////////////

	//カウントアップ
	m_nCnt++;

	//カウントが一定時間経過するごとに花火を発生させる
	if(m_nCnt%COUNT_MAX==0)
	{
		//乱数から発生地を決める
		D3DXVECTOR3 createPos=D3DXVECTOR3(	(float)((rand()%FIELD_NUM_X-(FIELD_NUM_X/2))*(int)FIELDSIZE_X),
											FIREWORKS_POSY,
											(float)((rand()%FIELD_NUM_Z-(FIELD_NUM_Z/2))*(int)FIELDSIZE_Z));

		//パーティクルの生成
		CParticle::Create(TYPE_FIREWORKS,rand()%RANGE_PARTICLE_NUM,createPos,(float)(rand()%RANGE_PARTICLE_SIZE),(float)(rand()%RANGE_PARTICLE_SIZE));
	}

	//シーンオブジェクトの更新
	CScene::UpdateAll();

	//エンターキーでゲームへ遷移
	if((CInputKeyboard::GetKeyTrigger(DIK_RETURN)	||
		CInputJoystick::GetPadTrigger(BUTTON_START))&&
		CFade::GetFade()==FADE_NONE					&&
		m_bTutorial==false)
	{
		//開始効果音再生
		CSound::PlaySoundA(SOUND_LABEL_SE_START);

		//チュートリアルを表示させる
		m_bTutorial=true;
		CTutorialBG::Create();
	}
}
//=============================================================================
//描画
//=============================================================================
void CTitle::Draw()
{
}
//EOF