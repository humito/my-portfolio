//=============================================================================
//所持数表示処理[CItemPossession.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CITEMPOSSESSION_H_
#define _CITEMPOSSESSION_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "scene2D.h"
#include "CNumber.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//所持数表示クラス
class CPossessionNum : public CScene2D
{
	//外部
	public:
		CPossessionNum();										//コンストラクタ
		~CPossessionNum();										//デストラクタ
		HRESULT Init(D3DXVECTOR3 pos,NUMBER_DISP type);			//初期化
		void Uninit();											//終了
		void Update();											//更新
		void Draw();											//描画
		static void Create(D3DXVECTOR3 pos,NUMBER_DISP type);	//インスタンス生成
		static void SetNum(int num);							//数値のセット
	
	//内部
	private:
		static int m_nNumItems;									//アイテム所持数
		int m_nUpperLimit;										//有効範囲の上限
		int m_nLowerLimit;										//有効範囲の下限
		int m_nDigitNum;										//桁数
		NUMBER_DISP m_dispType;									//保存用の表示タイプ
		CNumber m_Number;										//数値表示用インスタンス
};
#endif
//EOF