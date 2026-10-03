//=============================================================================
// マルチレンダーターゲット[MultiRenderTarget.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "DeferredRendering.h"
#include "../main.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../System/Light.h"
#include "../System/Camera.h"
#include "../System/Common.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CDeferred::CDeferred()
{
	for (int i = 0; i < RENDER_NUM; ++i)
	{
		m_RenderTexture[i].pD3DTexture = NULL;
		m_RenderTexture[i].pSurface = NULL;
		m_backBuffOrg[i] = NULL;
	}
}
//=============================================================================
//初期化
//=============================================================================
void CDeferred::Load()
{
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//レンダリング用
	CShader::CreateShader(	pDevice,"data/HLSL/DeferredRender.hlsl",
							"VS_3D","PS_3D",
							"vs_2_0","ps_3_0",
							&m_pVSConstantTable, &m_pPSConstantTable,
							&m_pVertexShader, &m_pPixelShader);

	//ポストエフェクト用
	CShader::CreateShader(pDevice, "data/HLSL/DeferredRender.hlsl",
							"VS_2D", "PS_2D",
							"vs_2_0", "ps_3_0",
							&m_pVS2DConstantTable, &m_pPS2DConstantTable,
							&m_p2DVertexShader, &m_p2DPixelShader);

	///////////////////////////
	//	頂点バッファの設定	//
	/////////////////////////

	//頂点座標の代入
	m_aVtx[0].vtx = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);
	m_aVtx[1].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_aVtx[2].vtx = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	m_aVtx[3].vtx = D3DXVECTOR3(SCREEN_WIDTH, 0.0f, 0.0f);

	//幅
	m_aVtx[0].rhw = 1.0f;
	m_aVtx[1].rhw = 1.0f;
	m_aVtx[2].rhw = 1.0f;
	m_aVtx[3].rhw = 1.0f;

	//反射光
	m_aVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_aVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_aVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_aVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	//テクスチャ座標
	m_aVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
	m_aVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	m_aVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	m_aVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

	//テクスチャとサーフェイスの生成
	CreateTexSurface();
}
//=============================================================================
//テクスチャとサーフェイスの生成
//=============================================================================
void CDeferred::CreateTexSurface()
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//各レンダリングのテクスチャ生成
	for (int i = 0; i < RENDER_NUM; ++i)
	{
		//フォーマット決める
		D3DFORMAT d3dFormat = D3DFMT_A8R8G8B8;

		if (i == RENDER_COLOR || i == RENDER_NORMAL)
			d3dFormat = D3DFMT_R8G8B8;
		else if (i == RENDER_ZBUFF)
			d3dFormat = D3DFMT_R32F; //D3DFMT_R32Fも試す;
		else if (i == RENDER_POSITION)
			d3dFormat = D3DFMT_A32B32G32R32F;

		D3DXCreateTexture(pDevice,
						SCREEN_WIDTH,			//バックバッファのサイズ
						SCREEN_HEIGHT,
						1,						//ミップマップレベル
						D3DUSAGE_RENDERTARGET,	//レンダーターゲットに指定
						d3dFormat,				//ピクセルフォーマット
						D3DPOOL_DEFAULT,
						&m_RenderTexture[i].pD3DTexture);		//生成したテクスチャのポインタ

		//生成したテクスチャからサーフェイスの取得
		m_RenderTexture[i].pD3DTexture->GetSurfaceLevel(0, &m_RenderTexture[i].pSurface);
	}
}
//=============================================================================
//終了
//=============================================================================
void CDeferred::Uninit()
{
	//各テクスチャとサーフェイスの解放
	for (int i = 0; i < RENDER_NUM; ++i)
	{
		RELEASE_OBJECT(m_RenderTexture[i].pD3DTexture);
		RELEASE_OBJECT(m_RenderTexture[i].pSurface);
	}

	RELEASE_OBJECT(m_pVS2DConstantTable);
	RELEASE_OBJECT(m_pPS2DConstantTable);
	RELEASE_OBJECT(m_p2DVertexShader);
	RELEASE_OBJECT(m_p2DPixelShader);

	//シェーダーの終了
	CShader::Release();
}
//=============================================================================
//シェーダー開始
//=============================================================================
void CDeferred::Begin(LPDIRECT3DDEVICE9 pDevice)
{
	if (m_pVertexShader)
		pDevice->SetVertexShader(m_pVertexShader);

	if (m_pPixelShader)
		pDevice->SetPixelShader(m_pPixelShader);
}
//=============================================================================
//シェーダー終了
//=============================================================================
void CDeferred::End(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//マトリックスの設定
//=============================================================================
void CDeferred::SetMatrix(LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX *pMtxWorld)
{
	//ビュー、プロジェクションマトリクス取得
	D3DXMATRIX matView, matProj;
	pDevice->GetTransform(D3DTS_VIEW, &matView);
	pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	//ワールドマトリクスセット
	m_pVSConstantTable->SetMatrix(pDevice, "g_World", pMtxWorld);
	m_pVSConstantTable->SetMatrix(pDevice, "g_WorldViewProjection", &(*pMtxWorld * matView * matProj));
}
//=============================================================================
//マテリアルの設定
//=============================================================================
void CDeferred::SetMaterial(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR4 materialVec)
{
	m_pVSConstantTable->SetVector(pDevice, "g_Color", &materialVec);
}
//=============================================================================
//ライトの準備
//=============================================================================
void CDeferred::PreparationLight(LPDIRECT3DDEVICE9 pDevice)
{
	CLight *pLight = CManager::GetLight();

	//ポイントライト数
	m_pPS2DConstantTable->SetInt(pDevice, "g_PointLightNum", (LIGHT_NUM - 1));

	//4つのポイントライトから定数レジスタに送る
	D3DXVECTOR3 pointLightPos[LIGHT_NUM - 1];		//ポイントライト座標
	D3DXVECTOR3 pointLightColor[LIGHT_NUM - 1];		//ポイントライト色

	//ライトから各情報を取得
	for (int i = 1; i < LIGHT_NUM; ++i)
	{
		pointLightPos[i - 1]	= pLight->GetLightPos(i);
		pointLightColor[i - 1]	= pLight->GetLightColor(i);
	}

	//ポイントライト座標のセット
	m_pPS2DConstantTable->SetVectorArray(pDevice, "g_PointLightPosW",
										(D3DXVECTOR4*)pointLightPos,
										(LIGHT_NUM - 1));

	//ポイントライト色のセット
	m_pPS2DConstantTable->SetVectorArray(pDevice, "g_LightColor",
										(D3DXVECTOR4*)pointLightColor,
										(LIGHT_NUM - 1));

	//ポイントライト減衰率のセット
	float fAttenuation[3] = { 0.0f, 0.1f, 0.3f };
	m_pPS2DConstantTable->SetFloatArray(pDevice, "g_Attenuation", fAttenuation, 3);
}
//=============================================================================
//カメラの準備
//=============================================================================
void CDeferred::PreparationCamera(LPDIRECT3DDEVICE9 pDevice)
{
	//カメラ座標のセット
	D3DXVECTOR4 cameraPos = D3DXVECTOR4(CManager::GetCamera()->GetPosCamera(), 1.0f);
	m_pPS2DConstantTable->SetVector(pDevice, "g_CameraPos", &cameraPos);
}
//=============================================================================
//描画(シェーダーあり)
//=============================================================================
void CDeferred::Draw(LPDIRECT3DDEVICE9 pDevice)
{
	//Zバッファ無効
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	if (m_p2DVertexShader)
		pDevice->SetVertexShader(m_p2DVertexShader);

	if (m_p2DPixelShader)
		pDevice->SetPixelShader(m_p2DPixelShader);

	//テクスチャの設定
	pDevice->SetTexture(m_pPS2DConstantTable->GetSamplerIndex("g_ColorSampler"),
						m_RenderTexture[RENDER_COLOR].pD3DTexture);
	pDevice->SetTexture(m_pPS2DConstantTable->GetSamplerIndex("g_NormalSampler"),
						m_RenderTexture[RENDER_NORMAL].pD3DTexture);
	pDevice->SetTexture(m_pPS2DConstantTable->GetSamplerIndex("g_PositionSampler"),
						m_RenderTexture[RENDER_POSITION].pD3DTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &m_aVtx[0], sizeof(VERTEX_2D));

	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);

	//Zバッファ戻す
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}
//=============================================================================
//描画(シェーダーなし)
//=============================================================================
void CDeferred::Draw(LPDIRECT3DDEVICE9 pDevice, RENDER_TYPE type)
{
	//Zバッファ無効
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_RenderTexture[type].pD3DTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &m_aVtx[0], sizeof(VERTEX_2D));

	//Zバッファ戻す
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}
//=============================================================================
//マルチレンダーターゲット全て描画
//=============================================================================
void CDeferred::DrawAllRendering(LPDIRECT3DDEVICE9 pDevice)
{
	//Zバッファ無効
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//分割した画面サイズ
	float fSizeX = SCREEN_WIDTH / 5;
	float fSizeY = SCREEN_HEIGHT / RENDER_NUM;

	//レンダリング数分描画
	for (int i = 0; i < RENDER_NUM; ++i)
	{
		//頂点座標を画面縦4分割する
		m_aVtx[0].vtx = D3DXVECTOR3(0.0f, fSizeY * (i + 1), 0.0f);
		m_aVtx[1].vtx = D3DXVECTOR3(0.0f, fSizeY * i, 0.0f);
		m_aVtx[2].vtx = D3DXVECTOR3(fSizeX, fSizeY * (i + 1), 0.0f);
		m_aVtx[3].vtx = D3DXVECTOR3(fSizeX, fSizeY * i, 0.0f);

		//テクスチャの設定
		pDevice->SetTexture(0, m_RenderTexture[i].pD3DTexture);

		//ポリゴンの描画
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
								2,
								&m_aVtx[0],
								sizeof(VERTEX_2D));
	}

	//Zバッファ戻す
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	//頂点座標を戻す
	m_aVtx[0].vtx = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);
	m_aVtx[1].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_aVtx[2].vtx = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	m_aVtx[3].vtx = D3DXVECTOR3(SCREEN_WIDTH, 0.0f, 0.0f);
}
//=============================================================================
//全レンダーターゲット切替
//=============================================================================
void CDeferred::ChangeSurfaceAll(LPDIRECT3DDEVICE9 pDevice)
{
	for (int i = 0; i < RENDER_NUM; ++i)
	{
		//バックバッファのポインタ保持
		pDevice->GetRenderTarget(i, &m_backBuffOrg[i]);

		//各レンダーターゲットをテクスチャに設定
		pDevice->SetRenderTarget(i, m_RenderTexture[i].pSurface);
	}
}
//=============================================================================
//全レンダーターゲット戻す
//=============================================================================
void CDeferred::ReturnSurfaceAll(LPDIRECT3DDEVICE9 pDevice)
{
	for (int i = 0; i < RENDER_NUM; ++i)
	{
		//レンダーターゲットをバックバッファに戻す
		pDevice->SetRenderTarget(i, m_backBuffOrg[i]);
		//バックバッファ用を解放
		RELEASE_OBJECT(m_backBuffOrg[i]);
	}
}
//=============================================================================
//レンダリングテクスチャの取得
//=============================================================================
LPDIRECT3DTEXTURE9 CDeferred::GetRenderingTexture(RENDER_TYPE type)
{
	//指定した種類によってそれぞれのレンダリングテクスチャを返す
	return m_RenderTexture[type].pD3DTexture;
}
//EOF