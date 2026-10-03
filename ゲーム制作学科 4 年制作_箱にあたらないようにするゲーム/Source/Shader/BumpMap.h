//=============================================================================
// バンプマップ [BumpMap.h]
// Author : 木村 文登
//=============================================================================
#ifndef _BUMPMAP_H_
#define _BUMPMAP_H_

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Shader.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//バンプマップクラス
class CBumpMap : public CShader
{
	//外部
	public:
		CBumpMap();										//コンストラクタ
		~CBumpMap(){}									//デストラクタ
		void Load();									//読込
		void Uninit();									//終了
		void Begin(LPDIRECT3DDEVICE9 pDevice);			//シェーダー開始
		void End(LPDIRECT3DDEVICE9 pDevice);			//シェーダー終了

		void SetMatrix(LPDIRECT3DDEVICE9 pDevice,		//マトリックスの設定
						D3DXMATRIX *pMtxWorld);

		void SetMaterial(LPDIRECT3DDEVICE9 pDevice,		//マテリアルの設定
						D3DXVECTOR4 materialVec,
						D3DXVECTOR4 materialSpecVec);

	//内部
	private:
		LPDIRECT3DTEXTURE9		m_pD3DNormalMap;		//法線マップテクスチャ
		LPDIRECT3DPIXELSHADER9	m_pNormalMapShader;		//法線計算用ピクセルシェーダー
};
#endif
//EOF