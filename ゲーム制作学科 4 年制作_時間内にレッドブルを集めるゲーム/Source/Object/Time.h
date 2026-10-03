//=============================================================================
// タイム処理 [Time.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _TIME_H_
#define _TIME_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "../Scene/scene2D.h"
#include "Number.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define TIME_DIGIT_MAX (2)	//桁数
#define TIME_WIDTH (100.0f)	//タイムの幅
#define TIME_HEIGHT (110.0f)//タイムの高さ

//*****************************************************************************
//クラス定義
//*****************************************************************************
//タイムクラス
class CTime : public CScene2D
{
	//外部
	public:
		CTime(){}														//コンストラクタ
		~CTime(){}														//デストラクタ
		HRESULT Init(D3DXVECTOR3 pos, float fWidth, float fHeight,		//初期化
					int mode, NUMBER_DISP type, int nDigitNum);
		void Uninit();													//終了
		void Update();													//更新
		void Draw();													//描画
		static void AddTime(float fTime);								//タイム加算
		static void Create(D3DXVECTOR3 pos, float fWidth, float fHeight,//インスタンス生成
						int mode, NUMBER_DISP type, int nDigitNum);
	//内部
	private:
		static float m_fTime;											//タイムの数値
		CNumber m_Number;												//表示用数値インスタンス
		bool m_bTimeUp;													//タイムアップフラグ
};
#endif
//EOF