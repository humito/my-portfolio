//=============================================================================
//
// ライト処理 [Light.cpp]
// Author : HUMITO KIMURA
//
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Light.h"
#include "../main.h"
#include "renderer.h"
#include "../manager.h"
#include "Input/InputKeyboard.h"
#include "Camera.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define LIGHT_MOVE_POS (10.0f)	//ライト移動量

//=============================================================================
//初期化
//=============================================================================
HRESULT CLight::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//////////////////////
	//	ライトの初期化	//
	//////////////////////

	//ライトをサイズ分確保
	ZeroMemory(&m_aLight, sizeof(D3DLIGHT9) * LIGHT_NUM);

	//ディレクショナルライト初期化
	InitDirectionalLight(pDevice);

	//ポイントライト初期化
	InitPointLight(pDevice);

	for (int i = 0; i < LIGHT_NUM; ++i)
	{
		//ライトのセット
		pDevice->SetLight(i, &m_aLight[i]);

		//ライトON
		pDevice->LightEnable(i, TRUE);
	}

	//ライティングON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}
//=============================================================================
//ディレクショナルライト初期化
//=============================================================================
void CLight::InitDirectionalLight(LPDIRECT3DDEVICE9 pDevice)
{
	//ライトの種類
	m_aLight[0].Type = D3DLIGHT_DIRECTIONAL;

	//ライトの拡散光
	m_aLight[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//ライトの視点ベクトル
	m_LightEye = D3DXVECTOR3(-0.5f, 0.5f, -0.5f);

	//ライトの上ベクトル
	m_LightUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//ライトの視線ベクトル
	m_LightAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//平行光源の回転マトリックス
	D3DXMATRIX mx, my;
	D3DXMatrixRotationY(&my, D3DXToRadian(170.0f));
	D3DXMatrixRotationX(&mx, D3DXToRadian(30.0f));

	//ライトのビュー行列
	D3DXMATRIX LightMatrix = mx * my;

	//行列に合わせて光源の座標変換
	D3DXVec3TransformCoord(&m_LightEye, &m_LightEye, &LightMatrix);
	D3DXVec3TransformCoord(&m_LightUp, &m_LightUp, &LightMatrix);
	D3DXVec3TransformCoord(&m_LightAt, &m_LightAt, &LightMatrix);

	//ライトベクトル
	m_vecDir = (D3DXVECTOR3)(m_LightAt - m_LightEye);

	//ライトの法線ベクトルの設定
	D3DXVec3Normalize((D3DXVECTOR3 *)&m_aLight[0].Direction,
		&m_vecDir);

	//ライト位置からビュー座標系の設定(カメラように扱う)
	D3DXVECTOR3 longEye = m_LightEye * 700.0f;
	D3DXMatrixLookAtLH(	&m_matViewLight,
						&longEye,
						&m_LightAt,
						&m_LightUp);

	//ライト位置から正射影行列の設定(左手正射影行列を作成)
	D3DXMatrixOrthoLH(	&m_matProjLight,
						(float)SCREEN_WIDTH,
						(float)SCREEN_HEIGHT,
						300.0f,
						1000.0f);
}
//=============================================================================
//ポイントライト初期化
//=============================================================================
void CLight::InitPointLight(LPDIRECT3DDEVICE9 pDevice)
{
	//ポイントライト3つ
	for (int i = 1; i < LIGHT_NUM; ++i)
		m_aLight[i].Type = D3DLIGHT_POINT;

	//ライト座標
	m_aLight[1].Position = D3DXVECTOR3(-500.0f, 500.0f, 500.0f);
	m_aLight[2].Position = D3DXVECTOR3(500.0f, 100.0f, -300.0f);
	m_aLight[3].Position = D3DXVECTOR3(0.0f, 300.0f, -500.0f);

	//有効範囲
	m_aLight[1].Range = 100.5f;
	m_aLight[2].Range = 100.5f;
	m_aLight[3].Range = 100.5f;

	//ライトの拡散光
	m_aLight[1].Diffuse = D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f);
	m_aLight[2].Diffuse = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);
	m_aLight[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);

	//ライトのスペキュラ色
	m_aLight[1].Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_aLight[2].Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_aLight[3].Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}
//=============================================================================
//更新
//=============================================================================
void CLight::Update()
{

}
//=============================================================================
//ライトセット
//=============================================================================
void CLight::Set()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//ライトの法線ベクトルの設定
	D3DXVec3Normalize((D3DXVECTOR3 *)&m_aLight[0].Direction, &m_vecDir);

	//ライティングON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}
//=============================================================================
//ライト座標の取得
//=============================================================================
D3DXVECTOR3 CLight::GetLightPos(int nIndex)
{
	return (D3DXVECTOR3)m_aLight[nIndex].Position;
}
//=============================================================================
//ライトカラー取得
//=============================================================================
D3DXVECTOR3 CLight::GetLightColor(int nIndex)
{
	return D3DXVECTOR3(	m_aLight[nIndex].Diffuse.r,
						m_aLight[nIndex].Diffuse.g,
						m_aLight[nIndex].Diffuse.b);
}
//=============================================================================
//ライトスペキュラ―取得
//=============================================================================
D3DXVECTOR3 CLight::GetLightSpecular(int nIndex)
{
	return D3DXVECTOR3(	m_aLight[nIndex].Specular.r,
						m_aLight[nIndex].Specular.g,
						m_aLight[nIndex].Specular.b);
}
//EOF