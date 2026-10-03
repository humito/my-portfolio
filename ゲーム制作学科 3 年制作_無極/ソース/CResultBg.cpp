//=============================================================================
// リザルト背景処理 [CResultBg.h]
// Author : 木村　文登
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CResultBg.h"
#include "renderer.h"
#include "manager.h"
//=============================================================================
//コンストラクタ
//=============================================================================
CResultBg::CResultBg()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CResultBg::~CResultBg()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CResultBg::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	///////////////////////////
	//	頂点バッファの設定	//
	/////////////////////////

	//頂点バッファの生成
	if(FAILED(pDevice->CreateVertexBuffer
				(sizeof(VERTEX_2D)*4,
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
	m_pD3DVtxBuff->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の代入
	pVtx[0].vtx=D3DXVECTOR3(0.0f,1000.0f,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(0.0f,0.0f,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(1000.0f,1000.0f,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(1000.0f,0.0f,0.0f);

	//幅
	pVtx[0].rhw=1.0f;
	pVtx[1].rhw=1.0f;
	pVtx[2].rhw=1.0f;
	pVtx[3].rhw=1.0f;

	//反射光
	pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

	//テクスチャ
	pVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	pVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	pVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	//頂点設定の解除
	m_pD3DVtxBuff->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								"data/TEXTURE/imgb0467.jpg",
								&m_pD3DTex);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CResultBg::Uninit()
{
	//自身の終了
	CScene2D::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CResultBg::Update()
{
}
//=============================================================================
//描画
//=============================================================================
void CResultBg::Draw()
{
	//レンダラーゲット
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//頂点バッファのバインド
	pDevice->SetStreamSource(0,m_pD3DVtxBuff,0,sizeof(VERTEX_2D));

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0,m_pD3DTex);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
							0,//ポリゴンの数
							2);
}
//=============================================================================
//リザルト背景インスタンス生成
//=============================================================================
void CResultBg::Create()
{
	//リザルト背景インスタンス生成
	CResultBg *pResultBg=new CResultBg();

	//初期化
	pResultBg->Init();
}
//EOF