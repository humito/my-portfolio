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
#include "renderer.h"
#include "manager.h"
#include "InputKeyboard.h"
#include "Camera.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define LIGHT_MOVE_POS (10.0f)//ライト移動量

//=============================================================================
//コンストラクタ
//=============================================================================
CLight::CLight()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CLight::~CLight()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CLight::Init(D3DXVECTOR3 pos, D3DLIGHTTYPE type, DWORD index)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//////////////////////
	//	ライトの初期化	//
	//////////////////////

	//有効ライトの番号
	m_lightIndex = index;

	//ライトをサイズ分確保
	ZeroMemory(&m_light, sizeof(D3DLIGHT9));
	//ライトの種類
	m_light.Type = type;
	//ライトの拡散光
	m_light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//拡散ライトならベクトルに設定
	if (m_light.Type == D3DLIGHT_DIRECTIONAL)
	{
		//ライトベクトル
		m_vecDir = pos;
		//ライトの法線ベクトルの設定
		D3DXVec3Normalize((D3DXVECTOR3 *)&m_light.Direction,
			&m_vecDir);
	}
	//それ以外はライトの座標に入れる
	else
	{
		m_light.Position = pos;
	}

	//ライトのセット
	pDevice->SetLight(m_lightIndex, &m_light);
	//ライトON
	pDevice->LightEnable(m_lightIndex, TRUE);

	//ライティングON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

//=============================================================================
//更新
//=============================================================================
void CLight::Update()
{
//デバッグ時
#ifdef _DEBUG
	//カメラインスタンス取得
	CCamera *m_pCamera = CManager::GetCamera();
	//カメラの向き取得
	D3DXVECTOR3 rot = m_pCamera->GetRotCamera();

	///////////////////////////
	//		ライトの操作	//
	/////////////////////////

	//上
	if (CInputKeyboard::GetKeyPress(DIK_U))
	{
		m_vecDir.y += LIGHT_MOVE_POS;
	}

	//下
	if (CInputKeyboard::GetKeyPress(DIK_M))
	{
		m_vecDir.y -= LIGHT_MOVE_POS;
	}

	//左
	if (CInputKeyboard::GetKeyPress(DIK_LEFT))
	{
		m_vecDir.x -= sinf(rot.y + D3DX_PI / 2)*LIGHT_MOVE_POS;
		m_vecDir.z -= cosf(rot.y + D3DX_PI / 2)*LIGHT_MOVE_POS;
	}

	//右
	if (CInputKeyboard::GetKeyPress(DIK_RIGHT))
	{
		m_vecDir.x += sinf(rot.y + D3DX_PI / 2)*LIGHT_MOVE_POS;
		m_vecDir.z += cosf(rot.y + D3DX_PI / 2)*LIGHT_MOVE_POS;
	}

	//前
	if (CInputKeyboard::GetKeyPress(DIK_UP))
	{
		m_vecDir.x -= cosf(rot.y + D3DX_PI / 2)*LIGHT_MOVE_POS;
		m_vecDir.z += sinf(rot.y + D3DX_PI / 2)*LIGHT_MOVE_POS;
	}

	//後
	if (CInputKeyboard::GetKeyPress(DIK_DOWN))
	{
		m_vecDir.x += cosf(rot.y + D3DX_PI / 2)*LIGHT_MOVE_POS;
		m_vecDir.z -= sinf(rot.y + D3DX_PI / 2)*LIGHT_MOVE_POS;
	}
#endif
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
	D3DXVec3Normalize(	(D3DXVECTOR3 *)&m_light.Direction,
						&m_vecDir);

	//ライトのセット
	pDevice->SetLight(m_lightIndex, &m_light);

	//ライトON
	pDevice->LightEnable(m_lightIndex, TRUE);
}
//=============================================================================
//ライトのベクトル取得
//=============================================================================
D3DXVECTOR3 CLight::GetVecDir()
{
	if (m_light.Type == D3DLIGHT_DIRECTIONAL)
	{
		return m_vecDir;
	}

	return m_light.Position;
}
//EOF