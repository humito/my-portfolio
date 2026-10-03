//=============================================================================
//その他オブジェクト処理[CObject.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//定数定義
//*****************************************************************************
#define PRIVATE_HOUSE1_SIZEX (250.0f)	//民家1のサイズX
#define PRIVATE_HOUSE1_SIZEZ (190.0f)	//民家1のサイズZ
#define PRIVATE_HOUSE0_SIZE (300.0f)	//民家0のサイズ
#define TOWER_SIZE (100.0f)				//塔のサイズ
#define SHRINE_SIZE (300.0f)			//神社のサイズ
#define OBJECT_HEIGHT (650.0f)			//オブジェクトの高さ
#define REST_HOUSE_SIZE (100.0f)		//休憩所のサイズ
#define REST_HOUSE_HEIGHT (120.0f)		//休憩所の高さ
#define CASTLE_SIZE (450.0f)			//城のサイズ

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CObject.h"
#include "renderer.h"
#include "manager.h"
#include "Ccamera.h"
#include "CMeshField.h"
#include "HitCheck.h"
#include "HitCheckSphere.h"
#include "CToonShader.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CObject::CObject()
{
	//オブジェクト代入
	m_objType=OBJECT_TYPE_OBJECT;
	//当たり判定用球体NULLセット
	m_pSphere=NULL;
}
//=============================================================================
//デストラクタ
//=============================================================================
CObject::~CObject()
{
}

