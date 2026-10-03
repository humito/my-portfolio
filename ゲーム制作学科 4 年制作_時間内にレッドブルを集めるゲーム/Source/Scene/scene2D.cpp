//=============================================================================
//2Dシーン処理[scene2D.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "../System/renderer.h"
#include "../manager.h"
#include "scene.h"
#include "scene2D.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CScene2D::CScene2D(int priority):CScene(priority)
{
	m_pD3DTex=NULL;
	m_pD3DVtxBuff=NULL;
}
//=============================================================================
//デストラクタ
//=============================================================================
CScene2D::~CScene2D()
{
}
//=============================================================================
//シーン初期化
//=============================================================================
HRESULT CScene2D::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//頂点バッファ
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
	pVtx[0].vtx=D3DXVECTOR3(0,500,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(0,0,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(300,500,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(300,0,0.0f);

	//中身
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
	D3DXCreateTextureFromFile(pDevice,
								"data/TEXTURE/akira000.png",
								&m_pD3DTex);
	return S_OK;

}
//=============================================================================
//シーン終了
//=============================================================================
void CScene2D::Uninit()
{
	
	//頂点バッファの解放
	if(m_pD3DVtxBuff!=NULL)
	{
		m_pD3DVtxBuff->Release();//解放
		m_pD3DVtxBuff=NULL;		 //NULLセット
	}
	
	//テクスチャの開放
	if(m_pD3DTex!=NULL)
	{
		m_pD3DTex->Release();//解放
		m_pD3DTex=NULL;		 //NULLセット
	}

	//自身を解放
	this->Release();
}
//=============================================================================
//シーン更新
//=============================================================================
void CScene2D::Update()
{
}
//=============================================================================
//シーン描画
//=============================================================================
void CScene2D::Draw()
{
	//レンダラーゲット
	CRenderer *pRenderer=CManager::GetRenderer();

	//頂点バッファ
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	////////////////////////////////////////
	//			2Dポリゴンの描画		 //
	//////////////////////////////////////

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
//2Dポリゴンインスタンス生成
//=============================================================================
void CScene2D::Create()
{
	//シーン2Dポインタ
	CScene2D *pScene2D;

	//シーン2D動的確保
	pScene2D=new CScene2D();

	//2Dポリゴン初期化
	pScene2D->Init();
}
//EOF