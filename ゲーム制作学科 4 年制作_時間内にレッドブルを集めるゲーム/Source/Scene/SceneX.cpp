//=============================================================================
//Xファイルシーン処理[SceneX.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "SceneX.h"
#include "../manager.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CSceneX::CSceneX(int priority) :CScene(priority)
{
	m_pD3DXMeshModel = NULL;
	m_pD3DTexture1 = NULL;
	m_pD3DTexture2 = NULL;
	m_pD3DXBuffMatModel = NULL;
	m_nNumMatModel = NULL;
}
//=============================================================================
//デストラクタ
//=============================================================================
CSceneX::~CSceneX()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CSceneX::Init()
{
	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CSceneX::Uninit()
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

	//テクスチャ1終了
	if (m_pD3DTexture1 != NULL)
	{
		m_pD3DTexture1->Release();
		m_pD3DTexture1 = NULL;
	}

	//テクスチャ2終了
	if (m_pD3DTexture2 != NULL)
	{
		m_pD3DTexture2->Release();
		m_pD3DTexture2 = NULL;
	}

	//自身を解放
	this->Release();
}
//=============================================================================
//更新
//=============================================================================
void CSceneX::Update()
{
}
//=============================================================================
//描画
//=============================================================================
void CSceneX::Draw()
{
}
//=============================================================================
//モデルの生成
//=============================================================================
HRESULT CSceneX::CreateModel(char *pFileName)
{
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//隣接接続バッファ
	LPD3DXBUFFER pAdjBuff = NULL;

	//Xファイルの読込
	if (FAILED(D3DXLoadMeshFromX(pFileName,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		&pAdjBuff,
		&m_pD3DXBuffMatModel,
		NULL,
		&m_nNumMatModel,
		&m_pD3DXMeshModel)))
	{
		return E_FAIL;
	}

	///////////////////////////////////////
	//		クローンメッシュを作成		//
	/////////////////////////////////////

	//頂点情報配列作成
	D3DVERTEXELEMENT9 vElement[] = {
		//頂点座標
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION, 0 },

		//UV座標
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_TEXCOORD, 0 },

		//法線ベクトル
		{ 0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_NORMAL, 0 },

		//終端
		D3DDECL_END()
	};

	//コンバート（クローンメッシュの作成）
	LPD3DXMESH pTempMesh = NULL;
	m_pD3DXMeshModel->CloneMesh(D3DXMESH_SYSTEMMEM, &vElement[0],
								pDevice, &pTempMesh);

	//メッシュ情報解放
	m_pD3DXMeshModel->Release();
	m_pD3DXMeshModel = NULL;

	//メッシュ情報最適化した状態で生成
	pTempMesh->Optimize(D3DXMESH_MANAGED |
						D3DXMESHOPT_COMPACT |
						D3DXMESHOPT_ATTRSORT |
						D3DXMESHOPT_VERTEXCACHE,
						(DWORD*)pAdjBuff->GetBufferPointer(),
						nullptr, nullptr, nullptr, &m_pD3DXMeshModel);

	//後始末
	pTempMesh->Release();
	pTempMesh = NULL;
	pAdjBuff->Release();
	pAdjBuff = NULL;

	//テクスチャ読込
	D3DXCreateTextureFromFile(pDevice,
							"data/TEXTURE/1x1.tga",
							&m_pD3DTexture1);

	D3DXCreateTextureFromFile(pDevice,
							"data/TEXTURE/1x1.tga",
							&m_pD3DTexture2);

	return S_OK;
}
//=============================================================================
//Xファイルインスタンス生成
//=============================================================================
void CSceneX::Create()
{
	//シーンXファイルポインタ
	CSceneX *pSceneX;

	//シーンXファイル動的確保
	pSceneX = new CSceneX();

	//Xファイル初期化
	pSceneX->Init();
}
//EOF