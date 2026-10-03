//=============================================================================
// ランキング処理 [Ranking.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#define _CRT_SECURE_NO_WARNINGS	//警告対策用
#include "Ranking.h"
#include "Menu.h"
#include <stdio.h>

//*****************************************************************************
//定数定義
//*****************************************************************************
#define RANKING_START_POS (200.0f)
#define RANKING_SPACE_POS (100.0f)
#define RANKING_MOVE_RATE (0.05f)
#define RANK_NUM_WIDTH (60.0f)
#define RANK_NUM_SPACE (20.0f)

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
int CRanking::m_nNewScore = 0;					//新しいスコア
int CRanking::m_nScoreAll[RANK_MAX + 1] = {};	//全ランクスコア
int CRanking::m_nRank = 0;						//ランク
int CRanking::m_nStageMenu = 0;					//ステージメニュー

//=============================================================================
//コンストラクタ
//=============================================================================
CRanking::CRanking()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CRanking::~CRanking()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CRanking::Init(	D3DXVECTOR3 pos,
						float fWidth,
						float fHeight,
						NUMBER_DISP type,
						int nDigitNum)
{
	//座標
	m_pos = pos;

	//自身の持つスコアを代入
	m_nScore = m_nScoreAll[m_nRank];
	
	//目的の位置を設定
	m_fDestPosY = RANKING_START_POS + (m_nRank * RANKING_SPACE_POS);

	//次のランクへ
	m_nRank++;

	//数字インスタンス初期化
	m_Number.Init(	"data/TEXTURE/number002.png", pos, type,
					fWidth, fHeight, nDigitNum);

	//順位数字インスタンス初期化
	m_RankNumber.Init(	"data/TEXTURE/number001.png", pos, type,
						RANK_NUM_WIDTH, fHeight, 1);

	//数値セット
	m_Number.SetValue(m_nScore);
	//順位セット
	m_RankNumber.SetValue(m_nRank);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CRanking::Uninit()
{
	//数値の終了
	m_Number.Uninit();

	//順位用数値終了
	m_RankNumber.Uninit();

	//自身の終了
	this->Release();
}
//=============================================================================
//更新
//=============================================================================
void CRanking::Update()
{
	m_pos.y = m_pos.y + (m_fDestPosY - m_pos.y) * RANKING_MOVE_RATE;

	//数値に座標セット
	m_Number.SetPos(m_pos);

	m_RankNumber.SetPos(D3DXVECTOR3(m_pos.x - (RANK_NUM_WIDTH + RANK_NUM_SPACE),
									m_pos.y,
									0.0f));
}
//=============================================================================
//描画
//=============================================================================
void CRanking::Draw()
{
	//数値の描画
	m_Number.Draw();
	//順位用数値描画
	m_RankNumber.Draw();
}
//=============================================================================
//スコアの並び替え
//=============================================================================
void CRanking::RankSort()
{
	//ファイルポインタ
	FILE *pFile = NULL;

	//ファイル名
	char *pFileName = NULL;

	//ランクリセット
	m_nRank = 0;

	///////////////////////////////////////////////////////////////
	//		ステージ番号からランキングのファイル名を設定		//
	/////////////////////////////////////////////////////////////
	//ステージ１
	if (m_nStageMenu == STAGE1)
	{
		pFileName = "data/SAVE_DATA/save_score0.dat";
	}
	//ステージ２
	else if (m_nStageMenu == STAGE2)
	{
		pFileName = "data/SAVE_DATA/save_score1.dat";
	}
	//ステージ３
	else
	{
		pFileName = "data/SAVE_DATA/save_score2.dat";
	}

	///////////////////////////////
	//		スコアの読込		//
	/////////////////////////////
	//ファイルオープン
	pFile = fopen(pFileName, "rb");

	//ファイルが存在する場合
	if (pFile != NULL)
	{
		//スコアを読込
		fread(m_nScoreAll, sizeof(int), RANK_MAX, pFile);
	}

	//最後のランクに新しいスコアを代入
	m_nScoreAll[RANK_MAX] = m_nNewScore;

	//新しいスコアリセット
	m_nNewScore = 0;

	///////////////////////////////////
	//		スコアの並び替え		//
	/////////////////////////////////
	//降順に並び替え
	for (int i = 0; i < RANK_MAX; ++i)
	{
		for (int j = i + 1; j < RANK_MAX + 1; ++j)
		{
			//i番目の値がj番目の値より小さい場合
			if (m_nScoreAll[i] < m_nScoreAll[j])
			{
				//並び替え
				int nWork = m_nScoreAll[i];
				m_nScoreAll[i] = m_nScoreAll[j];
				m_nScoreAll[j] = nWork;
			}
		}
	}

	//ファイル解放
	if (pFile != NULL)
	{
		fclose(pFile);
	}

	///////////////////////////////////
	//		スコアの書き込み		//
	/////////////////////////////////
	//ファイルオープン
	pFile = fopen(pFileName, "wb");

	//ファイルが存在する場合
	if (pFile != NULL)
	{
		//スコアを書き込み
		fwrite(m_nScoreAll, sizeof(int), RANK_MAX, pFile);
	}

	//ファイル解放
	fclose(pFile);
}
//=============================================================================
//ゲーム内スコアのセット
//=============================================================================
void CRanking::SetScore(int nScore)
{
	m_nNewScore = nScore;
}
//=============================================================================
//インスタンス生成
//=============================================================================
CRanking *CRanking::Create(	D3DXVECTOR3 pos,
							float fWidth,
							float fHeight,
							NUMBER_DISP type,
							int nDigitNum)
{
	//インスタンス生成
	CRanking *pRanking = new CRanking();
	//初期化
	pRanking->Init(pos, fWidth, fHeight, type, nDigitNum);
	return pRanking;
}
//EOF