//=============================================================================
//
// ライト処理 [Light.h]
// Author : HUMITO KIMURA
//
//=============================================================================
#ifndef _CLIGHT_H_
#define _CLIGHT_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "d3dx9.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//ライトクラス
class CLight
{
	//外部
	public:
		CLight(){}					//コンストラクタ
		~CLight(){}					//デストラクタ
		HRESULT Init();				//初期化
		void Uninit(){}				//終了
		void Update();				//更新
		void Set();					//ライトセット
		D3DXVECTOR3 GetVecDir();	//ライトのベクトル取得

		//ビュー行列の取得
		D3DXMATRIX GetMatView()
		{ return m_matViewLight; }

		//プロジェクション行列の取得
		D3DXMATRIX GetMatProj()
		{ return m_matProjLight; }

	//内部
	private:
		D3DLIGHT9	m_aLight[3];	//ライト情報
		D3DXVECTOR3	m_LightEye;		//ライトの視点ベクトル
		D3DXVECTOR3	m_LightUp;		//ライトの上ベクトル
		D3DXVECTOR3	m_LightAt;		//ライトの視線ベクトル
		D3DXVECTOR3	m_vecDir;		//ライトベクトル
		D3DXMATRIX	m_matViewLight;	//ライトのビュー行列
		D3DXMATRIX	m_matProjLight;	//ライトのプロジェクション行列
};
#endif
//EOF