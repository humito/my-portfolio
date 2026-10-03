//=============================================================================
// 影描画処理 [Shadow.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Shadow.h"
#include "manager.h"
#include "renderer.h"
#include "main.h"
#include "Game.h"
#include "MeshField.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define SHADOW_SCALE_XZ (2.5f)	//影の幅の倍率
#define SHADOW_ALPHA (155)		//ステンシルシャドウのポリゴンの透過値

//=============================================================================
//コンストラクタ
//=============================================================================
CShadow::CShadow(int priority) : CSceneX(priority)
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CShadow::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//ポリゴンの設定
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_scl = D3DXVECTOR3(SHADOW_SCALE_XZ, 1.0f, SHADOW_SCALE_XZ);

	//Xファイル読み込み
	if (FAILED(D3DXLoadMeshFromX(	"data/MODEL/shadow000.x",
									D3DXMESH_SYSTEMMEM,
									pDevice,
									NULL,
									&m_pD3DXBuffMatModel,
									NULL,
									&m_nNumMatModel,
									&m_pD3DXMeshModel)))
	{
		return E_FAIL;
	}

	//頂点座標の代入
	m_aVtx[0].vtx = D3DXVECTOR3(0, SCREEN_HEIGHT, 0.0f);
	m_aVtx[1].vtx = D3DXVECTOR3(0, 0, 0.0f);
	m_aVtx[2].vtx = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	m_aVtx[3].vtx = D3DXVECTOR3(SCREEN_WIDTH, 0, 0.0f);

	//中身
	m_aVtx[0].rhw = 1.0f;
	m_aVtx[1].rhw = 1.0f;
	m_aVtx[2].rhw = 1.0f;
	m_aVtx[3].rhw = 1.0f;

	//反射光
	m_aVtx[0].diffuse = D3DCOLOR_RGBA(0, 0, 0, SHADOW_ALPHA);
	m_aVtx[1].diffuse = D3DCOLOR_RGBA(0, 0, 0, SHADOW_ALPHA);
	m_aVtx[2].diffuse = D3DCOLOR_RGBA(0, 0, 0, SHADOW_ALPHA);
	m_aVtx[3].diffuse = D3DCOLOR_RGBA(0, 0, 0, SHADOW_ALPHA);

	//マテリアル取得
	m_pD3DXMat = (D3DXMATERIAL*)m_pD3DXBuffMatModel->GetBufferPointer();

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CShadow::Uninit()
{
	//自身の終了
	CSceneX::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CShadow::Update()
{
	///////////////////////////////////////////
	//		フィールドと高さを合わせる		//
	/////////////////////////////////////////
	//フィールドインスタンス生成
	CMeshField *pField = CGame::GetField();
	//フィールドの高さに合わせる
	m_pos.y = pField->GetHeight(m_pos);
}
//=============================================================================
//描画
//=============================================================================
void CShadow::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);				//ステンシブルバッファ有効
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				//Zバッファへの書き込みをしない
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0);			//バックバッファへの書き込みをしない(カラーバッファ無効)
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);		//ステンシルバッファを全て描画
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);	//もし描画できるならインクリメント
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);			//表面だけ描画
	
	D3DMATERIAL9 matDef;
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;//サイズ,回転,位置

	//プロジェクションマトリックスを反映
	D3DXMatrixIdentity(&m_mtxWorld);

	//サイズを設定
	D3DXMatrixScaling(&mtxScl,
		m_scl.x,
		m_scl.y,
		m_scl.z);

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
	D3DXMatrixMultiply(	&m_mtxWorld, &m_mtxWorld,
						&mtxRot);


	//位置を設定
	D3DXMatrixTranslation(	&mtxTranslate,
							m_pos.x,
							m_pos.y,
							m_pos.z);

	//位置を反映
	D3DXMatrixMultiply(	&m_mtxWorld, &m_mtxWorld,
						&mtxTranslate);

	//マテリアル取得
	pDevice->GetMaterial(&matDef);

	//親マトリックスと合成
	//D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &m_mtxParent);

	//ワールドマトリクスセット
	pDevice->SetTransform(	D3DTS_WORLD,
							&m_mtxWorld);

	//モデル描画
	for (int nCntMat = 0; nCntMat<(int)m_nNumMatModel; nCntMat++)
	{
		pDevice->SetMaterial(&m_pD3DXMat[nCntMat].MatD3D);
		pDevice->SetTexture(0, NULL);
		m_pD3DXMeshModel->DrawSubset(nCntMat);
	}
	
	//マテリアルセット
	pDevice->SetMaterial(&matDef);

	//モデル描画後
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);	//デクリメント
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);			//裏面だけ描画

	//モデル描画
	for (int nCntMat = 0; nCntMat<(int)m_nNumMatModel; nCntMat++)
	{
		pDevice->SetMaterial(&m_pD3DXMat[nCntMat].MatD3D);
		pDevice->SetTexture(0, NULL);
		m_pD3DXMeshModel->DrawSubset(nCntMat);
	}

	//マテリアルセット
	pDevice->SetMaterial(&matDef);

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);		//カリングを戻す
	pDevice->SetRenderState(D3DRS_STENCILREF, 0x01);			//ステンシルの値が1なら描画OK
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);	//ステンシルファンク戻す
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);		//カラーライトイネーブル0xf
	
	//2Dポリゴン描画
	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
							2,
							&m_aVtx[0],
							sizeof(VERTEX_2D));

	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);		//Zバッファへの書き込みを戻す
	pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);	//ステンシルバッファ使用後戻す
}
//=============================================================================
//インスタンス生成
//=============================================================================
CShadow *CShadow::Create()
{
	//シャドウインスタンス生成
	CShadow *pShadow = new CShadow();
	//初期化
	pShadow->Init();

	return pShadow;
}
//EOF