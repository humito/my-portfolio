//=============================================================================
// マウス処理 [InputMouse.h]
// Author : 木村 文登
//=============================================================================
#ifndef _INPUTMOUSE_H_
#define _INPUTMOUSE_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Input.h"

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//マウスボタンの種類
enum MOUSE_TYPE
{
	MOUSE_BUTTON_LEFT = 0,	//左クリック
	MOUSE_BUTTON_RIGHT,		//右クリック
	MOUSE_BUTTON_MIDDLE,	//ホイールボタン
	MOUSE_BUTTON_MAX		//ボタン数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//マウス入力クラス
class CInputMouse : public CInput
{
	//外部
	public:
		CInputMouse();									//コンストラクタ
		~CInputMouse(){}								//デストラクタ
		HRESULT Init(HINSTANCE hInstance, HWND hWnd);	//初期化
		void Uninit(void);								//終了
		void Update(void);								//更新

		//マウスの(クライアント)座標の取得
		static D3DXVECTOR2 GetClientPos(){ return m_ClientPos; }

		//マウスの(ワールド)座標の取得
		static D3DXVECTOR3 GetWorldPos(){ return m_WorldPos; }

		//マウスの移動量取得
		static D3DXVECTOR3 GetVelocity(){ return m_Velocity; }

		//マウスワールド座標との当たり判定
		static bool HitCheckWorldPos(D3DXVECTOR3 pos, float fRadius);

		//クライアント内か判定
		static bool InClientCheck();

		static bool GetButtonPress(int nButton);	//プレス判定
		static bool GetButtonTrigger(int nButton);	//トリガー判定
		static bool GetButtonRelease(int nButton);	//リリース判定

	//内部
	private:
		D3DXMATRIX				m_matWorld;							//ワールドマトリクス
		D3DVIEWPORT9			m_ViewPort;							//ビューポート
		HWND					m_hWnd;								//ウィンドウハンドル
		LPDIRECTINPUTDEVICE8	m_pDIDevMouse;						//マウスデバイス
		static BYTE				m_aButtonState[MOUSE_BUTTON_MAX];	//キー情報
		static BYTE				m_pButtonState[MOUSE_BUTTON_MAX];	//前キー情報
		static BYTE				m_tButtonState[MOUSE_BUTTON_MAX];	//トリガー情報
		static BYTE				m_rButtonState[MOUSE_BUTTON_MAX];	//リリース情報
		static D3DXVECTOR3		m_Velocity;							//マウス速度
		static D3DXVECTOR3		m_ProjectionVec;					//ワールド空間に射影したベクトル
		static D3DXVECTOR2		m_ClientPos;						//マウス(クライアント)座標
		static D3DXVECTOR3		m_WorldPos;							//マウス(ワールド)座標

		//スクリーン座標からワールド座標へ
		void ScreenToWorldPos();
};
#endif
//EOF