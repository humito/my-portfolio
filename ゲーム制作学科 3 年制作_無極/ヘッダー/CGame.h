//=============================================================================
// ゲーム処理 [CGame.h]
// Author : 木村　文登
//=============================================================================
#ifndef _CGAME_H_
#define _CGAME_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <windows.h>
#include "renderer.h"

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CEnemy;			//敵
class CMeshField;		//フィールド
class CPlayer;			//プレイヤー
class CPause;			//ポーズ

//*****************************************************************************
//クラス定義
//*****************************************************************************
//ゲームクラス
class CGame
{
	//外部
	public:
		CGame();								//コンストラクタ
		~CGame();								//デストラクタ
		HRESULT Init();							//初期化
		void Uninit();							//終了
		void Update();							//更新
		void Draw();							//描画
		static CMeshField *GetField(void);		//フィールドのゲット
		static CPlayer *GetPlayer(void);		//プレイヤーのゲット
		static void SetPauseFlag(bool flag);	//ポーズフラグセット
		static void Reset(void);				//ゲーム内各要素リセット

	//内部
	private:
		static CMeshField *m_pMeshField;		//フィールドインスタンス
		static CPlayer *m_pPlayer;				//プレイヤーインスタンス
		static CPause *m_pPause;				//ポーズインスタンス
		static bool m_bPause;					//ポーズフラグ
		int m_nElapsedCnt;						//経過カウント

		//敵の発生
		void StartCreateEnemy(void);
};
#endif
//EOF