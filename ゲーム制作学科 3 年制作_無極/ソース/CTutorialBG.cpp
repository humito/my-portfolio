//=============================================================================
//チュートリアル背景処理[CTutorial.cpp]
//Author:HUMITO KIMURA
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CTutorialBG.h"
#include "renderer.h"
#include "manager.h"
#include "CInputKeyboard.h"
#include "CInputJoystick.h"
#include "CFade.h"
#include "CSound.h"
#include "main.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define PAGE_MAX (4)		//最大ページ数
#define NEXT_PAGE (1)		//次のページ
#define GAME_PAD_PAGE (2)	//ゲームパッドの操作方法があるページ

//=============================================================================
//コンストラクタ
//=============================================================================
CTutorialBG::CTutorialBG()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CTutorialBG::~CTutorialBG()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CTutorialBG::Init()
{
	//ページ初期化
	m_nPage=0;

	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	///////////////////////////
	//	頂点バッファの設定	//
	/////////////////////////

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

	//頂点バッファロック
	m_pD3DVtxBuff->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の代入
	pVtx[0].vtx=D3DXVECTOR3(0.0f,SCREEN_HEIGHT,0.0f);
	pVtx[1].vtx=D3DXVECTOR3(0.0f,0.0f,0.0f);
	pVtx[2].vtx=D3DXVECTOR3(SCREEN_WIDTH,SCREEN_HEIGHT,0.0f);
	pVtx[3].vtx=D3DXVECTOR3(SCREEN_WIDTH,0.0f,0.0f);

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
								"data/TEXTURE/tutorialBG000.png",
								&m_pD3DTex);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CTutorialBG::Uninit()
{
	//自身の終了
	CScene2D::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CTutorialBG::Update()
{
	//ページ変更フラグ
	bool bChange=false;

	//エンターキーが押されたらゲームへ遷移
	if((CInputKeyboard::GetKeyTrigger(DIK_RETURN)	||
		CInputJoystick::GetPadTrigger(BUTTON_START))&&
		CFade::GetFade()==FADE_NONE)
	{
		//開始効果音再生
		CSound::PlaySoundA(SOUND_LABEL_SE_START);

		//ゲーム画面へフェードイン
		CFade::SetFade(FADE_IN,MODE_GAME);
	}

	//次ページへ切替
	if(	CInputKeyboard::GetKeyTrigger(DIK_D)	||
		CInputJoystick::GetPadTrigger(ARROW_RIGHT))
	{
		m_nPage++;
		bChange=true;
	}

	//前ページへ切替
	if(	CInputKeyboard::GetKeyTrigger(DIK_A)	||
		CInputJoystick::GetPadTrigger(ARROW_LEFT))
	{
		m_nPage--;
		bChange=true;
	}

	//ページ数が負数にならないようにする
	if(m_nPage<0)
	{
		m_nPage+=PAGE_MAX;
	}

	//ページ数が最大値までの値になるようにする
	m_nPage=m_nPage%PAGE_MAX;

	//ページが変更されたらテクスチャを張り替える
	if(bChange)
	{
		//レンダラー情報取得
		CRenderer *pRenderer=CManager::GetRenderer();
		//デバイス取得
		LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

		//画像ファイル名の文字列
		char *pFileName;

		//ゲームの目的
		if(m_nPage==0)
		{
			pFileName="data/TEXTURE/tutorialBG000.png";
		}//操作方法(キーボード)
		else if(m_nPage==NEXT_PAGE)
		{
			pFileName="data/TEXTURE/tutorialBG001.png";
		}//操作方法(ゲームパッド)
		else if(m_nPage==GAME_PAD_PAGE)
		{
			pFileName="data/TEXTURE/tutorialBG003.png";
		}//画面の見方
		else
		{
			pFileName="data/TEXTURE/tutorialBG002.png";
		}

		//テクスチャの読み込み
		D3DXCreateTextureFromFile(	pDevice,
									pFileName,
									&m_pD3DTex);
	}
}
//=============================================================================
//描画
//=============================================================================
void CTutorialBG::Draw()
{
	//レンダラーゲット
	CRenderer *pRenderer=CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

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
//インスタンス生成
//=============================================================================
void CTutorialBG::Create()
{
	//チュートリアル背景のインスタンスを生成
	CTutorialBG *pTutorialBG=new CTutorialBG();

	//初期化
	pTutorialBG->Init();
}
//EOF