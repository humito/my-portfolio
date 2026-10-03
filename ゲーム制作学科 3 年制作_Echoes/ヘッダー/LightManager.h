//=============================================================================
//
// ライト管理 [LightManager.h]
// Author : HUMITO KIMURA
//
//=============================================================================
#ifndef _LIGHTMANAGER_H_
#define _LIGHTMANAGER_H_

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "d3dx9.h"

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CLight;

//*****************************************************************************
//構造体定義
//*****************************************************************************
struct LIGHT_DATA
{
	D3DXVECTOR3 pos;
	D3DLIGHTTYPE type;
};

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//管理してるライトのID
enum LIGHT_DATA_ID
{
	MAIN_LIGHT = 0,
	SUB_LIGHT1,
	SUB_LIGHT2,
	LIGHT_NUM
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//ライトマネージャークラス
class CLightManager
{
	//外部
	public:
		CLightManager();	//コンストラクタ
		~CLightManager();	//デストラクタ
		HRESULT InitAll();	//全ライト情報初期化
		void UpdateAll();	//全ライト情報更新
		void SetAll();		//全ライト情報セット

		CLight *GetLight(LIGHT_DATA_ID id)
		{ return m_pLight[id]; }

		//インスタンス取得
		static CLightManager *GetInstance()
		{
			if (m_pInstance == NULL)
			{
				m_pInstance = new CLightManager();
			}
			return m_pInstance;
		}

		//インスタンス解放
		static void Delete()
		{
			if (m_pInstance)
			{
				delete m_pInstance;
				m_pInstance = NULL;
			}
		}
	//内部
	private:
		//ライトインスタンス(自身のインスタンス)
		static CLightManager *m_pInstance;

		//ライト情報
		CLight *m_pLight[LIGHT_NUM];

		//ライト情報全て終了
		void UninitAll();

};
#endif
//EOF