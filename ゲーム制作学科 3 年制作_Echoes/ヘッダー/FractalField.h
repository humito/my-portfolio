//=============================================================================
// フラクタル地形 [FractalField.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _FRACTALFIELD_H_
#define _FRACTALFIELD_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "Scene3D.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define DIVISION_NUM (5)	//分割数
#define FIELD_SIZE (100.0f)	//フィールドのサイズ

//*****************************************************************************
//クラス定義
//*****************************************************************************
//フラクタル地形クラス
class CFractalField : CScene3D
{
	//外部
	public:
		CFractalField();						//コンストラクタ
		~CFractalField();						//デストラクタ

		//初期化
		HRESULT Init(char *pFileName,D3DXVECTOR3 pos,
					D3DXVECTOR3 rot,int nDivision, float fSize);

		void Uninit();							//終了
		void Update();							//更新
		void Draw();							//描画
		float GetHeight(D3DXVECTOR3 pos);		//高さ取得

		//インスタンス生成
		static CFractalField *Create(char *pFileName,D3DXVECTOR3 pos, D3DXVECTOR3 rot,
									int nDivision, float fSize);
	//内部
	private:
		int m_nNumBlock;						//縦横１列のブロック数
		float m_fSizeBlock;						//１ブロックのサイズ
		D3DXVECTOR3 **m_ppNormal;				//ポリゴンの法線ベクトル																							//各ポリゴン法線ベクトル
		float *m_pfHeightVtx;					//設定用頂点の高さ

		void InitHeightPolygon(int nDivision);	//ポリゴンの高さの初期化

		//ポリゴンのY座標取得
		float GetHeightPolygon(D3DXVECTOR3 pos0, D3DXVECTOR3 playerPos,
								D3DXVECTOR3 normal);

};
#endif
//EOF