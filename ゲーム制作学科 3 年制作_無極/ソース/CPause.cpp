//=============================================================================
// ポーズ画面処理 [CPause.cpp]
// Author : 木村　文登
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CPause.h"
#include "manager.h"
#include "renderer.h"
#include "CFade.h"
#include "CGame.h"
#include "CInputKeyboard.h"
#include "CInputJoystick.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define STR_SPACE (1800.0f)					//メニューの行間隔
#define CURSOR_MOVE_PERCENT (0.05f)			//カーソル移動の割合
#define CURSOL_POS_X (310.0f)				//カーソルX座標
#define CURSOL_WIDTH (CURSOL_POS_X+280.0f)	//カーソルの幅
#define CURSOL_POS_Y (370.0f)				//カーソルのY座標
#define CURSOL_HEIGHT (CURSOL_POS_Y+80.0f)	//カーソルの高さ
#define CURSOL_POS_Y_ADD (370.0f)			//カーソルの加算量

//=============================================================================
//コンストラクタ
//=============================================================================
CPause::CPause(int priority):CScene2D(priority)
{
	m_pD3DCursolTex=NULL;		//カーソルテクスチャへのポインタ
	m_pD3DCursolVtxBuff=NULL;	//カーソル頂点バッファへのポインタ
}

//=============================================================================
//デストラクタ
//=============================================================================
CPause::~CPause()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CPause::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//選択項目初期化
	m_selectMenu=PAUSE_CLOSE;

	///////////////////////////
	//	頂点バッファの設定	//
	/////////////////////////

	//ポーズ画面の頂点バッファの生成
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

	//ポーズ画面の頂点バッファロック
	m_pD3DVtxBuff->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の代入
	pVtx[0].vtx=D3DXVECTOR3(300.0f,700.0f,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(300.0f,300.0f,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(600.0f,700.0f,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(600.0f,300.0f,0.0f);

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

	//テクスチャ
	pVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	pVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	pVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	//頂点設定の解除
	m_pD3DVtxBuff->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								"data/TEXTURE/pause000.jpg",
								&m_pD3DTex);


	//カーソル頂点バッファの生成
	if(FAILED(pDevice->CreateVertexBuffer
				(sizeof(VERTEX_2D)*4,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_2D,
				D3DPOOL_MANAGED,
				&m_pD3DCursolVtxBuff,
				NULL)))
	{
		return E_FAIL;
	}

	//カーソルのY座標初期化
	m_fCurPosY=0.0f;
	//目的のカーソルのY座標初期化
	m_fDestCurPosY=0.0f;

	//カーソル頂点バッファロック
	m_pD3DCursolVtxBuff->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の代入
	pVtx[0].vtx=D3DXVECTOR3(310.0f,m_fCurPosY+450.0f,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(310.0f,m_fCurPosY+370.0f,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(590.0f,m_fCurPosY+450.0f,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(590.0f,m_fCurPosY+370.0f,0.0f);

	//幅
	pVtx[0].rhw=1.0f;
	pVtx[1].rhw=1.0f;
	pVtx[2].rhw=1.0f;
	pVtx[3].rhw=1.0f;

	//反射光
	pVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,125);
	pVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,125);
	pVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,125);
	pVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,125);

	//テクスチャ
	pVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	pVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	pVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	pVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	//頂点設定の解除
	m_pD3DCursolVtxBuff->Unlock();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(	pDevice,
								"data/TEXTURE/cursor000.jpg",
								&m_pD3DCursolTex);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CPause::Uninit()
{
	//カーソル頂点バッファの解放
	if(m_pD3DCursolVtxBuff!=NULL)
	{
		m_pD3DCursolVtxBuff->Release();	//解放
		m_pD3DCursolVtxBuff=NULL;		//NULLセット
	}
	
	//カーソルテクスチャの開放
	if(m_pD3DCursolTex!=NULL)
	{
		m_pD3DCursolTex->Release();	//解放
		m_pD3DCursolTex=NULL;		//NULLセット
	}

	//自身の終了
	CScene2D::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CPause::Update()
{
	///////////////////////////
	//		入力処理		//
	/////////////////////////

	//下
	if(	CInputKeyboard::GetKeyTrigger(DIK_S)		||
		CInputJoystick::GetPadTrigger(ARROW_DOWN)	||
		CInputJoystick::GetPadTrigger(STICK_L_DOWN))
	{
		m_selectMenu=(PAUSE_MENU)((m_selectMenu+1)%PAUSE_MAX);
	}

	//上
	if(	CInputKeyboard::GetKeyTrigger(DIK_W)	||
		CInputJoystick::GetPadTrigger(ARROW_UP)	||
		CInputJoystick::GetPadTrigger(STICK_L_UP))
	{
		m_selectMenu=(PAUSE_MENU)((m_selectMenu-1)%PAUSE_MAX);
	}

	//負の数の場合は正にする
	if(m_selectMenu<0)
	{
		m_selectMenu=(PAUSE_MENU)(m_selectMenu+PAUSE_MAX);
	}

	//エンターキーで項目の決定
	if(	CInputKeyboard::GetKeyTrigger(DIK_RETURN) ||
		CInputJoystick::GetPadTrigger(BUTTON_1))
	{
		//選択した項目によって処理を分ける
		switch(m_selectMenu)
		{
			//ポーズ画面を閉じる
			case PAUSE_CLOSE:
				//ポーズフラグfalseセット
				CGame::SetPauseFlag(false);
			break;

			//ゲームリセット
			case PAUSE_RESET:
				//ポーズフラグfalseセット
				CGame::SetPauseFlag(false);
				//ゲームシーンへ遷移し、フェードセット
				CFade::SetFade(FADE_IN,MODE_GAME);
			break;
		
			//タイトルへ移動
			case PAUSE_TITLE:
				//ポーズフラグfalseセット
				CGame::SetPauseFlag(false);
				//タイトルシーンへ遷移し、フェードセット
				CFade::SetFade(FADE_IN,MODE_TITLE);
			break;
		}
	}
	//目的カーソルY座標を設定
	m_fDestCurPosY=(m_selectMenu*STR_SPACE)+CURSOL_POS_Y_ADD;

	//カーソルを目的の位置まで移動
	m_fCurPosY=(m_fDestCurPosY-m_fCurPosY)*CURSOR_MOVE_PERCENT;
}
//=============================================================================
//描画
//=============================================================================
void CPause::Draw()
{
	//レンダラーゲット
	CRenderer *pRenderer=CManager::GetRenderer();

	//頂点バッファ
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	///////////////////////////////////
	//		ポーズ画面の描画		//
	/////////////////////////////////
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

	///////////////////////////////
	//		カーソルの描画		//
	/////////////////////////////

	//頂点情報の変更
	ChangeBuffer();

	//頂点バッファのバインド
	pDevice->SetStreamSource(0,m_pD3DCursolVtxBuff,0,sizeof(VERTEX_2D));

	//頂点フォーマットのセット
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0,m_pD3DCursolTex);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
							0,//ポリゴンの数
							2);
}
//=============================================================================
//頂点情報の変更
//=============================================================================
void CPause::ChangeBuffer()
{
	//2D頂点情報ポインタ
	VERTEX_2D *pVtx;

	//カーソル頂点バッファロック
	m_pD3DCursolVtxBuff->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の代入
	pVtx[0].vtx=D3DXVECTOR3(CURSOL_POS_X,m_fCurPosY+CURSOL_HEIGHT,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(CURSOL_POS_X,m_fCurPosY+CURSOL_POS_Y,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(CURSOL_WIDTH,m_fCurPosY+CURSOL_HEIGHT,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(CURSOL_WIDTH,m_fCurPosY+CURSOL_POS_Y,0.0f);

	//頂点設定の解除
	m_pD3DCursolVtxBuff->Unlock();
}
//=============================================================================
//インスタンス生成
//=============================================================================
CPause *CPause::Create()
{
	//ポーズインスタンス生成
	CPause *pPause=new CPause();

	//初期化
	pPause->Init();

	//インスタンスを返す
	return pPause;
}
//EOF