//=============================================================================
//影表示処理[CShadow.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CSHADOW_H_
#define _CSHADOW_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "CScene3D.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//影クラス
class CShadow : public CScene3D
{
	//外部
	public:
		CShadow();											//コンストラクタ
		~CShadow();											//デストラクタ
		HRESULT Init(float fSizeX,float fSizeZ);			//初期化
		void Uninit();										//終了
		void Draw();										//描画
		static CShadow *Create(float fSizeX,float fSizeZ);	//インスタンス生成
		void SetPos(D3DXVECTOR3 pos);						//座標セット
		void SetDisp(bool bFlag);							//表示フラグセット

	//内部
	private:
		float	m_fSizeX,m_fSizeZ;							//影のサイズ
		int		m_nAlpha;									//影のα値
		void	ChangeBuff();								//頂点座標の変更
};
#endif
//EOF