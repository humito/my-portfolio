//=============================================================================
//その他オブジェクト処理[Object.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _OBJECT_H_
#define _OBJECT_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "../Scene/SceneX.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
class CObject : public CSceneX
{
	//外部
	public:
		CObject(){}								//コンストラクタ
		~CObject(){}							//デストラクタ

		//初期化
		HRESULT Init(char *FileName,
					D3DXVECTOR3 pos,
					D3DXVECTOR3 rot);
		void Uninit();							//終了
		void Update();							//更新
		void Draw();							//描画

		//インスタンス生成
		static CObject *Create(char *FileName,
								D3DXVECTOR3 pos,
								D3DXVECTOR3 rot);

	//内部
	private:
};
#endif
//EOF