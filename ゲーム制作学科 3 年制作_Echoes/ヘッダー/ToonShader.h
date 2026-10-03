//=============================================================================
//トゥーンシェーダー処理[ToonShader.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _TOONSHADER_H_
#define _TOONSHADER_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "Shader.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//トゥーンシェーダークラス
class CToonShader : public CShader
{
	//外部
	public:
		CToonShader(){}							//コンストラクタ
		~CToonShader(){}						//デストラクタ
		HRESULT Load();							//読込
		void Uninit();							//終了
		void Begin();							//シェーダー開始
		void BeginPass(UINT Pass);				//パス開始
		void EndPass();							//パス終了
		void End();								//シェーダー終了
		void SetMatrix(D3DXMATRIX *pMatWorld);	//マトリクスのセット
		void SetColor(D3DXVECTOR4 diffuse);		//色のセット

	//内部
	private:
		LPDIRECT3DTEXTURE9	m_pD3DToonMap;	//テクスチャポインタ

		D3DXHANDLE m_pWVP,m_pWorld;			//WVPマトリクス、ワールドマトリクス
		D3DXHANDLE m_pColor, m_pLightDir;	//色(ディフューズ色)、ライトベクトル
		D3DXMATRIX m_matView, m_matProj;	//ビュー、プロジェクションマトリクス
};
#endif
//EOF