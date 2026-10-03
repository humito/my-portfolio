//=============================================================================
// 水面処理 [Water.cpp]
// Author : 木村 文登
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Water.h"
#include "../manager.h"
#include "../System/Input/InputKeyboard.h"
#include "../Shader/CubeMap.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define TEX_FILE_NAME ("data/TEXTURE/water-texture.jpg")

//=============================================================================
//初期化
//=============================================================================
HRESULT CWater::Init()
{
	m_fTime = 0.0f;

	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//フィールド情報
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//座標
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//回転

	//ブロック数
	m_nNumBlockX = VTX_NUM_X - 1;
	m_nNumBlockZ = VTX_NUM_Z - 1;

	//ブロックサイズ
	m_fSizeBlockX = 80.0f;
	m_fSizeBlockZ = 80.0f;

	//法線ベクトルをブロック数分確保
	m_ppNormal = new D3DXVECTOR3*[m_nNumBlockX];
	//2次元配列として各配列に動的確保
	for (int i = 0; i<m_nNumBlockX; i++)
	{
		m_ppNormal[i] = new D3DXVECTOR3[m_nNumBlockZ * 2];
	}

	//総ポリゴン数
	m_nNumPolygon = ((m_nNumBlockX * m_nNumBlockZ) * 2) + 4 * (m_nNumBlockZ - 1);

	//総頂点数
	m_nNumVertex = (m_nNumBlockX + 1) * 2 * m_nNumBlockZ + 2 * (m_nNumBlockZ - 1);

	//インデックス数
	m_nNumVertexIndex = (m_nNumBlockX + 1) * (m_nNumBlockZ + 1);

	for (int x = 0; x < m_nNumBlockX; ++x)
	{
		for (int z = 0; z < m_nNumBlockZ; ++z)
		{
			m_WaterPoint[x][z].fHeight = 0.0f;
			m_WaterPoint[x][z].fVelocity = 0.0f;
			m_WaterPoint[x][z].fMass = 1.0f;
		}
	}

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
	//奥から手前への頂点設定

	//※右横方向に頂点を設定
	//Zループ
	for (int z = 0; z < m_nNumBlockZ + 1; z++)
	{
		//Xループ
		for (int x = 0; x < m_nNumBlockX + 1; x++)
		{
			///////////////////////////////////////////////////////
			//		ブロック数とサイズから頂点座標を計算		//
			/////////////////////////////////////////////////////

			//X座標
			fVtxX = -(m_nNumBlockX / 2) * m_fSizeBlockX + x * m_fSizeBlockX;

			//Y座標
			fVtxY = m_WaterPoint[x][z].fHeight;

			//Z座標
			fVtxZ = (m_nNumBlockZ / 2) * m_fSizeBlockZ - z * m_fSizeBlockZ;

			//頂点座標を設定
			pVtx[z * (m_nNumBlockX + 1) + x].vtx = D3DXVECTOR3(fVtxX, fVtxY, fVtxZ);
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
	for (int z = 0; z<m_nNumBlockZ; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX; x++)
		{
			//四角形ポリゴン中の三角ポリゴンの法線ベクトルを求める
			vec1 = pVtx[z*(m_nNumBlockX + 1) + x].vtx - pVtx[(z + 1)*(m_nNumBlockX + 1) + x].vtx;
			vec2 = pVtx[z*(m_nNumBlockX + 1) + (x + 1)].vtx - pVtx[(z + 1)*(m_nNumBlockX + 1) + x].vtx;

			//方向ベクトルの外積を求める
			D3DXVec3Cross(&vecN, &vec1, &vec2);

			//外積で求めた法線ベクトルを正規化
			D3DXVec3Normalize(&normal, &vecN);

			//正規化した法線ベクトルを法線配列に代入
			m_ppNormal[x][z * 2] = normal;

			//四角形ポリゴン中のもう片方の三角ポリゴンの法線ベクトルを求める
			vec1 = pVtx[z*(m_nNumBlockX + 1) + (x + 1)].vtx - pVtx[(z + 1)*(m_nNumBlockX + 1) + x].vtx;
			vec2 = pVtx[(z + 1)*(m_nNumBlockX + 1) + (x + 1)].vtx - pVtx[(z + 1)*(m_nNumBlockX + 1) + x].vtx;

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
	for (int z = 0; z<m_nNumBlockZ + 1; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX + 1; x++)
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
					pVtx[z*(m_nNumBlockX + 1) + x].nor = m_ppNormal[x][z];
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
					pVtx[z*(m_nNumBlockX + 1) + x].nor = normal;
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
					pVtx[z*(m_nNumBlockX + 1) + x].nor = m_ppNormal[x - 1][(z - 1) * 2 + 1];

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
					pVtx[z*(m_nNumBlockX + 1) + x].nor = normal;
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
				pVtx[z*(m_nNumBlockX + 1) + x].nor = normal;
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
	for (int z = 0; z<m_nNumBlockZ + 1; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX + 1; x++)
		{
			//上
			pVtx[z*(m_nNumBlockX + 1) + x].tex = D3DXVECTOR2(0.0f + (x*1.0f), (z*1.0f));
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

	//※Z字になるようにつなげる
	//Xループ
	for (int x = 0; x<m_nNumBlockX; x++)
	{
		//Zループ
		for (int z = 0; z<m_nNumBlockZ + 1; z++)
		{
			//左
			pIndex[cnt] = x + (z*(m_nNumBlockX + 1));
			cnt++;

			//右
			pIndex[cnt] = (x + 1) + (z*(m_nNumBlockX + 1));
			cnt++;

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
	D3DXCreateTextureFromFile(	pDevice,
								TEX_FILE_NAME,
								&m_pD3DTexture);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CWater::Uninit()
{
	//確保した法線ベクトルの解放
	for (int i = 0; i < m_nNumBlockX; i++)
	{
		delete[] m_ppNormal[i];
	}
	delete[] m_ppNormal;

	CScene3D::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CWater::Update()
{
	m_fTime += 1.0f;

	if ((int)m_fTime % 100 == 0)
		m_WaterPoint[VTX_NUM_X / 2][VTX_NUM_Z / 2].fHeight = (float)(rand() % 500);

	//速度更新
	for (int x = 1; x < VTX_NUM_X - 1; ++x)
	{
		for (int z = 1; z < VTX_NUM_Z - 1; ++z)
		{
			//ばねの力の計算
			float force = 0.0f;
			force += (m_WaterPoint[x + 1][z].fHeight - m_WaterPoint[x][z].fHeight) * 0.005f;
			force += (m_WaterPoint[x - 1][z].fHeight - m_WaterPoint[x][z].fHeight) * 0.005f;
			force += (m_WaterPoint[x][z + 1].fHeight - m_WaterPoint[x][z].fHeight) * 0.005f;
			force += (m_WaterPoint[x][z - 1].fHeight - m_WaterPoint[x][z].fHeight) * 0.005f;
			//運動方程式の計算
			m_WaterPoint[x][z].fVelocity += force / m_WaterPoint[x][z].fMass;
		}
	}

	//位置の更新
	for (int x = 0; x < VTX_NUM_X; ++x)
	{
		for (int z = 0; z < VTX_NUM_Z; ++z)
		{
			m_WaterPoint[x][z].fHeight += m_WaterPoint[x][z].fVelocity;
		}
	}

	//3Dポリゴン用頂点
	VERTEX_3D *pVtx;

	//頂点バッファの設定開始
	m_pD3DVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//Zループ
	for (int z = 0; z < m_nNumBlockZ + 1; z++)
	{
		//Xループ
		for (int x = 0; x < m_nNumBlockX + 1; x++)
		{
			//Y座標
			pVtx[z * (m_nNumBlockX + 1) + x].vtx.y = m_WaterPoint[x][z].fHeight;
		}
	}


	///////////////////////////////////////////////////////////////////////
	//		各頂点の方向ベクトルからポリゴンの法線ベクトルを求める		//
	/////////////////////////////////////////////////////////////////////

	//法線ベクトル計算用
	D3DXVECTOR3 vec1, vec2, vecN, normal;

	//Zループ
	for (int z = 0; z<m_nNumBlockZ; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX; x++)
		{
			//四角形ポリゴン中の三角ポリゴンの法線ベクトルを求める
			vec1 = pVtx[z*(m_nNumBlockX + 1) + x].vtx - pVtx[(z + 1)*(m_nNumBlockX + 1) + x].vtx;
			vec2 = pVtx[z*(m_nNumBlockX + 1) + (x + 1)].vtx - pVtx[(z + 1)*(m_nNumBlockX + 1) + x].vtx;

			//方向ベクトルの外積を求める
			D3DXVec3Cross(&vecN, &vec1, &vec2);

			//外積で求めた法線ベクトルを正規化
			D3DXVec3Normalize(&normal, &vecN);

			//正規化した法線ベクトルを法線配列に代入
			m_ppNormal[x][z * 2] = normal;

			//四角形ポリゴン中のもう片方の三角ポリゴンの法線ベクトルを求める
			vec1 = pVtx[z*(m_nNumBlockX + 1) + (x + 1)].vtx - pVtx[(z + 1)*(m_nNumBlockX + 1) + x].vtx;
			vec2 = pVtx[(z + 1)*(m_nNumBlockX + 1) + (x + 1)].vtx - pVtx[(z + 1)*(m_nNumBlockX + 1) + x].vtx;

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
	for (int z = 0; z<m_nNumBlockZ + 1; z++)
	{
		//Xループ
		for (int x = 0; x<m_nNumBlockX + 1; x++)
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
					pVtx[z*(m_nNumBlockX + 1) + x].nor = m_ppNormal[x][z];
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
					pVtx[z*(m_nNumBlockX + 1) + x].nor = normal;
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
					pVtx[z*(m_nNumBlockX + 1) + x].nor = m_ppNormal[x - 1][(z - 1) * 2 + 1];

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
					pVtx[z*(m_nNumBlockX + 1) + x].nor = normal;
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
				pVtx[z*(m_nNumBlockX + 1) + x].nor = normal;
			}
		}
	}
	m_pD3DVtxBuff->Unlock();
}
//=============================================================================
//描画
//=============================================================================
void CWater::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//カリング裏面
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

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

	CCubeMap *pMap = pRenderer->GetCubeMap();
	pMap->SetMatrix(pDevice, &m_mtxWorld);
	pMap->Begin(pDevice);

	//頂点バッファのバインド
	pDevice->SetStreamSource(0, m_pD3DVtxBuff, 0, sizeof(VERTEX_3D));

	//インデックスをバインド
	pDevice->SetIndices(m_pD3DIndexBuff);

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(pMap->GetPSConstantTable()->GetSamplerIndex("g_Sampler"),
						m_pD3DTexture);

	//ポリゴンの描画(インデックス)
	pDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLESTRIP,
									0,
									0,
									m_nNumVertexIndex,
									0,
									m_nNumPolygon);

	//元に戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	pMap->End(pDevice);
}
//=============================================================================
//インスタンス生成
//=============================================================================
CWater *CWater::Create()
{
	CWater *pInstance = new CWater();
	pInstance->Init();
	return pInstance;
}
//EOF