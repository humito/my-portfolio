//=============================================================================
//スコア表示処理[Score.cpp]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _SCORE_H_
#define _SCORE_H_
//*****************************************************************************
//定数定義
//*****************************************************************************
#define SCORE_RESULT_MODE (0)	//リザルト用スコア
#define SOCRE_GAME_MODE (1)		//ゲーム用スコア
#define SCORE_DIGIT_MAX (5)		//桁数
#define SCORE_WIDTH (300.0f)	//スコアの幅
#define SCORE_HEIGHT (100.0f)	//スコアの高さ

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "../Scene/scene2D.h"
#include "Number.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//スコアクラス
class CScore : public CScene2D
{
	//外部
	public:
		CScore(){}														//コンストラクタ
		~CScore(){}														//デストラクタ
		HRESULT Init(D3DXVECTOR3 pos, float fWidth, float fHeight,		//初期化
					int mode, NUMBER_DISP type, int nDigitNum);		
		void Uninit();													//終了
		void Update();													//更新
		void Draw();													//描画
		static void AddScore(int num);									//スコア加算
		static void SetScore(int num);									//スコアのセット
		static void Create(D3DXVECTOR3 pos, float fWidth, float fHeight,//インスタンス生成
						int mode, NUMBER_DISP type, int nDigitNum);	

	//内部
	private:
		static int m_nScore;											//スコアの数値
		CNumber m_Number;												//表示用数値インスタンス
		int m_nScoreMode;												//スコアモード
};
#endif
//EOF