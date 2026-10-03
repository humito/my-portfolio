//=============================================================================
//トゥーンシェーダー処理[CToonShader.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "CToonShader.h"
#include "manager.h"
#include "CLight.h"
#include "Ccamera.h"
#include "CInputKeyboard.h"

//*****************************************************************************
//スタティックメンバ変数宣言
//*****************************************************************************
LPDIRECT3DTEXTURE9 CToonShader::m_pD3DTexture=NULL;	//テクスチャファイル
bool CToonShader::m_bToon=false;					//トゥーン設定フラグ

//=============================================================================
//コンストラクタ
//=============================================================================
CToonShader::CToonShader()
{
	m_pD3DTexture=NULL;
}
//=============================================================================
//デストラクタ
//=============================================================================
CToonShader::~CToonShader()
{
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CToonShader::Init()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

	//トゥーン設定フラグ
	m_bToon=true;

	//トゥーン用テクスチャの読込
	D3DXCreateTextureFromFileEx(pDevice,
								"data/TEXTURE/toon.bmp",//トゥーンマップテクスチャーファイル名
								D3DX_DEFAULT,
								D3DX_DEFAULT,
								1,
								0,
								D3DFMT_UNKNOWN,
								D3DPOOL_MANAGED,
								D3DX_DEFAULT,
								D3DX_DEFAULT,
								0x0,
								NULL,
								NULL,
								&m_pD3DTexture);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CToonShader::Uninit()
{
	//トゥーン用テクスチャ終了
	if(m_pD3DTexture)
	{
		m_pD3DTexture->Release();
		m_pD3DTexture=NULL;
	}
}
//=============================================================================
//シェーディング開始
//=============================================================================
void CToonShader::Begin()
{
#ifdef _DEBUG
	//デバッグ時のみ9キーが押されたらフラグ変更
	if(CInputKeyboard::GetKeyTrigger(DIK_9))
	{
		if(m_bToon)
		{
			m_bToon=false;
		}
		else
		{
			m_bToon=true;
		}
	}
#endif

	///////////////////////////////////////
	//		トゥーンシェーダー開始		//
	/////////////////////////////////////
	//フラグがtrueの場合のみ設定開始
	if(m_bToon)
	{
		//レンダラー情報取得
		CRenderer *pRenderer=CManager::GetRenderer();
		//デバイスのゲット
		LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();

		//ライトのゲット
		CLight *pLight=CManager::GetLight();
		//ライトベクトルのゲット
		D3DXVECTOR3 vecDir=pLight->GetVecDir();

		//カメラの取得
		CCamera *pCamera=CManager::GetCamera();
		//マトリックスビューの取得
		D3DXMATRIX mtxView=pCamera->GetMtxView();

		pDevice->SetTextureStageState( 0 , D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		pDevice->SetTexture( 0, m_pD3DTexture );
		D3DXMATRIX matTrans,matTrans0,matTrans1,matTrans2;
		D3DXVECTOR4 light1, light2;

		// ライト計算
		//カメラビューと座標マトリクス1を合成
		D3DXMatrixInverse(&matTrans1,NULL,&mtxView);
		//座標マトリクス2と合成
		D3DXMatrixTranspose(&matTrans2,&matTrans1);
		//ライトベクトルと座標マトリクス2の座標反映したものをライト1に設定
		D3DXVec3Transform(&light1, &vecDir, &matTrans2);
		//4Dベクトルだと何かと都合が悪いのでW値を消去
		light1.w = 0;
		//ライト1を正規化
		D3DXVec4Normalize(&light2,&light1);
		//座標マトリクス0のマトリクスを設定
		matTrans0 = D3DXMATRIX(
			1,	-light2.x,	0,	0,
			0,	-light2.y,	0,	0,
			0,	-light2.z,	1,	0,
			0,	0,		0,	1
		);

		// V値を0～1にスケーリング
		D3DXMatrixScaling( &matTrans1, 1.0f,-0.5f,1.0f );
		D3DXMatrixTranslation( &matTrans2, 0,0.5f,0 );
		matTrans = matTrans0 * matTrans1 * matTrans2;
		pDevice->SetTransform( D3DTS_TEXTURE0, &matTrans );
		pDevice->SetTextureStageState( 0 , D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACENORMAL);
	}
}
//=============================================================================
//シェーディング終了
//=============================================================================
void CToonShader::End()
{
	//レンダラー情報取得
	CRenderer *pRenderer=CManager::GetRenderer();
	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice=pRenderer->GetDevice();
	//設定したテクスチャを元に戻す
	pDevice->SetTexture( 0, NULL );
	pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);
	pDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
}
//EOF