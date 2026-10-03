//=============================================================================
// シェーダー基底クラス [Shader.h]
// Author : 木村 文登
//=============================================================================
#ifndef _SHADER_H_
#define _SHADER_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include <d3dx9.h>

//*****************************************************************************
//クラス定義
//*****************************************************************************
class CShader
{
	//外部
	public:
		CShader();	//コンストラクタ
		~CShader(){}//デストラクタ

		//シェーダーファイル読込
		virtual void Load() = 0;

		//シェーダー終了
		virtual void Uninit() = 0;

		//シェーダー開始
		virtual void Begin(LPDIRECT3DDEVICE9 pDevice) = 0;

		//シェーダー完了
		virtual void End(LPDIRECT3DDEVICE9 pDevice) = 0;

		//マトリックスのセット
		virtual void SetMatrix(	LPDIRECT3DDEVICE9 pDevice,
								D3DXMATRIX *pMtxWorld){}

		//マテリアルのセット
		virtual void SetMaterial(LPDIRECT3DDEVICE9 pDevice,
			D3DXVECTOR4 materialVec){}

		//シェーダー関連解放
		void Release();

		//頂点シェーダーチェック
		bool IsVS()
		{
			if (m_pVertexShader)
				return true;
			else
				return false;
		}
		//ピクセルシェーダーチェック
		bool IsPS()
		{
			if (m_pPixelShader)
				return true;
			else
				return false;
		}

		//頂点定数テーブル取得
		LPD3DXCONSTANTTABLE GetVSConstantTable()
		{ return m_pVSConstantTable; }

		//ピクセル定数テーブル取得
		LPD3DXCONSTANTTABLE GetPSConstantTable()
		{ return m_pPSConstantTable; }

	//派生クラス用外部
	protected:
	LPD3DXCONSTANTTABLE m_pVSConstantTable;	//頂点シェーダー定数テーブル
	LPD3DXCONSTANTTABLE m_pPSConstantTable;	//ピクセルシェーダー定数テーブル
	LPDIRECT3DVERTEXSHADER9 m_pVertexShader;//頂点シェーダー
	LPDIRECT3DPIXELSHADER9 m_pPixelShader;	//ピクセルシェーダー

	//シェーダー作成
	void CreateShader(LPDIRECT3DDEVICE9 pDevice,
						char *pShaderFileName,
						char *pVSFuncName,
						char *pPSFuncName,
						char *pVSVersion,
						char *pPSVersion,
						LPD3DXCONSTANTTABLE *pVSConstantTable,
						LPD3DXCONSTANTTABLE *pPSConstantTable,
						LPDIRECT3DVERTEXSHADER9 *pVertexShader,
						LPDIRECT3DPIXELSHADER9 *pPixelShader);
};
#endif
//EOF