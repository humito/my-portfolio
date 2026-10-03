//=============================================================================
//投影テクスチャシャドウ[Shadow.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _SHADOW_H_
#define _SHADOW_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Filter.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
//影シェーダーの種類
enum SHADER_SHADOW
{
	SHADER_SHADOW_CAST = 0,	//キャスト（影を送る）
	SHADER_SHADOW_RECEIVE	//レシーブ（影を受け取る）
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//エッジフィルター
class CShadow : public CFilter
{
	//外部
	public:
		CShadow(){}									//コンストラクタ
		~CShadow(){}								//デストラクタ
		void Load();								//読込
		void Uninit();								//終了

		void SetMatrix(LPDIRECT3DDEVICE9 pDevice,	//マトリックスのセット
						SHADER_SHADOW type,
						D3DXMATRIX *pMtxWorld);

		void SetColor(LPDIRECT3DDEVICE9 pDevice,	//マテリアル色のセット
						SHADER_SHADOW type,
						D3DXVECTOR4 color);

		void SetPlayerPos(LPDIRECT3DDEVICE9 pDevice,//プレイヤー座標のセット
						D3DXVECTOR3 pos);

		void Begin(LPDIRECT3DDEVICE9 pDevice,		//シェーダー開始
					SHADER_SHADOW type);

		void End(LPDIRECT3DDEVICE9 pDevice);		//シェーダー完了

		void Draw(LPDIRECT3DDEVICE9 pDevice);		//描画

	//内部
		//受け取るシェーダー関連
		LPD3DXCONSTANTTABLE m_pVSConstantTableRec;	//頂点シェーダー定数テーブル
		LPD3DXCONSTANTTABLE m_pPSConstantTableRec;	//ピクセルシェーダー定数テーブル
		LPDIRECT3DVERTEXSHADER9 m_pVertexShaderRec;	//頂点シェーダー
		LPDIRECT3DPIXELSHADER9 m_pPixelShaderRec;	//ピクセルシェーダー
};
#endif
//EOF