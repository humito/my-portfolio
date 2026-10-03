//=============================================================================
// 画像表示 [Image.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "../manager.h"
#include "../System/renderer.h"
#include "Image.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CImage::CImage()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CImage::~CImage()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CImage::Init(char *pFileName, D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//座標
	m_pos = pos;

	//スケール
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//半分の幅、高さを設定
	m_fHalfWidth = fWidth / 2;
	m_fHalfHeight = fHeight / 2;

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
	pVtx[0].vtx = D3DXVECTOR3(m_pos.x - m_fHalfWidth, m_pos.y + m_fHalfHeight, 0.0f);
	pVtx[1].vtx = D3DXVECTOR3(m_pos.x - m_fHalfWidth, m_pos.y - m_fHalfHeight, 0.0f);
	pVtx[2].vtx = D3DXVECTOR3(m_pos.x + m_fHalfWidth, m_pos.y + m_fHalfHeight, 0.0f);
	pVtx[3].vtx = D3DXVECTOR3(m_pos.x + m_fHalfWidth, m_pos.y - m_fHalfHeight, 0.0f);

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
	D3DXCreateTextureFromFile(pDevice, pFileName, &m_pD3DTex);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CImage::Uninit()
{
	//終了
	CScene2D::Uninit();
}
//=============================================================================
//描画
//=============================================================================
void CImage::Draw()
{
	//レンダラーゲット
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//頂点バッファの変更
	ChangeBuffer();

	//頂点バッファのバインド
	pDevice->SetStreamSource(0, m_pD3DVtxBuff, 0, sizeof(VERTEX_2D));

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pD3DTex);

	//ポリゴンの描画
	pDevice->DrawPrimitive(	D3DPT_TRIANGLESTRIP,
							0,
							2);
}
//=============================================================================
//頂点バッファの変更
//=============================================================================
void CImage::ChangeBuffer()
{
	//2D頂点情報ポインタ
	VERTEX_2D *pVtx;

	//頂点バッファロック
	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の代入
	pVtx[0].vtx = D3DXVECTOR3(m_pos.x - (m_fHalfWidth * m_scl.x), m_pos.y + (m_fHalfHeight * m_scl.y), 0.0f);
	pVtx[1].vtx = D3DXVECTOR3(m_pos.x - (m_fHalfWidth * m_scl.x), m_pos.y - (m_fHalfHeight * m_scl.y), 0.0f);
	pVtx[2].vtx = D3DXVECTOR3(m_pos.x + (m_fHalfWidth * m_scl.x), m_pos.y + (m_fHalfHeight * m_scl.y), 0.0f);
	pVtx[3].vtx = D3DXVECTOR3(m_pos.x + (m_fHalfWidth * m_scl.x), m_pos.y - (m_fHalfHeight * m_scl.y), 0.0f);

	//頂点設定の解除
	m_pD3DVtxBuff->Unlock();
}
//=============================================================================
//テクスチャの変更
//=============================================================================
void CImage::ChangeTexture(char *pFileName)
{
	//レンダラーゲット
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//テクスチャの開放
	if (m_pD3DTex != NULL)
	{
		m_pD3DTex->Release();	//解放
		m_pD3DTex = NULL;		//NULLセット
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, pFileName, &m_pD3DTex);
}
//=============================================================================
//インスタンス生成
//=============================================================================
CImage *CImage::Create(char *pFileName, D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	//インスタンス生成
	CImage *pImage = new CImage();

	//初期化
	pImage->Init(pFileName,pos, fWidth, fHeight);

	return pImage;
}
//EOF