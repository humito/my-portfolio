//=============================================================================
// リムライト [RimLight.h]
// Author : 木村　文登
//=============================================================================
#ifndef _RIMLIGHT_H_
#define _RIMLIGHT_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Shader.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//リムライトクラス
class CRimLight : public CShader
{
	//外部
	public:
		CRimLight(){}								//コンストラクタ
		~CRimLight(){}								//デストラクタ
		void Load();								//読込
		void Uninit();								//終了
		void Begin(LPDIRECT3DDEVICE9 pDevice);		//シェーダー開始
		void End(LPDIRECT3DDEVICE9 pDevice);		//シェーダー終了

		void SetMatrix(LPDIRECT3DDEVICE9 pDevice,	//マトリックスの設定
						D3DXMATRIX *pMtxWorld,
						D3DXVECTOR3 pos);

		void SetPower(LPDIRECT3DDEVICE9 pDevice,	//リムライトの強さのセット
						float fPower);

		void SetMaterial(LPDIRECT3DDEVICE9 pDevice,	//マテリアルの設定
						D3DXVECTOR4 materialVec);
};
#endif
//EOF