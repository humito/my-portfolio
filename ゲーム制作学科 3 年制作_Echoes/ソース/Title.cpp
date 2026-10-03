//=============================================================================
// タイトル処理 [Title.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Title.h"
#include "scene.h"
#include "Object.h"
#include "Enemy.h"
#include "MeshDoom.h"
#include "MeshField.h"
#include "Image.h"
#include "Particle.h"
#include "Fade.h"
#include "Sound.h"
#include "InputKeyboard.h"
#include "InputJoystick.h"
#include "manager.h"
#include "main.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define OBJECT_POS (D3DXVECTOR3(-25.0f, 0.0f, -30.0f))					//オブジェクトの座標
#define OBJECT_ROT (D3DXVECTOR3(0.0f, 10.0f, 0.0f))						//オブジェクトの角度
#define LOGO_POS (D3DXVECTOR3(SCREEN_WIDTH / 2, 150.0f, 0.0f))			//タイトルロゴの座標
#define LOGO_WIDTH (600.0f)												//タイトルロゴの幅
#define LOGO_HEIGHT (300.0f)											//タイトルロゴの高さ
#define PRESS_KEY_POS (D3DXVECTOR3(SCREEN_WIDTH / 2, 550.0f, 0.0f))		//プレスキーの座標
#define PRESS_KEY_WIDTH (500.0f)										//プレスキーの幅
#define PRESS_KEY_HEIGHT (200.0f)										//プレスキーの高さ
#define QR_MAIL_POS (D3DXVECTOR3(SCREEN_WIDTH - 210.0f, 650.0f, 0.0f))	//メールQRコードの座標
#define QR_FB_POS (D3DXVECTOR3(SCREEN_WIDTH - 90.0f, 650.0f, 0.0f))		//FBQRコードの座標
#define QR_SIZE (100.0f)												//QRコード共通のサイズ
#define PARTICLE_CREATE_COUNT (50)										//パーティクル生成するカウント
#define PARTICLE_POS_X (30)												//パーティクル発生位置X
#define PARTICLE_POS_Z (100)											//パーティクル発生位置Z
#define PARTICLE_NUM (100)												//パーティクル発生数
#define PARTICLE_SIZE (25)												//パーティクルサイズ

//=============================================================================
//初期化
//=============================================================================
HRESULT CTitle::Init()
{
	//カウント初期化
	m_nCount = 0;

	//オブジェクト
	CObject::Create("data/MODEL/light_body.x",
					OBJECT_POS,
					OBJECT_ROT);

	//ドーム生成
	CMeshDoom::Create("data/TEXTURE/sky000.jpg",
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					DOOM_BLOCK_X,
					DOOM_BLOCK_Y,
					DOOM_HALF);

	//フィールド生成
	CMeshField::Create(	NO_LOAD_FIELD,
						NULL,
						"data/TEXTURE/field001.jpg",
						D3DXVECTOR3(0.0f, 0.0f, 0.0f),
						D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//タイトルロゴ
	CImage::Create(	"data/TEXTURE/title_logo.png",
					LOGO_POS,
					LOGO_WIDTH, LOGO_HEIGHT);

	//プレスキー
	CImage::Create(	"data/TEXTURE/press_key.png",
					PRESS_KEY_POS,
					PRESS_KEY_WIDTH, PRESS_KEY_HEIGHT);

	//QRコード生成(Mail,FaceBook)
	CImage::Create(	"data/TEXTURE/QR_GMail.png",
					QR_MAIL_POS,
					QR_SIZE, QR_SIZE);
	CImage::Create(	"data/TEXTURE/QR_FB.png",
					QR_FB_POS,
					QR_SIZE, QR_SIZE);

	//タイトルBGM再生
	CSound::PlaySoundA(SOUND_LABEL_BGM_TITLE);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CTitle::Uninit()
{
	//全サウンド停止
	CSound::StopSound();

	//全シーン解放
	CScene::ReleaseAll();
}
//=============================================================================
//更新
//=============================================================================
void CTitle::Update()
{
	//カウントアップ
	m_nCount++;

	//シーンオブジェクトの更新
	CScene::UpdateAll();

	//エンターキーでゲーム開始
	if (CInputKeyboard::GetKeyTrigger(DIK_RETURN)
	||  CInputJoystick::GetPadTrigger(BUTTON_START)
	||  CInputJoystick::GetPadTrigger(BUTTON_1))
	{
		CSound::PlaySoundA(SOUND_LABEL_SE_ECHOES);
		CFade::SetFade(MENU_MODE);
	}

	//パーティクルを試す
	if (m_nCount % PARTICLE_CREATE_COUNT == 0)
	{
		D3DXVECTOR3 pos = D3DXVECTOR3((float)(rand() % PARTICLE_POS_X), 0, (float)(rand() % PARTICLE_POS_Z));
		CParticle::Create(	TYPE_EFFECT, 
							rand() % PARTICLE_NUM,
							pos, 
							(float)(rand() % PARTICLE_SIZE), 
							(float)(rand() % PARTICLE_SIZE));
	}
}
//EOF