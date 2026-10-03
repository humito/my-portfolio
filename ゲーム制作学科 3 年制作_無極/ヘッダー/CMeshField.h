//=============================================================================
//メッシュフィールド処理[CMeshField.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CMESHFIELD_H_
#define _CMESHFIELD_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "CScene3D.h"
//*****************************************************************************
//定数定義
//*****************************************************************************
#define FIELD_NUM_X (60)	//フィールドの数X defo60
#define FIELD_NUM_Z (60)	//フィールドの数Y
#define FIELDSIZE_X (100.0f)//フィールドのXサイズ defo100
#define FIELDSIZE_Z (100.0f)//フィールドのZサイズ
//*****************************************************************************
//クラス定義
//*****************************************************************************
//メッシュフィールドクラス
class CMeshField : public CScene3D
{
	//外部
	public:
		CMeshField();																											//コンストラクタ
		~CMeshField();																											//デストラクタ
		HRESULT Init(char *pFileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot,int nNumBlockX,int nNumBlockZ,float fSizeBlockX,float fSizeBlockZ);		//初期化
		void Uninit();																											//終了
		void Update();																											//更新
		void Draw();																											//描画
		float GetHeight(D3DXVECTOR3 pos);																				//高さ取得
		static CMeshField *Create(char *pFileName,D3DXVECTOR3 pos,D3DXVECTOR3 rot,int nNumBlockX,int nNumBlockZ,float fSizeBlockX,float fSizeBlockZ);	//フィールドインスタンス生成

	//内部
	private:
		int m_nNumBlockX, m_nNumBlockZ;																							//ブロック数
		float m_fSizeBlockX, m_fSizeBlockZ;																						//ブロックサイズ
		D3DXVECTOR3 **m_ppNormal;																								//各ポリゴン法線ベクトル
		float GetHeightPolygon(D3DXVECTOR3 pos0,D3DXVECTOR3 playerPos,D3DXVECTOR3 normal);										//ポリゴンのY座標取得
};
#endif
//EOF