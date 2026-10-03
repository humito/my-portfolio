#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "scene2D.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//背景クラス
class CBackGround : public CScene2D
{
	//外部
	public:
		CBackGround();
		~CBackGround();
		HRESULT Init(char *pFileName);
		void Uninit();
		void Update();
		void Draw();
		static void Create(char *pFileName);

};
#endif
//EOF