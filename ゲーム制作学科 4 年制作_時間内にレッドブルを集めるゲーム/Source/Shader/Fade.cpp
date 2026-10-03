//=============================================================================
// フェードシェーダー[Fade.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Fade.h"
#include "../manager.h"
#include "../System/renderer.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define FADE_TIME_MAX (1.0f)	//フェード時間の最大値
#define ADD_FADE_TIME (0.01f)	//フェード時間加算量

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
FADE_MODE CFade::m_FadeMode = FADE_NONE;		//フェードの状態
STATE_INDEX CFade::m_nextState = TITLE_STATE;	//次のゲームシーン

//=============================================================================
//シェーダー読込
//=============================================================================
void CFade::Load()
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//フェードシェーダー生成
	CreateShader(pDevice, "data/HLSL/Fade.hlsl",
				"VertexShader3D", "PixelShader3D",
				"vs_2_0", "ps_2_0",
				&m_pVSConstantTable, &m_pPSConstantTable,
				&m_pVertexShader, &m_pPixelShader);

	//フィルターの初期化
	CFilter::Init();

	//フェードの状態
	m_FadeMode = FADE_NONE;

	//フェードの時間初期化
	m_fFadeTime = 0.0f;
}
//=============================================================================
//終了
//=============================================================================
void CFade::Uninit()
{
	CFilter::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CFade::Update()
{
	///////////////////////////////////////////////
	//		フェード状態から処理が変わる		//
	/////////////////////////////////////////////
	switch (m_FadeMode)
	{
		//フェードイン
		case FADE_IN:
		{
			//フェード時間アップ
			m_fFadeTime += ADD_FADE_TIME;

			//フェード時間が最大値に達した場合
			if (m_fFadeTime >= FADE_TIME_MAX)
			{
				//フェードアウトに変更
				m_FadeMode = FADE_OUT;

				//ゲームシーンの変更
				CManager::SetState(m_nextState);
			}
			break;
		}

		//フェードアウト
		case FADE_OUT:
		{
			//フェード時間ダウン
			m_fFadeTime -= ADD_FADE_TIME;

			//フェード時間が最小値に達した場合
			if (m_fFadeTime <= 0.0f)
			{
				//フェードなしに変更
				m_FadeMode = FADE_NONE;

				m_fFadeTime = 0.0f;
			}

			break;
		}
	}
}
//=============================================================================
//描画
//=============================================================================
void CFade::Draw(LPDIRECT3DDEVICE9 pDevice)
{
	//フェードが開始されてる場合のみ描画する
	if (m_FadeMode == FADE_IN
	||	m_FadeMode == FADE_OUT)
	{
		//フェード時間をセット
		m_pPSConstantTable->SetFloat(pDevice, "g_fFadeTime", m_fFadeTime);

		//頂点シェーダーのセット
		if (m_pVertexShader)
			pDevice->SetVertexShader(m_pVertexShader);

		//ピクセルシェーダーのセット
		if (m_pPixelShader)
			pDevice->SetPixelShader(m_pPixelShader);

		//フィルターの描画
		CFilter::Draw(pDevice);

		//シェーダーを戻す
		pDevice->SetVertexShader(NULL);
		pDevice->SetPixelShader(NULL);
	}
}
//EOF