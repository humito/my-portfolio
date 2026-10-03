//=============================================================================
// シェーダー基底クラス [Shader.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Shader.h"
#include "../System/Common.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CShader::CShader()
{
	m_pVSConstantTable = NULL;
	m_pPSConstantTable = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
}
//=============================================================================
//解放
//=============================================================================
void CShader::Release()
{
	//頂点シェーダー定数テーブル
	RELEASE_OBJECT(m_pVSConstantTable);

	//ピクセルシェーダー定数テーブル
	RELEASE_OBJECT(m_pPSConstantTable);

	//頂点シェーダー
	RELEASE_OBJECT(m_pVertexShader);

	//ピクセルシェーダー
	RELEASE_OBJECT(m_pPixelShader);
}
//=============================================================================
//シェーダー作成
//=============================================================================
void CShader::CreateShader(LPDIRECT3DDEVICE9 pDevice,
							char *pShaderFileName,
							char *pVSFuncName,
							char *pPSFuncName,
							char *pVSVersion,
							char *pPSVersion,
							LPD3DXCONSTANTTABLE *pVSConstantTable,
							LPD3DXCONSTANTTABLE *pPSConstantTable,
							LPDIRECT3DVERTEXSHADER9 *pVertexShader,
							LPDIRECT3DPIXELSHADER9 *pPixelShader)
{
	///////////////////////////////////
	//		シェーダーの初期化		//
	/////////////////////////////////

	LPD3DXBUFFER code = NULL;	//シェーダーコード
	LPD3DXBUFFER error = NULL;	//エラー

	///////////////////////////////////////
	//		頂点シェーダーの初期化		//
	/////////////////////////////////////

	//頂点シェーダーコンパイル
	HRESULT hr = D3DXCompileShaderFromFile(pShaderFileName, NULL, NULL, pVSFuncName,
		pVSVersion, 0, &code, &error, &*pVSConstantTable);

	//コンパイル失敗した場合メッセージ表示
	if (FAILED(hr))
		MessageBox(NULL, (LPSTR)error->GetBufferPointer(), "error", 0);
	//コードから頂点シェーダーを生成
	else
		pDevice->CreateVertexShader((DWORD*)code->GetBufferPointer(), &*pVertexShader);

	///////////////////////////////////////////
	//		ピクセルシェーダーの初期化		//
	/////////////////////////////////////////

	//ピクセルシェーダーコンパイル
	hr = D3DXCompileShaderFromFile(pShaderFileName, NULL, NULL, pPSFuncName,
		pPSVersion, 0, &code, &error, &*pPSConstantTable);

	//コンパイル失敗した場合メッセージ表示
	if (FAILED(hr))
		MessageBox(NULL, (LPSTR)error->GetBufferPointer(), "error", 0);
	//コードからピクセルシェーダーを生成
	else
		pDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), &*pPixelShader);

	//各バッファ解放
	//コード
	RELEASE_OBJECT(code);

	//エラーコード
	RELEASE_OBJECT(error);
}
//EOF