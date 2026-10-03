//=============================================================================
// 表示用オブジェクト [Object.cpp]
// Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Object.h"
#include "FrustumCulling.h"
#include "ToonShader.h"
#include "ShaderManager.h"
#include "manager.h"
#include "renderer.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define DEFAULT_RADIUS (45.0f)

//=============================================================================
//初期化
//=============================================================================
HRESULT CObject::Init(char *pFileName,
					D3DXVECTOR3 pos,
					D3DXVECTOR3 rot)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//座標・角度・スケール設定
	m_pos = pos;
	m_rot = rot;
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//描画フラグ
	m_bDraw = false;

	//Xファイルのロード
	if (FAILED(D3DXLoadMeshFromX(pFileName,
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

	//マテリアルのバッファポインタ取得
	m_pD3DXMat = (D3DXMATERIAL*)m_pD3DXBuffMatModel->GetBufferPointer();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
							"data/TEXTURE/1x1.tga",
							&m_pD3DTexture);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CObject::Uninit()
{
	//テクスチャ解放
	if (m_pD3DTexture)
	{
		m_pD3DTexture->Release();
		m_pD3DTexture = NULL;
	}

	//終了処理
	CSceneX::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CObject::Update()
{
	//描画フラグチェック
	m_bDraw = CFrustum::MeshFOVCheck(m_pos, DEFAULT_RADIUS);
}
//=============================================================================
//描画
//=============================================================================
void CObject::Draw()
{
	//描画可能な場合描画処理をする
	if (m_bDraw)
	{
		//レンダラー情報取得
		CRenderer *pRenderer = CManager::GetRenderer();
		//デバイスのゲット
		LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

		//マテリアルプロパティ
		D3DMATERIAL9 matDef;

		//サイズ,回転,位置
		D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

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
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);


		//位置を設定
		D3DXMatrixTranslation(&mtxTranslate,
							m_pos.x,
							m_pos.y,
							m_pos.z);

		//位置のセット
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
							&mtxTranslate);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

		//マテリアル取得
		pDevice->GetMaterial(&matDef);

		//トゥーンシェーダー開始
		CToonShader *pToon = (CToonShader*)CShaderManager::GetInstance()->GetShader(TOON_SHADER);
		pToon->SetMatrix(&m_mtxWorld);
		pToon->Begin();

		//マテリアルとテクスチャの設定
		for (int nCntMat = 0; nCntMat<(int)m_nNumMatModel; nCntMat++)
		{
			pToon->SetColor(D3DXVECTOR4(m_pD3DXMat[nCntMat].MatD3D.Diffuse.r,
										m_pD3DXMat[nCntMat].MatD3D.Diffuse.g,
										m_pD3DXMat[nCntMat].MatD3D.Diffuse.b,
										m_pD3DXMat[nCntMat].MatD3D.Diffuse.a));
			pDevice->SetTexture(0, m_pD3DTexture);
			pToon->BeginPass(0);
			m_pD3DXMeshModel->DrawSubset(nCntMat);
			pToon->EndPass();
		}
		pToon->End();
		//マテリアルセット
		pDevice->SetMaterial(&matDef);
	}
}
//=============================================================================
//インスタンス生成
//=============================================================================
void CObject::Create(char *pFileName,
					D3DXVECTOR3 pos,
					D3DXVECTOR3 rot)
{
	//インスタンス生成して初期化
	CObject *pObject = new CObject();
	pObject->Init(pFileName, pos, rot);
}
//EOF