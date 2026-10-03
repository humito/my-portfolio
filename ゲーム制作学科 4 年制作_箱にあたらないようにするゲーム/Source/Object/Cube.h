//=============================================================================
//剛体立方体[Cube.h]
//Author : HUMITO KIMURA
//=============================================================================
#ifndef _CUBE_H_
#define _CUBE_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "../Scene/SceneX.h"

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CShader;

//*****************************************************************************
//定数定義
//*****************************************************************************
#define RB_POINT_NUM (8)

//*****************************************************************************
//列挙型定義
//*****************************************************************************
//使用するシェーダー
enum USE_SHADER_ID
{
	USE_BUMP = 0,
	USE_FUR,
	USE_TOON,
	USE_SHADER_MAX
};

//*****************************************************************************
//構造体定義
//*****************************************************************************
//リジットボディ(剛体)
struct RB_POINT
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 newPos;
	D3DXVECTOR3 oldPos;
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
class CCube : public CSceneX
{
	//外部
	public:
		CCube(){}							//コンストラクタ
		~CCube(){}							//デストラクタ

		//初期化
		HRESULT Init(char *FileName,
					D3DXVECTOR3 pos,
					D3DXVECTOR3 rot);
		void Uninit();							//終了
		void Update();							//更新
		void Draw();							//描画

		//インスタンス生成
		static CCube *Create(char *FileName,
			D3DXVECTOR3 pos,
			D3DXVECTOR3 rot);

	//内部
	private:
		float			m_fMass;		//質量
		float			m_fMOI;			//慣性モーメント
		float			m_fMovResist;	//移動抵抗
		float			m_fRotResist;	//回転抵抗
		float			m_fAirResist;	//空気抵抗
		float			m_fGroundResist;//地面抵抗
		float			m_fFricResist;	//摩擦抵抗

		int				m_nTime;		//経過時間

		D3DXVECTOR3		m_MovVelocity;	//速度
		D3DXVECTOR3		m_RotVelocity;	//回転速度
		D3DXVECTOR3		m_originPoint;	//原点
		D3DXQUATERNION	m_Quaternion;	//角度

		USE_SHADER_ID	m_UseShaderID;	//使用するシェーダーID
		CShader			*m_pUseShader;	//使用するシェーダー

		//頂点のポイント
		RB_POINT		m_RBPoint[RB_POINT_NUM];

		//ワールドマトリクスの算出
		void CalcWorldMtx();

		//オブジェクトのとの当たり判定
		void HitCheck();

		//力の加算
		void AddForce(D3DXVECTOR3 force);
		void AddForce(D3DXVECTOR3 force, D3DXVECTOR3 pos);
};
#endif
//EOF