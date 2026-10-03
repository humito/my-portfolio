//=============================================================================
//残り弾数表示処理[CLeftoverBullet.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CLEFTOVERBULLET_H_
#define _CLEFTOVERBULLET_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "scene2D.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//残弾数表示クラス
class CLeftoverBullet : public CScene2D
{
	//外部
	public:
		CLeftoverBullet();					//コンストラクタ
		~CLeftoverBullet();					//デストラクタ
		HRESULT Init();						//初期化
		void Uninit();						//終了
		void Update();						//更新
		void Draw();						//描画
		static void Create();				//インスタンス生成
		static int GetLeftoverNum(void);	//残弾数取得
		static void AddNum (void);			//残弾数加算
		static void SubNum (void);			//残弾数減算

	//内部
	private:
		static int m_nLeftoverBullet;		//残弾数

		//頂点情報の変更
		void ChangeBuffer(int num);
};

#endif
//EOF