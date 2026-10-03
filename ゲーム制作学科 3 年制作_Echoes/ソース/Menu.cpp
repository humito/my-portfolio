//=============================================================================
// メニュー管理処理 [Menu.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Menu.h"
#include "main.h"
#include "manager.h"
#include "Game.h"
#include "Result.h"
#include "Fade.h"
#include "Sound.h"
#include "InputKeyboard.h"
#include "InputJoystick.h"
#include "Image.h"
#include "BackGround.h"
#include "Ranking.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define DEST_SCALE_A (D3DXVECTOR3(1.3f,1.3f,1.0f))	//目的のスケール1
#define DEST_SCALE_B (D3DXVECTOR3(0.7f,0.7f,1.0f))	//目的のスケール2
#define ADD_TIME (0.01f)							//タイム加算量

//=============================================================================
//コンストラクタ
//=============================================================================
CMenu::CMenu()
{
	//各ポインタNULLセット
	m_pButton[BUTTON_NUM] = {};
	m_pMessage = NULL;
	m_pTutorial = NULL;
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CMenu::Init()
{
	//メニュー選択状態
	m_status = MAIN_SELECT;

	//アニメーションのタイム
	m_fTime = 0.0f;

	//各スケール
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);	//現在
	m_destScl = DEST_SCALE_A;				//目的

	//画面サイズから均等な幅を計算
	float fSize = SCREEN_WIDTH / BUTTON_NUM;
	float fHalfSize = fSize / 2;

	//選択メニュー番号
	m_nSelectIndex = TUTORIAL;
	//前選択メニュー番号
	m_nPrevSelectIndex = TUTORIAL;

	//選択番号最小値
	m_nSelectMin = 0;

	//選択番号最大値
	m_nSelectMax = BUTTON_NUM;

	//背景生成
	CBackGround::Create("data/TEXTURE/back001.png");

	///////////////////////////////////
	//		各ボタン画像の生成		//
	/////////////////////////////////

	//ボタンの数分ループ
	for (int i = 0; i < BUTTON_NUM; ++i)
	{
		//ファイル名
		char *pFileName = NULL;

		///////////////////////////////////////////////////////
		//		各ラベルからテクスチャファイル名を設定		//
		/////////////////////////////////////////////////////
		//チュートリアル
		if (i == TUTORIAL)
			pFileName = "data/TEXTURE/button003.png";

		//ステージ１
		else if (i == STAGE1)
			pFileName = "data/TEXTURE/button000.png";

		//ステージ２
		else if (i == STAGE2)
			pFileName = "data/TEXTURE/button001.png";

		//ステージ３
		else if (i == STAGE3)
			pFileName = "data/TEXTURE/button002.png";

		//ランキング
		else
			pFileName = "data/TEXTURE/button004.png";

		//各ボタンの座標を計算
		D3DXVECTOR3 pos = D3DXVECTOR3(	fHalfSize + (fSize * i),
										fHalfSize,
										0.0f);
		//ボタン画像の生成
		m_pButton[i] = CImage::Create(pFileName, pos, fSize, fSize);
	}

	//メッセージ画像の生成
	m_pMessage = CImage::Create("data/TEXTURE/message000.png",
								D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT - (SCREEN_HEIGHT / 4), 0.0f),
								SCREEN_WIDTH,
								SCREEN_HEIGHT / 2);

	//メニューBGM再生
	CSound::PlaySoundA(SOUND_LABEL_BGM_MENU);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CMenu::Uninit()
{
	//全サウンド停止
	CSound::StopSound();

	//全シーン解放
	CScene::ReleaseAll();
}
//=============================================================================
//更新
//=============================================================================
void CMenu::Update()
{
	//メニュー操作
	MenuInput();

	//選択番号が切り替わった場合
	if (m_nPrevSelectIndex != m_nSelectIndex)
	{
		//前のボタンのスケールを元に戻す
		m_pButton[m_nPrevSelectIndex]->SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));

		///////////////////////////////////////////
		//		メッセージのテクスチャ変更		//
		/////////////////////////////////////////
		//ファイル名
		char *pFileName = NULL;

		//チュートリアル
		if (m_nSelectIndex == TUTORIAL)
			pFileName = "data/TEXTURE/message000.png";

		//ステージ１
		else if (m_nSelectIndex == STAGE1)
			pFileName = "data/TEXTURE/message001.png";

		//ステージ２
		else if (m_nSelectIndex == STAGE2)
			pFileName = "data/TEXTURE/message002.png";

		//ステージ３
		else if (m_nSelectIndex == STAGE3)
			pFileName = "data/TEXTURE/message003.png";

		//ランキング
		else
			pFileName = "data/TEXTURE/message004.png";

		//メッセージのテクスチャ貼り替え
		m_pMessage->ChangeTexture(pFileName);

		//チュートリアル画像がある場合
		if (m_pTutorial != NULL)
		{
			///////////////////////////////////////////////
			//		チュートリアルのテクスチャ変更		//
			/////////////////////////////////////////////
			//チュートリアル用のファイル名
			char *pTutorialFile = NULL;

			if (m_nSelectIndex == TUTORIAL
			||	m_nSelectIndex == STAGE2
			||	m_nSelectIndex == RANKING)
			{
				pTutorialFile = "data/TEXTURE/tutorial000.png";
			}
			else
			{
				pTutorialFile = "data/TEXTURE/tutorial001.png";
			}

			//チュートリアルのテクスチャ貼り替え
			m_pTutorial->ChangeTexture(pTutorialFile);
		}
	}

	//ボタンアニメーション
	ButtonAnim();

	//前の選択番号を更新
	m_nPrevSelectIndex = m_nSelectIndex;

	//シーンオブジェクトの更新
	CScene::UpdateAll();
}
//=============================================================================
//メニュー操作
//=============================================================================
void CMenu::MenuInput()
{
	//右ボタン(次のメニュー番号)
	if(CInputKeyboard::GetKeyTrigger(DIK_D)
	|| CInputKeyboard::GetKeyTrigger(DIK_RIGHT)
	|| CInputJoystick::GetPadTrigger(STICK_L_RIGHT)
	|| CInputJoystick::GetPadTrigger(ARROW_RIGHT))
	{
		//次の番号
		m_nSelectIndex = (m_nSelectIndex + 1) % m_nSelectMax;

		//ランキング選択時のみ0番目にいかせない
		if (m_nSelectMax == RANKING
		&&	m_nSelectIndex == 0)
			m_nSelectIndex++;

		//効果音再生
		CSound::PlaySoundA(SOUND_LABEL_SE_NEXT);
	}

	//左ボタン(前のメニュー番号)
	else if (CInputKeyboard::GetKeyTrigger(DIK_A)
	||		 CInputKeyboard::GetKeyTrigger(DIK_LEFT)
	||		 CInputJoystick::GetPadTrigger(STICK_L_LEFT)
	||		 CInputJoystick::GetPadTrigger(ARROW_LEFT))
	{
		//前の番号
		m_nSelectIndex--;
		//効果音再生
		CSound::PlaySoundA(SOUND_LABEL_SE_PREV);
	}

	//メニュー番号が最小より下になる場合、ボタン数分加算
	if (m_nSelectIndex < m_nSelectMin)
	{
		//ランキング選択時のみ番号減算
		int nSubIndex = 0;
		if (m_nSelectMax == RANKING)
			nSubIndex = 1;

		m_nSelectIndex += (m_nSelectMax - nSubIndex);
	}

	//決定ボタン
	if (CInputKeyboard::GetKeyTrigger(DIK_RETURN)
	||	CInputJoystick::GetPadTrigger(BUTTON_1)
	||	CInputJoystick::GetPadTrigger(BUTTON_START))
	{
		//チュートリアル表示されてる場合終了(非表示)
		if (m_pTutorial != NULL)
		{
			//終了
			m_pTutorial->Uninit();
			m_pTutorial = NULL;
			//メインセレクトへ
			m_status = MAIN_SELECT;
		}

		//ステージを選択してるならゲームにセット
		else if (m_nSelectIndex >= STAGE1
			&&	 m_nSelectIndex <= STAGE3)
		{
			//メインセレクト時の場合
			if (m_status == MAIN_SELECT)
			{
				//ゲームにステージ情報セット
				CGame::SetStage(m_nSelectIndex);

				//ゲームへ遷移
				CFade::SetFade(GAME_MODE);
			}

			//ランキングセレクト時の場合
			else if (m_status == STAGE_RANK_SELECT)
			{
				//前ステートセット
				CResult::SetPrevState(PREV_MENU);

				//ステージ情報セット
				CRanking::SetStage(m_nSelectIndex);

				//リザルトへ遷移
				CFade::SetFade(RESULT_MODE);
			}
		}

		//チュートリアル選択してる場合
		else if (m_nSelectIndex == TUTORIAL)
		{
			//チュートリアルの表示
			if (m_pTutorial == NULL)
			{
				//チュートリアル画像生成
				m_pTutorial = CImage::Create(	"data/TEXTURE/tutorial000.png",
												D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f),
												SCREEN_WIDTH,
												SCREEN_HEIGHT);
			}

			//番号リセット
			m_nSelectIndex = TUTORIAL;

			//チュートリアルへ変更
			m_status = TUTORIAL_MODE;
		}

		//ランキングの場合
		else
		{
			//ランキングステータスへ変更
			m_status = STAGE_RANK_SELECT;

			//選択番号をステージ１にする
			m_nSelectIndex = STAGE1;

			//選択番号最小最大設定
			m_nSelectMin = STAGE1;
			m_nSelectMax = RANKING;

			//チュートリアルとランキング非表示
			m_pButton[TUTORIAL]->SetDraw(false);
			m_pButton[RANKING]->SetDraw(false);
		}
	}//決定ボタン

	//戻るボタン
	if (CInputKeyboard::GetKeyTrigger(DIK_BACKSPACE)
	||	CInputJoystick::GetPadTrigger(BUTTON_2))
	{
		//メインセレクト時の処理
		if (m_status == MAIN_SELECT)
		{
			//タイトルへ遷移
			CFade::SetFade(TITLE_MODE);
		}

		//ランキングセレクト時又は、チュートリアル時
		else if (m_status == STAGE_RANK_SELECT
			||	 m_status == TUTORIAL_MODE)
		{
			//メインセレクトに戻す
			m_status = MAIN_SELECT;

			//選択番号最小最大設定
			m_nSelectMin = TUTORIAL;
			m_nSelectMax = BUTTON_NUM;

			//チュートリアルとランキング表示
			m_pButton[TUTORIAL]->SetDraw(true);
			m_pButton[RANKING]->SetDraw(true);
		}

		//チュートリアル画像がある場合終了(非表示)
		if (m_pTutorial != NULL)
		{
			m_pTutorial->Uninit();
			m_pTutorial = NULL;
		}
	}//戻るボタン
}
//=============================================================================
//ボタンアニメーション
//=============================================================================
void CMenu::ButtonAnim()
{
	///////////////////////////////////////////////////////
	//		選択したボタンのスケールアニメーション		//
	/////////////////////////////////////////////////////

	//タイムの加算
	m_fTime += ADD_TIME;

	//目的に近づくスケールの計算
	m_scl = D3DXVECTOR3(m_destScl.x * m_fTime + m_scl.x * (1.0f - m_fTime),
		m_destScl.y * m_fTime + m_scl.y * (1.0f - m_fTime),
		1.0f);

	//現在のスケールをセット
	m_pButton[m_nSelectIndex]->SetScale(m_scl);

	//スケールが目的に達した場合
	if (m_scl == m_destScl)
	{
		//タイムリセット
		m_fTime = 0.0f;

		//目的のスケールの変更
		if (m_destScl == DEST_SCALE_A)
			m_destScl = DEST_SCALE_B;
		else
			m_destScl = DEST_SCALE_A;
	}
}
//EOF