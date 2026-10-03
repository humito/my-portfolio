//=============================================================================
//ビルボードシーン処理[CsceneBillboard.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CSCENEBILLBOARD_H_
#define _CSCENEBILLBOARD_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "renderer.h"
//*****************************************************************************
//クラス定義
//*****************************************************************************
//ビルボードモデルシーン
class CSceneBillboard : public CScene
{
	//外部
	public:
	CSceneBillboard(int priority=3);			//コンストラクタ
	~CSceneBillboard();							//デストラクタ
	HRESULT Init(void);							//初期化
	void Uninit(void);							//終了
	void Update(void);							//更新
	void Draw(void);							//描画
	static void Create();						//インスタンス生成
	//条件付き外部
	protected:
	LPDIRECT3DTEXTURE9		m_pD3DTextureBill;	//テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9	m_pD3DVtxBuffBill;	//頂点バッファインターフェースへのポインタ
	LPDIRECT3DINDEXBUFFER9	m_pD3DIndexBuffBill;//インデックスバッファのポインタ
	D3DXMATRIX				m_mtxWorld;			//ワールドマトリックス
};

#endif
//EOF