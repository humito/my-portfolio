//=============================================================================
// マウス処理 [InputMouse.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "InputMouse.h"
#include "../../main.h"
#include "../../manager.h"
#include "../Camera.h"
#include "../DebugProc.h"

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
BYTE CInputMouse::m_aButtonState[MOUSE_BUTTON_MAX] = {};				//キー情報
BYTE CInputMouse::m_pButtonState[MOUSE_BUTTON_MAX] = {};				//キー情報
BYTE CInputMouse::m_tButtonState[MOUSE_BUTTON_MAX] = {};				//トリガー情報
BYTE CInputMouse::m_rButtonState[MOUSE_BUTTON_MAX] = {};				//リリース情報
D3DXVECTOR2  CInputMouse::m_ClientPos;									//マウス(クライアント)座標
D3DXVECTOR3  CInputMouse::m_WorldPos;									//マウス(ワールド)座標
D3DXVECTOR3 CInputMouse::m_Velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//マウス速度
D3DXVECTOR3 CInputMouse::m_ProjectionVec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//ワールド空間に射影したベクトル

//=============================================================================
//コンストラクタ
//=============================================================================
CInputMouse::CInputMouse()
{
	m_pDIDevMouse = NULL;
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	//ビューポート取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();
	pDevice->GetViewport(&m_ViewPort);

	//キー情報初期化
	for (int i = 0; i<MOUSE_BUTTON_MAX; i++)
	{
		m_aButtonState[i] = 0;//キー情報
		m_pButtonState[i] = 0;//前キー情報
		m_tButtonState[i] = 0;//トリガー情報
		m_rButtonState[i] = 0;//リリース情報
	}

	//入力処理初期化
	CInput::Init(hInstance, hWnd);

	//ウィンドウハンドルセット
	m_hWnd = hWnd;

	//デバイスオブジェクトを作成
	m_pDInput->CreateDevice(GUID_SysMouse,
				&m_pDIDevMouse, NULL);

	//データフォーマットを設定
	m_pDIDevMouse->SetDataFormat(&c_dfDIMouse2);

	//軸モードの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL; //相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）
	m_pDIDevMouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);

	//マウスのアクセス権を獲得
	m_pDIDevMouse->Acquire();

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CInputMouse::Uninit()
{
	//入力処理終了
	CInput::Uninit();

	//マウスデバイスオブジェクトの解放
	if (m_pDIDevMouse)
	{
		m_pDIDevMouse->Unacquire();
		m_pDIDevMouse->Release();
		m_pDIDevMouse = NULL;
	}
}
//=============================================================================
//マウス更新
//=============================================================================
void CInputMouse::Update()
{
	//ボタン情報取得用
	DIMOUSESTATE2  diMouseState;

	//初期化
	for (int i = 0; i < MOUSE_BUTTON_MAX; ++i)
		diMouseState.rgbButtons[i] = 0;

	//データを取得
	if (SUCCEEDED(m_pDIDevMouse->GetDeviceState(
		sizeof(DIMOUSESTATE2),
		&diMouseState)))
	{
		//キーの更新
		for (int nButton = 0; nButton < MOUSE_BUTTON_MAX; nButton++)
		{
			m_pButtonState[nButton] = m_aButtonState[nButton];														//Prev
			m_aButtonState[nButton] = diMouseState.rgbButtons[nButton];												//Read
			m_tButtonState[nButton] = (m_pButtonState[nButton] ^ m_aButtonState[nButton]) & m_aButtonState[nButton];//trig
			m_rButtonState[nButton] = (m_pButtonState[nButton] ^ m_aButtonState[nButton]) & m_pButtonState[nButton];//release
		}

		//マウス速度の更新
		m_Velocity = D3DXVECTOR3((float)diMouseState.lX,
								(float)diMouseState.lY,
								(float)diMouseState.lZ);
	}
	else
	{
		//マウスへのアクセス権取得（入力できる状態）
		m_pDIDevMouse->Acquire();
	}

	//マウスのクライアント座標取得
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(m_hWnd, &mousePos);
	m_ClientPos = D3DXVECTOR2((float)mousePos.x, (float)mousePos.y);

	//マウススクリーン座標計算
	ScreenToWorldPos();

#ifdef _DEBUG
	CDebug::Print("\nクライアント座標(X:%f, Y:%f)", m_ClientPos.x,
													m_ClientPos.y);
#endif
}
//=============================================================================
//スクリーン座標からワールド座標へ
//=============================================================================
void CInputMouse::ScreenToWorldPos()
{
	//カメラインスタンス取得
	CCamera *pCamera = CManager::GetCamera();

	//カメラ座標
	D3DXVECTOR3 cameraPos = pCamera->GetPosCamera();

	//ワールドマトリックス初期化
	D3DXMatrixIdentity(&m_matWorld);

	//マウスカーソルをワールド座標化
	D3DXVec3Unproject(&m_ProjectionVec,
						&D3DXVECTOR3(m_ClientPos.x, m_ClientPos.y, 0.0f),
						&m_ViewPort,
						&pCamera->GetMtxProj(),
						&pCamera->GetMtxView(),
						&m_matWorld);

	//射影ベクトルをワールド座標用として平面との交点をとる
	m_WorldPos = m_ProjectionVec;

	//ZX平面
	D3DXPLANE plane = D3DXPLANE(0.0f, 1.0f, 0.0f, 0.0f);

	//ZX平面とカメラの向いてる方向との交点を求める
	if (cameraPos.y - m_WorldPos.y != 0.0f)
	{
		D3DXPlaneIntersectLine(&m_WorldPos, &plane, &cameraPos, &m_WorldPos);
	}
	else
	{
		//交差しない場合はfar平面との交点を求める
		plane = D3DXPLANE(0.0f, 0.0f, -1.0f, 100.0f);
		D3DXPlaneIntersectLine(&m_WorldPos, &plane, &cameraPos, &m_WorldPos);
	}

#ifdef _DEBUG
	CDebug::Print("\nワールド座標(X:%f, Y:%f, Z:%f)",
		m_WorldPos.x, m_WorldPos.y, m_WorldPos.z);
#endif
}
//=============================================================================
//マウスワールド座標との当たり判定
//=============================================================================
bool CInputMouse::HitCheckWorldPos(D3DXVECTOR3 pos, float fRadius)
{
	//カメラ座標取得
	D3DXVECTOR3 cameraPos = CManager::GetCamera()->GetPosCamera();

	//カメラから目標の対象オブジェクトへのベクトル
	D3DXVECTOR3 PQ = pos - cameraPos;

	//目標の視点からの距離
	float fZdepth = D3DXVec3LengthSq(&PQ);

	//設定された目標より後ろなら返す
	if (fZdepth >= FLT_MAX)
		return false;

	//カメラとワールド座標
	D3DXVECTOR3 worldVec = m_ProjectionVec - cameraPos;

	//内積から目標との距離
	float fPQV = D3DXVec3Dot(&PQ, &worldVec);
	float fHQ = D3DXVec3Dot(&PQ, &PQ) - fPQV *
			fPQV / D3DXVec3Dot(&worldVec, &worldVec);

	//目標との当たり判定 (fHQ < 半径の２乗)
	if (fHQ < (fRadius * fRadius))
		return true;

	return false;
}
//=============================================================================
//クライアント内か判定
//=============================================================================
bool CInputMouse::InClientCheck()
{
	if ((m_ClientPos.x > 0.0f && m_ClientPos.x < SCREEN_WIDTH)
		&& (m_ClientPos.y > 0.0f && m_ClientPos.y < SCREEN_HEIGHT))
		return true;

	return false;
}
//=============================================================================
//マウスプレス判定
//=============================================================================
bool CInputMouse::GetButtonPress(int nButton)
{
	//引数と同じキーが押されたら
	if (m_aButtonState[nButton] & 0x80)
		return true;

	//何も押されない時はfalseで返す
	return false;
}
//=============================================================================
//マウストリガー判定
//=============================================================================
bool CInputMouse::GetButtonTrigger(int nButton)
{
	//引数と同じキーが押されたら
	if (m_tButtonState[nButton] & 0x80)
		return true;

	//何も押されない時はfalseで返す
	return false;
}
//=============================================================================
//マウスリリース判定
//=============================================================================
bool CInputMouse::GetButtonRelease(int nButton)
{
	//引数と同じキーが押されたら
	if (m_rButtonState[nButton] & 0x80)
		return true;

	//何も押されない時はfalseで返す
	return false;
}
//EOF