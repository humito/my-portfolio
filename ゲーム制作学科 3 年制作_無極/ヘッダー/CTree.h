//=============================================================================
//その他オブジェクト(木)処理[CTree.cpp]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CTREE_H_
#define _CTREE_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "CSceneBillboard.h"
//*****************************************************************************
//クラス定義
//*****************************************************************************
//木クラス
class CTree : public CSceneBillboard
{
	//外部
	public:
		CTree();																						//コンストラクタ
		~CTree();																						//デストラクタ
		HRESULT Init(char *FileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot,float width,float height);			//初期化
		void Uninit();																					//終了
		void Update();																					//更新
		void Draw();																					//描画
		static void Create(char *FileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot,float width,float height);	//木インスタンス生成

		//内部
	private:
};

#endif
//EOF