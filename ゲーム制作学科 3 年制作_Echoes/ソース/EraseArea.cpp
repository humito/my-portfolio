//=============================================================================
// 敵消滅エリア [EraseArea.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "manager.h"
#include "renderer.h"
#include "EraseArea.h"

//=============================================================================
//初期化
//=============================================================================
HRESULT CEraseArea::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//座標・角度
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer
		(sizeof(VERTEX_3D)* 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pD3DVtxBuff,
		NULL)))
	{
		return E_FAIL;
	}

	VERTEX_3D *pVtx;

	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点情報の設定
	pVtx[0].vtx = D3DXVECTOR3(-4500.0f, 35.0f, -4500.0f);
	pVtx[1].vtx = D3DXVECTOR3(-4500.0f, 35.0f, -4000.0f);
	pVtx[2].vtx = D3DXVECTOR3(4500.0f, 35.0f, -4500.0f);
	pVtx[3].vtx = D3DXVECTOR3(4500.0f, 35.0f, -4000.0f);

	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 190);
	pVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 190);
	pVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 190);
	pVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 190);

	pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

	m_pD3DVtxBuff->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/000.jpg",
		&m_pD3DTexture);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CEraseArea::Uninit()
{
	//終了
	CScene3D::Uninit();
}
//=============================================================================
//描画
//=============================================================================
void CEraseArea::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//マトリックスに関する変数
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//スケールを設定
	D3DXMatrixScaling(&mtxScl, 1.0f, 1.0f, 1.0f);

	//スケールを反映
	D3DXMatrixMultiply(&m_mtxWorld,
		&m_mtxWorld,
		&mtxScl);

	//回転を設定
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
									0.0f,
									0.0f,
									0.0f);

	//回転を反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
						&mtxRot);

	//位置を設定
	D3DXMatrixTranslation(&mtxTranslate,
						0.0f,
						0.0f,
						0.0f);

	//位置を反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
					&mtxTranslate);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファのバインド
	pDevice->SetStreamSource(0, m_pD3DVtxBuff, 0, sizeof(VERTEX_3D));

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pD3DTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
							0,
							2);
}
//=============================================================================
//色の設定
//=============================================================================
void CEraseArea::SetColor(D3DCOLOR color)
{
	VERTEX_3D *pVtx;

	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].diffuse = color;
	pVtx[1].diffuse = color;
	pVtx[2].diffuse = color;
	pVtx[3].diffuse = color;

	m_pD3DVtxBuff->Unlock();
}
//=============================================================================
//インスタンス生成
//=============================================================================
CEraseArea *CEraseArea::Create()
{
	//インスタンス生成
	CEraseArea *pEraseArea = new CEraseArea();
	//初期化
	pEraseArea->Init();
	return pEraseArea;
}
//EOF