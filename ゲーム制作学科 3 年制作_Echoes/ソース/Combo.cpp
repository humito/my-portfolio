//=============================================================================
// コンボ表示 [Combo.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "manager.h"
#include "renderer.h"
#include "Sound.h"
#include "Combo.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define COMBO_DISP_COUNT_MAX (250)	//表示カウント最大値
#define COMBO_DISP_COUNT (0)		//カウントが表示されるカウント
#define COMBO_MAX (99)				//最大コンボ

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
int CCombo::m_nCombo = 0;		//コンボ数
bool CCombo::m_bDisp = false;	//表示フラグ

//=============================================================================
//初期化
//=============================================================================
HRESULT CCombo::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//表示フラグ
	m_bDisp = false;

	//カウント
	m_nCount = 0;

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
	pVtx[0].vtx = D3DXVECTOR3(0.0f, 260.0f, 0.0f);
	pVtx[1].vtx = D3DXVECTOR3(0.0f, 170.0f, 0.0f);
	pVtx[2].vtx = D3DXVECTOR3(150.0f, 200.0f, 0.0f);
	pVtx[3].vtx = D3DXVECTOR3(150.0f, 170.0f, 0.0f);

	//幅
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//反射光
	pVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	//テクスチャ
	pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

	//頂点設定の解除
	m_pD3DVtxBuff->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, "data/TEXTURE/combo.png", &m_pD3DTex);

	//数値の初期化
	m_Number.Init(	"data/TEXTURE/number000.png",
					D3DXVECTOR3(20.0f,30.0f,0.0f),
					DISP_2DPOLYGON,
					100.0f,
					150.0f,
					2);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CCombo::Uninit()
{
	//数値の終了
	m_Number.Uninit();

	//終了
	CScene2D::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CCombo::Update()
{
	//表示フラグture時のみ更新
	if (m_bDisp)
	{
		//コンボ最大値を超えない
		if (m_nCombo > COMBO_MAX)
		{
			m_nCombo = COMBO_MAX;
		}

		//数値セット
		m_Number.SetValue(m_nCombo);

		//カウントアップ
		m_nCount++;

		//カウント最大値に達したらカウントリセット、非表示
		if (m_nCount > COMBO_DISP_COUNT_MAX)
		{
			if (m_nCount >= 5)
			{
				CSound::PlaySoundA(SOUND_LABEL_SE_INSIDE);
			}

			m_bDisp = false;
			m_nCount = 0;
			m_nCombo = 0;
		}
	}
}
//=============================================================================
//描画
//=============================================================================
void CCombo::Draw()
{
	if (m_bDisp)
	{
		//レンダラーゲット
		CRenderer *pRenderer = CManager::GetRenderer();
		//デバイスの取得
		LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

		//頂点バッファのバインド
		pDevice->SetStreamSource(0, m_pD3DVtxBuff, 0, sizeof(VERTEX_2D));

		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_2D);

		//テクスチャの設定
		pDevice->SetTexture(0, m_pD3DTex);

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
								0,
								2);
		//数値の描画
		m_Number.Draw();
	}
}
//=============================================================================
//コンボ加算
//=============================================================================
void CCombo::AddCombo()
{
	//コンボ数加算
	m_nCombo++;
	//コンボ数が一定の数値なら表示させる
	if (m_nCombo > COMBO_DISP_COUNT)
	{
		m_bDisp = true;
	}
}
//=============================================================================
//インスタンス生成
//=============================================================================
void CCombo::Create()
{
	//インスタンス生成して初期化
	CCombo *pCombo = new CCombo();
	pCombo->Init();
}
//EOF