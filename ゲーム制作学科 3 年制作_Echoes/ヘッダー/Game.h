//=============================================================================
// ゲーム管理処理 [Game.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _GAME_H_
#define _GAME_H_

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <windows.h>

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CMeshField;	//フィールドインスタンス
class CPlayer;		//プレイヤーインスタンス
class CPause;		//ポーズインスタンス
class CEraseArea;	//消滅エリア

//*****************************************************************************
//クラス定義
//*****************************************************************************
//タイトルクラス
class CGame
{
	//外部
	public:
		CGame(){}											//コンストラクタ
		~CGame(){}											//デストラクタ
		HRESULT Init();										//初期化
		void Uninit();										//終了
		void Update();										//更新
		
		static CMeshField *GetField(){ return m_pField; }	//フィールド取得
		static CPlayer *GetPlayer(){ return m_pPlayer; }	//プレイヤー取得
		static CEraseArea *GetArea(){ return m_pArea; }		//消滅エリア取得
		static void NoPause();								//ポーズしない

		static void SetStage(int nStage)					//ステージのセット
		{ m_nStageMenu = nStage; }

	//内部
	private:
		static int m_nStageMenu;		//ステージメニュー
		static bool m_bPause;			//ポーズフラグ
		static CMeshField *m_pField;	//フィールドインスタンス
		static CPlayer *m_pPlayer;		//プレイヤーインスタンス
		static CEraseArea *m_pArea;		//消滅エリアインスタンス
		static CPause *m_pPause;		//ポーズインスタンス
		int *m_pnSpace;					//空間分割スペース
		int m_nCount;					//ゲーム経過カウント
		int m_nStatus;					//ゲームステータス
		float m_fBlockSizeX;			//ブロックサイズX
		float m_fHalfFieldSizeX;		//フィールドの半分のサイズX
		float m_fHalfFieldSizeZ;		//フィールドの半分のサイズZ

		void StartEnemy();				//敵の生成開始
};
#endif
//EOF