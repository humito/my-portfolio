//=============================================================================
//エネミー処理[CEnemy.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CENEMY_H_
#define _CENEMY_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "renderer.h"
#include "CSceneX.h"

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CShadow;

//*****************************************************************************
//構造体定義
//*****************************************************************************
//敵のレベル
enum ENEMY_LEVEL
{
	ENEMY_VERYEASY=0,	//ベリーイージー
	ENEMY_EASY,			//イージー
	ENEMY_NORMAL,		//ノーマル
	ENEMY_HARD,			//ハード
	ENEMY_VERYHARD,		//ベリーハード
	LEVEL_MAX			//レベル数
};

//敵の種類
enum ENEMY_TYPE
{
	TYPE_BOM=0,
	TYPE_ROBO,
	TYPE_HOMO,
	ENEMY_TYPE_MAX
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//敵クラス
class CEnemy : public CSceneX
{
	//外部
	public:
		CEnemy();																//コンストラクタ
		~CEnemy();																//デストラクタ
		HRESULT Init(ENEMY_TYPE type,ENEMY_LEVEL level,D3DXVECTOR3 pos);		//初期化
		void Uninit();															//終了
		void Update();															//更新
		void Draw();															//描画
		static void Create(ENEMY_TYPE type,ENEMY_LEVEL level,D3DXVECTOR3 pos);	//インスタンス生成
		void GetSize(float *fSizeX,float *fSizeY,float *fSizeZ);
	//内部
	private:
		D3DXVECTOR3 m_posMove;													//速度
		D3DXVECTOR3 m_posOld;													//前座標
		float m_fSizeX,m_fSizeY,m_fSizeZ;										//敵のサイズ
		CShadow *m_pShadow;												//影インスタンス
};

#endif
//EOF