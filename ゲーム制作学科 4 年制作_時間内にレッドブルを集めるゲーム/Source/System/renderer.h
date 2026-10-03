//=============================================================================
// レンダラー [renderer.h]
// Author : 木村 文登
//=============================================================================
#ifndef _RENDERER_H_
#define _RENDERER_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <d3dx9.h>
#include "../Scene/scene.h"

//*****************************************************************************
// 定数定義
//*****************************************************************************
// ２Ｄポリゴン頂点フォーマット( 頂点座標[2D] / 反射光 / テクスチャ座標 )
#define FVF_VERTEX_2D (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)//頂点フォーマット(２Ｄ用)
// ３Ｄポリゴン頂点フォーマット( 頂点座標[3D] / 法線 / 反射光 / テクスチャ座標 )
#define	FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
//2D頂点情報
typedef struct
{
	D3DXVECTOR3 vtx;	//頂点座標
	float       rhw;	//(中身は1.0f)
	D3DCOLOR    diffuse;//反射光
	D3DXVECTOR2 tex;	//テクスチャ座標
}VERTEX_2D;

// 上記３Ｄポリゴン頂点フォーマットに合わせた構造体を定義
typedef struct
{
	D3DXVECTOR3 vtx;		// 頂点座標
	D3DXVECTOR3 nor;		// 法線ベクトル
	D3DCOLOR diffuse;		// 反射光
	D3DXVECTOR2 tex;		// テクスチャ座標
}VERTEX_3D;

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CToonShader;	//トゥーンシェーダー
class CFur;			//ファーシェーダー
class CRimLight;	//リムライト
class CGaussian;	//ガウスフィルター
class CDepthOfField;//被写界深度
class CMotionBlur;	//モーションブラー
class CShadow;		//シャドウ
class CFade;		//フェード

//*****************************************************************************
// クラス定義
//*****************************************************************************
//レンダラークラス(DirectXの全ての処理行う)
class CRenderer
{
	//外部
	public:
		CRenderer(){}												//コンストラクタ
		~CRenderer(){}												//デストラクタ

		HRESULT Init(HINSTANCE hInstance,HWND hWnd, BOOL bWindow);	//初期化
		void Uninit();												//終了
		void Update();												//更新
		void Draw();												//描画

		LPDIRECT3DDEVICE9 GetDevice(void);							//デバイスのゲット
		LPDIRECT3D9	m_pD3D;											// Direct3Dオブジェクト
		LPDIRECT3DDEVICE9	m_pD3DDevice;							// Deviceオブジェクト(描画に必要)

		//トゥーンシェーダーの取得
		CToonShader *GetToon()
		{ return m_pToonShader; }

		//ファーシェーダー取得
		CFur *GetFur()
		{ return m_pFur; }

		//リムライトシェーダー取得
		CRimLight *GetRimLight()
		{ return m_pRimLight; }

		//ガウスシェーダーの取得
		CGaussian *GetGauss()
		{ return m_pGauss; }

		//影用ガウスシェーダーの取得
		CGaussian *GetShadowGauss()
		{ return m_pShadowGauss; }

		//モーションブラーシェーダー取得
		CMotionBlur *GetMotionBlur()
		{ return m_pMotionBlur; }

		//シャドウシェーダーの取得
		CShadow *GetShadow()
		{ return m_pShadow; }

		//フェードフィルターの取得
		static CFade *GetFade()
		{ return m_pFade; }

		//ステート番号のセット
		static void SetStateIndex(int nIndex)
		{ m_nStateIndex = nIndex; }

		//フレームレート関連
		static DWORD m_dwExecLastTime;
		static DWORD m_dwFPSLastTime;
		static DWORD m_dwCurrentTime;
		static DWORD m_dwFrameCount;

//デバッグ
#ifdef _DEBUG
		static int	m_nCountFPS;									// FPSカウンタ
		void DrawFPS (void);										//FPSの表示
#endif

	//内部
	private:

		VERTEX_2D			m_aVtx[4];		//描画用頂点情報

		//トゥーンシェーダー
		CToonShader *m_pToonShader;

		//ファーシェーダー
		CFur *m_pFur;

		//リムライトシェーダー
		CRimLight *m_pRimLight;

		//ガウスフィルター
		CGaussian *m_pGauss;

		//影用ガウスフィルター
		CGaussian *m_pShadowGauss;

		//被写界深度
		CDepthOfField *m_pDepth;

		//モーションブラー
		CMotionBlur *m_pMotionBlur;

		//シャドウ
		CShadow *m_pShadow;

		//フェード
		static CFade *m_pFade;

		//ステート番号
		static int m_nStateIndex;

		//ブラーのレンダリング
		void RenderBlur();

		//被写界深度のレンダリング
		void RenderDepth();

		//モーションブラーのレンダリング
		void RenderMotionBlur();

		//影のレンダリング
		void RenderShadow();

//デバッグ
#ifdef _DEBUG
		LPD3DXFONT	m_pD3DXFont;									// フォントへのポインタ
#endif
};
#endif
//EOF