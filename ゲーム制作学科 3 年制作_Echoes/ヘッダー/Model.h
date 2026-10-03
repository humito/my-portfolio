//=============================================================================
//階層構造モデル処理[Model.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _MODEL_H_
#define _MODEL_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "renderer.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//パーツモデルクラス
class CModel
{
	//外部
	public:
		CModel();									//コンストラクタ
		~CModel();									//デストラクタ
		HRESULT Init(	char *pFileName,			//初期化
						D3DXVECTOR3 pos,
						D3DXVECTOR3 rot);
		void Uninit();								//終了
		void Draw();								//描画
		static CModel *Create(	char *pFileName,	//インスタンス生成
								D3DXVECTOR3 pos,
								D3DXVECTOR3 rot);
		void SetParent(CModel *pParent);			//親ポインタセット
		D3DXMATRIX GetMatrix();						//マトリックスの取得

		D3DXVECTOR3 GetPos();						//座標取得
		D3DXVECTOR3 GetRot();						//角度取得
		void SetRot(D3DXVECTOR3 rot);				//角度セット
		void SetPos(D3DXVECTOR3 pos);				//座標セット

	//内部
	private:
		LPDIRECT3DTEXTURE9	m_pD3DTexture;				//テクスチャポインタ
		LPD3DXMESH			m_pD3DXMeshModel;			//メッシュ情報へのポインタ
		LPD3DXBUFFER		m_pD3DXBuffMatModel;		//マテリアル情報へのポインタ
		DWORD				m_nNumMatModel;				//マテリアル情報の数
		D3DXMATERIAL		*m_pD3DXMat;				//マテリアル
		D3DXVECTOR3			m_pos;						//座標
		D3DXVECTOR3			m_rot;						//角度
		D3DXMATRIX			m_Matrix;					//マトリックス
		CModel				*m_pParent;					//親ポインタ
};

#endif
//EOF