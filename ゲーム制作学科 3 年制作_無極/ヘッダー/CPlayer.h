//=============================================================================
//プレイヤー処理[CPlayer.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CPLAYER_H_
#define _CPLAYER_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "CSceneX.h"

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CShadow;

//*****************************************************************************
//クラス定義
//*****************************************************************************
//プレイヤークラス
class CPlayer : public CSceneX
{
	//外部
	public:
		CPlayer();									//コンストラクタ
		~CPlayer();									//デストラクタ
		HRESULT Init();								//初期化
		void Uninit();								//終了
		void Update();								//更新
		void Draw();								//描画
		D3DXVECTOR3 GetRotation();					//プレイヤー角度取得
		static CPlayer *Create();					//インスタンス生成
		int GetItemNum();							//所持アイテム数取得

	//内部
	private:

		//プレイヤーアニメーションの種類
		enum PLAYER_ANIMATION
		{
			DOWN_ANIME=0,	//ダウン状態
			ROCKET_ANIME,	//ロケット状態
			SHOT_ANIME,		//弾発射情報
			RUN_ANIME,		//走る状態
			POSE_ANIME,		//ポーズ情報
			ANIMATION_MAX	//種類数
		};

		D3DXVECTOR3 m_posOld;						//プレイヤーの前座標
		D3DXVECTOR3 m_posMove;						//プレイヤーの移動量
		bool m_bJumpFlag;							//ジャンプフラグ
		bool m_bLandFlag;							//着地フラグ
		float m_fDestPosY;							//プレイヤージャンプ後の高さ
		int m_nNumItem;								//アイテム所持数
		int m_nDownCnt;								//ダウン時のカウント
		int m_nSoundCnt;							//アイテム取得効果音の再生回数
		int m_nElapsedCnt;							//経過カウント
		PLAYER_ANIMATION m_animeType;				//アニメーションのタイプ
		PLAYER_ANIMATION m_animePrevType;			//前のアニメーションのタイプ
		static CShadow *m_pShadow;					//影インスタンス

		//プレイヤーアニメーションの更新
		void PlayAnimation(void);
};
#endif
//EOF