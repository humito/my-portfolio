//=============================================================================
//Xファイルシーン処理[SceneX.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CSCENEX_H_
#define _CSCENEX_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "renderer.h"
#include "MyAllocateHierarchy.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//Xファイルモデルシーン
class CSceneX : public CScene
{
	//外部
	public:
		CSceneX(int priority = 1);						//コンストラクタ
		~CSceneX();										//デストラクタ
		HRESULT Init(void);								//初期化
		void Uninit(void);								//終了
		void Update(void);								//更新
		void Draw(void);								//描画
		static void Create();							//インスタンス生成

	//派生クラスのみ
	protected:
		LPD3DXMESH					m_pD3DXMeshModel;	//メッシュ情報へのポインタ
		LPD3DXBUFFER				m_pD3DXBuffMatModel;//マテリアル情報へのポインタ
		DWORD						m_nNumMatModel;		//マテリアル情報の数
		D3DXMATERIAL				*m_pD3DXMat;		//マテリアル情報
		D3DXVECTOR3					m_rotDestModel;		//目的の向き
		LPD3DXFRAME					m_pFrameRoot;		//フレームリストの先頭ポインタ
		ID3DXAnimationController	*m_pAnimController;	//アニメーションの制御
		LPD3DXANIMATIONSET			*m_pAnimSet;		//アニメーションセット
		CAllocateHierarchy			m_alloc;			//フレーム階層
		bool						m_bLoop;			//アニメーションループフラグ
		bool						m_bStop;			//アニメーション再生終了フラグ
		double						m_dFrameSpeed;		//フレーム速度
		int							m_nAnimeNum;		//アニメーション番号

		//アニメーション切替
		void SetAnimation(int unType, double dFrameSpeed, bool bLoop);

		//アニメーション再生
		void PlayAnimation(void);
};
#endif
//EOF