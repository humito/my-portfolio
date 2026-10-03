//=============================================================================
//メッシュフィールド処理[MeshField.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#define _CRT_SECURE_NO_WARNINGS//警告対策用
#include <stdio.h>
#include "MeshField.h"
#include "../../manager.h"
#include "../../System/renderer.h"
#include "../../Shader/Shadow.h"
#include "../../Shader/Gaussian.h"
#include "../../Shader/Fur.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define RECT_PRIMITIVE (2)		//四角形ポリゴンのプリミティブ数
#define NEXT_POLIGON_NUM (4)	//行へ移動する時のポリゴン数
#define NEXT_VERTEX (1)			//次の頂点
#define PREV_VERTEX (1)			//前の頂点
#define LINE_LAST_VERTEX (2)	//行の最後の頂点
#define FIELD_NUM_X (6)			//フィールドの数X
#define FIELDSIZE_X (90.0f)		//フィールドのXサイズ 
#define FIELD_NUM_Z (6)			//フィールドの数Z
#define FIELDSIZE_Z (90.0f)		//フィールドのZサイズ

//*****************************************************************************
//グローバル変数
//*****************************************************************************
//予備用高さマップ
float g_HeightMap[10][10] =
{
	{ 6.0f, 13.0f, 13.0f, 25.0f, 14.0f, 11.0f, 16.0f, 37.0f, 13.0f, 13.0f },
	{ 7.0f, 10.0f, 5.0f, 6.0f, 17.0f, 7.0f, 22.0f, 15.0f, 14.0f, 8.0f },
	{ 13.0f, 3.0f, 18.0f, 25.0f, 19.0f, 8.0f, 17.0f, 7.0f, 22.0f, 15.0f },
	{ 5.0f, 24.0f, 10.0f, 18.0f, 13.0f, 14.0f, 2.0f, 16.0f, 15.0f, 9.0f },
	{ 4.0f, 12.0f, 26.0f, 28.0f, 12.0f, 25.0f, 6.0f, 7.0f, 17.0f, 12.0f }
};

