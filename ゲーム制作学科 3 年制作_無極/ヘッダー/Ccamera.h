//=============================================================================
//
// カメラクラス処理 [CCamera.h]
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
//定数定義
//*****************************************************************************
#define CAMERA_RECT_VERTEX_MAX (4)//カメラ描画範囲の頂点数

//*****************************************************************************
//クラス定義
//*****************************************************************************
//カメラクラス
class CCamera
{
	//外部
	public:
	CCamera();										//コンストラクタ
	~CCamera();										//デストラクタ
	HRESULT Init(void);								//初期化
	void Uninit(void);								//終了
	void Update(void);								//更新
	void Set(void);									//カメラセット
	D3DXVECTOR3 GetPosCamera(void);					//カメラ座標取得
	D3DXVECTOR3 GetRotCamera(void);					//カメラの角度取得
	D3DXMATRIX GetMtxView(void);					//カメラのマトリックスを取得
	void GetVertexPos(D3DXVECTOR3 *vertexPos,int i);//カメラ描画範囲の頂点座標取得
	void SetPosLock(bool flag);						//カメラ固定フラグの設定

#ifdef _DEBUG
	void SetDebugFlag (bool bFlag);					//デバッグ操作フラグセット
#endif

	//内部
	private:
	D3DXVECTOR3			m_posCameraP;				//カメラの視点
	D3DXVECTOR3			m_posCameraR;				//カメラの注視点
	D3DXVECTOR3			m_vecCameraU;				//カメラの上方向ベクトル
	D3DXVECTOR3			m_posDestCameraP;			//カメラの目的の視点
	D3DXVECTOR3			m_posPrevPlayer;			//プレイヤー前座標
	D3DXMATRIX			m_mtxView;					//ビューマトリックス
	D3DXVECTOR3			m_rotCamera;				//カメラの向き（回転角）
	D3DXMATRIX			m_mtxProjection;			//プロジェクションマトリックス
	float				m_fRotDestCameraY;			//カメラの目的の角度
	float				m_fCam;						//カメラの一定距離
	int					m_nWaitCnt;					//カメラの待ち時間
	D3DXVECTOR3			m_cameraVertexpos[4];		//カメラ範囲の頂点座標
	float				m_fWidth;					//幅(X)
	float				m_fDepth;					//奥行(Z)
	bool				m_bOperation;				//カメラ操作フラグ
	bool				m_bPosLock;					//カメラ座標固定フラグ

#ifdef _DEBUG
		void FreeOperate (void);					//カメラ自由操作
#endif

};
#endif
//EOF