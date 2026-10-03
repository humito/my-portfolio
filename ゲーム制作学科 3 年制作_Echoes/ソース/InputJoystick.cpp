//=============================================================================
//ジョイスティック入力処理[CInputJoyStick.h]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "InputJoystick.h"
#include "Debugproc.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define STICK_REFERENCE_VALUE_X (32895)	//スティックX座標の基準値
#define STICK_REFERENCE_VALUE_Y (32638)	//スティックY座標の基準値
#define STICK_INPUT_VALUE (10000)		//スティック入力判定範囲
#define ARROW_NUM (4500)				//十字キーの単位数
#define ARROW_NO_PUSH (4294967295)		//十字キーを押してない場合の数値
#define BUTTON_MAX (8)					//ゲームパッドのボタン数
#define ARROW_BUTTON_MAX (4)			//ゲームパッドの十字キー数

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
BYTE CInputJoystick::m_aPadState[PAD_MAX]={};//ボタンを押した情報
BYTE CInputJoystick::m_pPadState[PAD_MAX]={};//ボタンを押した前情報
BYTE CInputJoystick::m_tPadState[PAD_MAX]={};//ボタンを押した瞬間の情報
BYTE CInputJoystick::m_rPadState[PAD_MAX]={};//ボタンを離した瞬間の情報

