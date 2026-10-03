//=============================================================================
// リザルト管理処理 [Result.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Result.h"
#include "scene.h"
#include "BackGround.h"
#include "Score.h"
#include "Ranking.h"
#include "Image.h"
#include "Fade.h"
#include "Sound.h"
#include "InputKeyboard.h"
#include "InputJoystick.h"
#include "Menu.h"
#include "manager.h"
#include "main.h"

//*****************************************************************************
//スタティックメンバ変数
//*****************************************************************************
bool CResult::m_bRankOpen = false;
int CResult::m_nPrevState = 0;

//*****************************************************************************
//定数定義
//*****************************************************************************
#define STAGE_POS (D3DXVECTOR3(150.0f, 50.0f, 0.0f))		//ステージ番号の座標
#define STAGE_WIDTH (300.0f)								//ステージ番号の幅
#define STAGE_HEIGHT (100.0f)								//ステージ番号の高さ
#define RANK_POS (D3DXVECTOR3(SCREEN_WIDTH/2,120.0f,0.0f))	//ランキング画像の座標
#define RANKIMAGE_WIDTH (500.0f)							//ランキング画像の幅
#define RANKIMAGE_HEIGHT (150.0f)							//ランキング画像の高さ

//=============================================================================
//初期化
//=============================================================================
HRESULT CResult::Init()
{
	//画像のファイル名
	char *pFileName = NULL;

	//ランキングの持ってるステージ情報取得
	int nStage = CRanking::GetStage();

	//ステージからファイル名を分ける
	//ステージ１
	if (nStage == STAGE1)
		pFileName = "data/TEXTURE/stage1.png";
	//ステージ２
	else if (nStage == STAGE2)
		pFileName = "data/TEXTURE/stage2.png";
	//ステージ３
	else
		pFileName = "data/TEXTURE/stage3.png";

	//背景生成
	CBackGround::Create("data/TEXTURE/back000.jpg");

	//ステージ文字画像生成
	CImage::Create(	pFileName,
					STAGE_POS,
					STAGE_WIDTH, STAGE_HEIGHT);

	//ランキング画像
	CImage::Create(	"data/TEXTURE/ranking.png",
					RANK_POS,
					RANKIMAGE_WIDTH, RANKIMAGE_HEIGHT);

	//スコア生成
	CScore::Create(D3DXVECTOR3(SCREEN_WIDTH - SCORE_WIDTH, 0.0f, 0.0f),
					SCORE_WIDTH,
					SCORE_HEIGHT,
					SCORE_RESULT_MODE,
					DISP_2DPOLYGON,
					SCORE_DIGIT_MAX);

	//ランキングのソート
	CRanking::RankSort();

	//ランキング表示フラグ
	m_bRankOpen = false;

	//リザルトBGM再生
	CSound::PlaySoundA(SOUND_LABEL_BGM_RESULT);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CResult::Uninit()
{
	//全サウンド停止
	CSound::StopSound();

	//全シーン解放
	CScene::ReleaseAll();
}
//=============================================================================
//更新
//=============================================================================
void CResult::Update()
{
	//シーンオブジェクトの更新
	CScene::UpdateAll();

	//Enterキーでタイトルへ遷移
	if (CInputKeyboard::GetKeyTrigger(DIK_RETURN)
	||  CInputJoystick::GetPadTrigger(BUTTON_START)
	||  CInputJoystick::GetPadTrigger(BUTTON_1))
	{
		//ランキング表示されてるなら
		if (m_bRankOpen)
		{
			//エコー効果音再生
			CSound::PlaySoundA(SOUND_LABEL_SE_ECHOES);

			//前ステートがゲームなら
			if (m_nPrevState == PREV_GAME)
			{
				//タイトルへ遷移
				CFade::SetFade(TITLE_MODE);
			}
			//前ステートがメニューなら
			else
			{
				//メニューへ遷移
				CFade::SetFade(MENU_MODE);
			}
		}

		//ランキングを表示
		else
		{
			//ランキング効果音再生
			CSound::PlaySoundA(SOUND_LABEL_SE_RANKING);

			//ランキングスコアの生成
			for (int i = 0; i < RANK_MAX; ++i)
			{
				D3DXVECTOR3 setPos = D3DXVECTOR3((SCREEN_WIDTH / 2) - (RANK_WIDTH / 2), SCREEN_HEIGHT, 0.0f);
				CRanking::Create(	setPos,
									RANK_WIDTH,
									RANK_HEIGHT,
									DISP_2DPOLYGON,
									SCORE_DIGIT_MAX);
			}

			//ランキング表示フラグtrue
			m_bRankOpen = true;
		}
	}
}
//EOF