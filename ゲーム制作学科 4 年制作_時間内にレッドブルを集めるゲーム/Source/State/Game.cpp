//=============================================================================
// タイトル管理処理 [Game.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Game.h"
#include "../Object/Score.h"
#include "../main.h"
#include "../manager.h"
#include "../Scene/scene.h"
#include "../Object/Player.h"
#include "../Object/Item.h"
#include "../Object/Time.h"
#include "../Object/Mesh/MeshField.h"
#include "../Object/Mesh/MeshDoom.h"
#include "../System/Input/InputKeyboard.h"
#include "../Shader/Fade.h"
#include "../System/renderer.h"
#include <stdio.h>
#include <time.h>

//*****************************************************************************
//定数定義
//*****************************************************************************
#define ITEM_BEGIN_SET_NUM (10)//開始時に生成されるアイテムの数
#define FIELD_LIMIT_SIZE (252.0f)			//フィールド範囲内のサイズ
#define FIELD_SIZE (FIELD_LIMIT_SIZE * 2)	//フィールド全体のサイズ

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
int CGame::m_nNumItem = 0;			//所持アイテム数
CMeshField *CGame::m_pField = NULL;	//フィールドインスタンス
CPlayer *CGame::m_pPlayer = NULL;	//プレイヤーインスタンス

//=============================================================================
//初期化
//=============================================================================
void CGame::Init()
{
	//乱数初期化
	srand((unsigned int)time(NULL));

	//プレイヤー生成
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f),
								D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//フィールド生成
	m_pField = CMeshField::Create(NO_LOAD_FIELD,
									NULL,
									"data/TEXTURE/stone-blocks.jpg",
									D3DXVECTOR3(0.0f, 0.0f, 0.0f),
									D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//ドーム生成
	CMeshDoom::Create("data/TEXTURE/sky.jpg",
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					DOOM_BLOCK_X, DOOM_BLOCK_Y, DOOM_HALF);

	//タイム生成
	CTime::Create(D3DXVECTOR3((SCREEN_WIDTH / 2) - (TIME_WIDTH / 2), 0.0f, 0.0f),
				TIME_WIDTH,
				TIME_HEIGHT,
				1,
				DISP_2DPOLYGON,
				TIME_DIGIT_MAX);

	//所持アイテム数リセット
	m_nNumItem = 0;

	//アイテム生成
	AppItems();
}
//=============================================================================
//終了
//=============================================================================
void CGame::Uninit()
{
	//所持アイテム数をセット
	CScore::SetScore(m_nNumItem);

	//アイテムのテクスチャ配列解放
	CItem::DeleteTextureArray();

	//全オブジェクト削除
	CScene::ReleaseAll();
}
//=============================================================================
//更新
//=============================================================================
void CGame::Update()
{
	//全オブジェクト更新
	CScene::UpdateAll();

//デバッグ時のみキーで遷移できる
#ifdef _DEBUG
	if (CInputKeyboard::GetKeyTrigger(DIK_RETURN))
		CManager::GetRenderer()->GetFade()->StartFade(RESULT_STATE);
#endif
}
//=============================================================================
//一定量のアイテム生成
//=============================================================================
void CGame::AppItems()
{
	//一定量のアイテムの生成
	for (int i = 0; i < ITEM_BEGIN_SET_NUM; ++i)
	{
		D3DXVECTOR3 startPos = D3DXVECTOR3(-FIELD_LIMIT_SIZE + (float)(rand() % (int)FIELD_SIZE),
											10.0f,
											-FIELD_LIMIT_SIZE + (float)(rand() % (int)FIELD_SIZE));

		CItem::Create(startPos, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}
}
//=============================================================================
//所持アイテム数加算
//=============================================================================
void CGame::AddItemNum()
{
	//所持数加算
	m_nNumItem++;

	//フィールドのファーを掛ける回数セット
	if (m_nNumItem % 10 == 0)
		m_pField->SetFurTimes(m_nNumItem);
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