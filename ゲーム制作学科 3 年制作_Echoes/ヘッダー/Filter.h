//=============================================================================
//シェーダー使用のフィルター[Filter.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _FILTER_H_
#define _FILTER_H_

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "Shader.h"
#include "scene2D.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//フィルタークラス
class CFilter : public CShader
{
	//外部
	public:
		CFilter();										//コンストラクタ
		~CFilter(){}									//デストラクタ
		void Init();									//初期化
		void Uninit();									//終了
		void ChangeSurface1(LPDIRECT3DDEVICE9 pDevice);	//サーフェイスとレンダーターゲット切替
		void ChangeSurface2(LPDIRECT3DDEVICE9 pDevice);	//サーフェイスとレンダーターゲット切替
		void ReturnSurface1(LPDIRECT3DDEVICE9 pDevice);	//レンダーターゲットを戻す
		void ReturnSurface2(LPDIRECT3DDEVICE9 pDevice);	//レンダーターゲットを戻す
		void Draw(LPDIRECT3DDEVICE9 pDevice);			//描画

		LPDIRECT3DTEXTURE9 GetTexture()					//テクスチャの取得
		{ return m_pD3DTexture; }

		void SetTexture(LPDIRECT3DTEXTURE9 pTexture)	//テクスチャのセット
		{ m_pD3DTexture = pTexture; }

	//派生クラスのみ外部
	protected:
		VERTEX_2D			m_aVtx[4];		//描画用頂点情報
		LPDIRECT3DTEXTURE9	m_pD3DTexture;	//テクスチャポインタ

		LPDIRECT3DSURFACE9 m_TexSurface;	//サーフェイス
		LPDIRECT3DSURFACE9 m_TexZSBuff;		//Zバッファ
		LPDIRECT3DSURFACE9 m_backBuffOrg;	//バックアップ用サーフェイス
		LPDIRECT3DSURFACE9 m_ZSBuffOrg;		//バックアップ用Zバッファ
};
#endif
//EOF