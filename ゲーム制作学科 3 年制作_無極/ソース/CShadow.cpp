//=============================================================================
//影表示処理[CShadow.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CShadow.h"
#include "manager.h"
#include "renderer.h"
#include "CMeshField.h"
#include "CGame.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define SHADOW_ALPHA_MAX (255)//影の最大α値

//=============================================================================
//コンストラクタ
//=============================================================================
CShadow::CShadow()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CShadow::~CShadow()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CShadow::Init(float fSizeX,float fSizeZ)
{
	//レンダラーのゲット
	CRenderer *pRenderer=CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//表示フラグ初期化
	m_bDisp=true;
	//サイズセット
	m_fSizeX=fSizeX;
	m_fSizeZ=fSizeZ;
	//α値
	m_nAlpha=SHADOW_ALPHA_MAX;

	//頂点バッファの生成
	if(FAILED(pDevice->CreateVertexBuffer
			(	sizeof(VERTEX_3D)*4,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&m_pD3DVtxBuff,
				NULL)))
	{
		return E_FAIL;
	}

	//3Dポリゴン用頂点
	VERTEX_3D *pVtx;

	//頂点情報の設定開始
	m_pD3DVtxBuff->Lock(0,0,(void**)&pVtx,0);

	///////////////////////
	//		頂点設定	//
	/////////////////////
	pVtx[0].vtx=D3DXVECTOR3(-m_fSizeX,0.0f,-m_fSizeZ);
	pVtx[1].vtx=D3DXVECTOR3(-m_fSizeX,0.0f,m_fSizeZ);
	pVtx[2].vtx=D3DXVECTOR3(m_fSizeX,0.0f,-m_fSizeZ);
	pVtx[3].vtx=D3DXVECTOR3(m_fSizeX,0.0f,m_fSizeZ);

	///////////////////////////////////////////
	//			法線ベクトル設定			//
	/////////////////////////////////////////
	pVtx[0].nor=D3DXVECTOR3(0.0f,1.0f,0.0f);
	pVtx[1].nor=D3DXVECTOR3(0.0f,1.0f,0.0f);
	pVtx[2].nor=D3DXVECTOR3(0.0f,1.0f,0.0f);
	pVtx[3].nor=D3DXVECTOR3(0.0f,1.0f,0.0f);

	///////////////////////////
	//		反射光設定		//
	/////////////////////////
	pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,m_nAlpha);
	pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,m_nAlpha);
	pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,m_nAlpha);
	pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,m_nAlpha);

	///////////////////////////////
	//		テクスチャ設定		//
	/////////////////////////////
	pVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	pVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	pVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	//頂点情報の設定終了
	m_pD3DVtxBuff->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								"data/TEXTURE/shadow000.jpg",
								&m_pD3DTexture);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CShadow::Uninit()
{
	//自身の終了
	CScene3D::Uninit();
}
//=============================================================================
//描画
//=============================================================================
void CShadow::Draw()
{
	//表示フラグtrueのみ描画処理をする
	if(m_bDisp)
	{
		//レンダラー情報取得
		CRenderer *pRenderer=CManager::GetRenderer();

		//デバイスのゲット
		LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

		//頂点バッファの変更
		ChangeBuff();

		//減算合成
		pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_REVSUBTRACT);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

		//マトリックスに関する変数
		D3DXMATRIX mtxScl,mtxRot,mtxTranslate;

		//アイデンティティに関する変数
		D3DXMatrixIdentity(&m_mtxWorld);

		//拡大縮小を設定
		D3DXMatrixScaling(&mtxScl,1.0f,1.0f,1.0f);

		//サイズを反映
		D3DXMatrixMultiply(	&m_mtxWorld,
							&m_mtxWorld,
							&mtxScl);

		//回転を設定
		D3DXMatrixRotationYawPitchRoll(	&mtxRot,
										0.0f,
										0.0f,
										0.0f);

		//回転を反映
		D3DXMatrixMultiply(	&m_mtxWorld,&m_mtxWorld,
							&mtxRot);

		//位置を設定
		D3DXMatrixTranslation(	&mtxTranslate,
								m_pos.x,
								m_pos.y,
								m_pos.z);

		//位置をセット
		D3DXMatrixMultiply(	&m_mtxWorld,&m_mtxWorld,
							&mtxTranslate);

		//ワールドマトリックスの設定
		pDevice->SetTransform(	D3DTS_WORLD,
								&m_mtxWorld);

		//頂点バッファのバインド
		pDevice->SetStreamSource(0,m_pD3DVtxBuff, 0, sizeof(VERTEX_3D));

		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0,m_pD3DTexture);

		//ポリゴンの描画(頂点)
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
										0,			//開始するポリゴン頂点
										2);			//ポリゴン数

		//元に戻す
		pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	}
}
//=============================================================================
//頂点座標の変更
//=============================================================================
void CShadow::ChangeBuff()
{
	//3Dポリゴン用頂点
	VERTEX_3D *pVtx;

	//頂点情報の設定開始
	m_pD3DVtxBuff->Lock(0,0,(void**)&pVtx,0);

	//反射光設定
	pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,m_nAlpha);
	pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,m_nAlpha);
	pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,m_nAlpha);
	pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,m_nAlpha);

	//頂点情報の設定終了
	m_pD3DVtxBuff->Unlock();
}
//=============================================================================
//座標セット
//=============================================================================
void CShadow::SetPos(D3DXVECTOR3 pos)
{
	//座標セット
	m_pos=pos;

	///////////////////////////////////////
	//		フィールドからY座標取得		//
	/////////////////////////////////////
	//フィールドインスタンス取得
	CMeshField *pMeshField=CGame::GetField();
	//フィールドの高さを取得して影座標に設定
	m_pos.y=pMeshField->GetHeight(m_pos)+1.0f;

	//α値の設定
	m_nAlpha=(int)(SHADOW_ALPHA_MAX-((pos.y+1.0f)-m_pos.y));

	//負数にしない
	if(m_nAlpha<0)
	{
		m_nAlpha=0;
	}
}
//=============================================================================
//表示フラグセット
//=============================================================================
void CShadow::SetDisp(bool bFlag)
{
	m_bDisp=bFlag;
}
//=============================================================================
//インスタンス生成
//=============================================================================
CShadow *CShadow::Create(float fSizeX,float fSizeZ)
{
	//影インスタンス生成
	CShadow *pShadow=new CShadow();

	//初期化
	pShadow->Init(fSizeX,fSizeZ);

	//インスタンスを返す
	return pShadow;
}
//EOF