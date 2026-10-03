//=============================================================================
//キャラクター表情処理[CFace.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CFace.h"
#include "manager.h"
#include "renderer.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define FACE_COLOR_MAX (3)		//顔の色数
#define WAIT_COUNT_MAX (20)		//待ち時間最大値
#define WHITE_COLOR_LIMIT (5000)//白の状態を表示するまでのアイテム数限度
#define WHITE_COLOR_NUM (0.0f)	//白の番号
#define PINK_COLOR_NUM (1.0f)	//ピンクの番号
#define PINK_COLOR_LIMIT (10000)//ピンクの状態を表示するまでのアイテム数限度
#define BROWN_COLOR_NUM (2.0f)	//茶色の番号

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
int CFace::m_nItemNum=0;				//アイテム数
int CFace::m_nWaitCnt=0;				//待ちカウント
EXPRESSION CFace::m_face=NORMAL_FACE;	//表情

//=============================================================================
//コンストラクタ
//=============================================================================
CFace::CFace()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CFace::~CFace()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CFace::Init()
{
	m_nItemNum=0;		//アイテム数
	m_face=NORMAL_FACE;	//表情
	m_nWaitCnt=0;		//待ちカウント

	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	///////////////////////////////////
	//		頂点バッファ生成		//
	/////////////////////////////////

	//頂点バッファの生成
	if(FAILED(pDevice->CreateVertexBuffer
				(sizeof(VERTEX_2D)*4,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_2D,
				D3DPOOL_MANAGED,
				&m_pD3DVtxBuff,
				NULL)))
	{
		return E_FAIL;
	}

	//2D頂点情報ポインタ
	VERTEX_2D *pVtx;

	//頂点バッファの設定
	m_pD3DVtxBuff->Lock(0,0,(void**)&pVtx,0);

	//頂点座標
	pVtx[0].vtx=D3DXVECTOR3(0.0f,860.0f,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(0.0f,660.0f,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(200.0f,860.0f,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(200.0f,660.0f,0.0f);

	//幅
	pVtx[0].rhw=1.0f;
	pVtx[1].rhw=1.0f;
	pVtx[2].rhw=1.0f;
	pVtx[3].rhw=1.0f;

	//反射光
	pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,255);
	pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

	//頂点情報設定終了
	m_pD3DVtxBuff->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								"data/TEXTURE/face.png",
								&m_pD3DTex);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CFace::Uninit()
{
	//自身の終了
	CScene2D::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CFace::Update()
{
	//驚いた表情を一定時間描画させるためカウント処理をする
	if(m_face==SURPRISE_FACE)
	{
		m_nWaitCnt++;
	}
}
//=============================================================================
//描画
//=============================================================================
void CFace::Draw()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//頂点情報の変更
	ChangeBuffer();

	//頂点バッファのバインド
	pDevice->SetStreamSource(0,m_pD3DVtxBuff,0,sizeof(VERTEX_2D));

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0,m_pD3DTex);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
							0,//ポリゴンの数
							2);
}
//=============================================================================
//頂点情報の変更
//=============================================================================
void CFace::ChangeBuffer()
{
	//テクスチャ座標U移動量
	float fTexMoveU=1.0f/EXPRESSION_MAX;
	//テクスチャ座標V移動量
	float fTexMoveV=1.0f/FACE_COLOR_MAX;

	//アイテム数による顔の色を設定
	float fColorFace;

	//アイテム数が5000より下なら白の表情
	if(m_nItemNum<WHITE_COLOR_LIMIT)
	{
		fColorFace=WHITE_COLOR_NUM;
	}//アイテム数が5000より上かつ10000より下ならピンクの表情
	else if(m_nItemNum>WHITE_COLOR_LIMIT && m_nItemNum<PINK_COLOR_LIMIT)
	{
		fColorFace=PINK_COLOR_NUM;
	}//アイテム数が10000より上なら茶色の表情
	else
	{
		fColorFace=BROWN_COLOR_NUM;
	}

	//2D頂点情報ポインタ
	VERTEX_2D *pVtx;

	//頂点バッファの設定
	m_pD3DVtxBuff->Lock(0,0,(void**)&pVtx,0);

	//テクスチャ座標変更
	pVtx[0].tex=D3DXVECTOR2(fTexMoveU*m_face,fTexMoveV+fTexMoveV*fColorFace);
	pVtx[1].tex=D3DXVECTOR2(fTexMoveU*m_face,fTexMoveV*fColorFace);
	pVtx[2].tex=D3DXVECTOR2(fTexMoveU+fTexMoveU*m_face,fTexMoveV+fTexMoveV*fColorFace);
	pVtx[3].tex=D3DXVECTOR2(fTexMoveU+fTexMoveU*m_face,fTexMoveV*fColorFace);

	//頂点情報設定終了
	m_pD3DVtxBuff->Unlock();
}
//=============================================================================
//表情のセット
//=============================================================================
void CFace::SetFace(EXPRESSION face)
{
	/*条件：驚いた表情以外の顔か驚いた表情で待ちカウントが一定以上か
			やられ顔でかつ引数が通常の顔の場合のみセットさせる*/
	if(	m_face!=face				&&
		m_face==NORMAL_FACE			||
		m_nWaitCnt>WAIT_COUNT_MAX	||
		(m_face==SURPRISE_FACE		&&
		(face==BEATEN_FACE			||
		face==HAPPY_FACE))			||
		((m_face==BEATEN_FACE		||
		m_face==HAPPY_FACE)			&&
		face!=SURPRISE_FACE))
	{
		//表情をセット
		m_face=face;
		//カウントリセット
		m_nWaitCnt=0;
	}
}
//=============================================================================
//アイテム数のセット
//=============================================================================
void CFace::SetItemNum(int num)
{
	m_nItemNum=num;
}
//=============================================================================
//インスタンス生成
//=============================================================================
void CFace::Create()
{
	//表情インスタンス生成
	CFace *pFace=new CFace();

	//初期化
	pFace->Init();
}
//EOF