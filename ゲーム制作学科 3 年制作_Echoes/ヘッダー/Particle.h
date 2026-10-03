//=============================================================================
//パーティクル処理[Particle.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "SceneBillboard.h"

//*****************************************************************************
//構造体定義
//*****************************************************************************
//パーティクルの種類
enum PARTICLE_TYPE
{
	TYPE_EFFECT = 0,	//エフェクト
	TYPE_SMOKE,			//煙
	PARTICLE_TYPE_MAX	//種類数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//パーティクルクラス
class CParticle : public CSceneBillboard
{
	//外部
	public:
		CParticle();													//コンストラクタ
		~CParticle();													//デストラクタ
		HRESULT Init(PARTICLE_TYPE type, int nNumParticle,				//初期化
					D3DXVECTOR3 pos, float fWidth, float fHeight);
		void Uninit();													//終了
		void Update();													//更新
		void Draw();													//描画
		static void Create(PARTICLE_TYPE type, int nNumParticle,		//インスタンス生成
						D3DXVECTOR3 pos, float fWidth, float fHeight);

	//内部
	private:
		//パーティクル構造体
		struct PARTICLE
		{
			D3DXVECTOR3	pos;		//座標
			D3DXVECTOR3	velocity;	//速度
			int			nCnt;		//カウント
			float		fWidth;		//幅
			float		fHeight;	//高さ
		};

		int			m_nDispCnt;							//表示カウント
		int			m_nNumParticle;						//パーティクル数
		int			m_nAlpha;							//α値
		PARTICLE	*m_pParticle;						//パーティクルポインタ
		D3DXVECTOR3	m_startPos;							//パーティクル発進地
		PARTICLE_TYPE m_typeParticle;					//パーティクルの種類

		void ChangeBuffer();							//頂点バッファの情報変更
};

#endif
//EOF