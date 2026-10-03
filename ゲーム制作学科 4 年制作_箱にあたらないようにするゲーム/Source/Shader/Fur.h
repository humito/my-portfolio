//=============================================================================
//ファーシェーダー[Fur.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _FUR_H_
#define _FUR_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Shader.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
class CFur : public CShader
{
	//外部
	public:
		CFur(){}									//コンストラクタ
		~CFur(){}									//デストラクタ
		void Load();								//読込
		void Uninit();								//終了

		void Begin(LPDIRECT3DDEVICE9 pDevice);		//シェーダー開始
		void End(LPDIRECT3DDEVICE9 pDevice);		//シェーダー終了

		void SetMatrix(LPDIRECT3DDEVICE9 pDevice,	//マトリックスの設定
						D3DXMATRIX *pMtxWorld,
						D3DXVECTOR3 pos);

		void SetOffset(LPDIRECT3DDEVICE9 pDevice,	//オフセットのセット
						float fOffset);

		void SetMaterial(LPDIRECT3DDEVICE9 pDevice,	//マテリアルの設定
						D3DXVECTOR4 materialVec);

	//内部
	private:
		LPDIRECT3DTEXTURE9 m_pD3DFurTex;
};
#endif
//EOF