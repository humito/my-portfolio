//=============================================================================
//弾処理[CBullet.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CBULLET_H_
#define _CBULLET_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "CSceneX.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//弾クラス
class CBullet :public CSceneX
{
	//外部
	public:
		CBullet();																	//コンストラクタ
		~CBullet();																	//デストラクタ
		HRESULT Init(D3DXVECTOR3 pos,D3DXVECTOR3 rot,D3DXVECTOR3 velocity);			//初期化
		void Uninit();																//終了
		void Update();																//更新
		void Draw();																//描画
		static void Create(D3DXVECTOR3 pos,D3DXVECTOR3 rot,D3DXVECTOR3 velocity);	//弾インスタンス生成

	//内部
	private:
		D3DXVECTOR3 m_velocity;														//弾の速度
};
#endif
//EOF