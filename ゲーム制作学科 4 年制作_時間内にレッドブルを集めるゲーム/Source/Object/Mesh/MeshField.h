//=============================================================================
//メッシュフィールド処理[MeshField.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CMESHFIELD_H_
#define _CMESHFIELD_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "../../Scene/Scene3D.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define NO_LOAD_FIELD (0)	//ロードしない
#define LOAD_FIELD (1)		//ロードする

//*****************************************************************************
//クラス定義
//*****************************************************************************
//メッシュフィールドクラス
class CMeshField : public CScene3D
{
	//外部
	public:
		CMeshField();									//コンストラクタ
		~CMeshField();									//デストラクタ
		HRESULT Init(	int nLoad,						//初期化
						char *pFieldName,
						char *pTexFileName,
						D3DXVECTOR3 pos,
						D3DXVECTOR3 rot);
		void Uninit();									//終了
		void Update();									//更新
		void Draw();									//描画

		//ファー回数セット
		void SetFurTimes(int nTimes)
		{ m_nFurTimes = nTimes; }

		float GetHeight(D3DXVECTOR3 pos);				//高さ取得
		static CMeshField *Create(	int nLoad,			//フィールドインスタンス生成
									char *pFieldName,
									char *pTexFileName,
									D3DXVECTOR3 pos,
									D3DXVECTOR3 rot);

	//内部
	private:
		D3DXVECTOR3 **m_ppNormal;						//各ポリゴン法線ベクトル
		float GetHeightPolygon(	D3DXVECTOR3 pos0,		//ポリゴンのY座標取得
								D3DXVECTOR3 playerPos,
								D3DXVECTOR3 normal);
		int m_nFurTimes;								//ファーをかける回数
};
#endif
//EOF