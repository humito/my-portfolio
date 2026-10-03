//=============================================================================
//当たり判定用球体表示処理[CHitCheckSphere.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _HITCHECKSPHERE_H_
#define _HITCHECKSPHERE_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "Scene3D.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//当たり判定用球体クラス
class CHitCheckSphere : public CScene3D
{
	//外部
	public:
		CHitCheckSphere();									//コンストラクタ
		~CHitCheckSphere();									//デストラクタ
		HRESULT Init(float fHalfSize);						//初期化
		void Uninit();										//終了
		void Update();										//更新
		void Draw();										//描画
		static CHitCheckSphere *Create(float fHalfSize);	//球体インスタンス生成
		void SetPos(D3DXVECTOR3 pos);						//座標セット
	//内部
	private:
		float m_fHalfSize;									//半径
		float m_fSideRot;									//頂点の横との間の角度
		float m_fLengthRot;									//頂点の縦との間の角度
		bool m_bDispSphere;									//表示フラグ
};

#endif
//EOF