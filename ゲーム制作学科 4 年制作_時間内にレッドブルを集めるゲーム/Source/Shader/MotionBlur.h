//=============================================================================
//モーションブラー[MotionBlur.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _MOTIONBLUR_H_
#define _MOTIONBLUR_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Filter.h"

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//レンダーターゲット切替の種類
enum BLUR_SURFACE
{
	BACK_SURFACE = 0,	//バックバッファ用
	VELOCITY_SURFACE,	//速度テクスチャ用
	BLUR_SURFACE_NUM	//種類数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
class CMotionBlur : public CFilter
{
	//外部
	public:
		CMotionBlur(){}								//コンストラクタ
		~CMotionBlur(){}							//デストラクタ
		void Load();								//読込
		void Uninit();								//終了
		void Begin(LPDIRECT3DDEVICE9 pDevice);		//シェーダー開始
		void End(LPDIRECT3DDEVICE9 pDevice);		//シェーダー終了
		void Draw(LPDIRECT3DDEVICE9 pDevice);		//描画

		//ワールドマトリックスのセット
		void SetMatrix(LPDIRECT3DDEVICE9 pDevice,
						D3DXMATRIX *pMtxWorldNew,
						D3DXMATRIX *pMtxWorldOld);

		//速度のセット
		void SetVelocity(LPDIRECT3DDEVICE9 pDevice,
						D3DXVECTOR4 velocity);

		//ブラー用のサーフェイス切替
		void ChangeBlurSurface(LPDIRECT3DDEVICE9 pDevice,
								BLUR_SURFACE type);

		//ブラー用サーフェイスを戻す
		void ReturnBlurSurface(LPDIRECT3DDEVICE9 pDevice);

		//速度マップ取得
		LPDIRECT3DTEXTURE9 GetVelocityTex()
		{ return m_pD3DVelocityTexture; }

		//Zバッファ取得
		LPDIRECT3DTEXTURE9 GetZBuff()
		{ return m_pD3DZBuffTexture; }

	//内部
	private:
		//送るシェーダー関連
		LPD3DXCONSTANTTABLE		m_pVSConstantTableCas;	//頂点シェーダー定数テーブル
		LPD3DXCONSTANTTABLE		m_pPSConstantTableCas;	//ピクセルシェーダー定数テーブル
		LPDIRECT3DVERTEXSHADER9	m_pVertexShaderCas;		//頂点シェーダー
		LPDIRECT3DPIXELSHADER9	m_pPixelShaderCas;		//ピクセルシェーダー

		LPDIRECT3DTEXTURE9		m_pD3DZBuffTexture;		//Zバッファテクスチャ
		LPDIRECT3DTEXTURE9		m_pD3DVelocityTexture;	//速度用テクスチャ
		LPDIRECT3DSURFACE9		m_TexVelocitySurface;	//速度用サーフェイス

		D3DXMATRIX				m_mtxViewOld;			//前回のビューマトリクス

		//ブラー用のテクスチャとサーフェイスの生成
		void CreateBlurTexSurface(LPDIRECT3DDEVICE9 pDevice);
};
#endif
//EOF