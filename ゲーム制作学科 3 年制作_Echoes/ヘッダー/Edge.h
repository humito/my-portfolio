//=============================================================================
//エッジフィルター[Edge.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _EDGE_H_
#define _EDGE_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <windows.h>
#include "Filter.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//エッジフィルタークラス
class CEdge : public CFilter
{
	//外部
	public:
		CEdge(){}								//コンストラクタ
		~CEdge(){}								//デストラクタ
		HRESULT Load();							//読込
		void Uninit();							//終了
		void Begin();							//シェーダー開始
		void BeginPass(UINT Pass);				//パス開始
		void EndPass();							//パス終了
		void End();								//シェーダー終了
		void Draw(LPDIRECT3DDEVICE9 pDevice);	//描画
		void SetTexel(D3DXVECTOR2 *pTexel);		//テクセル（幅）のセット
		void SetID(int nID);					//IDのセット
		void SetColor(D3DXVECTOR4 color);		//エッジの色セット

		LPDIRECT3DTEXTURE9 GetZBuffTexture()	//Zバッファテクスチャ取得
		{ return m_pD3DZBuffTexture; }

	//内部
	private:
		LPDIRECT3DTEXTURE9	m_pD3DZBuffTexture;	//テクスチャポインタ

		D3DXHANDLE m_pTex;		//テクセル用ハンドル
		D3DXHANDLE m_pID;		//ID用ハンドル
		D3DXHANDLE m_pEdgeColor;//エッジ色用ハンドル
};
#endif
//EOF