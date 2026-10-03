//=============================================================================
// フェード処理 [Fade.cpp]
// Author : 木村　文登
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Fade.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define FADE_COUNT_DELAY (20)	//フェードカウント遅延
#define ALPHA_MAX (255)			//α値最大

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
FADE_MODE CFade::m_FadeMode = FADE_NONE;	//フェードの状態
MODE CFade::m_nextMode = TITLE_MODE;		//次のゲームシーン

//=============================================================================
//コンストラクタ
//=============================================================================
CFade::CFade()
{
	m_pD3DTex = NULL;
	m_pD3DVtxBuff = NULL;
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CFade::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	m_fWidth = SCREEN_WIDTH;		//幅
	m_fHeight = SCREEN_HEIGHT;	//高さ
	m_nFadeCnt = 0;				//フェードカウント
	m_nAlpha = 0;					//α値

	///////////////////////////
	//	頂点バッファの設定	//
	/////////////////////////

	//頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer
		(sizeof(VERTEX_2D)* 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pD3DVtxBuff,
		NULL)))
	{
		return E_FAIL;
	}

	//2D頂点情報ポインタ
	VERTEX_2D *pVtx;

	//頂点バッファロック
	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の代入
	pVtx[0].vtx = D3DXVECTOR3(0.0f, m_fHeight, 0.0f);
	pVtx[1].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[2].vtx = D3DXVECTOR3(m_fWidth, m_fHeight, 0.0f);
	pVtx[3].vtx = D3DXVECTOR3(m_fWidth, 0.0f, 0.0f);

	//幅
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//反射光
	pVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);

	//テクスチャ
	pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

	//頂点設定の解除
	m_pD3DVtxBuff->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/n.png",
		&m_pD3DTex);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CFade::Uninit()
{
	//テクスチャ解放
	if (m_pD3DTex)
	{
		m_pD3DTex->Release();
		m_pD3DTex = NULL;
	}

	//頂点バッファ解放
	if (m_pD3DVtxBuff)
	{
		m_pD3DVtxBuff->Release();
		m_pD3DVtxBuff = NULL;
	}
}
//=============================================================================
//更新
//=============================================================================
void CFade::Update()
{
	///////////////////////////////////////////////
	//		フェードの状態で処理を分ける		//
	/////////////////////////////////////////////
	switch (m_FadeMode)
	{
		//フェードイン
		case FADE_IN:
		{
			//カウントアップ
			m_nFadeCnt++;

			//カウント毎にα値を加算
			m_nAlpha += m_nFadeCnt / FADE_COUNT_DELAY;

			//α値が最大に達した場合
			if (m_nAlpha >= ALPHA_MAX)
			{
				//フェードアウトに変更
				m_FadeMode = FADE_OUT;

				//カウントリセット
				m_nFadeCnt = 0;

				//ゲームシーンのセット
				CManager::SetMode(m_nextMode);
			}

			break;
		}

			//フェードアウト
		case FADE_OUT:
		{
			//カウントアップ
			m_nFadeCnt++;

			//カウント毎にα値を減算
			m_nAlpha -= m_nFadeCnt / FADE_COUNT_DELAY;

			//α値が最小に達した場合
			if (m_nAlpha <= 0)
			{
				//フェードなしに変更
				m_FadeMode = FADE_NONE;
				//カウントリセット
				m_nFadeCnt = 0;
			}

			break;
		}
	}
}
//=============================================================================
//描画
//=============================================================================
void CFade::Draw()
{
	//レンダラーゲット
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//頂点情報の変更
	ChangeBuffer();

	//頂点バッファのバインド
	pDevice->SetStreamSource(0, m_pD3DVtxBuff, 0, sizeof(VERTEX_2D));

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pD3DTex);

	//ポリゴンの描画
	pDevice->DrawPrimitive(	D3DPT_TRIANGLESTRIP,
							0,//ポリゴンの数
							2);
}
//=============================================================================
//頂点情報の変更
//=============================================================================
void CFade::ChangeBuffer()
{
	//2D頂点情報ポインタ
	VERTEX_2D *pVtx;

	//頂点バッファロック
	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の代入
	pVtx[0].vtx = D3DXVECTOR3(0.0f, m_fHeight, 0.0f);
	pVtx[1].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[2].vtx = D3DXVECTOR3(m_fWidth, m_fHeight, 0.0f);
	pVtx[3].vtx = D3DXVECTOR3(m_fWidth, 0.0f, 0.0f);

	//幅
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//反射光
	pVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);

	//テクスチャ
	pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

	//頂点設定の解除
	m_pD3DVtxBuff->Unlock();
}
//=============================================================================
//フェードの取得
//=============================================================================
FADE_MODE CFade::GetFade(void)
{
	return m_FadeMode;
}
//=============================================================================
//フェードのセット
//=============================================================================
void CFade::SetFade(MODE next)
{
	//フェード無し状態のみフェード開始できる
	if (m_FadeMode == FADE_NONE)
	{
		m_FadeMode = FADE_IN;
		m_nextMode = next;
	}
}
//=============================================================================
//フェードインスタンス生成
//=============================================================================
CFade *CFade::Create()
{
	//フェードインスタンス生成
	CFade *pFade = new CFade();

	//初期化
	pFade->Init();

	//インスタンスを返す
	return pFade;
}
//EOF