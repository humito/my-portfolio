//=============================================================================
//シェーダー管理[ShaderManager.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _SHADERMANAGER_H_
#define _SHADERMANAGER_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CShader;

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//シェーダーの種類
enum SHADER_TYPE
{
	TOON_SHADER = 0,
	EDGE_FILTER,
	SHADER_NUM
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//シェーダー管理クラス
class CShaderManager
{
	//外部
	public:
		CShaderManager(){}	//コンストラクタ
		~CShaderManager(){}	//デストラクタ
		HRESULT Init();		//初期化

		//自身のインスタンスの取得
		static CShaderManager *GetInstance()
		{
			if (m_pInstance == NULL)
			{
				m_pInstance = new CShaderManager();
			}

			return m_pInstance;
		}

		//自身のインスタンスの解放
		static void Delete()
		{
			if (m_pInstance)
			{
				m_pInstance->Uninit();
				delete m_pInstance;
				m_pInstance = NULL;
			}
		}

		//シェーダーの取得
		static CShader *GetShader(SHADER_TYPE type)
		{ return m_pShader[type]; }


	//内部
	private:
		//自身のインスタンス
		static CShaderManager *m_pInstance;
		//シェーダー
		static CShader *m_pShader[SHADER_NUM];

		//終了
		void Uninit();
};
#endif
//EOF