//=============================================================================
// テクスチャアニメーションエフェクト [AnimationEffect.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _ANIMATIONEFFECT_H_
#define _ANIMATIONEFFECT_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "SceneBillboard.h"

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//エフェクト種類
enum ANIMEFFECT
{
	SMOKE_EFFECT = 0,	//煙
	EXPLOSION_EFFECT,	//爆発
	ANIMEFFECT_NUM		//アニメーションの種類
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//アニメーションエフェクト
class CAnimEffect : public CSceneBillboard
{
	//外部
	public:
		CAnimEffect(){}							//コンストラクタ
		~CAnimEffect(){}						//デストラクタ
		HRESULT Init(D3DXVECTOR3 pos,			//初期化
					int nCountMax,
					float fSize,
					ANIMEFFECT type);
		void Uninit();							//終了
		void Draw();							//描画
		void Update();							//更新
		static void Create(	D3DXVECTOR3 pos,	//インスタンス生成
							int nCountMax,
							float fSize,
							ANIMEFFECT type);

	//内部
	private:
		int m_nAnimNum;							//アニメーション数
		int m_nCountMax;						//最大表示カウント
		float m_fCount;							//表示カウント
		float m_fSize;							//サイズ
		float m_fMoveU, m_fMoveV;				//UV座標移動量

		//テクスチャ座標変更
		void ChangeTexture();
};
#endif
//EOF