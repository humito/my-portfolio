#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "../Scene/scene2D.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//背景クラス
class CBackGround : public CScene2D
{
	//外部
	public:
		CBackGround(){}						//コンストラクタ
		~CBackGround(){}					//デストラクタ
		HRESULT Init(char *pFileName);		//初期化
		void Uninit();						//終了
		void Update();						//更新
		void Draw();						//描画
		static void Create(char *pFileName);//インスタンス生成
};
#endif
//EOF