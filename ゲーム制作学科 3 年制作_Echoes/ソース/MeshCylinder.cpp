//=============================================================================
// メッシュ筒の処理 [MeshCylinder.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "MeshCylinder.h"
#include "manager.h"
#include "renderer.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define HALF (2)			//半分
#define ADD_ROT (0.001f);	//回転量
#define RECT_PRIMITIVE (2)	//四角形ポリゴンのプリミティブ数
#define NEXT_VERTEX (1)		//次の頂点
#define PREV_VERTEX (1)		//前の頂点
#define LINE_LAST_VERTEX (2)//行の最後の頂点

//=============================================================================
//コンストラクタ
//=============================================================================
CMeshCylinder::CMeshCylinder()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CMeshCylinder::~CMeshCylinder()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CMeshCylinder::Init(D3DXVECTOR3 pos, int nNumBlockX, int nNumBlockY, float fSizeCylinderY, float fHalfSize)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//ブロック数代入
	m_nNumBlockX = nNumBlockX;//X
	m_nNumBlockY = nNumBlockY;//Y

	//ブロック１つにつく角度(360度/Xブロック数)
	m_fCylinderRot = (D3DX_PI*HALF) / m_nNumBlockX;

	//ポリゴンの高さを計算
	m_fSizeBlockY = fSizeCylinderY / nNumBlockY;

	//総ポリゴン数
	m_nNumPolygon = ((m_nNumBlockX*m_nNumBlockY) * 2) + ((m_nNumBlockY - 1) * 4);

	//総頂点数
	m_nNumVertex = (m_nNumBlockX + NEXT_VERTEX)*RECT_PRIMITIVE*m_nNumBlockY + RECT_PRIMITIVE*(m_nNumBlockY - PREV_VERTEX);

	//インデックス数
	m_nNumVertexIndex = (m_nNumBlockX + NEXT_VERTEX)*(m_nNumBlockY + NEXT_VERTEX);

	//ポリゴンの設定
	m_pos = pos;							//座標
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//回転

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


	VERTEX_3D *pVtx;//3Dポリゴン用頂点

	//頂点バッファの設定開始
	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	///////////////////////
	//	頂点設定ループ	//
	/////////////////////

	//Yループ
	for (int y = 0; y<m_nNumBlockY + 1; y++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX + 1; x++)
		{
			//小数点を切り捨てする
			float fRot = m_fCylinderRot*x;			//角度量
			int fRotX = (int)(cosf(fRot)*fHalfSize);	//X
			int fRotZ = (int)(sinf(fRot)*fHalfSize);	//Z


			//上から設定する
			pVtx[y*(m_nNumBlockX + NEXT_VERTEX) + x].vtx = D3DXVECTOR3((float)fRotX,						//X
				fSizeCylinderY - (m_fSizeBlockY*y),	//Y
				(float)fRotZ						//Z
				);
		}
	}

	///////////////////////////////////////////
	//	法線ベクトル＆反射光　設定ループ	//
	/////////////////////////////////////////
	for (int i = 0; i<m_nNumVertexIndex; i++)
	{
		pVtx[i].nor = D3DXVECTOR3(0.0f, 1.0f, -1.0f);
		pVtx[i].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	}

	///////////////////////////////
	//	テクスチャ設定ループ	//
	/////////////////////////////

	//テクスチャのサイズをX・Yブロック分分割する
	float fTexSizeX = 1.0f / m_nNumBlockX;
	float fTexSizeY = 1.0f / m_nNumBlockY;

	//Yループ
	for (int y = 0; y<m_nNumBlockY + NEXT_VERTEX; y++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX + NEXT_VERTEX; x++)
		{
			//上頂点から設定
			pVtx[y*(m_nNumBlockX + NEXT_VERTEX) + x].tex = D3DXVECTOR2((fTexSizeX*x), (fTexSizeY*y));
		}
	}

	//頂点バッファの設定終了
	m_pD3DVtxBuff->Unlock();


	///////////////////////////////////////////
	//		インデックスバッファの生成		//
	/////////////////////////////////////////

	//インデックスバッファの生成
	if (FAILED(pDevice->CreateIndexBuffer
		(sizeof(WORD)*m_nNumVertex,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pD3DIndexBuff,
		NULL)))
	{
		return E_FAIL;
	}

	//インデックスのポインタ
	WORD *pIndex;

	//インデックスバッファの設定開始
	m_pD3DIndexBuff->Lock(0, 0, (void**)&pIndex, 0);

	//インデックス用カウント
	int cnt = 0;

	//Yループ
	for (int y = 0; y<m_nNumBlockY; y++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX + NEXT_VERTEX; x++)
		{
			//下側
			pIndex[cnt] = (x + (m_nNumBlockX + 1)) + (y*(m_nNumBlockY + 1));
			cnt++;

			//上側
			pIndex[cnt] = x + (y*(m_nNumBlockY + 1));
			cnt++;

			//縮退ポリゴンに合わせて番号を付ける
			if (y != m_nNumBlockY - 1 && x == m_nNumBlockX)
			{
				//上
				pIndex[cnt] = x + (y*(m_nNumBlockY + 1));
				cnt++;

				//下
				pIndex[cnt] = (m_nNumBlockX + 1) + ((y + 1)*(m_nNumBlockY + 1));
				cnt++;
			}

		}
	}

	//インデックスバッファの設定終了
	m_pD3DIndexBuff->Unlock();


	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								"data/TEXTURE/mountain000.png",
								&m_pD3DTexture);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CMeshCylinder::Uninit()
{
	//自身の終了
	CScene3D::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CMeshCylinder::Update()
{
	//ドームの回転
	m_rot.y += ADD_ROT;
}
//=============================================================================
//描画
//=============================================================================
void CMeshCylinder::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//カリング表面
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	//描画用変数　(マトリックス)
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	//マトリクス初期化
	D3DXMatrixIdentity(&m_mtxWorld);
	//スケール設定
	D3DXMatrixScaling(&mtxScl, 1.0f, 1.0f, 1.0f);
	//スケール反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScl);

	//回転を設定
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
									m_rot.y,
									m_rot.x,
									m_rot.z);

	//回転を反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
						&mtxRot);


	//位置を反映
	D3DXMatrixTranslation(	&mtxTranslate,
							m_pos.x,
							m_pos.y,
							m_pos.z);

	//位置をセット
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
						&mtxTranslate);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD,&m_mtxWorld);

	//３Ｄポリゴンの描画
	pDevice->SetStreamSource(0, m_pD3DVtxBuff, 0, sizeof(VERTEX_3D));

	//インデックスをバインド
	pDevice->SetIndices(m_pD3DIndexBuff);

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pD3DTexture);

	//ポリゴンの描画(インデックス)
	pDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLESTRIP,
									0,
									0,
									m_nNumVertexIndex,
									0,
									m_nNumPolygon);

	//元に戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
//=============================================================================
//メッシュシリンダーインスタンス生成
//=============================================================================
CMeshCylinder *CMeshCylinder::Create(D3DXVECTOR3 pos, int nNumBlockX, int nNumBlockY, float fSizeCylinderY, float fHalfSize)
{
	//インスタンスの生成
	CMeshCylinder *pMeshCylinder = new CMeshCylinder();
	//初期化
	pMeshCylinder->Init(pos, nNumBlockX, nNumBlockY, fSizeCylinderY, fHalfSize);

	return pMeshCylinder;
}
//EOF