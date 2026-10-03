//=============================================================================
//フィードバックブラー[FeedBackBlur.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _FEEDBACKBLUR_H_
#define _FEEDBACKBLUR_H_

//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "scene2D.h"
#include <Windows.h>

//*****************************************************************************
//クラス定義
//*****************************************************************************
//フィードバックエフェクトクラス
class CFeedBackBlur
{
	//外部
	public:
		CFeedBackBlur();										//コンストラクタ
		~CFeedBackBlur(){}										//デストラクタ
		HRESULT Init(LPDIRECT3DDEVICE9 pDevice);				//初期化
		void Uninit();											//終了
		void SetupBackBuffer(LPDIRECT3DDEVICE9 pDevice);		//バックバッファポインタ保持
		void ReturnBackBuffer(LPDIRECT3DDEVICE9 pDevice);		//バックバッファを戻す
		void DrawFirst(LPDIRECT3DDEVICE9 pDevice);				//最初の描画
		void DrawSecond(LPDIRECT3DDEVICE9 pDevice);				//次の描画
		void EndDrawSecond(LPDIRECT3DDEVICE9 pDevice);			//次描画の終了
		static CFeedBackBlur *Create(LPDIRECT3DDEVICE9 pDevice);//インスタンス生成
		static bool IsOK(){ return m_bBlur; }					//使用可能チェック
		static void SetUse(bool bUse){ m_bBlur = bUse; }		//使用フラグ変更

	//内部
	private:
		static bool m_bBlur;							//ブラー使用フラグ

		VERTEX_2D m_aVtx[4];							//描画用頂点情報
		LPDIRECT3DTEXTURE9 m_Texture1, m_Texture2;		//レンダリング用テクスチャ
		LPDIRECT3DSURFACE9 m_TexSurface1, m_TexSurface2;//サーフェイス
		LPDIRECT3DSURFACE9 m_TexZSBuff1, m_TexZSBuff2;	//Zバッファ
		D3DVIEWPORT9 m_ViewPort;						//レンダリング用ビューポート

		LPDIRECT3DSURFACE9 m_backBuffOrg;					//バックアップ用サーフェイス
		LPDIRECT3DSURFACE9 m_ZSBuffOrg;
		D3DVIEWPORT9 m_viewportOrg;
};

#endif
//EOF