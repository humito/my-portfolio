//=============================================================================
//当たり判定用球体表示処理[CHitCheckSphere.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "HitCheckSphere.h"
#include "manager.h"
#include "renderer.h"
#include "CInputKeyboard.h"
//*****************************************************************************
// 定数定義
//*****************************************************************************
#define SPHERE_NUM_X (8)		//球体のXブロック数
#define SPHERE_NUM_Y (8)		//球体のYブロック数
#define NEXT_VERTEX (1)			//次の頂点
#define PREV_VERTEX (1)			//前の頂点
#define RECT_PRIMITIVE (2)		//四角形ポリゴンのプリミティブ数
#define BUFF_BLOCK (4)			//１つのブロックにつく頂点数
#define REVOLUTION (D3DX_PI*2)	//360°回転
#define HALF_PI (D3DX_PI/2)		//90°回転
//=============================================================================
//コンストラクタ
//=============================================================================
CHitCheckSphere::CHitCheckSphere()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CHitCheckSphere::~CHitCheckSphere()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CHitCheckSphere::Init(float fHalfSize)
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//表示フラグOFF
	m_bDispSphere=false;

	//頂点を横にずらす角度(360度/Xブロック数)
	m_fSideRot=REVOLUTION/SPHERE_NUM_X;

	//頂点を縦にずらす角度(180度/Yブロック数)
	m_fLengthRot=(D3DX_PI)/SPHERE_NUM_Y;

	//総ポリゴン数
	m_nNumPolygon=((SPHERE_NUM_X*SPHERE_NUM_Y)*RECT_PRIMITIVE)+BUFF_BLOCK*(SPHERE_NUM_Y-PREV_VERTEX);

	//総頂点数
	m_nNumVertex=(SPHERE_NUM_X+NEXT_VERTEX)*RECT_PRIMITIVE*SPHERE_NUM_Y+RECT_PRIMITIVE*(SPHERE_NUM_Y-PREV_VERTEX);

	//インデックス数
	m_nNumVertexIndex=(SPHERE_NUM_X+NEXT_VERTEX)*(SPHERE_NUM_Y+NEXT_VERTEX);

	//ポリゴンの設定
	m_pos=D3DXVECTOR3(0.0f,0.0f,0.0f);	//座標
	m_rot=D3DXVECTOR3(0.0f,0.0f,0.0f);	//回転
	
	////////////////////////////////////////////////////////////////////////////
	//							頂点バッファの生成							 //
	////////////////////////////////////////////////////////////////////////////

	if(FAILED(	pDevice->CreateVertexBuffer
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
	m_pD3DVtxBuff->Lock(0,0,(void**)&pVtx,0);

	///////////////////////
	//	頂点設定ループ	//
	/////////////////////

	//Yループ
	for(int y=0;y<SPHERE_NUM_Y+NEXT_VERTEX;y++)
	{
		//Xループ
		for(int x=0;x<SPHERE_NUM_X+NEXT_VERTEX;x++)
		{
			//カウンタ事に頂点座標をずらす(小数点を切り捨てする)
			int nRotY=(int)(sinf(HALF_PI-(m_fLengthRot*y))*fHalfSize);
			int nLength=(int)(cosf(HALF_PI-(m_fLengthRot*y))*fHalfSize);
			int nRotX=(int)(cosf(m_fSideRot*x)*nLength);
			int nRotZ=(int)(sinf(m_fSideRot*x)*nLength);

			//上頂点から設定
			pVtx[y*(SPHERE_NUM_X+NEXT_VERTEX)+x].vtx=D3DXVECTOR3((float)nRotX,	//X
																(float)nRotY,	//Y
																(float)nRotZ	//Z
																);
		}
	}

	///////////////////////////////////////////
	//	法線ベクトル＆反射光　設定ループ	//
	/////////////////////////////////////////
	for(int i=0;i<m_nNumVertexIndex;i++)
	{
		pVtx[i].nor=D3DXVECTOR3(0.0f,1.0f,1.0f);
		pVtx[i].diffuse=D3DCOLOR_RGBA(0,255,0,255);
	}

	//頂点バッファの設定終了
	m_pD3DVtxBuff->Unlock();

	
	////////////////////////////////////////////////////////////////////////////
	//					インデックスバッファの生成				              //
	///////////////////////////////////////////////////////////////////////////

	//インデックスバッファの生成
	if(FAILED(	pDevice->CreateIndexBuffer
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
	m_pD3DIndexBuff->Lock(0,0,(void**)&pIndex,0);

	//インデックスバッファのカウント
	int cnt=0;

	//Yループ
	for(int y=0;y<SPHERE_NUM_Y;y++)
	{
		//Xループ
		for(int x=0;x<SPHERE_NUM_X+NEXT_VERTEX;x++)
		{
			//下側
			pIndex[cnt]=(x+(SPHERE_NUM_X+1))+(y*(SPHERE_NUM_Y+1));
			cnt++;

			//上側
			pIndex[cnt]=x+(y*(SPHERE_NUM_Y+1));
			cnt++;

			//縮退ポリゴンに合わせて番号を付ける
			if(y!=SPHERE_NUM_Y-1 && x==SPHERE_NUM_X)
			{
				//上
				pIndex[cnt]=x+(y*(SPHERE_NUM_Y+1));
				cnt++;

				//下
				pIndex[cnt]=(SPHERE_NUM_X+1)+((y+1)*(SPHERE_NUM_Y+1));
				cnt++;
			}

		}
	}

	//インデックスバッファの設定終了
	m_pD3DIndexBuff->Unlock();

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CHitCheckSphere::Uninit()
{
	//自身の終了
	CScene3D::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CHitCheckSphere::Update()
{
	//１キーが押されたら
	if(CInputKeyboard::GetKeyTrigger(DIK_1))
	{
		//表示フラグを切り替える
		if(m_bDispSphere)
		{
			//表示フラグOFF
			m_bDispSphere=false;
		}
		else
		{
			//表示フラグON
			m_bDispSphere=true;
		}
	}
}
//=============================================================================
//描画
//=============================================================================
void CHitCheckSphere::Draw()
{
	//表示フラグONのみ描画処理実行
	if(m_bDispSphere)
	{
		//レンダラー情報取得
		CRenderer *pRenderer=CManager::GetRenderer();

		//デバイスのゲット
		LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

		///////////////////////////
		//		描画前設定		//
		/////////////////////////

		//ワイヤーフレーム
		pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);

		//ライトOFF
		pDevice->LightEnable(0,FALSE);

		//ライティングOFF
		pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

		//描画用変数 (マトリックス)
		D3DXMATRIX mtxScl,mtxRot,mtxTranslate;

		D3DXMatrixIdentity(&m_mtxWorld);

		D3DXMatrixScaling(&mtxScl,1.0f,1.0f,1.0f);

		D3DXMatrixMultiply(	&m_mtxWorld,
							&m_mtxWorld,
							&mtxScl);

		//回転を設定
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
										m_rot.y,
										m_rot.x,
										m_rot.z);

		//回転を反映
		D3DXMatrixMultiply(	&m_mtxWorld,&m_mtxWorld,
							&mtxRot);


		//位置を反映
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
		pDevice->SetStreamSource(0,m_pD3DVtxBuff, 0, sizeof(VERTEX_3D));

		//インデックスをバインド
		pDevice->SetIndices(m_pD3DIndexBuff);

		//頂点フォーマットのセット
		pDevice->SetFVF(FVF_VERTEX_3D);

		//ポリゴンの描画(インデックス)
		pDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLESTRIP,
										0,
										0,
										m_nNumVertexIndex,
										0,
										m_nNumPolygon);
									

		///////////////////////
		//		後始末		//
		/////////////////////

		//ワイヤーフレームを元に戻す
		pDevice->SetRenderState(D3DRS_FILLMODE,NULL);

		//ライトON
		pDevice->LightEnable(0,TRUE);

		//ライティングON
		pDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
	}
}
//=============================================================================
//座標セット
//=============================================================================
void CHitCheckSphere::SetPos(D3DXVECTOR3 pos)
{
	m_pos=pos;
}
//=============================================================================
//インスタンス生成
//=============================================================================
CHitCheckSphere *CHitCheckSphere::Create(float fHalfSize)
{
	//インスタンスを動的確保
	CHitCheckSphere *pHitCheckSphere=new CHitCheckSphere();
	//初期化
	pHitCheckSphere->Init(fHalfSize);
	//インスタンスを返す
	return pHitCheckSphere;
}
//EOF