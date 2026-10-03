//=============================================================================
//
// カメラクラス処理 [Camera.h]
// Author : 木村　文登
//
//=============================================================================
#ifndef _CCAMERA_H_
#define _CCAMERA_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "renderer.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//カメラクラス
class CCamera
{
	//外部
public:
	CCamera();
	~CCamera();
	HRESULT Init(void);				//初期化
	void Uninit(void){}				//終了
	void Update(void);				//更新
	void Set(void);					//カメラセット
	D3DXVECTOR3 GetPosCamera()		//カメラ座標取得
	{ return m_posCameraP; }
	D3DXVECTOR3 GetRotCamera(void)	//カメラの角度取得
	{ return m_rotCamera; }
	D3DXMATRIX GetMtxView(void)		//カメラのマトリックスを取得
	{ return m_mtxView; }
	D3DXMATRIX GetMtxProj(void)		//カメラのプロジェクションを取得
	{ return m_mtxProjection; }

	//内部
	//ポリゴン複数表示
	//内部
private:
	D3DXVECTOR3			m_posCameraP;		//カメラの視点
	D3DXVECTOR3			m_posCameraR;		//カメラの注視点
	D3DXVECTOR3			m_vecCameraU;		//カメラの上方向ベクトル
	D3DXVECTOR3			m_posDestCameraP;	//カメラの目的の視点
	D3DXVECTOR3			m_rotCamera;		//カメラの向き（回転角）
	D3DXMATRIX			m_mtxView;			//ビューマトリックス
	D3DXMATRIX			m_mtxProjection;	//プロジェクションマトリックス
	float				m_fRotDestCameraY;	//カメラの目的の角度
	float				m_fCam;				//カメラの一定距離
	int					m_nWaitCnt;			//カメラの待ち時間

	void TitleUpdate();
	void GameUpdate();
	void ResultUpdate();
};
#endif
//EOF