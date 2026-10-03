//=============================================================================
//パーティクル処理[CParticle.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CPARTICLE_H_
#define _CPARTICLE_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "CSceneBillboard.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define ITEM_SIZE (50.0f)	//アイテムのサイズ
#define ITEM_NUM (300)		//アイテムの数

//*****************************************************************************
//構造体定義
//*****************************************************************************
//パーティクルの種類
enum PARTICLE_TYPE
{
	TYPE_ITEM=0,		//アイテム
	TYPE_FIREWORKS,		//花火
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
		CParticle();																						//コンストラクタ
		~CParticle();																						//デストラクタ
		HRESULT Init(PARTICLE_TYPE type,int nNumParticle,D3DXVECTOR3 pos,float fWidth,float fHeight);		//初期化
		void Uninit();																						//終了
		void Update();																						//更新
		void Draw();																						//描画
		static void Create(PARTICLE_TYPE type,int nNumParticle,D3DXVECTOR3 pos,float fWidth,float fHeight);	//インスタンス生成
		int GetNum();																						//パーティクル数取得
		D3DXVECTOR3 GetParticlePos(int nIndex);																//各パーティクルの座標取得
		PARTICLE_TYPE GetType();																			//タイプの取得
		void SetVelocity(int nIndex,D3DXVECTOR3 velocity);													//移動量のセット
		void SetUseFlag(int nIndex,bool bFlag);																//使用フラグのセット
		bool GetUseFlag(int nIndex);																		//使用フラグの取得
		bool CheckLand(int nIndex);																			//着地チェック

	//内部
	private:
		//パーティクル構造体
		struct PARTICLE
		{
			D3DXVECTOR3	pos;							//座標
			D3DXVECTOR3	velocity;						//速度
			int			nCnt;							//カウント
			float		fHeight;						//高さ
			bool		bUse;							//使用フラグ
			bool		bDisp;							//表示フラグ
			bool		bLand;							//着地フラグ
		};

		int			m_nDispCnt;							//表示カウント
		int			m_nNumParticle;						//パーティクル数
		PARTICLE	*m_pParticle;						//パーティクルポインタ
		D3DXVECTOR3	m_startPos;							//パーティクル発進地
		PARTICLE_TYPE m_type;							//パーティクルの種類
		D3DCOLORVALUE m_color[4];						//4頂点の光源

		void SetParticle(D3DXVECTOR3 pos,int nIndex);	//パーティクルのセット
		void ChangeBuffer();							//頂点バッファの情報変更
};
#endif
//EOF