//=============================================================================
//数値表示処理[Number.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Number.h"
#include "manager.h"
#include "renderer.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define CUT (10)//桁ずらし用

//=============================================================================
//コンストラクタ
//=============================================================================
CNumber::CNumber()
{
	m_pD3DTex = NULL;		//テクスチャポインタ
	m_pD3DVtxBuff = NULL;	//頂点バッファ
}
//=============================================================================
//デストラクタ
//=============================================================================
CNumber::~CNumber()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CNumber::Init(char *pFileName, D3DXVECTOR3 pos, NUMBER_DISP type, float fWidth, float fHeight, int nDigitNum)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	m_pos = pos;								//座標
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		//角度
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);		//サイズ
	m_nDigitNum = nDigitNum;					//桁数
	m_fWidth = fWidth / m_nDigitNum;			//１桁の幅
	m_fHeight = fHeight;						//高さ

	//1桁用数値は桁数分確保する
	m_pnValue = new int[m_nDigitNum];

	//抽出用桁数を求める
	m_nCutNum = 1;
	for (int i = 0; i<m_nDigitNum - 1; i++)
	{
		m_nCutNum *= CUT;
	}

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

	//3D頂点バッファのポインタ
	//VERTEX_3D *pVtx;

	//2D頂点情報ポインタ
	//VERTEX_2D *pVtx;

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							pFileName,
							&m_pD3DTex);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CNumber::Uninit()
{
	//テクスチャへのポインタ終了
	if (m_pD3DTex != NULL)
	{
		m_pD3DTex->Release();
		m_pD3DTex = NULL;
	}

	//頂点バッファへのポインタ終了
	if (m_pD3DVtxBuff != NULL)
	{
		m_pD3DVtxBuff->Release();
		m_pD3DVtxBuff = NULL;
	}

	//１ケタ数値を解放
	delete[] m_pnValue;
}
//=============================================================================
//セットされた数値を1桁ずつ抽出
//=============================================================================
void CNumber::SetValue(int num)
{
	//桁数抽出用数値を用意
	int nCutNum = m_nCutNum;

	//引数の数値を１桁ずつ配列にセット
	for (int i = 0; i<m_nDigitNum; i++)
	{
		//1桁の数値を配列に代入
		m_pnValue[i] = (num / nCutNum) % CUT;

		//桁をずらす
		nCutNum /= CUT;
	}
}
//=============================================================================
//描画
//=============================================================================
void CNumber::Draw()
{
	//レンダラーゲット
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	///////////////////////////////////////////
	//			数値ポリゴンの描画			//
	/////////////////////////////////////////
	//桁数分ループ
	for (int i = 0; i<m_nDigitNum; i++)
	{
		//頂点情報の変更
		ChangeBuffer(i);

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
}
//=============================================================================
//頂点情報の変更
//=============================================================================
void CNumber::ChangeBuffer(int num)
{
	//2D頂点情報ポインタ
	VERTEX_2D *pVtx;

	//頂点バッファロック
	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の代入
	pVtx[0].vtx = D3DXVECTOR3(m_pos.x + (m_fWidth * num), m_pos.y + m_fHeight, 0.0f);
	pVtx[1].vtx = D3DXVECTOR3(m_pos.x + (m_fWidth * num), m_pos.y, 0.0f);
	pVtx[2].vtx = D3DXVECTOR3(m_pos.x + m_fWidth + (m_fWidth * num), m_pos.y + m_fHeight, 0.0f);
	pVtx[3].vtx = D3DXVECTOR3(m_pos.x + m_fWidth + (m_fWidth * num), m_pos.y, 0.0f);

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
	pVtx[0].tex = D3DXVECTOR2(0.0f + (m_pnValue[num] * 0.1f), 1.0f);
	pVtx[1].tex = D3DXVECTOR2(0.0f + (m_pnValue[num] * 0.1f), 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.1f + (m_pnValue[num] * 0.1f), 1.0f);
	pVtx[3].tex = D3DXVECTOR2(0.1f + (m_pnValue[num] * 0.1f), 0.0f);

	//頂点設定の解除
	m_pD3DVtxBuff->Unlock();
}
//=============================================================================
//座標セット
//=============================================================================
void CNumber::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}
//EOF