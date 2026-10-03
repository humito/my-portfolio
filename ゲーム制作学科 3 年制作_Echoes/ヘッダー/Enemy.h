//=============================================================================
// 敵の処理 [Enemy.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _ENEMY_H_
#define _ENEMY_H_

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "SceneX.h"
#include <windows.h>

//*****************************************************************************
//定数定義
//*****************************************************************************
#define ENEMY_RADIUS (40.0f)//敵サイズの半径

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//敵の色
enum ENEMY_COLOR
{
	ENEMY_RED = 0,	//赤
	ENEMY_GREEN,	//緑
	ENEMY_BLUE,		//青
	COLOR_NUM		//色数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//敵クラス
class CEnemy : public CSceneX
{
	//外部
	public:
		CEnemy();									//コンストラクタ
		~CEnemy(){}									//デストラクタ

		HRESULT Init(D3DXVECTOR3 pos,				//初期化
					 ENEMY_COLOR color);
		void Uninit();								//終了
		void Update();								//更新
		void Draw();								//描画

		void Shot(D3DXVECTOR3 rot,					//敵の発射
				  D3DXVECTOR3 velocity);

		void Fixation(D3DXVECTOR3 offset);			//敵の固定

		void SetParentMtx(D3DXMATRIX parentMtx);	//親マトリクスセット

		//次・前ポインタ取得
		CEnemy *GetNext(){ return m_pNext; }
		CEnemy *GetPrev(){ return m_pPrev; }

		//次・前ポインタセット
		void SetNext(CEnemy *pNext){ m_pNext = pNext; }
		void SetPrev(CEnemy *pPrev){ m_pPrev = pPrev; }

		//自身の色取得
		ENEMY_COLOR GetColor(){ return m_myColor; }

		//自身の所持色の加算
		void AddColorNum(ENEMY_COLOR color);

		//インスタンス生成
		static CEnemy *Create(D3DXVECTOR3 pos,
								ENEMY_COLOR color);

	//内部
	private:

		ENEMY_COLOR m_myColor;						//色
		D3DXVECTOR3 m_velocity;						//移動量
		D3DXVECTOR3 m_offSet;						//オフセット値
		D3DXVECTOR3 m_diffPos;						//差分
		D3DXMATRIX m_parentMtx;						//親マトリクス

		bool m_bMove;								//移動フラグ
		bool m_bFixity;								//固定フラグ
		bool m_bShot;								//弾化フラグ
		bool m_bOnlyColor[COLOR_NUM];				//その色のみのフラグ
		bool m_bLOD;								//LODフラグ

		int m_nStickColor[COLOR_NUM];				//色の所持数をいれる
		int m_nShotCount;							//発射中のカウント

		float m_fHalfFieldSizeX;					//フィールドのサイズX
		float m_fHalfFieldSizeZ;					//フィールドのサイズZ

		//接触している敵ポインタ
		CEnemy *m_pNext;
		CEnemy *m_pPrev;

		//当たり判定
		void HitCheck();

		//敵終了
		void End();
};
#endif
//EOF