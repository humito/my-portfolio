//=============================================================================
// キーボード処理 [InputKeyboard.cpp]
// Author : 木村　文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "InputKeyboard.h"

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
BYTE CInputKeyboard::m_aKeyState[KEY_MAX] = {};	//リピート情報
BYTE CInputKeyboard::m_pKeyState[KEY_MAX] = {};	//キー情報
BYTE CInputKeyboard::m_tKeyState[KEY_MAX] = {};	//トリガー情報
BYTE CInputKeyboard::m_rKeyState[KEY_MAX] = {};	//リリース情報
int	CInputKeyboard::m_nCount = 0;				//カウント

//=============================================================================
//コンストラクタ
//=============================================================================
CInputKeyboard::CInputKeyboard()
{
	m_pDIDevKeyboard = NULL;
}
//=============================================================================
//デストラクタ
//=============================================================================
CInputKeyboard::~CInputKeyboard()
{

}
//=============================================================================
//初期化
//=============================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	//キー情報初期化
	for (int i = 0; i<KEY_MAX; i++)
	{
		m_aKeyState[i] = 0;//キー情報
		m_pKeyState[i] = 0;//前キー情報
		m_tKeyState[i] = 0;//トリガー情報
		m_rKeyState[i] = 0;//リリース情報
	}

	//カウント初期化
	m_nCount = 0;

	//入力処理初期化
	CInput::Init(hInstance, hWnd);

	//デバイスオブジェクトを作成
	m_pDInput->CreateDevice(GUID_SysKeyboard,
		&m_pDIDevKeyboard, NULL);

	//データフォーマットを設定
	m_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);

	//協調モードを設定（フォアグラウンド＆非排他モード）
	m_pDIDevKeyboard->SetCooperativeLevel(hWnd,
		(DISCL_FOREGROUND
		| DISCL_NONEXCLUSIVE));

	//キーボードへのアクセス権を獲得(入力制御開始)
	m_pDIDevKeyboard->Acquire();

	return S_OK;
}
//=============================================================================
//キーボード終了処理
//=============================================================================
void CInputKeyboard::Uninit()
{
	//入力処理終了
	CInput::Uninit();

	//キーボードデバイスオブジェクトの開放
	if (m_pDIDevKeyboard != NULL)
	{
		m_pDIDevKeyboard->Unacquire();
		m_pDIDevKeyboard->Release();
		m_pDIDevKeyboard = NULL;
	}
}
//=============================================================================
// キーボードの更新処理
//=============================================================================
void CInputKeyboard::Update(void)
{
	//キー情報取得用
	BYTE aKeyState[KEY_MAX];

	//データを取得
	if (SUCCEEDED(m_pDIDevKeyboard->GetDeviceState(
		sizeof(aKeyState),
		&aKeyState[0])))
	{
		//キーの更新
		for (int nKey = 0; nKey<KEY_MAX; nKey++)
		{
			m_pKeyState[nKey] = m_aKeyState[nKey];											//Prev
			m_aKeyState[nKey] = aKeyState[nKey];												//Read
			m_tKeyState[nKey] = (m_pKeyState[nKey] ^ m_aKeyState[nKey]) & m_aKeyState[nKey];	//trig
			m_rKeyState[nKey] = (m_pKeyState[nKey] ^ m_aKeyState[nKey]) & m_pKeyState[nKey];	//release
		}
	}
	else
	{
		//キーボードへのアクセス権取得（入力できる状態）
		m_pDIDevKeyboard->Acquire();
	}
}
//=============================================================================
// キーボードのプレス状態を取得
//=============================================================================
bool CInputKeyboard::GetKeyPress(int nKey)
{
	//引数と同じキーが押されたら
	if ((m_aKeyState[nKey] & 0x80))
	{
		return true;
	}

	//何も押されない時はfalseで返す
	return false;
}
//=============================================================================
// キーボードのトリガー状態を取得
//=============================================================================
bool CInputKeyboard::GetKeyTrigger(int nKey)
{
	if (m_tKeyState[nKey] & 0x80)
	{
		return true;
	}

	return false;
}
//=============================================================================
// キーボードのリピート状態を取得
//=============================================================================
bool CInputKeyboard::GetKeyRepeat(int nKey, bool push)
{
	//入力判定
	if (m_aKeyState[nKey] & 0x80)
	{
		//カウント5～35入力不可
		if (m_nCount<5 || m_nCount>35)
		{
			m_nCount++;
			return true;
		}
		else
		{
			//5～35の間はカウントアップ
			m_nCount++;
		}
	}
	else if (push == true)
	{
		//何も押されてなくて押した判定がtrueならカウントリセット
		m_nCount = 0;
	}

	return false;
}
//=============================================================================
// キーボードのリリ－ス状態を取得
//=============================================================================
bool CInputKeyboard::GetKeyRelease(int nKey)
{
	if (m_rKeyState[nKey] & 0x80)
	{
		return true;
	}

	return false;
}
//EOF