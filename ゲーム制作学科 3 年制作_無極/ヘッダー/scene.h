//=============================================================================
//
// シーンクラス処理 [scene.h]
// Author : 木村　文登
//
//=============================================================================
#ifndef _SCENE_H_
#define _SCENE_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "renderer.h"

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CHitCheckSphere;//当たり判定用球体ポリゴン

//*****************************************************************************
// 定数定義
//*****************************************************************************
#define PRIORITY_MAX (7)		//プライオリティ数
#define PRIORITY_SCENEX (1)		//Xファイルのプライオリティ
#define PRIORITY_BILLBOARD (3)	//ビルボードのプライオリティ

//*****************************************************************************
//構造体定義
//*****************************************************************************
//オブジェクトの種類
enum OBJECT_TYPE
{
	OBJECT_TYPE_PLAYER=0,	//プレイヤー
	OBJECT_TYPE_BULLET,		//弾
	OBJECT_TYPE_OBJECT,		//オブジェクト
	OBJECT_TYPE_ENEMY,		//敵
	OBJECT_TYPE_PARTICLE,	//パーティクル
	OBJECT_TYPE_FADE,		//フェード
	OBJECT_TYPE_MAX			//種類数
};
//*****************************************************************************
//クラス定義
//*****************************************************************************
//シーンクラス
class CScene
{
	//外部
	public:
		//メンバ関数
		CScene(int priority);					//コンストラクタ
		~CScene();								//デストラクタ
		virtual HRESULT Init()=0;				//初期化
		virtual void Uninit()=0;				//終了
		virtual void Update()=0;				//更新
		virtual void Draw()=0;					//描画

		void SetPosition(D3DXVECTOR3 pos);		//座標セット

		static void UpdateAll();				//ワーク配列全て更新
		static void DrawAll();					//ワーク配列全て描画
		static void ReleaseAll();				//ワーク配列全て解放
		static CScene *GetListTop(int num);		//リスト構造の先頭取得
		void Release();							//解放
		D3DXVECTOR3 GetPos(){return m_pos;}		//座標取得
		OBJECT_TYPE GetType(){return m_objType;}//オブジェクトの種類取得
		bool GetDisp(){return m_bDisp;}			//表示フラグ取得
		CScene *GetNextScene();					//次ポインタ取得
	
	//派生クラスのみ外部
	protected:
		D3DXVECTOR3 m_pos;						//座標
		D3DXVECTOR3 m_rot;						//向き
		D3DXVECTOR3	m_scl;						//スケール
		OBJECT_TYPE	m_objType;					//オブジェクトの種類
		CHitCheckSphere *m_pSphere;				//当たり判定用球体ポリゴンインスタンス
		bool m_bDisp;							//シーン表示フラグ
	
	//内部
	private:
		static CScene *m_pTop[PRIORITY_MAX];	//リスト先頭ポインタ
		static CScene *m_pCur[PRIORITY_MAX];	//リスト終端ポインタ
		static int m_nNumScene;					//総シーン数
		CScene *m_pPrev;						//前のポインタ
		CScene *m_pNext;						//次のポインタ
		bool m_bDelete;							//削除フラグ
		static void UnLinkList();				//リストから削除
};

#endif
//EOF