//=============================================================================
//シェーダー[Shader.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CSHADER_H_
#define _CSHADER_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include <d3dx9.h>

//*****************************************************************************
//クラス定義
//*****************************************************************************
//シェーダー基底クラス
class CShader
{
	//外部
	public:
		CShader();								//コンストラクタ
		~CShader();								//デストラクタ
		virtual HRESULT Load() = 0;				//シェーダーの読込
		virtual void Uninit() = 0;				//終了
		virtual void Begin() = 0;				//シェーダー開始
		virtual void BeginPass(UINT Pass) = 0;	//パスの開始
		virtual void EndPass() = 0;				//パスの終了
		virtual void End() = 0;					//シェーダーの終了
		void Restore();							//復元
		void CommitChanges();					//ステート変更

		//エフェクトの取得
		LPD3DXEFFECT GetEffect()
		{ return m_pEffect; }

		//エフェクトチェック
		bool IsOK();

	//条件付き外部
	protected:
		LPD3DXEFFECT m_pEffect;					//エフェクト
		D3DXHANDLE m_pTechnique;				//テクニックハンドル
};
#endif
//EOF