//=============================================================================
//モーションブラー[MotionBlur.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "MotionBlur.h"
#include "../main.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../System/Common.h"

//TODO: 2.5Dモーションブラー http://maverickproj.web.fc2.com/pg70.html

//=============================================================================
//シェーダー読込
//=============================================================================
void CMotionBlur::Load()
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	///////////////////////////////////
	//		シェーダーの初期化		//
	/////////////////////////////////

	//モーションブラーフィルター
	CreateShader(pDevice, "data/HLSL/MotionBlurFilter.hlsl",
				"VertexShader3D", "PixelShader3D",
				"vs_2_0", "ps_3_0",
				&m_pVSConstantTable, &m_pPSConstantTable,
				&m_pVertexShader, &m_pPixelShader);

	//モーションブラー
	CreateShader(pDevice, "data/HLSL/MotionBlur.hlsl",
				"VertexShader3D", "PixelShader3D",
				"vs_2_0", "ps_2_0",
				&m_pVSConstantTableCas, &m_pPSConstantTableCas,
				&m_pVertexShaderCas, &m_pPixelShaderCas);

	//テクスチャとサーフェイスの生成
	CreateBlurTexSurface(pDevice);

	//フィルターの初期化
	CFilter::Init();
}
//=============================================================================
//モーションブラー用テクスチャとサーフェイスの生成
//=============================================================================
void CMotionBlur::CreateBlurTexSurface(LPDIRECT3DDEVICE9 pDevice)
{
	//レンダーターゲット用テクスチャの生成
	D3DXCreateTexture(pDevice,
					SCREEN_WIDTH,			//バックバッファのサイズ
					SCREEN_HEIGHT,
					1,						//ミップマップレベル
					D3DUSAGE_RENDERTARGET,	//レンダーターゲットに指定
					D3DFMT_A8R8G8B8,		//ピクセルフォーマット
					D3DPOOL_DEFAULT,
					&m_pD3DTexture);		//生成したテクスチャのポインタ

	//速度マップ用
	D3DXCreateTexture(pDevice,
					SCREEN_WIDTH,
					SCREEN_HEIGHT,
					1,
					D3DUSAGE_RENDERTARGET,
					D3DFMT_A8R8G8B8,
					D3DPOOL_DEFAULT,
					&m_pD3DVelocityTexture);

	//Zバッファ
	D3DXCreateTexture(pDevice,
						SCREEN_WIDTH,
						SCREEN_HEIGHT,
						1,
						D3DUSAGE_RENDERTARGET,
						D3DFMT_A8R8G8B8,
						D3DPOOL_DEFAULT,
						&m_pD3DZBuffTexture);

	//サーフェイスの取得
	m_pD3DTexture->GetSurfaceLevel(0, &m_TexSurface);
	m_pD3DVelocityTexture->GetSurfaceLevel(0, &m_TexVelocitySurface);
	m_pD3DZBuffTexture->GetSurfaceLevel(0,&m_TexZSBuff);
}
//=============================================================================
//シェーダー終了
//=============================================================================
void CMotionBlur::Uninit()
{
	//頂点シェーダー定数テーブル
	RELEASE_OBJECT(m_pVSConstantTableCas);

	//ピクセルシェーダー定数テーブル
	RELEASE_OBJECT(m_pPSConstantTableCas);

	//頂点シェーダー
	RELEASE_OBJECT(m_pVertexShaderCas);

	//ピクセルシェーダー
	RELEASE_OBJECT(m_pPixelShaderCas);

	//速度マップ用テクスチャ
	RELEASE_OBJECT(m_pD3DVelocityTexture);

	//速度マップ用サーフェイス
	RELEASE_OBJECT(m_TexVelocitySurface);

	//Zバッファテクスチャ解放
	RELEASE_OBJECT(m_pD3DZBuffTexture);

	//フィルターの終了
	CFilter::Uninit();
}
//=============================================================================
//シェーダー開始
//=============================================================================
void CMotionBlur::Begin(LPDIRECT3DDEVICE9 pDevice)
{
	//頂点シェーダーのセット
	if (m_pVertexShaderCas)
		pDevice->SetVertexShader(m_pVertexShaderCas);

	//ピクセルシェーダーのセット
	if (m_pPixelShaderCas)
		pDevice->SetPixelShader(m_pPixelShaderCas);
}
//=============================================================================
//シェーダー完了
//=============================================================================
void CMotionBlur::End(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//ワールドマトリックスのセット
//=============================================================================
void CMotionBlur::SetMatrix(LPDIRECT3DDEVICE9 pDevice,
							D3DXMATRIX *pMtxWorldNew,
							D3DXMATRIX *pMtxWorldOld)
{
	//ビュー、プロジェクションマトリクス取得
	D3DXMATRIX mtxView, mtxProj;
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProj);

	//カメラ基準の行列変換マトリクスをセット
	D3DXMATRIX mtx = (*pMtxWorldNew * mtxView * mtxProj);
	m_pVSConstantTableCas->SetMatrix(pDevice, "g_WorldViewProjectionNew", &mtx);

	//回転成分のみのワールドビュープロジェクション行列
	mtx._11 = 1.0f; mtx._22 = 1.0f; mtx._33 = 1.0f;
	mtx._41 = 0.0f; mtx._42 = 0.0f; mtx._43 = 0.0f;
	m_pVSConstantTableCas->SetMatrix(pDevice, "g_RotationOnlyWVP", &mtx);

	//前回の行列変換マトリクスをセット
	mtx = (*pMtxWorldOld * m_mtxViewOld * mtxProj);
	m_pVSConstantTableCas->SetMatrix(pDevice, "g_WorldViewProjectionOld", &mtx);

	//前回のビュー行列にコピーする
	CopyMemory(&m_mtxViewOld, &mtxView, sizeof(D3DXMATRIX));
}
//=============================================================================
//速度のセット
//=============================================================================
void CMotionBlur::SetVelocity(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR4 velocity)
{
	m_pVSConstantTableCas->SetVector(pDevice, "g_Velocity", &velocity);
}
//=============================================================================
//描画
//=============================================================================
void CMotionBlur::Draw(LPDIRECT3DDEVICE9 pDevice)
{
	//頂点シェーダーのセット
	if (m_pVertexShader)
		pDevice->SetVertexShader(m_pVertexShader);

	//ピクセルシェーダーのセット
	if (m_pPixelShader)
		pDevice->SetPixelShader(m_pPixelShader);

	//速度マップテクスチャセット
	pDevice->SetTexture(1, m_pD3DVelocityTexture);

	//フィルターの描画
	CFilter::Draw(pDevice);

	//テクスチャを戻す
	pDevice->SetTexture(1, NULL);

	//シェーダーを戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//ブラー用レンダーターゲットの切替
//=============================================================================
void CMotionBlur::ChangeBlurSurface(LPDIRECT3DDEVICE9 pDevice,BLUR_SURFACE type)
{
	//バックバッファのポインタ保持
	pDevice->GetRenderTarget(0, &m_backBuffOrg);

	//通常のレンダーターゲット
	if (type == BACK_SURFACE)
	{
		pDevice->SetRenderTarget(0, m_TexSurface);
		pDevice->SetRenderTarget(1, m_TexZSBuff);
	}
	//速度マップ用
	else
		pDevice->SetRenderTarget(0, m_TexVelocitySurface);
}
//=============================================================================
//ブラー用レンダーターゲットを戻す
//=============================================================================
void CMotionBlur::ReturnBlurSurface(LPDIRECT3DDEVICE9 pDevice)
{
	//レンダーターゲットをバックバッファに戻す
	pDevice->SetRenderTarget(0, m_backBuffOrg);
	pDevice->SetRenderTarget(1, NULL);

	//バックバッファ用を解放
	m_backBuffOrg->Release();
}
//EOF