//=============================================================================
//その他オブジェクト(木)処理[CTree.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CTree.h"
#include "renderer.h"
#include "manager.h"
#include "Ccamera.h"
//=============================================================================
//コンストラクタ
//=============================================================================
CTree::CTree()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CTree::~CTree()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CTree::Init(char *FileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot,float width,float height)
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//ポリゴンの設定
	m_pos=pos;							//座標
	m_rot=rot;							//角度
	m_bDisp=false;						//表示フラグ

	///////////////////////////////////
	//		頂点バッファの生成		//
	/////////////////////////////////

	if(FAILED(	pDevice->CreateVertexBuffer
				(sizeof(VERTEX_3D)*4,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&m_pD3DVtxBuffBill,
				NULL)))
	{
		return E_FAIL;
	}

	//頂点バッファのポインタ
	VERTEX_3D *pVtx;

	///////////////////////////////////
	//		頂点バッファの設定		//
	/////////////////////////////////

	//頂点バッファ設定開始
	m_pD3DVtxBuffBill->Lock(0,0,(void**)&pVtx,0);

	//頂点座標
	pVtx[0].vtx=D3DXVECTOR3(-(width/2),0.0f,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(-(width/2),height,0.0f);
	pVtx[2].vtx=D3DXVECTOR3((width/2),0.0f,0.0f);
	pVtx[3].vtx=D3DXVECTOR3((width/2),height,0.0f);

	//法線ベクトル
	pVtx[0].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);
	pVtx[1].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);
	pVtx[2].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);
	pVtx[3].nor=D3DXVECTOR3(0.0f,1.0f,-1.0f);

	//光源
	pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

	//テクスチャ座標
	pVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	pVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	pVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	//頂点バッファ設定終了
	m_pD3DVtxBuffBill->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								FileName,
								&m_pD3DTextureBill);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CTree::Uninit()
{
	//ビルボードの終了
	CSceneBillboard::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CTree::Update()
{
	//カメラの取得
	CCamera *pCamera=CManager::GetCamera();

	//当たり判定処理

	///////////////////////////////////////////////
	//		カメラの範囲内との当たり判定		//
	/////////////////////////////////////////////

	//カメラ範囲の頂点座標
	D3DXVECTOR3 cameraVertexPos[4];

	//4頂点数分ループ
	for(int i=0;i<4;i++)
	{
		//カメラ範囲の頂点座標取得
		pCamera->GetVertexPos(&cameraVertexPos[i],i);
	}

	//カメラベクトルの内側である回数
	int nCrossCnt=0;

	//カメラ範囲4頂点数分ループ
	for(int j=0;j<4;j++)
	{
		//カメラ範囲の頂点座標の方向ベクトル
		D3DXVECTOR3 vec1=cameraVertexPos[(j+1)%4]-cameraVertexPos[j];
		//カメラ範囲の頂点座標からオブジェクトへの方向ベクトル
		D3DXVECTOR3 vec2=m_pos-cameraVertexPos[j];

		//オブジェクト頂点座標がカメラ頂点の方向ベクトルの内側なら
		if(vec1.x*vec2.z-vec1.z*vec2.x<0.0f)
		{
			//カウントアップ
			nCrossCnt++;
		}
		else
		{
			//外側ならループから抜ける
			break;
		}
	}

	//カメラ範囲内にオブジェクトの座標がある場合
	if(nCrossCnt>=4)
	{
		//表示フラグtrueにしてループから抜ける
		m_bDisp=true;
	}
	else
	{
		//表示フラグfalseにする
		m_bDisp=false;
	}
}
//=============================================================================
//描画
//=============================================================================
void CTree::Draw()
{
	//表示フラグtrueの場合のみ描画処理をする
	if(m_bDisp==true)
	{
		//レンダラー情報取得
		CRenderer *pRenderer=CManager::GetRenderer();

		//デバイスのゲット
		LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

		//カメラのゲット
		CCamera *pCamera=CManager::GetCamera();

		//アルファテスト
		pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

		//不透明にする値の設定
		pDevice->SetRenderState( D3DRS_ALPHAREF, 0x66 );

		//行列合成用変数
		D3DXMATRIX mtxScl,mtxRot,mtxTranslate,mtxView;

		//カメラ情報取得
		mtxView=pCamera->GetMtxView();

		//ワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_mtxWorld);
		D3DXMatrixInverse(&m_mtxWorld,NULL,&mtxView);

		m_mtxWorld._41=0.0f;
		m_mtxWorld._42=0.0f;
		m_mtxWorld._43=0.0f;

		//スケールの設定
		D3DXMatrixScaling(&mtxScl,1.0f,1.0f,1.0f);

		//スケールを反映
		D3DXMatrixMultiply(	&m_mtxWorld,
							&m_mtxWorld,
							&mtxScl);

		//回転を設定
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
										0,
										0,
										0);

		//回転を反映
		D3DXMatrixMultiply(&m_mtxWorld,&m_mtxWorld,
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

		//３Ｄポリゴンの描画
		pDevice->SetStreamSource(0,m_pD3DVtxBuffBill, 0, sizeof(VERTEX_3D));

		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0,m_pD3DTextureBill);

		//ポリゴンの描画
		pDevice->DrawPrimitive(	D3DPT_TRIANGLESTRIP,
								0,
								2);

		//元に戻す
		pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	}
}
//=============================================================================
//インスタンス生成
//=============================================================================
void CTree::Create(char *FileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot,float width,float height)
{
	//インスタンスのポインタ
	CTree *pTree;

	//インスタンス生成
	pTree=new CTree;

	//初期化
	pTree->Init(FileName,pos,rot,width,height);
}
//EOF