//=============================================================================
//コンストラクタ
//=============================================================================
CMeshField::CMeshField()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CMeshField::~CMeshField()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CMeshField::Init(int nLoad, char *pFieldName, char *pTexFileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//ファー回数初期化
	m_nFurTimes = 0;

	///////////////////////////////////////
	//		フィールドデータロード		//
	/////////////////////////////////////

	//ファイルポインタ
	FILE *pFile = NULL;

	//各頂点の高さ配列
	float *pfHeightArray = NULL;

	//ロードするときのみフィールドデータの読込
	if (nLoad == LOAD_FIELD)
	{
		//フィールドデータのロード
		pFile = fopen(pFieldName, "rb");

		//ファイルがあるならデータ読込
		if (pFile)
		{
			//フィールド数
			fread(&m_nNumBlockX, sizeof(int), 1, pFile);//X
			fread(&m_nNumBlockZ, sizeof(int), 1, pFile);//Z
			//ブロックサイズ
			fread(&m_fSizeBlockX, sizeof(float), 1, pFile);//X
			fread(&m_fSizeBlockZ, sizeof(float), 1, pFile);//Z
		}
		else
		{
			//ファイルがない場合はロードなし状態にする
			nLoad = NO_LOAD_FIELD;
		}
	}

	//ロードなし又は、ファイルが見つからない場合
	if (nLoad == NO_LOAD_FIELD)
	{
		//定数からブロック数の代入
		m_nNumBlockX = FIELD_NUM_X;
		m_nNumBlockZ = FIELD_NUM_Z;

		//定数からサイズの代入
		m_fSizeBlockX = FIELDSIZE_X;
		m_fSizeBlockZ = FIELDSIZE_Z;
	}

	//法線ベクトルをブロック数分確保
	m_ppNormal = new D3DXVECTOR3*[m_nNumBlockX];
	//2次元配列として各配列に動的確保
	for (int i = 0; i<m_nNumBlockX; i++)
	{
		m_ppNormal[i] = new D3DXVECTOR3[m_nNumBlockZ * 2];
	}

	//総ポリゴン数
	m_nNumPolygon = ((m_nNumBlockX*m_nNumBlockZ)*RECT_PRIMITIVE) + NEXT_POLIGON_NUM*(m_nNumBlockZ - PREV_VERTEX);

	//総頂点数
	m_nNumVertex = (m_nNumBlockX + NEXT_VERTEX)*RECT_PRIMITIVE*m_nNumBlockZ + RECT_PRIMITIVE*(m_nNumBlockZ - PREV_VERTEX);

	//インデックス数
	m_nNumVertexIndex = (m_nNumBlockX + NEXT_VERTEX)*(m_nNumBlockZ + NEXT_VERTEX);

	//ロード時のみ頂点の高さ読込
	if (nLoad == LOAD_FIELD)
	{
		//頂点数分フィールド情報確保
		pfHeightArray = new float[m_nNumVertexIndex];
		//高さ情報の読込
		fread(pfHeightArray, sizeof(float), m_nNumVertexIndex, pFile);
	}

	//フィールド情報
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
	for (int z = 0; z<m_nNumBlockZ + NEXT_VERTEX; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX + NEXT_VERTEX; x++)
		{
			///////////////////////////////////////////////////////
			//		ブロック数とサイズから頂点座標を計算		//
			/////////////////////////////////////////////////////

			//X座標
			fVtxX = -(m_nNumBlockX / 2)*m_fSizeBlockX + x*m_fSizeBlockX;

			//Y座標
			//高さ配列を読み込んでいるなら配列から代入
			if (pfHeightArray)
			{
				fVtxY = pfHeightArray[z*(m_nNumBlockX + NEXT_VERTEX) + x];
			}
			//読み込んでない場合は高さマップから代入
			else
			{
				fVtxY = g_HeightMap[x % 5][z % 5];
			}

			//Z座標
			fVtxZ = (m_nNumBlockZ / 2)*m_fSizeBlockZ - z*m_fSizeBlockZ;

			//頂点座標を設定
			pVtx[z*(m_nNumBlockX + NEXT_VERTEX) + x].vtx = D3DXVECTOR3(fVtxX, fVtxY, fVtxZ);
		}
	}

	//ファイルを読み込んでいるなら閉じる
	if (pFile)
	{
		//ファイルを閉じる
		fclose(pFile);
	}

	//高さ配列解放
	if (pfHeightArray)
	{
		delete[] pfHeightArray;
		pfHeightArray = NULL;
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
	for (int z = 0; z<m_nNumBlockZ; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX; x++)
		{
			//四角形ポリゴン中の三角ポリゴンの法線ベクトルを求める
			vec1 = pVtx[z*(m_nNumBlockX + NEXT_VERTEX) + x].vtx - pVtx[(z + 1)*(m_nNumBlockX + NEXT_VERTEX) + x].vtx;
			vec2 = pVtx[z*(m_nNumBlockX + NEXT_VERTEX) + (x + NEXT_VERTEX)].vtx - pVtx[(z + 1)*(m_nNumBlockX + NEXT_VERTEX) + x].vtx;

			//方向ベクトルの外積を求める
			D3DXVec3Cross(&vecN, &vec1, &vec2);

			//外積で求めた法線ベクトルを正規化
			D3DXVec3Normalize(&normal, &vecN);

			//正規化した法線ベクトルを法線配列に代入
			m_ppNormal[x][z * 2] = normal;

			//四角形ポリゴン中のもう片方の三角ポリゴンの法線ベクトルを求める
			vec1 = pVtx[z*(m_nNumBlockX + NEXT_VERTEX) + (x + NEXT_VERTEX)].vtx - pVtx[(z + 1)*(m_nNumBlockX + NEXT_VERTEX) + x].vtx;
			vec2 = pVtx[(z + 1)*(m_nNumBlockX + NEXT_VERTEX) + (x + 1)].vtx - pVtx[(z + 1)*(m_nNumBlockX + NEXT_VERTEX) + x].vtx;

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
	for (int z = 0; z<m_nNumBlockZ + NEXT_VERTEX; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX + NEXT_VERTEX; x++)
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
					pVtx[z*(m_nNumBlockX + NEXT_VERTEX) + x].nor = m_ppNormal[x][z];
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
					pVtx[z*(m_nNumBlockX + NEXT_VERTEX) + x].nor = normal;
				}
			}//頂点右端
			else if (x == m_nNumBlockX)
			{
				///////////////////////////
				//		頂点右端		//
				/////////////////////////

				//頂点右下
				if (z == m_nNumBlockZ)
				{
					//頂点右下のみの場合
					pVtx[z*(m_nNumBlockX + NEXT_VERTEX) + x].nor = m_ppNormal[x - 1][(z - 1) * 2 + 1];

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
					pVtx[z*(m_nNumBlockX + NEXT_VERTEX) + x].nor = normal;
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
				else if (z == m_nNumBlockZ)
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
				pVtx[z*(m_nNumBlockX + NEXT_VERTEX) + x].nor = normal;
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
	for (int z = 0; z<m_nNumBlockZ + NEXT_VERTEX; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX + NEXT_VERTEX; x++)
		{
			//上
			pVtx[z*(m_nNumBlockX + NEXT_VERTEX) + x].tex = D3DXVECTOR2(0.0f + (x*1.0f), (z*1.0f));
		}
	}

	//頂点バッファの終了
	m_pD3DVtxBuff->Unlock();

	///////////////////////////////////////////////////////////////////////////////
	//					インデックスバッファの生成								//
	/////////////////////////////////////////////////////////////////////////////

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
	for (int x = 0; x<m_nNumBlockX; x++)
	{
		//Zループ
		for (int z = 0; z<m_nNumBlockZ + NEXT_VERTEX; z++)
		{
			//左
			pIndex[cnt] = x + (z*(m_nNumBlockX + 1));
			cnt++;

			//(x+(m_nNumBlockX+1))+(z*(m_nNumBlockZ+1));

			//右
			pIndex[cnt] = (x + 1) + (z*(m_nNumBlockX + 1));
			cnt++;
			//x+(z*(m_nNumBlockX+1));

			//縮退ポリゴンに合わせて番号を付ける
			if (x != m_nNumBlockX - 1 && z == m_nNumBlockZ)
			{
				//右
				pIndex[cnt] = (x + 1) + (z*(m_nNumBlockX + 1));
				cnt++;

				//左
				pIndex[cnt] = (x + 1) + ((z - m_nNumBlockZ)*(m_nNumBlockX + 1));
				cnt++;
			}

		}
	}

	//インデックスバッファ設定の終了
	m_pD3DIndexBuff->Unlock();


	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							pTexFileName,
							&m_pD3DTexture);
	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CMeshField::Uninit()
{
	//確保した法線ベクトルの解放
	for (int i = 0; i<m_nNumBlockX; i++)
	{
		delete[] m_ppNormal[i];
	}
	delete[] m_ppNormal;

	//自身の終了
	CScene3D::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CMeshField::Update()
{
}
//=============================================================================
//描画
//=============================================================================
void CMeshField::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

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
	D3DXMatrixTranslation(&mtxTranslate,
						m_pos.x,
						m_pos.y,
						m_pos.z);

	//位置をセット
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
					&mtxTranslate);

	//ワールドマトリックスの設定
	//pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファのバインド
	pDevice->SetStreamSource(0, m_pD3DVtxBuff, 0, sizeof(VERTEX_3D));

	//インデックスをバインド
	pDevice->SetIndices(m_pD3DIndexBuff);

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_3D);

	//投影シャドウシェーダー取得
	CShadow *pShadow = pRenderer->GetShadow();

	//投影シャドウにマトリックスのセット
	pShadow->SetMatrix(pDevice, SHADER_SHADOW_RECEIVE, &m_mtxWorld);
	//マテリアル色のセット
	pShadow->SetColor(pDevice, SHADER_SHADOW_RECEIVE,
					D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	//投影シャドウシェーダー開始
	pShadow->Begin(pDevice, SHADER_SHADOW_RECEIVE);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pD3DTexture);
	//影テクスチャのセット
	//pDevice->SetTexture(1, pShadow->GetTexture());
	pDevice->SetTexture(1, pRenderer->GetShadowGauss()->GetBlurTexture());

	//ポリゴンの描画(インデックス)
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
								0,
								0,
								m_nNumVertexIndex,
								0,
								m_nNumPolygon);

	//投影シャドウシェーダー完了
	pShadow->End(pDevice);

	//ファーシェーダー開始
	CFur *pFur = pRenderer->GetFur();
	pFur->SetMatrix(pDevice, &m_mtxWorld, m_pos);
	pFur->Begin(pDevice);

	for (int i = 0; i < m_nFurTimes; ++i)
	{
		pFur->SetOffset(pDevice, i * 0.02f);

		//ポリゴンの描画(インデックス)
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
										0,
										0,
										m_nNumVertexIndex,
										0,
										m_nNumPolygon);
	}

	//ファーシェーダー完了
	pFur->End(pDevice);
}
//=============================================================================
//フィールドの高さ取得
//=============================================================================
float CMeshField::GetHeight(D3DXVECTOR3 pos)
{
	float fHeight = 1.0f;				//返り値とするフィールドの高さ(Y座標)
	D3DXVECTOR3 pos0, pos1, pos2;		//各頂点の座標
	D3DXVECTOR3 vec0, vec1, vec2;		//三角形ポリゴン各頂点の方向ベクトル
	D3DXVECTOR3 vecP0, vecP1, vecP2;	//各頂点からプレイヤーの方向ベクトル
	D3DXVECTOR3 cross;					//外積判定用変数
	int nVtxX = 0, nVtxZ = 0;			//フィールドの縦横からの頂点番号
	float fLengthX, fLengthZ;			//頂点左上からプレイヤーまでの距離

	///////////////////////////////////////////////////
	//		プレイヤーの位置から頂点番号を計算		//
	/////////////////////////////////////////////////

	//頂点番号0番目(左上)から対象の座標(プレイヤー)までの距離
	fLengthX = pos.x + (FIELD_NUM_X / 2)*FIELDSIZE_X;
	fLengthZ = (FIELD_NUM_Z / 2)*FIELDSIZE_Z - pos.z;

	//距離から頂点番号を求める
	nVtxX = (int)(fLengthX / (FIELDSIZE_X));
	nVtxZ = (int)(fLengthZ / (FIELDSIZE_Z));

	//頂点番号がフィールドの外の場合はそのまま値を返す
	if (nVtxX<0 || nVtxZ<0 ||
		nVtxX >= m_nNumBlockX || nVtxZ >= m_nNumBlockZ)
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
	pos0 = pVtx[nVtxZ*(m_nNumBlockX + NEXT_VERTEX) + nVtxX].vtx;
	pos1 = pVtx[nVtxZ*(m_nNumBlockX + NEXT_VERTEX) + (nVtxX + 1)].vtx;
	pos2 = pVtx[(nVtxZ + 1)*(m_nNumBlockX + NEXT_VERTEX) + nVtxX].vtx;

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
				m_pD3DVtxBuff->Unlock();
				return fHeight;
			}
		}
	}

	//左上の位置にいない場合は右下ポリゴンの高さを返す
	pos0 = pVtx[nVtxZ*(m_nNumBlockX + NEXT_VERTEX) + (nVtxX + 1)].vtx;
	fHeight = GetHeightPolygon(pos0, pos, m_ppNormal[nVtxX][nVtxZ * 2 + 1]);
	m_pD3DVtxBuff->Unlock();
	return fHeight;


	//頂点バッファの設定終了
	m_pD3DVtxBuff->Unlock();

	//高さを返す
	return fHeight;
}
//=============================================================================
//ポリゴンのY座標を取得
//=============================================================================
float CMeshField::GetHeightPolygon(D3DXVECTOR3 pos0, D3DXVECTOR3 playerPos, D3DXVECTOR3 normal)
{
	//法線ベクトルが0なら1を返す
	if (normal.y == 0)
	{
		return 1.0f;
	}

	//返り値用高さ変数
	float fHeight;

	//プレイヤーの位置のポリゴンのY座標を計算する
	fHeight = pos0.y - ((playerPos.x - pos0.x)*normal.x +
		(playerPos.z - pos0.z)*normal.z) / normal.y;

	//高さ(Y座標)を返す
	return fHeight;
}
//=============================================================================
//インスタンス生成
//=============================================================================
CMeshField *CMeshField::Create(int nLoad, char *pFieldName, char *pTexFileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//メッシュフィールドポインタ
	CMeshField *pMeshField;

	//インスタンス生成
	pMeshField = new CMeshField;

	//初期化
	pMeshField->Init(nLoad, pFieldName, pTexFileName, pos, rot);

	return pMeshField;
}
//EOF