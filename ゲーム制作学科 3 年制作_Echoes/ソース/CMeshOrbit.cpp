//=============================================================================
//メッシュ奇跡エフェクト処理[CMeshOrbit.cpp]
//Author:HUMITO KIMURA
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CMeshOrbit.h"
#include "renderer.h"
#include "manager.h"
#include "CDebugproc.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define RECT_PRIMITIVE (2)		//四角形ポリゴンのプリミティブ数
#define NEXT_POLIGON_NUM (4)	//行へ移動する時のポリゴン数
#define NEXT_VERTEX (1)			//次の頂点
#define PREV_VERTEX (1)			//前の頂点
#define LINE_LAST_VERTEX (2)	//行の最後の頂点

//=============================================================================
//コンストラクタ
//=============================================================================
CMeshOrbit::CMeshOrbit()
{
}

//=============================================================================
//デストラクタ
//=============================================================================
CMeshOrbit::~CMeshOrbit()
{
}


//=============================================================================
//初期化
//=============================================================================
HRESULT CMeshOrbit::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//ブロック数の代入
	m_nNumBlockX = nNumBlockX;
	m_nNumBlockZ = nNumBlockZ;

	//サイズの代入
	m_fSizeBlockX = fSizeBlockX;
	m_fSizeBlockZ = fSizeBlockZ;

	//総ポリゴン数
	m_nNumPolygon = ((m_nNumBlockX*m_nNumBlockZ)*RECT_PRIMITIVE) + NEXT_POLIGON_NUM*(m_nNumBlockZ - PREV_VERTEX);

	//総頂点数
	m_nNumVertex = (m_nNumBlockX + NEXT_VERTEX)*RECT_PRIMITIVE*m_nNumBlockZ + RECT_PRIMITIVE*(m_nNumBlockZ - PREV_VERTEX);

	//インデックス数
	m_nNumVertexIndex = (m_nNumBlockX + NEXT_VERTEX)*(m_nNumBlockZ + NEXT_VERTEX);

	m_pos = pos;//座標
	m_rot = rot;//回転

	//オフセット座標の更新
	m_OffsetPoint[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_OffsetPoint[1] = D3DXVECTOR3(0.0f, 100.0f, 0.0f);

	//頂点座標の動的確保
	m_pPoint = new D3DXVECTOR3[m_nNumVertexIndex];


	////////////////////////////////////////////////////////////////////////////
	//							頂点バッファの生成							 //
	////////////////////////////////////////////////////////////////////////////

	if (FAILED(pDevice->CreateVertexBuffer
		(sizeof(VERTEX_3D)*m_nNumVertexIndex,
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

	//頂点バッファの設定
	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	///////////////////////////////
	//		頂点設定ループ		//
	/////////////////////////////

	for (int z = 0; z<m_nNumBlockZ; z++)
	{
		//列
		for (int x = 0; x<m_nNumBlockX + 1; x++)
		{
			//頂点番号
			int num = (x * 2);

			//下頂点
			pVtx[num].vtx = D3DXVECTOR3(x * m_fSizeBlockX,
										0.0f,
										0.0f
										);
			//上頂点
			pVtx[1 + num].vtx = D3DXVECTOR3(x * m_fSizeBlockX,
											m_fSizeBlockZ,
											0.0f
											);
		}
	}


	///////////////////////////////////////////
	//	法線ベクトル＆反射光　設定ループ	//
	/////////////////////////////////////////
	for (int i = 0; i<m_nNumVertexIndex; i++)
	{
		pVtx[i].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[i].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	}

	///////////////////////////////
	//	テクスチャ設定ループ	//
	/////////////////////////////
	//行
	for (int y = 0; y<m_nNumBlockZ + NEXT_VERTEX; y++)
	{
		//列
		for (int x = 0; x<m_nNumBlockX + NEXT_VERTEX; x++)
		{
			//頂点
			pVtx[y*(m_nNumBlockX + NEXT_VERTEX) + x].tex = D3DXVECTOR2((x*1.0f), (y*1.0f));
		}
	}

	//頂点バッファの設定終了
	m_pD3DVtxBuff->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								"data/TEXTURE/field008.jpg",
								&m_pD3DTexture);
								
	return S_OK;
}

//=============================================================================
//終了
//=============================================================================
void CMeshOrbit::Uninit()
{
	//確保した頂点座標の解放
	if (m_pPoint)
	{
		delete[] m_pPoint;
	}

	//自身の終了
	CScene3D::Uninit();
}

//=============================================================================
//更新
//=============================================================================
void CMeshOrbit::Update()
{
	//頂点座標更新
	for (int i = 0; i < m_nNumBlockX; i++)
	{
		m_pPoint[i * 2] = m_pPoint[(i + 1) * 2];
		m_pPoint[i * 2 + 1] = m_pPoint[(i + 1) * 2 + 1];
	}

	//下頂点
	D3DXVec3TransformCoord(&m_pPoint[m_nNumBlockX * 2],		//座標変換結果
							&m_OffsetPoint[0],				//座標変換対象
							&m_ParentMtx);					//座標変換マトリクス対象
	//上頂点
	D3DXVec3TransformCoord(&m_pPoint[m_nNumBlockX * 2 + 1],
							&m_OffsetPoint[1],
							&m_ParentMtx);

	//3Dポリゴン用頂点
	VERTEX_3D *pVtx;

	//頂点バッファの設定
	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標設定
	for (int i = 0; i < m_nNumVertexIndex;i++)
	{
		pVtx[i].vtx = m_pPoint[i];
		pVtx[i].diffuse = D3DCOLOR_RGBA(0, 255, 0, (int)((255 / m_nNumVertexIndex)*i));
	}

	//頂点バッファの設定終了
	m_pD3DVtxBuff->Unlock();
	CDebug::Print("\n%f", m_pPoint[m_nNumBlockX * 2].x);

}

//=============================================================================
//描画
//=============================================================================
void CMeshOrbit::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//両面を描画
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//加算合成
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	//ポリゴンの重なりによる隠れを防ぐ
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//ワイヤーフレーム
//	pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);

	//マトリックスに関する変数
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//アイデンティティに関する変数
	D3DXMatrixIdentity(&m_mtxWorld);

	//拡大縮小を設定
	D3DXMatrixScaling(&mtxScl, 1.0f, 1.0f, 1.0f);

	//サイズを反映
	D3DXMatrixMultiply(	&m_mtxWorld,
						&m_mtxWorld,
						&mtxScl);

	//回転を設定
	D3DXMatrixRotationYawPitchRoll(	&mtxRot,
									m_rot.y,
									m_rot.x,
									m_rot.z);

	//回転を反映
	D3DXMatrixMultiply(	&m_mtxWorld, &m_mtxWorld,
						&mtxRot);


	//位置を反映
	D3DXMatrixTranslation(	&mtxTranslate,
							m_pos.x,
							m_pos.y,
							m_pos.z);

	//位置をセット
	D3DXMatrixMultiply(	&m_mtxWorld, &m_mtxWorld,
						&mtxTranslate);

	//ワールドマトリックスの設定
	pDevice->SetTransform(	D3DTS_WORLD,
							&m_mtxWorld);

	//頂点バッファのバインド
	pDevice->SetStreamSource(0, m_pD3DVtxBuff, 0, sizeof(VERTEX_3D));

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pD3DTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitive(	D3DPT_TRIANGLESTRIP,
							0,
							m_nNumPolygon);

	//元に戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

//	pDevice->SetRenderState(D3DRS_FILLMODE, NULL);
}

//=============================================================================
//親マトリクスセット
//=============================================================================
void CMeshOrbit::SetParentMtx(D3DXMATRIX parentMtx)
{
	m_ParentMtx = parentMtx;
}

//=============================================================================
//軌跡エフェクト生成
//=============================================================================
CMeshOrbit *CMeshOrbit::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ)
{
	//インスタンス生成
	CMeshOrbit *pMeshOrbit = new CMeshOrbit();

	//初期化
	pMeshOrbit->Init(pos,rot,nNumBlockX,nNumBlockZ,fSizeBlockX,fSizeBlockZ);

	return pMeshOrbit;
}
//EOF