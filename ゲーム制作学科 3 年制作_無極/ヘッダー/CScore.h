//=============================================================================
//スコア表示処理[CScore.cpp]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CSCORE_H_
#define _CSCORE_H_
//*****************************************************************************
//定数定義
//*****************************************************************************
#define SCORE_RESULT_MODE (0)	//リザルト用スコア
#define SOCRE_GAME_MODE (1)		//ゲーム用スコア
#define SCORE_DIGIT_MAX (5)		//桁数
#define ADD_SCORE_NUM (10)		//スコア加算量

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "scene2D.h"
#include "CNumber.h"
//*****************************************************************************
//クラス定義
//*****************************************************************************
//スコアクラス
class CScore : public CScene2D
{
	//外部
	public:
		CScore();																								//コンストラクタ
		~CScore();																								//デストラクタ
		HRESULT Init(D3DXVECTOR3 pos,float fWidth,float fHeight,int mode,NUMBER_DISP type,int nDigitNum);		//初期化
		void Uninit();																							//終了
		void Update();																							//更新
		void Draw();																							//描画
		static void AddScore(int num);																			//スコア加算
		static void Create(D3DXVECTOR3 pos,float fWidth,float fHeight,int mode,NUMBER_DISP type,int nDigitNum);	//インスタンス生成

	//内部
	private:
		static int m_nScore;																					//スコアの数値
		CNumber m_Number;																						//表示用数値インスタンス
};

#endif
//EOF