//=============================================================================
//初期化
//=============================================================================
HRESULT CObject::Init(char *FileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot)
{
	//表示フラグfalse
	m_bDisp=true;

	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//ポリゴンの設定
	m_pos=pos;
	m_rot=rot;
	m_scl=D3DXVECTOR3(1.0f,1.0f,1.0f);

	//Xファイルのロード
	if(FAILED(D3DXLoadMeshFromX(	FileName,
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

	///////////////////////////////////////////////////////
	//		オブジェクトの種類によってサイズを設定		//
	/////////////////////////////////////////////////////

	//休憩所以外は共通の高さで設定
	m_fSizeY=OBJECT_HEIGHT;

	//民家１
	if(strcmp(FileName,"data/MODEL/minka01.x")==0)
	{
		m_fSizeX=PRIVATE_HOUSE1_SIZEX;
		m_fSizeZ=PRIVATE_HOUSE1_SIZEZ;
	}//民家2
	else if(strcmp(FileName,"data/MODEL/minka00.x")==0)
	{
		m_fSizeX=PRIVATE_HOUSE0_SIZE;
		m_fSizeZ=PRIVATE_HOUSE0_SIZE;
	}//塔
	else if(strcmp(FileName,"data/MODEL/sir.x")==0)
	{
		m_fSizeX=TOWER_SIZE;
		m_fSizeZ=TOWER_SIZE;
	}//神社
	else if(strcmp(FileName,"data/MODEL/yume.x")==0)
	{
		m_fSizeX=SHRINE_SIZE;
		m_fSizeZ=SHRINE_SIZE;
	}//休憩所
	else if(strcmp(FileName,"data/MODEL/ste.x")==0)
	{
		m_fSizeX=REST_HOUSE_SIZE;
		m_fSizeY=REST_HOUSE_HEIGHT;
		m_fSizeZ=REST_HOUSE_SIZE;
	}//城
	else if(strcmp(FileName,"data/MODEL/siro.x")==0)
	{
		m_fSizeX=CASTLE_SIZE;
		m_fSizeZ=CASTLE_SIZE;
	}
	else if(strcmp(FileName,"data/MODEL/tori.x")==0)
	{
		m_fSizeX=0;
		m_fSizeY=0;
		m_fSizeZ=0;
	}

//デバッグ
#ifdef _DEBUG
	//当たり判定用球体ポリゴン
	m_pSphere=CHitCheckSphere::Create(m_fSizeX);
#endif

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CObject::Uninit()
{
	//終了処理
	CSceneX::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CObject::Update()
{
	///////////////////////////////////////////////
	//		カメラの範囲内との当たり判定		//
	/////////////////////////////////////////////

	//カメラの取得
	CCamera *pCamera=CManager::GetCamera();

	//カメラ範囲の頂点座標
	D3DXVECTOR3 cameraVertexPos[CAMERA_RECT_VERTEX_MAX];

	//4頂点数分ループ
	for(int i=0;i<CAMERA_RECT_VERTEX_MAX;i++)
	{
		//カメラ範囲の頂点座標取得
		pCamera->GetVertexPos(&cameraVertexPos[i],i);
	}

	//オブジェクト4頂点数分ループ
	for(int i=0;i<OBJECT_RECT_VERTEX_MAX;i++)
	{
		//カメラベクトルの内側である回数
		int nCrossCnt=0;

		//オブジェクトの横,奥の幅含めた(短形)各頂点座標を求める
		D3DXVECTOR3 objVertexPos=D3DXVECTOR3(	m_pos.x+(-m_fSizeX+((m_fSizeX*2)*(i/2))),
												0.0f,
												m_pos.z+(-m_fSizeZ+((m_fSizeZ*2)*(i%2)))
											);
		//カメラ範囲4頂点数分ループ
		for(int j=0;j<CAMERA_RECT_VERTEX_MAX;j++)
		{
			//カメラ範囲の頂点座標の方向ベクトル
			D3DXVECTOR3 vec1=cameraVertexPos[(j+1)%CAMERA_RECT_VERTEX_MAX]-cameraVertexPos[j];
			//カメラ範囲の頂点座標からオブジェクトへの方向ベクトル
			D3DXVECTOR3 vec2=objVertexPos-cameraVertexPos[j];

			//オブジェクト頂点座標がカメラ頂点の方向ベクトルの内側なら
			if(vec1.x*vec2.z-vec1.z*vec2.x<0.0f)
			{
				//カウントアップ
				nCrossCnt++;
			}
			else
			{
				//外側なら表示フラグfalseにしてループから抜ける
				m_bDisp=false;
				break;
			}
		}

		//カメラ範囲内にオブジェクトの座標がある場合
		if(nCrossCnt>=CAMERA_RECT_VERTEX_MAX)
		{
			//表示フラグtrueにしてループから抜ける
			m_bDisp=true;
			break;
		}
	}//オブジェクト頂点座標forループ 終端

//デバッグ
#ifdef _DEBUG
	//当たり判定用球体ポリゴンに自身の座標をセット
	m_pSphere->SetPos(D3DXVECTOR3(m_pos.x,m_pos.y+100.0f,m_pos.z));
#endif
}
//=============================================================================
//描画
//=============================================================================
void CObject::Draw()
{
	//表示フラグtrueの場合のみ描画処理をする
	if(m_bDisp==true)
	{
		//レンダラー情報取得
		CRenderer *pRenderer=CManager::GetRenderer();

		//デバイスのゲット
		LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

		D3DXMATERIAL *pD3DXMat;
		D3DMATERIAL9 matDef;
		D3DXMATRIX mtxScl,mtxRot,mtxTranslate;//サイズ,回転,位置
	
		//プロジェクションマトリックスを反映
		D3DXMatrixIdentity(&m_mtxWorld);

		//スケールを設定
		D3DXMatrixScaling(	&mtxScl,
							m_scl.x,
							m_scl.y,
							m_scl.z);

		//スケールを反映
		D3DXMatrixMultiply(	&m_mtxWorld,
							&m_mtxWorld,
							&mtxScl);
		//回転を設定
		D3DXMatrixRotationYawPitchRoll(	&mtxRot,
										m_rot.y,
										m_rot.x,
										m_rot.z);

		//回転を反映
		D3DXMatrixMultiply(	&m_mtxWorld,&m_mtxWorld,
							&mtxRot);


		//位置の設定
		D3DXMatrixTranslation(	&mtxTranslate,
								m_pos.x,
								m_pos.y,
								m_pos.z);

		//位置の反映
		D3DXMatrixMultiply(	&m_mtxWorld,&m_mtxWorld,
							&mtxTranslate);

		//ワールドマトリックスのセット
		pDevice->SetTransform(	D3DTS_WORLD,
								&m_mtxWorld);

		//マテリアルの取得
		pDevice->GetMaterial(&matDef);

		//頂点バッファのポインタをマテリアルに変換
		pD3DXMat=(D3DXMATERIAL*)m_pD3DXBuffMatModel->GetBufferPointer();

		//トゥーンシェーダー開始
		CToonShader::Begin();

		for(int nCntMat=0;nCntMat<(int)m_nNumMatModel;nCntMat++)
		{
			pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);
			m_pD3DXMeshModel->DrawSubset(nCntMat);
		}

		//トゥーンシェーダー終了
		CToonShader::End();

		//マテリアルセット
		pDevice->SetMaterial(&matDef);
	}
}
//=============================================================================
//サイズの取得
//=============================================================================
void CObject::GetSize(float *pfSizeX,float *pfSizeY,float *pfSizeZ)
{
	*pfSizeX=m_fSizeX;
	*pfSizeY=m_fSizeY;
	*pfSizeZ=m_fSizeZ;
}
//=============================================================================
//その他オブジェクトインスタンス生成
//=============================================================================
void CObject::Create(char *FileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot)
{
	//オブジェクトポインタ
	CObject *pObject;

	//オブジェクトインスタンス生成
	pObject=new CObject();

	//オブジェクト初期化
	pObject->Init(FileName,pos,rot);
}
//EOF