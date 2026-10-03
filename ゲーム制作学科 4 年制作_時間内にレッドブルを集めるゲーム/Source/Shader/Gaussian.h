//=============================================================================
//ガウスフィルター[Gaussian.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _GAUSSIAN_H_
#define _GAUSSIAN_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Filter.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//ガウスフィルター
class CGaussian : public CFilter
{
	//外部
	public:
		CGaussian();								//コンストラクタ
		~CGaussian(){}								//デストラクタ
		void Load();								//読込
		void Uninit();								//終了
		void Draw(LPDIRECT3DDEVICE9 pDevice);		//描画(X方向ブラー)
		void DrawBlur(LPDIRECT3DDEVICE9 pDevice);	//描画(XY方向ブラー)

		//Y方向ブラー用にレンダーターゲット切替
		void ChangeSurfaceBlurY(LPDIRECT3DDEVICE9 pDevice);

		//Y方向ブラー用にレンダーターゲット戻す
		void ReturnSurfaceBlurY(LPDIRECT3DDEVICE9 pDevice);

		//ブラーフィルターテクスチャ取得
		LPDIRECT3DTEXTURE9 GetBlurTexture()
		{ return m_pD3DTextureY; }

		//ブラーフィルターZバッファテクスチャ取得
		LPDIRECT3DTEXTURE9 GetBlurZBuff()
		{ return m_pD3DZBuffTextureX; }

	//内部
	private:

		LPD3DXCONSTANTTABLE		m_pPSConstantTableBlurY;//Y方向ブラー用定数テーブル
		LPDIRECT3DPIXELSHADER9	m_pPixelShaderBlurY;	//Y方向ブラー用ピクセルシェーダー

		LPDIRECT3DTEXTURE9		m_pD3DTextureY;			//Y方向ブラー用テクスチャ
		LPDIRECT3DTEXTURE9		m_pD3DZBuffTextureY;	//Y方向ブラー用Zバッファテクスチャ
		LPDIRECT3DTEXTURE9		m_pD3DZBuffTextureX;	//X方向ブラー用Zバッファテクスチャ

		LPDIRECT3DSURFACE9		m_pTexSurfaceBlur;		//Y方向ブラー用サーフェイス
		LPDIRECT3DSURFACE9		m_pTexZSBuffBlur;		//Y方向ブラー用Zバッファサーフェイス

		//ブラー用テクスチャとサーフェイスの生成
		void CreateBlurTexSurface(LPDIRECT3DDEVICE9 pDevice);
};
#endif
//EOF