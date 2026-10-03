//=============================================================================
//
// ライト処理 [CLight.cpp]
// Author : HUMITO KIMURA
//
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CLight.h"
#include "renderer.h"
#include "manager.h"
#include "CInputKeyboard.h"
#include "Ccamera.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define LIGHT_MOVE_POS (20.0f)//ライト移動量

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
HRESULT CLight::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//////////////////////
	//	ライトの初期化	//
	//////////////////////

	//ライトをサイズ分確保
	ZeroMemory(&m_aLight[0],sizeof(D3DLIGHT9));

	//ライトの種類
	m_aLight[0].Type=D3DLIGHT_DIRECTIONAL;

	//ライトの拡散光
	m_aLight[0].Diffuse=D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);

	//ライトベクトル
	m_vecDir=D3DXVECTOR3(500.0f,-2000.0f,700.0f);


	//ライトの法線ベクトルの設定
	D3DXVec3Normalize(	(D3DXVECTOR3 *)&m_aLight[0].Direction,
						&m_vecDir);

	//ライトのセット
	pDevice->SetLight(0,&m_aLight[0]);

	//ライトON
	pDevice->LightEnable(0,TRUE);

	//ライティングON
	pDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

	return S_OK;
}
//=============================================================================
//更新
//=============================================================================
void CLight::Update()
{

//デバッグ時のみライトの操作が可能
#ifdef _DEBUG

	//カメラインスタンス取得
	CCamera *m_pCamera=CManager::GetCamera();

	//カメラの向き取得
	D3DXVECTOR3 rot=m_pCamera->GetRotCamera();

	///////////////////////////
	//		ライトの操作	//
	/////////////////////////

	//上
	if(CInputKeyboard::GetKeyPress(DIK_RSHIFT))
	{
		m_vecDir.y+=LIGHT_MOVE_POS;
	}

	//下
	if(CInputKeyboard::GetKeyPress(DIK_LSHIFT))
	{
		m_vecDir.y-=LIGHT_MOVE_POS;
	}

	//左
	if(CInputKeyboard::GetKeyPress(DIK_LEFT))
	{
		m_vecDir.x-=sinf(rot.y+D3DX_PI/2)*LIGHT_MOVE_POS;
		m_vecDir.z-=cosf(rot.y+D3DX_PI/2)*LIGHT_MOVE_POS;
	}

	//右
	if(CInputKeyboard::GetKeyPress(DIK_RIGHT))
	{
		m_vecDir.x+=sinf(rot.y+D3DX_PI/2)*LIGHT_MOVE_POS;
		m_vecDir.z+=cosf(rot.y+D3DX_PI/2)*LIGHT_MOVE_POS;
	}

	//前
	if(CInputKeyboard::GetKeyPress(DIK_UP))
	{
		m_vecDir.x-=cosf(rot.y+D3DX_PI/2)*LIGHT_MOVE_POS;
		m_vecDir.z+=sinf(rot.y+D3DX_PI/2)*LIGHT_MOVE_POS;
	}

	//後
	if(CInputKeyboard::GetKeyPress(DIK_DOWN))
	{
		m_vecDir.x+=cosf(rot.y+D3DX_PI/2)*LIGHT_MOVE_POS;
		m_vecDir.z-=sinf(rot.y+D3DX_PI/2)*LIGHT_MOVE_POS;
	}

#endif
}
//=============================================================================
//ライトセット
//=============================================================================
void CLight::Set()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//ライトの法線ベクトルの設定
	D3DXVec3Normalize(	(D3DXVECTOR3 *)&m_aLight[0].Direction,
						&m_vecDir);

	//ライトのセット
	pDevice->SetLight(0,&m_aLight[0]);

	//ライトON
	pDevice->LightEnable(0,TRUE);

	//ライティングON
	pDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
}
//=============================================================================
//ライトベクトルの取得
//=============================================================================
D3DXVECTOR3 CLight::GetVecDir()
{
	return m_vecDir;
}
//EOF