//=============================================================================
//コンストラクタ
//=============================================================================
CInputJoystick::CInputJoystick()
{
	m_pDIDevJoystick=NULL;
}
//=============================================================================
//デストラクタ
//=============================================================================
CInputJoystick::~CInputJoystick()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CInputJoystick::Init(HINSTANCE hInstance, HWND hWnd)
{
	//各パッド情報初期化
	for(int i=0;i<PAD_MAX;i++)
	{
		m_aPadState[i]=0;//パッド情報
		m_pPadState[i]=0;//前パッド情報
		m_tPadState[i]=0;//トリガー情報
		m_rPadState[i]=0;//リリース情報
	}

	//入力処理初期化
	CInput::Init(hInstance,hWnd);

	//デバイスの作成
	m_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL,(LPDIENUMDEVICESCALLBACKA)EnumJoysticksCallback(),NULL,DIEDFL_ATTACHEDONLY);

	//ジョイスティックデバイスが生成された場合のみ初期化
	if(m_pDIDevJoystick!=NULL)
	{
		//データフォーマットの設定
		m_pDIDevJoystick->SetDataFormat(&c_dfDIJoystick);

		//協調モードを設定（フォアグラウンド＆排他モード）
		m_pDIDevJoystick->SetCooperativeLevel( hWnd ,DISCL_FOREGROUND | DISCL_EXCLUSIVE);

		//デバイス能力取得
		m_pDIDevJoystick->GetCapabilities(&m_diDevCaps);

		//十字キーの範囲指定
		m_pDIDevJoystick->EnumObjects((LPDIENUMDEVICEOBJECTSCALLBACKA)EnumAxesCallback(),(void*)hWnd,DIDFT_AXIS);
	
		//デバイスからデータを取得できるか確認
		if(m_pDIDevJoystick->Poll())
		{
			//アクセス権を取得
			m_pDIDevJoystick->Acquire();
		}
	}

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CInputJoystick::Uninit()
{
	//入力処理の終了
	CInput::Uninit();

	//ジョイスティックデバイスオブジェクトの開放
	if(m_pDIDevJoystick!=NULL)
	{
		m_pDIDevJoystick->Unacquire();
		m_pDIDevJoystick->Release();
		m_pDIDevJoystick=NULL;
	}
}
//=============================================================================
//更新
//=============================================================================
void CInputJoystick::Update()
{
	//ジョイスティックデバイスが存在する場合のみ入力更新する
	if(m_pDIDevJoystick)
	{
		//ジョイスティック入力情報
		DIJOYSTATE js;

		//デバイスからのデータ取得確認
		if(m_pDIDevJoystick->Poll())
		{
			//アクセス権の取得
			if(SUCCEEDED(m_pDIDevJoystick->GetDeviceState(sizeof(DIJOYSTATE),&js)))
			{
				///////////////////////////////////
				//		パッド情報の更新		//
				/////////////////////////////////
				//前パッド情報更新
				for(int i=0;i<PAD_MAX;i++)
				{
					m_pPadState[i]=m_aPadState[i];
					//TODO:現在の入力情報をリセット
				}

				///////////////////////////////////////////////
				//		取得したパッド情報を配列へ代入		//
				/////////////////////////////////////////////

				//十字キーの設定
				//残りの十字キーの入力情報を0にする
				for(int i=0;i<ARROW_BUTTON_MAX;i++)
				{
					m_aPadState[ARROW_UP+i]=0;
				}

				//十字キーが押されている場合
				if(js.rgdwPOV[0]!=ARROW_NO_PUSH)
				{
					//十字キーの番号を計算
					int nArrowIndex=js.rgdwPOV[0]/ARROW_NUM;

					//番号が偶数の場合(十字キーのどれかを押した場合)
					if(nArrowIndex%2==0)
					{
						//直接十字キーの配列内に入力情報を入れる
						m_aPadState[ARROW_UP+nArrowIndex/2]=0x80;

					}//番号が奇数の場合(斜めに押した場合)
					else if(nArrowIndex%2==1)
					{
						//前の十字キーの配列内に入れる
						m_aPadState[ARROW_UP+nArrowIndex/2]=0x80;
						//次の十字キーの配列内に入れる
						m_aPadState[ARROW_UP+(nArrowIndex/2+1)%ARROW_BUTTON_MAX]=0x80;
					}
				}

				//ボタン入力セット
				for(int i=0;i<BUTTON_MAX;i++)
				{
					//ボタン入力情報代入
					m_aPadState[BUTTON_1+i]=js.rgbButtons[i];
				}

				//左スティック
				//上
				if(js.lY<STICK_REFERENCE_VALUE_Y-STICK_INPUT_VALUE)
				{
					m_aPadState[STICK_L_UP]=0x80;
					m_aPadState[STICK_L_DOWN]=0;
				}//下
				else if(js.lY>STICK_REFERENCE_VALUE_Y+STICK_INPUT_VALUE)
				{
					m_aPadState[STICK_L_UP]=0;
					m_aPadState[STICK_L_DOWN]=0x80;
				}
				else
				{
					m_aPadState[STICK_L_UP]=0;
					m_aPadState[STICK_L_DOWN]=0;
				}

				//右
				if(js.lX>STICK_REFERENCE_VALUE_X+STICK_INPUT_VALUE)
				{
					m_aPadState[STICK_L_RIGHT]=0x80;
					m_aPadState[STICK_L_LEFT]=0;
				}//左
				else if(js.lX<STICK_REFERENCE_VALUE_X-STICK_INPUT_VALUE)
				{
					m_aPadState[STICK_L_RIGHT]=0;
					m_aPadState[STICK_L_LEFT]=0x80;
				}
				else
				{
					m_aPadState[STICK_L_RIGHT]=0;
					m_aPadState[STICK_L_LEFT]=0;
				}

				//右スティック
				//上
				if(js.lRy<STICK_REFERENCE_VALUE_Y-STICK_INPUT_VALUE)
				{
					m_aPadState[STICK_R_UP]=0x80;
					m_aPadState[STICK_R_DOWN]=0;
				}//下
				else if(js.lRy>STICK_REFERENCE_VALUE_Y+STICK_INPUT_VALUE)
				{
					m_aPadState[STICK_R_UP]=0;
					m_aPadState[STICK_R_DOWN]=0x80;
				}
				else
				{
					m_aPadState[STICK_R_UP]=0;
					m_aPadState[STICK_R_DOWN]=0;
				}

				//右
				if(js.lRx>STICK_REFERENCE_VALUE_X+STICK_INPUT_VALUE)
				{
					m_aPadState[STICK_R_RIGHT]=0x80;
					m_aPadState[STICK_R_LEFT]=0;
				}//左
				else if(js.lRx<STICK_REFERENCE_VALUE_X-STICK_INPUT_VALUE)
				{
					m_aPadState[STICK_R_RIGHT]=0;
					m_aPadState[STICK_R_LEFT]=0x80;
				}
				else
				{
					m_aPadState[STICK_R_RIGHT]=0;
					m_aPadState[STICK_R_LEFT]=0;
				}

				//トリガー、リリースパッド情報更新
				for(int i=0;i<PAD_MAX;i++)
				{
					m_tPadState[i]=(m_pPadState[i] ^ m_aPadState[i]) & m_aPadState[i];	//trig
					m_rPadState[i]=(m_pPadState[i] ^ m_aPadState[i]) & m_pPadState[i];	//release
				}
			}
			else
			{
				//アクセス権の取得
				m_pDIDevJoystick->Acquire();
			}
		}
	}
}
//=============================================================================
//ゲームパッドプレス判定
//=============================================================================
bool CInputJoystick::GetPadPress(GAME_PAD_NAME PadName)
{
	if(m_aPadState[PadName] & 0x80)
	{
		return true;
	}

	return false;
}
//=============================================================================
//ゲームパッドトリガー判定
//=============================================================================
bool CInputJoystick::GetPadTrigger(GAME_PAD_NAME PadName)
{
	if(m_tPadState[PadName] & 0x80)
	{
		return true;
	}

	return false;
}
//=============================================================================
//ゲームパッドリリース判定
//=============================================================================
bool CInputJoystick::GetPadRelease(GAME_PAD_NAME PadName)
{
	if(m_rPadState[PadName] & 0x80)
	{
		return true;
	}

	return false;
}
//=============================================================================
//ジョイスティックデバイスの生成
//=============================================================================
BOOL CALLBACK CInputJoystick::EnumJoysticksCallback()
{
	hr=m_pDInput->CreateDevice(GUID_Joystick,&m_pDIDevJoystick,NULL);
	
	if(FAILED(hr))
	{
		return DIENUM_CONTINUE;
	}
	
	return DIENUM_STOP;
}
//=============================================================================
//ジョイスティックプロパティの設定
//=============================================================================
BOOL CALLBACK CInputJoystick::EnumAxesCallback()
{
	//デバイスのオブジェクト内の情報(プロパティ)
	DIPROPRANGE diprg;

	//プロパティの設定
	diprg.diph.dwSize=sizeof( DIPROPRANGE );
	diprg.diph.dwHeaderSize=sizeof( DIPROPHEADER );
	diprg.diph.dwHow=DIPH_BYID;
	//diprg.diph.dwObj=m_pDidoi->dwType;
	diprg.lMin=0-1000;
	diprg.lMax=0+1000;

	//プロパティをデバイスにセット
	hr=m_pDIDevJoystick->SetProperty(DIPROP_RANGE,&diprg.diph);
	
	if(FAILED(hr))
	{
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}
//EOF