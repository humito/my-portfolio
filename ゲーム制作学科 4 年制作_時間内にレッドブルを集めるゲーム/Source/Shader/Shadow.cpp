//=============================================================================
//投影テクスチャシャドウ[Shadow.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Shadow.h"
#include "../main.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../System/Light.h"
#include "../System/Common.h"

//=============================================================================
//シェーダー読込
//=============================================================================
void CShadow::Load()
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	///////////////////////////////////
	//		シェーダーの初期化		//
	/////////////////////////////////

	//キャスト用シェーダー
	CreateShader(pDevice, "data/HLSL/ShadowCast.hlsl",
				"VertexShader3D", "PixelShader3D",
				"vs_2_0", "ps_2_0",
				&m_pVSConstantTable, &m_pPSConstantTable,
				&m_pVertexShader, &m_pPixelShader);

	//レシーブ用シェーダー
	CreateShader(pDevice, "data/HLSL/ShadowReceive.hlsl",
				"VertexShader3D", "PixelShader3D",
				"vs_2_0", "ps_2_0",
				&m_pVSConstantTableRec, &m_pPSConstantTableRec,
				&m_pVertexShaderRec, &m_pPixelShaderRec);

	///////////////////////////////////////////////
	//		テクスチャとサーフェイスの生成		//
	/////////////////////////////////////////////

	//レンダーターゲット用テクスチャの生成
	D3DXCreateTexture(pDevice,
					SCREEN_WIDTH,			//バックバッファのサイズ
					SCREEN_HEIGHT,
					1,						//ミップマップレベル
					D3DUSAGE_RENDERTARGET,	//レンダーターゲットに指定
					D3DFMT_A8R8G8B8,		//ピクセルフォーマット
					D3DPOOL_DEFAULT,
					&m_pD3DTexture);		//生成したテクスチャのポインタ

	//サーフェイスの取得
	m_pD3DTexture->GetSurfaceLevel(0, &m_TexSurface);

	//Zバッファ生成
	pDevice->CreateDepthStencilSurface(SCREEN_WIDTH,
										SCREEN_HEIGHT,
										D3DFMT_D24S8,// D3DFMT_D16の可能性あり
										D3DMULTISAMPLE_NONE,
										0,
										TRUE,
										&m_TexZSBuff,
										NULL);

	//フィルターの初期化
	CFilter::Init();
}
//=============================================================================
//シェーダー終了
//=============================================================================
void CShadow::Uninit()
{
	//頂点シェーダー定数テーブル
	RELEASE_OBJECT(m_pVSConstantTableRec);

	//ピクセルシェーダー定数テーブル
	RELEASE_OBJECT(m_pPSConstantTableRec);

	//頂点シェーダー
	RELEASE_OBJECT(m_pVertexShaderRec);

	//ピクセルシェーダー
	RELEASE_OBJECT(m_pPixelShaderRec);

	//フィルターの終了
	CFilter::Uninit();
}
//=============================================================================
//マトリックスのセット
//=============================================================================
void CShadow::SetMatrix(LPDIRECT3DDEVICE9 pDevice,
						SHADER_SHADOW type,
						D3DXMATRIX *pMtxWorld)
{
	//ライト基準のビュー行列とプロジェクション行列取得
	D3DXMATRIX matViewLight = CManager::GetLight()->GetMatView();
	D3DXMATRIX matProjLight = CManager::GetLight()->GetMatProj();
	//ライト基準の行列変換マトリクス
	D3DXMATRIX matWorldLight = (*pMtxWorld) * matViewLight * matProjLight;

	//キャスト側
	if (type == SHADER_SHADOW_CAST)
	{
		//ワールド行列 * ビュー行列(ライト) * プロジェクション行列(ライト)の行列セット
		m_pVSConstantTable->SetMatrix(pDevice, "g_WorldViewProjection", &matWorldLight);
	}
	//レシーブ
	else
	{
		//ビュー、プロジェクションマトリクス取得
		D3DXMATRIX matView, matProj;
		pDevice->GetTransform(D3DTS_VIEW, &matView);
		pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
		//カメラ基準の行列変換マトリクスをセット
		m_pVSConstantTableRec->SetMatrix(pDevice, "g_WorldViewProjection", &(*pMtxWorld * matView * matProj));

		//正規化したライトベクトルのセット
		D3DXMATRIX m;
		D3DXVECTOR4 v;
		D3DXVECTOR4 lightDir = D3DXVECTOR4(CManager::GetLight()->GetVecDir(), 1.0f);
		D3DXMatrixInverse(&m, NULL, pMtxWorld);
		D3DXVec4Transform(&v, &lightDir, &m);
		D3DXVec3Normalize((D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v);
		m_pVSConstantTableRec->SetVector(pDevice, "g_LightDir", &v);

		//ライト基準の行列変換マトリクスのセット
		m_pVSConstantTableRec->SetMatrix(pDevice, "g_WorldViewProjectionLight", &matWorldLight);

		//テクスチャ座標系へ変換
		D3DXMATRIX mtxScl, mtxTrs;
		D3DXMatrixScaling(&mtxScl,0.5f, -0.5f, 1.0f);
		D3DXMatrixTranslation(&mtxTrs, 0.5f, 0.5f, 0.0f);
		m = matWorldLight * mtxScl * mtxTrs;
		m_pVSConstantTableRec->SetMatrix(pDevice, "g_WorldViewProjectionLightTex", &m);
	}
}
//=============================================================================
//プレイヤー座標のセット
//=============================================================================
void CShadow::SetPlayerPos(LPDIRECT3DDEVICE9 pDevice, 
							D3DXVECTOR3 pos)
{
	m_pVSConstantTableRec->SetVector(pDevice, "g_PlayerPos", &D3DXVECTOR4(pos.x, pos.y, pos.z, 1.0f));
}
//=============================================================================
//マテリアル色のセット
//=============================================================================
void CShadow::SetColor(LPDIRECT3DDEVICE9 pDevice,
						SHADER_SHADOW type,
						D3DXVECTOR4 color)
{
	if (type == SHADER_SHADOW_RECEIVE)
		m_pVSConstantTableRec->SetVector(pDevice, "g_Color", &color);
	else
		m_pPSConstantTable->SetVector(pDevice, "g_Color", &color);
}
//=============================================================================
//シェーダー開始
//=============================================================================
void CShadow::Begin(LPDIRECT3DDEVICE9 pDevice, SHADER_SHADOW type)
{
	//影シェーダーの種類で始める処理を分ける
	switch (type)
	{
		//キャスト用シェーダー
		case SHADER_SHADOW_CAST:
		{
			//頂点シェーダーのセット
			if (m_pVertexShader)
				pDevice->SetVertexShader(m_pVertexShader);

			//ピクセルシェーダーのセット
			if (m_pPixelShader)
				pDevice->SetPixelShader(m_pPixelShader);

			break;
		}

		//レシーブ用シェーダー
		case SHADER_SHADOW_RECEIVE:
		{
			//頂点シェーダーのセット
			if (m_pVertexShaderRec)
				pDevice->SetVertexShader(m_pVertexShaderRec);

			//ピクセルシェーダーのセット
			if (m_pPixelShaderRec)
				pDevice->SetPixelShader(m_pPixelShaderRec);

			break;
		}
	}
}
//=============================================================================
//シェーダー完了
//=============================================================================
void CShadow::End(LPDIRECT3DDEVICE9 pDevice)
{
	//シェーダーを戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//描画
//=============================================================================
void CShadow::Draw(LPDIRECT3DDEVICE9 pDevice)
{
	//頂点シェーダーのセット
	/*if (m_pVertexShader)
		pDevice->SetVertexShader(m_pVertexShader);

	//ピクセルシェーダーのセット
	if (m_pPixelShader)
		pDevice->SetPixelShader(m_pPixelShader);
		*/
	//フィルターの描画
	CFilter::Draw(pDevice);

	//シェーダーを戻す
	//pDevice->SetVertexShader(NULL);
	//pDevice->SetPixelShader(NULL);
}
//EOF