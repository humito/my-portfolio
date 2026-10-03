//=============================================================================
//3Dシーン処理[Scene3D.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CSCENE3D_H_
#define _CSCENE3D_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "../System/renderer.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//3Dポリゴンクラス
class CScene3D : public CScene
{
	//外部
	public:
		CScene3D(int priority = 3);					//コンストラクタ
		~CScene3D();								//デストラクタ
		HRESULT Init();								//初期化
		void Uninit(void);							//終了
		void Update(void);							//更新
		void Draw(void);							//描画
		static void Create();						//インスタンス生成

	//派生クラスのみ外部
	protected:
		LPDIRECT3DTEXTURE9 m_pD3DTexture;		// テクスチャへのポインタ
		LPDIRECT3DVERTEXBUFFER9 m_pD3DVtxBuff;	// 頂点バッファへのポインタ
		LPDIRECT3DINDEXBUFFER9 m_pD3DIndexBuff;	//インデックスバッファへのポインタ

		D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
		D3DXVECTOR3 m_pos;						// 位置
		D3DXVECTOR3 m_rot;						// 向き

		int m_nNumBlockX, m_nNumBlockY,m_nNumBlockZ;
		float m_fSizeBlockX ,m_fSizeBlockZ;

		int m_nNumVertexIndex;					// 頂点の総インデックス数
		int m_nNumVertex;						// 総頂点数
		int m_nNumPolygon;						// 総ポリゴン数
		int m_nStartX;							//１行ごとのブロックの初め
};

#endif
//EOF