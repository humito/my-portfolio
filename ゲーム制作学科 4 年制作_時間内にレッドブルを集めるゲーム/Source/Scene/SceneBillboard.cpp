//=============================================================================
//ビルボードシーン処理[SceneBillboard.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "SceneBillboard.h"
#include "../System/Camera.h"
#include "../System/renderer.h"
#include "../manager.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CSceneBillboard::CSceneBillboard(int priority) :CScene(priority)
{
	m_pD3DTextureBill = NULL;
	m_pD3DVtxBuffBill = NULL;
}
//=============================================================================
//デストラクタ
//=============================================================================
CSceneBillboard::~CSceneBillboard()
{

}
//=============================================================================
//初期化
//=============================================================================
HRESULT CSceneBillboard::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//ポリゴンの設定
	m_posBill = D3DXVECTOR3(80.0f, 0.0f, 0.0f);
	m_rotBill = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_sclBill = D3DXVECTOR3(1.0f, 1.0f, 1.0f);


	//頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer
		(sizeof(VERTEX_3D)* 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pD3DVtxBuffBill,
		NULL)))
	{
		return E_FAIL;
	}

	VERTEX_3D *pVtx;

	m_pD3DVtxBuffBill->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].vtx = D3DXVECTOR3(-80.0f, 0.0f, 0.0f);
	pVtx[1].vtx = D3DXVECTOR3(-80.0f, 160.0f, 0.0f);
	pVtx[2].vtx = D3DXVECTOR3(80.0f, 0.0f, 0.0f);
	pVtx[3].vtx = D3DXVECTOR3(80.0f, 160.0f, 0.0f);

	pVtx[0].nor = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	pVtx[1].nor = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	pVtx[2].nor = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	pVtx[3].nor = D3DXVECTOR3(1.0f, 1.0f, -1.0f);

	pVtx[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

	m_pD3DVtxBuffBill->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/akira000.png",
		&m_pD3DTextureBill);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CSceneBillboard::Uninit()
{
	//テクスチャへのポインタ終了
	if (m_pD3DTextureBill != NULL)
	{
		m_pD3DTextureBill->Release();
		m_pD3DTextureBill = NULL;
	}

	//頂点バッファへのポインタ終了
	if (m_pD3DVtxBuffBill != NULL)
	{
		m_pD3DVtxBuffBill->Release();
		m_pD3DVtxBuffBill = NULL;
	}

	//自身を解放
	this->Release();
}
//=============================================================================
//更新
//=============================================================================
void CSceneBillboard::Update()
{

}
//=============================================================================
//描画
//=============================================================================
void CSceneBillboard::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//カメラのゲット
	CCamera *pCamera = CManager::GetCamera();

	D3DXMATRIX mtxScl, mtxRot, mtxTranslate, mtxView;

	//カメラ情報取得
	mtxView = pCamera->GetMtxView();

	D3DXMatrixIdentity(&m_mtxWorld);
	D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);


	m_mtxWorld._41 = 0.0f;
	m_mtxWorld._42 = 0.0f;
	m_mtxWorld._43 = 0.0f;


	D3DXMatrixScaling(&mtxScl, 1.0f, 1.0f, 1.0f);

	D3DXMatrixMultiply(&m_mtxWorld,
		&m_mtxWorld,
		&mtxScl);

	//回転を設定
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		0,
		0,
		0);

	//回転を反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
		&mtxRot);


	//位置を反映
	D3DXMatrixTranslation(&mtxTranslate,
		m_posBill.x,
		m_posBill.y,
		m_posBill.z);

	//ワールドマトリックスの設定
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
		&mtxTranslate);

	//位置をセット
	pDevice->SetTransform(D3DTS_WORLD,
		&m_mtxWorld);


	//３Ｄポリゴンの描画
	pDevice->SetStreamSource(0, m_pD3DVtxBuffBill, 0, sizeof(VERTEX_3D));
	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pD3DTextureBill);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//ポリゴンの数
		2);
}
//=============================================================================
//ビルボードインスタンス生成
//=============================================================================
void CSceneBillboard::Create()
{
	//ビルボードポインタ
	CSceneBillboard *pSceneBill;

	//シーンビルボード動的確保
	pSceneBill = new CSceneBillboard();

	//ビルボード初期化
	pSceneBill->Init();
}
//EOF