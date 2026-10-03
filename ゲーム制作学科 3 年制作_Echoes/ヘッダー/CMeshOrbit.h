//=============================================================================
//メッシュ奇跡エフェクト処理[CMeshOrbit.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CMESHORBIT_H_
#define _CMESHORBIT_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "CScene3D.h"


//*****************************************************************************
//クラス定義
//*****************************************************************************
//軌跡エフェクトクラス
class CMeshOrbit : public CScene3D
{
	//外部
	public:
		CMeshOrbit();	//コンストラクタ
		~CMeshOrbit();	//デストラクタ

		//初期化
		HRESULT Init(	D3DXVECTOR3 pos, D3DXVECTOR3 rot, 
						int nNumBlockX, int nNumBlockZ, 
						float fSizeBlockX, float fSizeBlockZ);

		void Uninit();	//終了
		void Update();	//更新
		void Draw();	//描画

		//インスタンス生成
		static CMeshOrbit *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot,
							int nNumBlockX, int nNumBlockZ, 
							float fSizeBlockX, float fSizeBlockZ);

		//親マトリクスセット
		void SetParentMtx(D3DXMATRIX parentMtx);

	//内部
	private:
		int m_nNumBlockX, m_nNumBlockZ;		//ブロック数																	//ブロック数
		float m_fSizeBlockX, m_fSizeBlockZ;	//1ブロックのサイズ																		//ブロックサイズ

		D3DXVECTOR3 m_OffsetPoint[2];		//オフセット座標
		D3DXVECTOR3 *m_pPoint;				//頂点座標

		D3DXMATRIX m_ParentMtx;				//親マトリクス
};
#endif
//EOF