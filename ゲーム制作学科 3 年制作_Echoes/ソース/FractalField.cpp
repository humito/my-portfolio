//=============================================================================
// フラクタル地形 [FractalField.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "FractalField.h"
#include "manager.h"
#include "renderer.h"
#include <stdlib.h>
#include <time.h>

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
CFractalField::CFractalField()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CFractalField::~CFractalField()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CFractalField::Init(char *pFileName,D3DXVECTOR3 pos, D3DXVECTOR3 rot,
							int nDivision, float fSize)
{

	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//分割数からブロック数を求める
	m_nNumBlock = 1;
	for (int i = 0; i < nDivision; ++i)
	{
		m_nNumBlock *= 2;
	}

	//法線ベクトルをブロック数分確保
	m_ppNormal = new D3DXVECTOR3*[m_nNumBlock];
	//2次元配列として各配列に動的確保
	for (int i = 0; i<m_nNumBlock; i++)
	{
		m_ppNormal[i] = new D3DXVECTOR3[m_nNumBlock * 2];
	}

	//サイズの代入
	m_fSizeBlock = fSize;

	//総ポリゴン数
	m_nNumPolygon = ((m_nNumBlock*m_nNumBlock)*RECT_PRIMITIVE) + NEXT_POLIGON_NUM*(m_nNumBlock - PREV_VERTEX);

	//総頂点数
	m_nNumVertex = (m_nNumBlock + NEXT_VERTEX)*RECT_PRIMITIVE*m_nNumBlock + RECT_PRIMITIVE*(m_nNumBlock - PREV_VERTEX);

	//インデックス数
	m_nNumVertexIndex = (m_nNumBlock + NEXT_VERTEX)*(m_nNumBlock + NEXT_VERTEX);

	//設定用の高さを頂点数分確保
	m_pfHeightVtx = new float[m_nNumVertexIndex];

	//頂点の高さを設定
	InitHeightPolygon(nDivision);

	//ポリゴンの設定
	m_pos = pos;//座標
	m_rot = rot;//回転

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

	//頂点バッファの設定開始
	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	///////////////////////
	//	頂点設定ループ	//
	/////////////////////

	float fVtxX, fVtxY, fVtxZ;

	//Zループ
	for (int z = 0; z<m_nNumBlock + NEXT_VERTEX; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlock + NEXT_VERTEX; x++)
		{
			///////////////////////////////////////////////////////
			//		ブロック数とサイズから頂点座標を計算		//
			/////////////////////////////////////////////////////

			//X座標
			fVtxX = -(m_nNumBlock / 2)*m_fSizeBlock + x*m_fSizeBlock;

			//Z座標
			fVtxZ = (m_nNumBlock / 2)*m_fSizeBlock - z*m_fSizeBlock;

			//Y座標
			fVtxY = m_pfHeightVtx[z*(m_nNumBlock + NEXT_VERTEX) + x];
			
			//頂点座標を設定
			pVtx[z*(m_nNumBlock + NEXT_VERTEX) + x].vtx = D3DXVECTOR3(fVtxX, fVtxY, fVtxZ);
		}
	}

	///////////////////////////////////////////
	//		法線ベクトル設定ループ			//
	/////////////////////////////////////////

	//法線ベクトル計算用
	D3DXVECTOR3 vec1, vec2, vecN, normal;

	///////////////////////////////////////////////////////////////////////
	//		各頂点の方向ベクトルからポリゴンの法線ベクトルを求める		//
	/////////////////////////////////////////////////////////////////////

	//Zループ
	for (int z = 0; z<m_nNumBlock; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlock; x++)
		{
			//四角形ポリゴン中の三角ポリゴンの法線ベクトルを求める
			vec1 = pVtx[z*(m_nNumBlock + NEXT_VERTEX) + x].vtx - pVtx[(z + 1)*(m_nNumBlock + NEXT_VERTEX) + x].vtx;
			vec2 = pVtx[z*(m_nNumBlock + NEXT_VERTEX) + (x + NEXT_VERTEX)].vtx - pVtx[(z + 1)*(m_nNumBlock + NEXT_VERTEX) + x].vtx;

			//方向ベクトルの外積を求める
			D3DXVec3Cross(&vecN, &vec1, &vec2);

			//外積で求めた法線ベクトルを正規化
			D3DXVec3Normalize(&normal, &vecN);

			//正規化した法線ベクトルを法線配列に代入
			m_ppNormal[x][z * 2] = normal;

			//四角形ポリゴン中のもう片方の三角ポリゴンの法線ベクトルを求める
			vec1 = pVtx[z*(m_nNumBlock + NEXT_VERTEX) + (x + NEXT_VERTEX)].vtx - pVtx[(z + 1)*(m_nNumBlock + NEXT_VERTEX) + x].vtx;
			vec2 = pVtx[(z + 1)*(m_nNumBlock + NEXT_VERTEX) + (x + 1)].vtx - pVtx[(z + 1)*(m_nNumBlock + NEXT_VERTEX) + x].vtx;

			//方向ベクトルの外積を求める
			D3DXVec3Cross(&vecN, &vec1, &vec2);

			//外積で求めた法線ベクトルを正規化
			D3DXVec3Normalize(&normal, &vecN);

			//正規化した法線ベクトルを法線配列に代入
			m_ppNormal[x][z * 2 + 1] = normal;
		}
	}

	///////////////////////////////////////////////////
	//		各頂点に求めた法線ベクトルを代入		//
	/////////////////////////////////////////////////
	//Zループ
	for (int z = 0; z<m_nNumBlock + NEXT_VERTEX; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlock + NEXT_VERTEX; x++)
		{
			//頂点左端
			if (x == 0)
			{
				///////////////////////////
				//		頂点左端		//
				/////////////////////////

				//頂点上端
				if (z == 0)
				{
					//頂点左上のみ
					pVtx[z*(m_nNumBlock + NEXT_VERTEX) + x].nor = m_ppNormal[x][z];
				}
				else
				{
					//頂点左端のみの場合

					//四角形ポリゴン2つの法線ベクトルの平均を求める
					vecN = (m_ppNormal[x][(z - 1) * 2] + m_ppNormal[x][(z - 1) * 2 + 1])*0.5f
						+ m_ppNormal[x][z * 2];

					//正規化
					D3DXVec3Normalize(&normal, &vecN);

					//左端のみの頂点に法線ベクトルを入れる
					pVtx[z*(m_nNumBlock + NEXT_VERTEX) + x].nor = normal;
				}
			}//頂点右端
			else if (x == m_nNumBlock)
			{
				///////////////////////////
				//		頂点右端		//
				/////////////////////////

				//頂点右下
				if (z == m_nNumBlock)
				{
					//頂点右下のみの場合
					pVtx[z*(m_nNumBlock + NEXT_VERTEX) + x].nor = m_ppNormal[x - 1][(z - 1) * 2 + 1];

				}
				else
				{
					//頂点右端のみの場合

					//頂点右上
					if (z == 0)
					{
						//四角形ポリゴン2つの法線ベクトルの平均を求める
						vecN = (m_ppNormal[x - 1][z * 2] + m_ppNormal[x - 1][z * 2 + 1])*0.5f;
					}
					else
					{
						//四角形ポリゴン2つの法線ベクトルの平均を求める
						vecN = (m_ppNormal[x - 1][z * 2] + m_ppNormal[x - 1][z * 2 + 1])*0.5f
							+ m_ppNormal[x - 1][(z - 1) * 2 + 1];
					}

					//正規化
					D3DXVec3Normalize(&normal, &vecN);

					//頂点右端に法線ベクトルを入れる
					pVtx[z*(m_nNumBlock + NEXT_VERTEX) + x].nor = normal;
				}
			}//頂点真中
			else
			{
				///////////////////////////////////
				//		頂点端以外(真中)		//
				/////////////////////////////////

				//頂点上端
				if (z == 0)
				{
					//四角形ポリゴン2つの法線ベクトルの平均を求める
					vecN = (m_ppNormal[x - 1][z * 2] + m_ppNormal[x - 1][z * 2 + 1])*0.5f
						+ m_ppNormal[x][z * 2];

					//正規化
					D3DXVec3Normalize(&normal, &vecN);

				}//頂点下端
				else if (z == m_nNumBlock)
				{
					//四角形ポリゴン2つの法線ベクトルの平均を求める
					vecN = (m_ppNormal[x][(z - 1) * 2] + m_ppNormal[x][(z - 1) * 2 + 1])*0.5f
						+ m_ppNormal[x - 1][(z - 1) * 2 + 1];

					//正規化
					D3DXVec3Normalize(&normal, &vecN);

				}//頂点真中
				else
				{
					//四角形ポリゴン2つの法線ベクトルの平均を求める
					vecN = m_ppNormal[x - 1][(z - 1) * 2 + 1]
						+ (m_ppNormal[x - 1][z * 2] + m_ppNormal[x - 1][z * 2 + 1])*0.5f
						+ (m_ppNormal[x][(z - 1) * 2] + m_ppNormal[x][(z - 1) * 2 + 1])*0.5f
						+ m_ppNormal[x][z * 2];

					//正規化
					D3DXVec3Normalize(&normal, &vecN);
				}

				//四角ポリゴン左下
				pVtx[z*(m_nNumBlock + NEXT_VERTEX) + x].nor = normal;
			}
		}
	}

	///////////////////////////////////////////
	//			反射光設定ループ			//
	/////////////////////////////////////////

	for (int i = 0; i<m_nNumVertexIndex; i++)
	{
		pVtx[i].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	}

	///////////////////////////////
	//	テクスチャ設定ループ	//
	/////////////////////////////

	//Zループ
	for (int z = 0; z<m_nNumBlock + NEXT_VERTEX; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlock + NEXT_VERTEX; x++)
		{
			//上
			pVtx[z*(m_nNumBlock + NEXT_VERTEX) + x].tex = D3DXVECTOR2(0.0f + (x*1.0f), (z*1.0f));
		}
	}

	//頂点バッファの終了
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

	//インデックスカウント用
	int cnt = 0;

	//Xループ
	for (int x = 0; x<m_nNumBlock; x++)
	{
		//Zループ
		for (int z = 0; z<m_nNumBlock + NEXT_VERTEX; z++)
		{
			//左
			pIndex[cnt] = x + (z*(m_nNumBlock + 1));
			cnt++;

			//右
			pIndex[cnt] = (x + 1) + (z*(m_nNumBlock + 1));
			cnt++;

			//縮退ポリゴンに合わせて番号を付ける
			if (x != m_nNumBlock - 1 && z == m_nNumBlock)
			{
				//右
				pIndex[cnt] = (x + 1) + (z*(m_nNumBlock + 1));
				cnt++;

				//左
				pIndex[cnt] = (x + 1) + ((z - m_nNumBlock)*(m_nNumBlock + 1));
				cnt++;
			}

		}
	}

	//インデックスバッファ設定の終了
	m_pD3DIndexBuff->Unlock();


	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								pFileName,
								&m_pD3DTexture);

	//設定用の頂点を解放
	delete[] m_pfHeightVtx;
	m_pfHeightVtx = NULL;

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CFractalField::Uninit()
{
	//法線ベクトルの解放
	for (int i = 0; i<m_nNumBlock; i++)
	{
		delete[] m_ppNormal[i];
	}
	//法線ベクトルの解放
	delete[] m_ppNormal;

	//自身の終了
	CScene3D::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CFractalField::Update()
{
}
//=============================================================================
//描画
//=============================================================================
void CFractalField::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//ワイヤーフレーム
	//pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
	//マトリックスに関する変数
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//アイデンティティに関する変数
	D3DXMatrixIdentity(&m_mtxWorld);

	//拡大縮小を設定
	D3DXMatrixScaling(&mtxScl, 1.0f, 1.0f, 1.0f);

	//サイズを反映
	D3DXMatrixMultiply(&m_mtxWorld,
						&m_mtxWorld,
						&mtxScl);

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
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファのバインド
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
	//pDevice->SetRenderState(D3DRS_FILLMODE,NULL);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}
//=============================================================================
//フィールドの高さ取得
//=============================================================================
float CFractalField::GetHeight(D3DXVECTOR3 pos)
{
	float fHeight = 1.0f;			//返り値とするフィールドの高さ(Y座標)
	D3DXVECTOR3 pos0, pos1, pos2;	//各頂点の座標
	D3DXVECTOR3 vec0, vec1, vec2;	//三角形ポリゴン各頂点の方向ベクトル
	D3DXVECTOR3 vecP0, vecP1, vecP2;//各頂点からプレイヤーの方向ベクトル
	D3DXVECTOR3 cross;				//外積判定用変数
	int nVtxX = 0, nVtxZ = 0;		//フィールドの縦横からの頂点番号
	float fLengthX, fLengthZ;		//頂点左上からプレイヤーまでの距離

	///////////////////////////////////////////////////
	//		プレイヤーの位置から頂点番号を計算		//
	/////////////////////////////////////////////////

	//頂点番号0番目(左上)から対象の座標(プレイヤー)までの距離
	fLengthX = pos.x + (m_nNumBlock / 2)*m_fSizeBlock;
	fLengthZ = (m_nNumBlock / 2)*m_fSizeBlock - pos.z;

	//距離から頂点番号を求める
	nVtxX = (int)(fLengthX / (m_fSizeBlock));
	nVtxZ = (int)(fLengthZ / (m_fSizeBlock));

	//頂点番号がフィールドの外の場合はそのまま値を返す
	if (nVtxX<0 || nVtxZ<0 ||
		nVtxX >= m_nNumBlock || nVtxZ >= m_nNumBlock)
	{
		return fHeight;
	}

	//3Dポリゴン用頂点
	VERTEX_3D *pVtx;

	//頂点バッファの設定開始
	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	///////////////////////////////////
	//		左上ポリゴンの判定		//
	/////////////////////////////////

	//ポリゴンの各頂点座標
	pos0 = pVtx[nVtxZ*(m_nNumBlock + NEXT_VERTEX) + nVtxX].vtx;
	pos1 = pVtx[nVtxZ*(m_nNumBlock + NEXT_VERTEX) + (nVtxX + 1)].vtx;
	pos2 = pVtx[(nVtxZ + 1)*(m_nNumBlock + NEXT_VERTEX) + nVtxX].vtx;

	//各頂点座標の方向ベクトル
	vec0 = pos1 - pos0;
	vec1 = pos2 - pos1;
	vec2 = pos0 - pos2;

	//各頂点からプレイヤーへの方向ベクトル
	vecP0 = pos - pos0;
	vecP1 = pos - pos1;
	vecP2 = pos - pos2;

	///////////////////////////////////////////////////
	//		左上ポリゴンの位置にいるか判定する		//
	/////////////////////////////////////////////////

	if (vec0.x*vecP0.z - vec0.z*vecP0.x<0)
	{
		if (vec1.x*vecP1.z - vec1.z*vecP1.x<0)
		{
			if (vec2.x*vecP2.z - vec2.z*vecP2.x<0)
			{
				//左上ポリゴンの高さを返す
				fHeight = GetHeightPolygon(pos0, pos, m_ppNormal[nVtxX][nVtxZ * 2]);
				return fHeight;
			}
		}
	}

	//左上の位置にいない場合は右下ポリゴンの高さを返す
	pos0 = pVtx[nVtxZ*(m_nNumBlock + NEXT_VERTEX) + (nVtxX + 1)].vtx;
	fHeight = GetHeightPolygon(pos0, pos, m_ppNormal[nVtxX][nVtxZ * 2 + 1]);
	return fHeight;


	//頂点バッファの設定終了
	m_pD3DVtxBuff->Unlock();

	//高さを返す
	return fHeight;
}
//=============================================================================
//ポリゴンの高さ取得
//=============================================================================
float CFractalField::GetHeightPolygon(	D3DXVECTOR3 pos0,
										D3DXVECTOR3 playerPos,
										D3DXVECTOR3 normal)
{
	//法線ベクトルが0なら1を返す
	if (normal.y == 0)
	{
		return 1.0f;
	}

	//返り値用高さ変数
	float fHeight = 0.0f;

	//プレイヤーの位置のポリゴンのY座標を計算する
	fHeight = pos0.y - ((playerPos.x - pos0.x) * normal.x +
			(playerPos.z - pos0.z)*normal.z) / normal.y;

	//高さ(Y座標)を返す
	return fHeight;
}
//=============================================================================
//ポリゴンの高さ初期化
//=============================================================================
void CFractalField::InitHeightPolygon(int nDivision)
{
	//乱数の初期化
	srand((unsigned int)time(NULL));

	//高さ初期設定
	for (int i = 0; i < m_nNumVertexIndex; ++i)
	{
		m_pfHeightVtx[i] = 0.0f;
	}

	//頂点Y座標設定
	//次に設定する頂点との間隔(初期位置からとの差)
	int nSpaceVtxNum = m_nNumBlock / 2;

	//頂点設定時の初期位置
	int nStartVtxNum = nSpaceVtxNum / 2;

	//振れ幅
	int nDeflection = 100;

	//割合
	float fRate = 1.0f;
	//割合減算量
	float fDownRate = fRate / m_nNumVertexIndex;

	//z*(m_nNumBlock + NEXT_VERTEX) + x

	int g = 0;

	//分割毎の頂点設定
	for (int i = 0; i < nDivision; ++i)
	{
		for (int z = 0; z < m_nNumBlock + 1; ++z)
		{
			for (int x = 0; x < m_nNumBlock + 1; ++x)
			{
				//頂点番号
				int nIndex = z * (m_nNumBlock + NEXT_VERTEX) + x;

				if (nIndex % nSpaceVtxNum == 0 && m_pfHeightVtx[nIndex] == 0.0f)
				{
					float ff = (float)(rand() % (int)(nDeflection * fRate)) + 50.0f;
					m_pfHeightVtx[nIndex] = ff;
				}
			}
		}

		fRate -= fDownRate;
		nSpaceVtxNum /= 2;
	}
}
//=============================================================================
//インスタンス生成
//=============================================================================
CFractalField *CFractalField::Create(	char *pFileName,
										D3DXVECTOR3 pos,
										D3DXVECTOR3 rot,
										int nDivision,
										float fSize)
{
	//インスタンス生成
	CFractalField *pFractalField = new CFractalField();
	//初期化
	pFractalField->Init(pFileName,pos, rot, nDivision, fSize);
	return pFractalField;
}
//EOF