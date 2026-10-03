//=============================================================================
// ランキング処理 [Ranking.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _RANKING_H_
#define _RANKING_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <windows.h>
#include "scene2D.h"
#include "Number.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define RANK_MAX (5)		//ランク数
#define RANK_WIDTH (300.0f)	//ランキング幅
#define RANK_HEIGHT (90.0f)	//ランキング高さ

//*****************************************************************************
//クラス定義
//*****************************************************************************
//ランキングクラス
class CRanking : CScene2D
{
	//外部
	public:
		CRanking();								//コンストラクタ
		~CRanking();							//デストラクタ

		HRESULT Init(	D3DXVECTOR3 pos,		//初期化
						float fWidth,
						float fHeight,
						NUMBER_DISP type,
						int nDigitNum);

		void Uninit();							//終了
		void Update();							//更新
		void Draw();							//描画
		static CRanking *Create(D3DXVECTOR3 pos,//インスタンス生成
								float fWidth,
								float fHeight,
								NUMBER_DISP type,
								int nDigitNum);

		static void RankSort();					//ランキングスコアの並び替え

		static void SetScore(int nScore);		//ゲーム内スコアのセット

		static void SetStage(int nStage)		//ステージ番号のセット
		{ m_nStageMenu = nStage; }

		static int GetStage()					//ステージ番号の取得
		{ return m_nStageMenu; }

	//内部
	private:
		CNumber m_Number;						//数字インスタンス
		CNumber m_RankNumber;					//順位数字インスタンス
		int m_nScore;							//自身のスコア
		float m_fDestPosY;						//目的のY座標
		static int m_nScoreAll[RANK_MAX + 1 ];	//全ランクスコア
		static int m_nRank;						//ランク
		static int m_nNewScore;					//ゲームで獲得した新しいスコア
		static int m_nStageMenu;				//ステージメニュー
};
#endif
//EOF