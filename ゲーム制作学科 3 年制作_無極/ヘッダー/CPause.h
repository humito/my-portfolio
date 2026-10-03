//=============================================================================
// ポーズ画面処理 [CPause.h]
// Author : 木村　文登
//=============================================================================
#ifndef _CPAUSE_H_
#define _CPAUSE_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "renderer.h"
#include "scene2D.h"
//*****************************************************************************
//構造体
//*****************************************************************************
//ポーズメニュー
enum PAUSE_MENU
{
	PAUSE_CLOSE=0,	//ポーズ画面を閉じる
	PAUSE_RESET,	//ゲームをリセット
	PAUSE_TITLE,	//タイトル画面へ
	PAUSE_MAX		//メニュー数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//ポーズクラス
class CPause : public CScene2D
{
	//外部
	public:
		CPause(int priority=5);
		~CPause();
		HRESULT Init();
		void Uninit();
		void Update();
		void Draw();
		static CPause *Create();
	//内部
	private:
		void ChangeBuffer();						//頂点情報の変更
		PAUSE_MENU m_selectMenu;					//選択した項目
		float m_fCurPosY;							//カーソルのY座標
		float m_fDestCurPosY;						//目的のカーソルY座標
		LPDIRECT3DTEXTURE9 m_pD3DCursolTex;			//カーソルテクスチャへのポインタ
		LPDIRECT3DVERTEXBUFFER9 m_pD3DCursolVtxBuff;//カーソル頂点バッファへのポインタ
};
#endif
//EOF