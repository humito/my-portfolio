//=============================================================================
// タイトル管理処理 [Result.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Result.h"
#include "../main.h"
#include "../manager.h"
#include "../Scene/scene.h"
#include "../System/Input/InputKeyboard.h"
#include "../Shader/Fade.h"
#include "../System/renderer.h"
#include "../Object/Mesh/MeshDoom.h"
#include "../Object/Water.h"
#include "../Object/Image.h"

//=============================================================================
//初期化
//=============================================================================
void CResult::Init()
{
	//ドーム生成
	CMeshDoom::Create(	"data/TEXTURE/sky.jpg",
						D3DXVECTOR3(0.0f, 0.0f, 0.0f),
						DOOM_BLOCK_X, DOOM_BLOCK_Y, DOOM_HALF);

	//水面生成
	CWater::Create();

	//メッセージ画像の生成
	D3DXVECTOR3 pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2.0f, 0.0f);
	CImage::Create("data/TEXTURE/result_message.png", pos,
					SCREEN_WIDTH / 1.5f, SCREEN_HEIGHT / 1.5f);
}
//=============================================================================
//終了
//=============================================================================
void CResult::Uninit()
{
	//全オブジェクト削除
	CScene::ReleaseAll();
}
//=============================================================================
//更新
//=============================================================================
void CResult::Update()
{
	//全オブジェクト更新
	CScene::UpdateAll();

	//エンターでタイトルへフェード
	if (CInputKeyboard::GetKeyTrigger(DIK_RETURN))
		CManager::GetRenderer()->GetFade()->StartFade(TITLE_STATE);
}
//=============================================================================
//インスタンス生成
//=============================================================================
CResult *CResult::Create()
{
	CResult *pInstance = new CResult();
	pInstance->Init();
	return pInstance;
}
//EOF