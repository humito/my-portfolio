//=============================================================================
// マルチレンダーターゲット[MultiRenderTarget.h]
// Author : 木村 文登
//=============================================================================
#ifndef _MULTIRENDERTARGET_H_
#define _MULTIRENDERTARGET_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Shader.h"
#include "../manager.h"

//*****************************************************************************
//構造体定義
//*****************************************************************************
//レンダーテクスチャ
struct RENDER_TEXTURE
{
	LPDIRECT3DTEXTURE9 pD3DTexture;	//レンダリング用テクスチャ
	LPDIRECT3DSURFACE9 pSurface;	//サーフェイス
};

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//レンダリングの種類
enum RENDER_TYPE
{
	RENDER_COLOR = 0,	//カラーレンダリング
	RENDER_NORMAL,		//法線マップ
	RENDER_ZBUFF,		//深度マップ
	RENDER_POSITION,	//座標マップ
	RENDER_NUM			//レンダリング数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//マルチレンダーターゲットクラス
class CDeferred : public CShader
{
	//外部
	public:
		CDeferred();							//コンストラクタ
		~CDeferred(){}							//デストラクタ
		void Load();							//初期化
		void Uninit();							//終了

		void Draw(LPDIRECT3DDEVICE9 pDevice);	//描画(シェーダーあり)
		void Draw(LPDIRECT3DDEVICE9 pDevice,	//描画(シェーダーなし)
					RENDER_TYPE type);

		//シェーダー開始
		void Begin(LPDIRECT3DDEVICE9 pDevice);

		//シェーダー終了
		void End(LPDIRECT3DDEVICE9 pDevice);

		//マトリックスの設定
		void SetMatrix(	LPDIRECT3DDEVICE9 pDevice,
						D3DXMATRIX *pMtxWorld);

		//マテリアルの設定
		void SetMaterial(	LPDIRECT3DDEVICE9 pDevice,
							D3DXVECTOR4 materialVec);

		//ライトの準備
		void PreparationLight(LPDIRECT3DDEVICE9 pDevice);

		//カメラの準備
		void PreparationCamera(LPDIRECT3DDEVICE9 pDevice);

		//マルチレンダーターゲット全て描画
		void DrawAllRendering(LPDIRECT3DDEVICE9 pDevice);

		//全レンダーターゲット切替
		void ChangeSurfaceAll(LPDIRECT3DDEVICE9 pDevice);

		//全レンダーターゲット戻す
		void ReturnSurfaceAll(LPDIRECT3DDEVICE9 pDevice);

		//レンダリングテクスチャの取得
		LPDIRECT3DTEXTURE9 GetRenderingTexture(RENDER_TYPE type);

	//内部
	private:
		VERTEX_2D				m_aVtx[4];					//描画用頂点情報
		RENDER_TEXTURE			m_RenderTexture[RENDER_NUM];//レンダリングテクスチャ
		LPDIRECT3DSURFACE9		m_backBuffOrg[RENDER_NUM];	//バックアップ用サーフェイス

		LPD3DXCONSTANTTABLE		m_pVS2DConstantTable;		//2D用頂点シェーダー定数テーブル
		LPD3DXCONSTANTTABLE		m_pPS2DConstantTable;		//2D用ピクセルシェーダー定数テーブル
		LPDIRECT3DVERTEXSHADER9	m_p2DVertexShader;			//2D用頂点シェーダー
		LPDIRECT3DPIXELSHADER9	m_p2DPixelShader;			//2D用ピクセルシェーダー

		//テクスチャとサーフェイスの生成
		void CreateTexSurface();
};
#endif
//EOF