//=============================================================================
//数値表示処理[Number.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CNUMBER_H_
#define _CNUMBER_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "../System/renderer.h"

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//表示方法
enum NUMBER_DISP
{
	DISP_2DPOLYGON = 0,	//2Dポリゴン
	DISP_BILLBOARD,		//ビルボード
	DISP_TYPE_MAX		//描画方法
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//数値クラス
class CNumber
{
	//外部
	public:
		CNumber();														//コンストラクタ
		~CNumber();														//デストラクタ
		HRESULT Init(char *pFileName, D3DXVECTOR3 pos, NUMBER_DISP type,
					float fWidth, float fHeight, int nDigitNum);		//初期化
		void Uninit();													//終了
		void Draw();													//描画
		void SetValue(int num);											//1桁の数値のセット
		void SetPos(D3DXVECTOR3 pos);									//座標セット

	//内部
	private:
		LPDIRECT3DTEXTURE9		m_pD3DTex;								//テクスチャへのポインタ
		LPDIRECT3DVERTEXBUFFER9	m_pD3DVtxBuff;							//頂点バッファインターフェースへのポインタ
		D3DXMATRIX				m_mtxWorld;								//ワールドマトリックス
		D3DXVECTOR3				m_pos;									//ポリゴンの位置
		D3DXVECTOR3				m_rot;									//ポリゴンの向き(回転)
		D3DXVECTOR3				m_scl;									//ポリゴンの大きさ(スケール)
		int						*m_pnValue;								//1桁の数値
		int						m_nType;								//数値を表示する種類
		int						m_nDigitNum;							//桁数
		int						m_nCutNum;								//桁数抽出用
		float					m_fWidth;								//横幅
		float					m_fHeight;								//高さ
		void					ChangeBuffer(int num);					//頂点座標情報の変更
};
#endif
//EOF