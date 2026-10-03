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
#include "../System/renderer.h"

//*****************************************************************************
// 定数定義
//*****************************************************************************
#define PRIORITY_MODEL (1)	//モデルのプライオリティ
#define PRIORITY_2D (5)		//2Dのプライオリティ
#define PRIORITY_3D (3)		//3Dオブジェクトのプライオリティ
#define PRIORITY_MAX (10)	//プライオリティ数

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//オブジェクトの種類
enum OBJECT_TYPE
{
	PLAYER_TYPE = 0,
	CUBE_TYPE,
	RB_BOX_TYPE,
	TYPE_NUM
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

		static void UpdateAll();				//ワーク配列全て更新
		static void DrawAll();					//ワーク配列全て描画
		static void DrawPriority(int priority);	//プライオリティを指定した描画
		static void ReleaseAll();				//ワーク配列全て解放
		void Release();							//解放

		//プライオリティの先頭ポインタ取得
		static CScene *GetListTop(int nIndex){ return m_pTop[nIndex]; }

		//次シーンの取得
		CScene *GetNext(){ return m_pNext; }

		//オブジェクトの種類取得
		OBJECT_TYPE GetType(){ return m_type; }

		//座標・角度の取得
		D3DXVECTOR3 GetPos(){ return m_pos; }
		D3DXVECTOR3 GetRot(){ return m_rot; }

		//座標・角度のセット
		void SetPos(D3DXVECTOR3 pos){ m_pos = pos; }
		void SetRot(D3DXVECTOR3 rot){ m_rot = rot; }

	//派生クラス用
	protected:
		D3DXVECTOR3 m_pos;					//座標
		D3DXVECTOR3 m_rot;					//角度
		D3DXVECTOR3 m_scl;					//スケール
		OBJECT_TYPE m_type;					//オブジェクトの種類

	//内部
	private:
		static CScene *m_pTop[PRIORITY_MAX];//リスト先頭ポインタ
		static CScene *m_pCur[PRIORITY_MAX];//リスト終端ポインタ
		CScene *m_pPrev;					//前のポインタ
		CScene *m_pNext;					//次のポインタ
		bool m_Delete;						//削除フラグ
};

#endif
//EOF