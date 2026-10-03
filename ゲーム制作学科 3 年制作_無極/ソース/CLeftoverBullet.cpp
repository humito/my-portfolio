//=============================================================================
//残り弾数表示処理[CLeftoverBullet.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CLeftoverBullet.h"
#include "CBullet.h"
#include "manager.h"
#include "renderer.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define LEF_POS (500.0f)	//残弾数の座標
#define LEF_SIZE (550.0f)	//残弾数のサイズ
#define LEF_HEIGHT (50.0f)	//残弾数の高さ
#define POS_SHIFT (60.0f)	//残弾数アイコンの座標をずらす値
#define LEFTOVER_MAX (5)	//残弾数最大値

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
int CLeftoverBullet::m_nLeftoverBullet=0;//残弾数

//=============================================================================
//コンストラクタ
//=============================================================================
CLeftoverBullet::CLeftoverBullet()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CLeftoverBullet::~CLeftoverBullet()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CLeftoverBullet::Init()
{
	//残弾数初期化
	m_nLeftoverBullet=LEFTOVER_MAX;

	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	///////////////////////////////////
	//		頂点バッファ生成		//
	/////////////////////////////////

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

	//頂点バッファの設定
	m_pD3DVtxBuff->Lock(0,0,(void**)&pVtx,0);

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

	//頂点情報設定終了
	m_pD3DVtxBuff->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								"data/TEXTURE/leftover_Bullet000.jpg",
								&m_pD3DTex);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CLeftoverBullet::Uninit()
{
	//自身の終了
	CScene2D::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CLeftoverBullet::Update()
{
	//残弾数は上限を超えないようにする
	if(m_nLeftoverBullet>LEFTOVER_MAX)
	{
		m_nLeftoverBullet=LEFTOVER_MAX;
	}
}
//=============================================================================
//描画
//=============================================================================
void CLeftoverBullet::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//残り弾数分ループ
	for(int i=0;i<m_nLeftoverBullet;i++)
	{
		//頂点情報の変更
		ChangeBuffer(i);

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
}
//=============================================================================
//頂点情報の変更
//=============================================================================
void CLeftoverBullet::ChangeBuffer(int num)
{
	//2D頂点情報ポインタ
	VERTEX_2D *pVtx;

	//頂点バッファの設定
	m_pD3DVtxBuff->Lock(0,0,(void**)&pVtx,0);

	//頂点座標
	pVtx[0].vtx=D3DXVECTOR3(LEF_POS+(POS_SHIFT*num),LEF_HEIGHT,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(LEF_POS+(POS_SHIFT*num),0.0f,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(LEF_SIZE+(POS_SHIFT*num),LEF_HEIGHT,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(LEF_SIZE+(POS_SHIFT*num),0.0f,0.0f);

	//頂点情報設定終了
	m_pD3DVtxBuff->Unlock();
}
//=============================================================================
//残弾数の取得
//=============================================================================
int CLeftoverBullet::GetLeftoverNum(void)
{
	return m_nLeftoverBullet;
}
//=============================================================================
//残弾数加算
//=============================================================================
void CLeftoverBullet::AddNum(void)
{
	m_nLeftoverBullet++;
}
//=============================================================================
//残弾数減算
//=============================================================================
void CLeftoverBullet::SubNum(void)
{
	m_nLeftoverBullet--;
}
//=============================================================================
//インスタンス生成
//=============================================================================
void CLeftoverBullet::Create()
{
	//弾表示インスタンス生成
	CLeftoverBullet *pLeftoverBullet=new CLeftoverBullet();

	//初期化
	pLeftoverBullet->Init();
}
//EOF