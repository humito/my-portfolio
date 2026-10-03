//=============================================================================
//階層構造モデル処理[Model.cpp]
//Author:HUMITO KIMURA
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Model.h"
#include "renderer.h"
#include "manager.h"
#include "ToonShader.h"
#include "ShaderManager.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CModel::CModel()
{
	m_pD3DXMeshModel = NULL;
	m_pD3DXBuffMatModel = NULL;
	m_nNumMatModel = NULL;
	m_pParent = NULL;
}
//=============================================================================
//デストラクタ
//=============================================================================
CModel::~CModel()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CModel::Init(char *pFileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//座標角度をセット
	m_pos = pos;
	m_rot = rot;

	//Xファイルのロード
	if (FAILED(D3DXLoadMeshFromX(	pFileName,
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
void CModel::Uninit()
{
	//マテリアル情報の終了
	if (m_pD3DXBuffMatModel != NULL)
	{
		m_pD3DXBuffMatModel->Release();
		m_pD3DXBuffMatModel = NULL;
	}

	//メッシュ情報の終了
	if (m_pD3DXMeshModel != NULL)
	{
		m_pD3DXMeshModel->Release();
		m_pD3DXMeshModel = NULL;
	}

	//テクスチャ解放
	if (m_pD3DTexture != NULL)
	{
		m_pD3DTexture->Release();
		m_pD3DTexture = NULL;
	}
}
//=============================================================================
//描画
//=============================================================================
void CModel::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//サイズ,回転,位置
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//親ポインタのマトリックス
	D3DXMATRIX matrixParent;

	//親ポインタがある場合
	if (m_pParent != NULL)
	{
		//親ポインタからマトリックスを取得
		matrixParent = m_pParent->GetMatrix();
	}
	else
	{
		//設定したワールドマトリックスを取得
		pDevice->GetTransform(D3DTS_WORLD, &matrixParent);
	}

	//プロジェクションマトリックスを反映
	D3DXMatrixIdentity(&m_Matrix);

	//サイズを設定
	D3DXMatrixScaling(	&mtxScl,
						1.0f,
						1.0f,
						1.0f);

	//サイズを反映
	D3DXMatrixMultiply(	&m_Matrix,
						&m_Matrix,
						&mtxScl);

	//回転を設定
	D3DXMatrixRotationYawPitchRoll(	&mtxRot,
									m_rot.y,
									m_rot.x,
									m_rot.z);

	//回転を反映
	D3DXMatrixMultiply(	&m_Matrix,
						&m_Matrix,
						&mtxRot);

	//位置を設定
	D3DXMatrixTranslation(	&mtxTranslate,
							m_pos.x,
							m_pos.y,
							m_pos.z);

	//位置を反映
	D3DXMatrixMultiply(	&m_Matrix,
						&m_Matrix,
						&mtxTranslate);

	//親マトリックスと合成
	D3DXMatrixMultiply(&m_Matrix, &m_Matrix, &matrixParent);

	//ワールドマトリックスのセット
	pDevice->SetTransform(D3DTS_WORLD, &m_Matrix);

	//トゥーンシェーダー開始
	CToonShader *pToon = (CToonShader*)CShaderManager::GetInstance()->GetShader(TOON_SHADER);
	pToon->SetMatrix(&m_Matrix);

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

}
//=============================================================================
//座標取得
//=============================================================================
D3DXVECTOR3 CModel::GetPos()
{
	return m_pos;
}
//=============================================================================
//角度取得
//=============================================================================
D3DXVECTOR3 CModel::GetRot()
{
	return m_rot;
}
//=============================================================================
//角度セット
//=============================================================================
void CModel::SetRot(D3DXVECTOR3 rot)
{
	m_rot = rot;
}
//=============================================================================
//座標セット
//=============================================================================
void CModel::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}
//=============================================================================
//マトリックスの取得
//=============================================================================
D3DXMATRIX CModel::GetMatrix()
{
	return m_Matrix;
}
//=============================================================================
//親ポインタのセット
//=============================================================================
void CModel::SetParent(CModel *pParent)
{
	m_pParent = pParent;
}
//=============================================================================
//インスタンス生成
//=============================================================================
CModel *CModel::Create(char *pFileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//インスタンス生成
	CModel *pModel = new CModel();

	//初期化
	pModel->Init(pFileName, pos, rot);

	//インスタンスを返す
	return pModel;
}
//EOF