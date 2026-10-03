//=============================================================================
//Xファイルシーン処理[SceneX.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CSCENEX_H_
#define _CSCENEX_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "../System/renderer.h"

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//キャストシェーダーのタイプ
enum CAST_TYPE
{
	CAST_NONE = 0,
	CAST_MOTIONBLUR,
	CAST_SHADOW,
	CAST_NUM
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//Xファイルモデルシーン
class CSceneX : public CScene
{
	//外部
	public:
		CSceneX(int priority = 1);//コンストラクタ
		~CSceneX();				//デストラクタ
		HRESULT Init(void);	//初期化
		void Uninit(void);	//終了
		void Update(void);	//更新
		void Draw(void);	//描画

		//モデルの生成
		HRESULT CreateModel(char *pFileName);

		//キャストシェーダーの種類のセット
		void SetCastType(CAST_TYPE type)
		{ m_castType = type; }

		static void Create();//インスタンス生成

	//派生クラスのみ
	protected:
		LPD3DXMESH			m_pD3DXMeshModel;		//メッシュ情報へのポインタ
		LPD3DXBUFFER		m_pD3DXBuffMatModel;	//マテリアル情報へのポインタ
		LPDIRECT3DTEXTURE9	m_pD3DTexture1;			// テクスチャへのポインタ1
		LPDIRECT3DTEXTURE9	m_pD3DTexture2;			// テクスチャへのポインタ2
		DWORD				m_nNumMatModel;			//マテリアル情報の数
		D3DXMATERIAL		*m_pD3DXMat;			//マテリアル情報
		D3DXVECTOR3			m_rotDestModel;			//目的の向き
		D3DXMATRIX			m_mtxWorld;				//ワールドマトリックス
		CAST_TYPE			m_castType;				//現在のシェーダーキャストの種類
};
#endif
//EOF