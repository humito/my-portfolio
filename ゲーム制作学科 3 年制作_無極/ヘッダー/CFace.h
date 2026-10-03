//=============================================================================
//キャラクター表情処理[CFace.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CFACE_H_
#define _CFACE_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "scene2D.h"

//*****************************************************************************
//構造体定義
//*****************************************************************************
//顔の表情
enum EXPRESSION
{
	NORMAL_FACE=0,	//通常の顔
	SURPRISE_FACE,	//驚いた顔
	BEATEN_FACE,	//やられ顔
	HAPPY_FACE,		//喜ぶ顔
	EXPRESSION_MAX	//表情の種類数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//表情クラス
class CFace : public CScene2D
{
	//外部
	public:
		CFace();								//コンストラクタ
		~CFace();								//デストラクタ
		HRESULT Init();							//初期化
		void Uninit();							//終了
		void Update();							//更新
		void Draw();							//描画
		static void Create();					//インスタンス生成
		static void SetFace(EXPRESSION face);	//表情のセット
		static void SetItemNum(int num);		//アイテム数のセット

	//内部
	private:
		static int m_nItemNum;					//アイテム数
		static EXPRESSION m_face;				//現在の表情
		static int m_nWaitCnt;					//待ちカウント

		//頂点情報の変更
		void ChangeBuffer(void);
};

#endif
//EOF