//=============================================================================
// タイトル管理処理 [Title.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Title.h"
#include "../manager.h"
#include "../main.h"
#include "../Scene/scene.h"
#include "../System/Input/InputKeyboard.h"
#include "../Shader/Fade.h"
#include "../Object/Object.h"
#include "../Object/Image.h"
#include "../Object/Mesh/MeshDoom.h"
#include "../Object/Mesh/MeshField.h"
#include "../System/renderer.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define MODEL_FILE_NAME ("data/MODEL/forklift.x")				//モデルファイル名
#define FIELD_TEX_FILE_NAME ("data/TEXTURE/stone-blocks.jpg")	//フィールドテクスチャファイル名
#define DOOM_TEX_FILE_NAME ("data/TEXTURE/sky.jpg")				//ドームテクスチャファイル名
#define LOGO_TEX_FILE_NAME ("data/TEXTURE/title_logo.png")		//タイトルロゴ

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
CMeshField *CTitle::m_pField = NULL;

//=============================================================================
//初期化
//=============================================================================
void CTitle::Init()
{
	//オブジェクト生成
	CObject::Create(MODEL_FILE_NAME, D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//フィールド生成
	m_pField = CMeshField::Create(NO_LOAD_FIELD, NULL, FIELD_TEX_FILE_NAME,
								D3DXVECTOR3(0.0f, 0.0f, 0.0f),
								D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//ドーム生成
	CMeshDoom::Create(	DOOM_TEX_FILE_NAME, D3DXVECTOR3(0.0f, 0.0f, 0.0f),
						DOOM_BLOCK_X, DOOM_BLOCK_Y, DOOM_HALF);

	//タイトルロゴ
	D3DXVECTOR3 pos = D3DXVECTOR3(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 5.0f) / 2.0f, 0.0f);
	CImage::Create(	LOGO_TEX_FILE_NAME, pos,
					SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 3.5f);
}
//=============================================================================
//終了
//=============================================================================
void CTitle::Uninit()
{
	//全オブジェクト削除
	CScene::ReleaseAll();
}
//=============================================================================
//更新
//=============================================================================
void CTitle::Update()
{
	//全オブジェクト更新
	CScene::UpdateAll();

	//エンターでゲームへフェード
	if (CInputKeyboard::GetKeyTrigger(DIK_RETURN))
		CManager::GetRenderer()->GetFade()->StartFade(GAME_STATE);
}
//=============================================================================
//インスタンス生成
//=============================================================================
CTitle *CTitle::Create()
{
	CTitle *pInstance = new CTitle();
	pInstance->Init();
	return pInstance;
}
//EOF