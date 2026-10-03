//=============================================================================
// トゥーンシェーダー [ToonShader.h]
// Author : 木村　文登
//=============================================================================
#ifndef _TOONSHADER_H_
#define _TOONSHADER_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Shader.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//トゥーンシェーダークラス
class CToonShader : public CShader
{
	//外部
	public:
		CToonShader(){}								//コンストラクタ
		~CToonShader(){}							//デストラクタ
		void Load();								//読込
		void Uninit();								//終了
		void Begin(LPDIRECT3DDEVICE9 pDevice);		//シェーダー開始
		void End(LPDIRECT3DDEVICE9 pDevice);		//シェーダー終了

		void SetMatrix(LPDIRECT3DDEVICE9 pDevice,	//マトリックスの設定
			D3DXMATRIX *pMtxWorld);

		void SetMaterial(LPDIRECT3DDEVICE9 pDevice,	//マテリアルの設定
			D3DXVECTOR4 materialVec);

	//内部
	private:
		LPDIRECT3DTEXTURE9	m_pD3DToonMap;			//テクスチャポインタ
};
#endif
//EOF