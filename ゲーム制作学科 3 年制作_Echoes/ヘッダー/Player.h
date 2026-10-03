//=============================================================================
// プレイヤー処理 [Player.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <windows.h>
#include "SceneX.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define PART_MAX (5)		//パーツ数
#define SHORT_MOTION (2)	//短いモーション数
#define MIDDLE_MOTION (3)	//普通くらいのモーション数

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CModel;	//モデル
class CEnemy;	//敵
class CShadow;	//影

//*****************************************************************************
//構造体定義
//*****************************************************************************
//キーフレームの座標、角度
struct KEY
{
	float PosX;//座標
	float PosY;
	float PosZ;
	float RotX;//角度
	float RotY;
	float RotZ;
};

//キーフレーム情報
struct KEY_INFO
{
	int Frame;			//フレーム数
	KEY key[PART_MAX];	//パーツの座標
};

struct KEY_DATA
{
	float fMotionTime;		//動作時間
	int nNumKey;			//現在の総キー数
	int nKey;				//現在のキー
	KEY_INFO *KeyInfo;		//現在再生中のキー
};

//*****************************************************************************
//列挙型定義
//*****************************************************************************

//モーションタイプ
enum MOTION_TYPE
{
	MOTION_NEUTORAL = 0,//ニュートラル
	MOTION_WALK,		//歩き
	MOTION_HOLD_WAIT,	//持ったまま待つ
	MOTION_HOLD_WALK,	//持ったまま歩く
	MOTION_THROW,		//投げる
	MOTION_MAX			//モーション数
};

//プレイヤーパーツ
enum PLAYER_PART
{
	PART_BODY = 0,	//体
	PART_HAND_R,	//右手
	PART_HAND_L,	//左手
	PART_FOOT_R,	//右足
	PART_FOOT_L		//左足
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//プレイヤークラス
class CPlayer : public CSceneX
{
	//外部
	public:
		CPlayer();								//コンストラクタ
		~CPlayer(){}							//デストラクタ
		HRESULT Init();							//初期化
		void Uninit();							//終了
		void Update();							//更新
		void Draw();							//描画
		static CPlayer *Create();				//インスタンス生成

	//内部
	private:
		D3DXVECTOR3 m_posPrev;					//前座標
		D3DXVECTOR3 m_velocity;					//移動量

		CShadow *m_pShadow;						//影インスタンス

		CModel *m_pModel[PART_MAX];				//階層モデル

		MOTION_TYPE m_MotionType;				//現在のモーション

		KEY_INFO m_KeyNeutral[SHORT_MOTION];	//ニュートラルモーション
		KEY_INFO m_KeyWalk[SHORT_MOTION];		//歩きモーション
		KEY_INFO m_KeyHoldWait[SHORT_MOTION];	//持つ状態(待ち)
		KEY_INFO m_KeyHoldWalk[SHORT_MOTION];	//持つ状態(歩き)
		KEY_INFO m_KeyThrow[MIDDLE_MOTION];		//投げモーション

		KEY_DATA m_KeyData;						//キーデータ
		KEY_DATA m_KeyDataBlend;				//ブレンド時のキーデータ

		int m_nFrameBlend;						//フレームブレンド
		int m_nCountBlend;						//ブレンド時のカウント
		int m_nNotHoldCnt;						//しばらく掴めないカウント
		int m_nClockUpCnt;						//クロックアップカウント

		float m_fMove;							//移動量

		float m_fHalfFieldSizeX;				//フィールドの半分サイズX
		float m_fHalfFieldSizeZ;				//フィールドの半分サイズZ
		float m_fBlockSizeX, m_fBlockSizeZ;		//フィールドのサイズXZ

		bool m_bMove;							//移動フラグ
		bool m_bHold;							//掴みフラグ
		bool m_bNotHold;						//掴めないフラグ
		bool m_bClock;							//クロックアップフラグ
		bool m_bBlend;							//モーションブレンドフラグ
		bool m_bAnimLoop;						//アニメーションループ

		CEnemy *m_pHoldEnemy;					//保持した敵インスタンス

		//当たり判定チェック
		void HitCheck();

		//プレイヤーパーツの準備
		void SetupPlayerPart();

		//モーションデータの読込
		void LoadMotionData();

		//プレイヤー操作処理
		void PlayerInput();

		//パーツアニメーション
		void PartAnimation();

		//モーション切替
		void ChangeMotion();
		
		//モーションのセット
		void SetMotion(MOTION_TYPE type);
};
#endif
//EOF