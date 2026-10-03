//=============================================================================
//ガウスフィルター[Gaussian.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Gaussian.h"
#include "../main.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../System/Common.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CGaussian::CGaussian()
{
	m_pPSConstantTableBlurY = NULL;
	m_pPixelShaderBlurY = NULL;
	m_pD3DZBuffTextureY = NULL;
	m_pD3DZBuffTextureX = NULL;
	m_pTexSurfaceBlur = NULL;
	m_pTexZSBuffBlur = NULL;
}
//=============================================================================
//シェーダー読込
//=============================================================================
void CGaussian::Load()
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//ガウスフィルターシェーダー生成
	CreateShader(pDevice, "data/HLSL/GaussianFilter.hlsl",
				"VertexShader3D", "PixelShaderXBlur",
				"vs_2_0", "ps_2_0",
				&m_pVSConstantTable, &m_pPSConstantTable,
				&m_pVertexShader, &m_pPixelShader);

	//Y方向ブラーのピクセルシェーダー関数用に生成
	LPD3DXBUFFER code = NULL;
	LPD3DXBUFFER error = NULL;

	//ピクセルシェーダーコンパイル
	HRESULT hr = D3DXCompileShaderFromFile("data/HLSL/GaussianFilter.hlsl",
											NULL, NULL, "PixelShaderYBlur",
											"ps_2_0", 0, &code, &error, &m_pPSConstantTableBlurY);

	//コンパイル失敗した場合メッセージ表示
	if (FAILED(hr))
		MessageBox(NULL, (LPSTR)error->GetBufferPointer(), "error", 0);
	//コードからピクセルシェーダーを生成
	else
		pDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), &m_pPixelShaderBlurY);

	//各バッファ解放
	//コード
	RELEASE_OBJECT(code);

	//エラーコード
	RELEASE_OBJECT(error);

	//テクスチャとサーフェイスの生成
	CreateBlurTexSurface(pDevice);

	//フィルターの初期化
	CFilter::Init();
}
//=============================================================================
//ブラー用テクスチャとサーフェイスの生成
//=============================================================================
void CGaussian::CreateBlurTexSurface(LPDIRECT3DDEVICE9 pDevice)
{
	//レンダーターゲット用テクスチャの生成(X方向ブラー用)
	D3DXCreateTexture(pDevice,
					SCREEN_WIDTH,			//バックバッファのサイズ
					SCREEN_HEIGHT,
					1,						//ミップマップレベル
					D3DUSAGE_RENDERTARGET,	//レンダーターゲットに指定
					D3DFMT_A8R8G8B8,		//ピクセルフォーマット
					D3DPOOL_DEFAULT,
					&m_pD3DTexture);		//生成したテクスチャのポインタ

	//Zバッファテクスチャ生成(X方向ブラー用)
	D3DXCreateTexture(pDevice,
					SCREEN_WIDTH,
					SCREEN_HEIGHT,
					1,
					D3DUSAGE_RENDERTARGET,
					D3DFMT_A8R8G8B8,
					D3DPOOL_DEFAULT,
					&m_pD3DZBuffTextureX);

	//サーフェイスの取得(X方向ブラー用)
	m_pD3DTexture->GetSurfaceLevel(0, &m_TexSurface);
	m_pD3DZBuffTextureX->GetSurfaceLevel(0, &m_TexZSBuff);

	//レンダーターゲット用テクスチャの生成(Y方向ブラー用)
	D3DXCreateTexture(pDevice,
					SCREEN_WIDTH,			//バックバッファのサイズ
					SCREEN_HEIGHT,
					1,						//ミップマップレベル
					D3DUSAGE_RENDERTARGET,	//レンダーターゲットに指定
					D3DFMT_A8R8G8B8,		//ピクセルフォーマット
					D3DPOOL_DEFAULT,
					&m_pD3DTextureY);		//生成したテクスチャのポインタ

	//Zバッファテクスチャ生成(Y方向ブラー用)
	D3DXCreateTexture(pDevice,
						SCREEN_WIDTH,
						SCREEN_HEIGHT,
						1,
						D3DUSAGE_RENDERTARGET,
						D3DFMT_A8R8G8B8,
						D3DPOOL_DEFAULT,
						&m_pD3DZBuffTextureY);

	//サーフェイスの取得(Y方向ブラー用)
	m_pD3DTextureY->GetSurfaceLevel(0, &m_pTexSurfaceBlur);
	m_pD3DZBuffTextureY->GetSurfaceLevel(0, &m_pTexZSBuffBlur);
}
//=============================================================================
//シェーダー終了
//=============================================================================
void CGaussian::Uninit()
{
	//Y方向ブラー関数用の定数テーブル終了
	RELEASE_OBJECT(m_pPSConstantTableBlurY);

	//Y方向ブラー関数用のピクセルシェーダー終了
	RELEASE_OBJECT(m_pPixelShaderBlurY);

	//Y方向ブラー用テクスチャ終了
	RELEASE_OBJECT(m_pD3DTextureY);

	//Zバッファテクスチャ(X方向)の終了
	RELEASE_OBJECT(m_pD3DZBuffTextureX);

	//Zバッファテクスチャ(Y方向)の終了
	RELEASE_OBJECT(m_pD3DZBuffTextureY);

	//Y方向ブラー用サーフェイス終了
	RELEASE_OBJECT(m_pTexSurfaceBlur);

	//Y方向ブラー用Zバッファサーフェイス終了
	RELEASE_OBJECT(m_pTexZSBuffBlur);

	//フィルターの終了
	CFilter::Uninit();
}
//=============================================================================
//描画(X方向ブラー)
//=============================================================================
void CGaussian::Draw(LPDIRECT3DDEVICE9 pDevice)
{
	//頂点シェーダーのセット
	if (m_pVertexShader)
		pDevice->SetVertexShader(m_pVertexShader);

	//ピクセルシェーダーのセット
	if (m_pPixelShader)
		pDevice->SetPixelShader(m_pPixelShader);

	//Zバッファをテクスチャ１にセット
	pDevice->SetTexture(1, m_pD3DZBuffTextureX);

	//フィルターの描画
	CFilter::Draw(pDevice);

	//シェーダーを戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//描画(XY方向ブラー)
//=============================================================================
void CGaussian::DrawBlur(LPDIRECT3DDEVICE9 pDevice)
{
	//頂点シェーダーのセット
	if (m_pVertexShader)
		pDevice->SetVertexShader(m_pVertexShader);

	//ピクセルシェーダーのセット
	if (m_pPixelShaderBlurY)
		pDevice->SetPixelShader(m_pPixelShaderBlurY);

	//Zバッファをテクスチャ１にセット
	pDevice->SetTexture(1, m_pD3DZBuffTextureY);

	//Zバッファ無効
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pD3DTextureY);

	//ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
							2,
							&m_aVtx[0],
							sizeof(VERTEX_2D));

	//Zバッファ戻す
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	//シェーダーを戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//Y方向ブラー用にレンダーターゲット切替
//=============================================================================
void CGaussian::ChangeSurfaceBlurY(LPDIRECT3DDEVICE9 pDevice)
{
	//バックバッファのポインタ保持
	pDevice->GetRenderTarget(0, &m_backBuffOrg);

	//レンダーターゲットをテクスチャに設定
	pDevice->SetRenderTarget(0, m_pTexSurfaceBlur);
	pDevice->SetRenderTarget(1, m_pTexZSBuffBlur);
}
//=============================================================================
//Y方向ブラー用にレンダーターゲット戻す
//=============================================================================
void CGaussian::ReturnSurfaceBlurY(LPDIRECT3DDEVICE9 pDevice)
{
	//レンダーターゲットをバックバッファに戻す
	pDevice->SetRenderTarget(0, m_backBuffOrg);
	pDevice->SetRenderTarget(1, NULL);

	//バックバッファ用を解放
	m_backBuffOrg->Release();
}
//EOF