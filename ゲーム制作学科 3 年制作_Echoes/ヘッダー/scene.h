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
// 定数定義
//*****************************************************************************
#define PRIORITY_MAX (10)		//プライオリティ数
#define PRIORITY_SCENEX (1)		//Xファイルモデルのプライオリティ
#define PRIORITY_BILLBOARD (3)	//ビルボードのプライオリティ

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CHitCheckSphere;	//当たり判定用球体

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//オブジェクトのタイプ
enum OBJECT_TYPE
{
	OBJECT_PLAYER = 0,	//プレイヤー
	OBJECT_ENEMY,		//敵
	OBJECT_PARTICLE,	//パーティクル
	OBJECT_ITEM,		//アイテム
	OBJECT_NUM			//種類数
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
		CScene(int priority);				//コンストラクタ
		~CScene();							//デストラクタ
		virtual HRESULT Init()=0;			//初期化
		virtual void Uninit()=0;			//終了
		virtual void Update()=0;			//更新
		virtual void Draw()=0;				//描画

		static void UpdateAll();			//ワーク配列全て更新
		static void DrawAll();				//ワーク配列全て描画
		static void ReleaseAll();			//ワーク配列全て解放
		void Release();						//解放

		//プライオリティの先頭ポインタ取得
		static CScene *GetListTop(int nIndex);
		//次シーンの取得
		CScene *GetNext();

		//マトリクス取得セット
		D3DXMATRIX GetMatrix(){ return m_mtxWorld; }
		void SetMatrix(D3DXMATRIX mtx){ m_mtxWorld = mtx; }

		//角度・座標取得
		D3DXVECTOR3 GetPos(){ return m_pos; }
		D3DXVECTOR3 GetRot(){ return m_rot; }
		//角度・座標セット
		void SetPos(D3DXVECTOR3 pos){ m_pos = pos; }
		void SetRot(D3DXVECTOR3 rot){ m_rot = rot; }

		//スケール取得
		D3DXVECTOR3 GetScale(){ return m_scl; }
		//スケールセット
		void SetScale(D3DXVECTOR3 scl){ m_scl = scl; }

		//タイプ取得
		OBJECT_TYPE GetType(){ return m_type; }

		//描画フラグ取得(描画できるオブジェクトかチェック)
		bool DrawCheck(){ return m_bDraw; }

	//条件付き外部
	protected:
		D3DXVECTOR3 m_pos;					//座標
		D3DXVECTOR3 m_rot;					//角度
		D3DXVECTOR3 m_scl;					//スケール
		D3DXMATRIX	m_mtxWorld;				//ワールドマトリックス
		bool m_bDraw;						//描画フラグ
		OBJECT_TYPE m_type;					//オブジェクトのタイプ

//デバッグ用
#ifdef _DEBUG
		CHitCheckSphere *m_pSphere;
#endif

	//内部
	private:
		static CScene *m_pTop[PRIORITY_MAX];//リスト先頭ポインタ
		static CScene *m_pCur[PRIORITY_MAX];//リスト終端ポインタ
		CScene *m_pPrev;					//前のポインタ
		CScene *m_pNext;					//次のポインタ
		bool m_Delete;						//削除フラグ
		static void UnLinkList();			//リストから削除
};

#endif
//EOF