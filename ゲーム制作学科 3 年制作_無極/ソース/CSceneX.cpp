//=============================================================================
//Xファイルシーン処理[CsceneX.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CSceneX.h"
//=============================================================================
//コンストラクタ
//=============================================================================
CSceneX::CSceneX(int priority):CScene(priority)
{
	m_pD3DXMeshModel=NULL;
	m_pD3DXBuffMatModel=NULL;
	m_nNumMatModel=NULL;
	m_pFrameRoot=NULL;
	m_pAnimController=NULL;
	m_pAnimSet=NULL;
	m_bLoop=false;
	m_bStop=false;
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
	if(m_pD3DXBuffMatModel!=NULL)
	{
		m_pD3DXBuffMatModel->Release();
		m_pD3DXBuffMatModel=NULL;
	}

	//メッシュ情報の終了
	if(m_pD3DXMeshModel!=NULL)
	{
		m_pD3DXMeshModel->Release();
		m_pD3DXMeshModel=NULL;
	}

	//フレームリスト終了
	if(m_pFrameRoot!=NULL)
	{
		m_alloc.DestroyFrame(m_pFrameRoot);
	}

	//アニメーション制御終了
	if(m_pAnimController!=NULL)
	{
		m_pAnimController->Release();
		m_pAnimController=NULL;
	}

	//アニメーション数解放
	if(m_pAnimSet!=NULL)
	{
		delete[] m_pAnimSet;
		m_pAnimSet=NULL;
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
//Xファイルアニメーション再生
//=============================================================================
void CSceneX::PlayAnimation(void)
{
	//アニメーション制御がある場合のみ再生
	if(m_pAnimController!=NULL)
	{
		//現在のフレーム時間を取得
		double dAnimTime=m_pAnimController->GetTime();
		//フレーム終端時間を取得
		double dPeriodTime=m_pAnimSet[m_nAnimeNum]->GetPeriod();

		//ループなしでかつアニメーションフレームが終端時間を超えた場合
		if(m_bLoop==false && dAnimTime>=dPeriodTime)
		{
			//アニメーションのフレームを止める
			m_dFrameSpeed=0.0;
			m_pAnimController->AdvanceTime(m_dFrameSpeed,NULL);
			//再生終了フラグtrueに変更
			m_bStop=true;
		}
		else
		{
			//アニメーションのフレームを加算
			m_pAnimController->AdvanceTime(m_dFrameSpeed,NULL);
		}
	}
}
//=============================================================================
//Xファイルアニメーション切替
//=============================================================================
void CSceneX::SetAnimation(int nType,double dFrameSpeed,bool bLoop)
{
	//指定したアニメーションの切替
	m_nAnimeNum=nType;
	m_pAnimController->SetTrackAnimationSet(0,m_pAnimSet[m_nAnimeNum]);

	//アニメーションフレームのリセット
	m_pAnimController->ResetTime();

	//アニメーションのフレーム再生速度の設定
	m_dFrameSpeed=dFrameSpeed;

	//ループの設定
	m_bLoop=bLoop;

	//再生終了フラグfalse
	m_bStop=false;
}
//=============================================================================
//Xファイルインスタンス生成
//=============================================================================
void CSceneX::Create()
{
	//シーンXファイルポインタ
	CSceneX *pSceneX;

	//シーンXファイル動的確保
	pSceneX=new CSceneX();

	//Xファイル初期化
	pSceneX->Init();
}
//EOF