//=============================================================================
//剛体立方体[Cube.cpp]
//Author : HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Cube.h"
#include "Mesh/MeshField.h"
#include "../manager.h"
#include "../System/renderer.h"
#include "../System/Common.h"
#include "../Shader/BumpMap.h"
#include "../Shader/Fur.h"
#include "../Shader/ToonShader.h"

#ifdef _DEBUG
	#include "../System/DebugProc.h"
#endif

//*****************************************************************************
//定数定義
//*****************************************************************************
#define GRAVITY_CONST_SECOND (9.8f)
#define MASS (50.0f)
#define MOI (20000.0f)
#define MOVE_RESIST (0.5f)
#define ROT_RESIST (0.5f)
#define TIME_UNTIL_EXIT (1000)

//=============================================================================
//初期化
//=============================================================================
HRESULT CCube::Init(char *FileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//レンダラー取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	m_UseShaderID = (USE_SHADER_ID)(rand() % USE_SHADER_MAX);

	//使用するシェーダーに応じてシェーダーの取得

	if (m_UseShaderID == USE_BUMP)
		m_pUseShader = pRenderer->GetBumpMap();
	else if (m_UseShaderID == USE_FUR)
		m_pUseShader = pRenderer->GetFur();
	else if (m_UseShaderID == USE_TOON)
		m_pUseShader = pRenderer->GetToon();

	//経過時間
	m_nTime = 0;

	//立方体の種類とする
	m_type = RB_BOX_TYPE;

	//座標・角度・スケール設定
	m_pos = pos;
	m_rot = rot;
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//速度
	m_MovVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_RotVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//質量
	m_fMass = MASS;

	m_fMOI = MOI;

	//移動抵抗
	m_fMovResist = MOVE_RESIST;

	//回転抵抗
	m_fRotResist = ROT_RESIST;

	//空気抵抗
	m_fAirResist = 0.1f;

	//地面抵抗
	m_fGroundResist = 2.0f;

	//摩擦抵抗
	m_fFricResist = -2.8f;

	//回転角初期化
	D3DXQuaternionRotationYawPitchRoll(	&m_Quaternion,
										m_rot.y, m_rot.x, m_rot.z);

	//立方体の8頂点の座標設定
	m_RBPoint[0].pos = D3DXVECTOR3(-50.0f, 50.0f, 50.0f);
	m_RBPoint[1].pos = D3DXVECTOR3(-50.0f, 50.0f, -50.0f);
	m_RBPoint[2].pos = D3DXVECTOR3(50.0f, 50.0f, 50.0f);
	m_RBPoint[3].pos = D3DXVECTOR3(50.0f, 50.0f, -50.0f);

	m_RBPoint[4].pos = D3DXVECTOR3(-50.0f, 0.0f, 50.0f);
	m_RBPoint[5].pos = D3DXVECTOR3(-50.0f, 0.0f, -50.0f);
	m_RBPoint[6].pos = D3DXVECTOR3(50.0f, 0.0f, 50.0f);
	m_RBPoint[7].pos = D3DXVECTOR3(50.0f, 0.0f, -50.0f);

	for (int i = 0; i < RB_POINT_NUM; ++i)
	{
		D3DXVec3TransformCoord(	&m_RBPoint[i].newPos,
								&m_RBPoint[i].pos,
								&m_mtxWorld);

		m_RBPoint[i].oldPos = m_RBPoint[i].newPos;
	}

	//Xファイルの生成
	CSceneX::CreateModel(FileName);

	//テクスチャ読込
	RELEASE_OBJECT(m_pD3DTexture1);
	D3DXCreateTextureFromFile(pDevice,
							"data/TEXTURE/sponge.jpg",
							&m_pD3DTexture1);

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CCube::Uninit()
{
	//終了処理
	CSceneX::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CCube::Update()
{
	//重力 ※9.8は1秒ごとの数値なので調整がいる
	D3DXVECTOR3 gravity = D3DXVECTOR3(0.0f, -(GRAVITY_CONST_SECOND / 60.0f) * m_fMass, 0.0f);
	//力の加算
	AddForce(gravity);

	//空気抵抗の力
	D3DXVECTOR3 resist = -(m_MovVelocity * m_fMovResist);
	AddForce(resist);

	//座標更新
	m_pos = m_pos + m_MovVelocity;

	m_originPoint = m_pos;
	m_originPoint.y += 50.0f;

	//回転抵抗から回転速度計算
	m_RotVelocity -= m_RotVelocity * m_fRotResist;

	//角速度の計算から角度を算出
	D3DXQUATERNION quat;
	float angle = D3DXVec3Length(&m_RotVelocity);
	D3DXQuaternionRotationAxis(&quat, &m_RotVelocity, angle);
	m_Quaternion = m_Quaternion * quat;

	//ワールドマトリクスの算出
	CalcWorldMtx();

	//ローカルの頂点座標をワールド行列を元に変換
	for (int i = 0; i < RB_POINT_NUM; ++i)
	{
		m_RBPoint[i].oldPos = m_RBPoint[i].newPos;
		D3DXVec3TransformCoord(	&m_RBPoint[i].newPos,
								&m_RBPoint[i].pos, &m_mtxWorld);
	}

	//オブジェクトとの当たり判定
	HitCheck();

	//一定の経過時間を過ぎると終了
	m_nTime++;
	if (m_nTime > TIME_UNTIL_EXIT)
		Uninit();
}
//=============================================================================
//ワールドマトリクスの算出
//=============================================================================
void CCube::CalcWorldMtx()
{
	//ワールドマトリクスの設定
	//サイズ,回転,位置
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//プロジェクションマトリックスを反映
	D3DXMatrixIdentity(&m_mtxWorld);

	//サイズを設定
	D3DXMatrixScaling(&mtxScl,
		m_scl.x,
		m_scl.y,
		m_scl.z);

	//サイズを反映
	D3DXMatrixMultiply(&m_mtxWorld,
		&m_mtxWorld,
		&mtxScl);

	//回転を設定
	D3DXMatrixRotationQuaternion(&mtxRot, &m_Quaternion);
	D3DXMatrixMultiply(	&m_mtxWorld,
						&m_mtxWorld,
						&mtxRot);

	//位置の設定
	D3DXMatrixTranslation(&mtxTranslate,
							m_pos.x,
							m_pos.y,
							m_pos.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
		&mtxTranslate);
}
//=============================================================================
//オブジェクトのとの当たり判定
//=============================================================================
void CCube::HitCheck()
{
	//戻す力
	D3DXVECTOR3 force;

	//オブジェクトと当った場合戻す力を加える
	CScene *pScene = CScene::GetListTop(1);
	while (pScene)
	{
		if (pScene->GetType() == RB_BOX_TYPE
		&&	pScene != this)
		{
			CCube *box = (CCube*)pScene;

			//対象の原点(中心座標)
			D3DXVECTOR3 boxPos = box->GetPos();
			boxPos.y += 50.0f;

			//オブジェクト同士の距離
			D3DXVECTOR3 vec = boxPos - m_originPoint;
			float len = D3DXVec3LengthSq(&vec);

			//(対象との距離 < (自身の半径 + 対象の半径)の2乗)
			if (len < (100.0f * 100.0f))
			{
				//D3DXVec3Normalize(&vec, &vec);

				//抵抗の計算
				force = vec * 0.5f;

				//作用反作用
				box->AddForce(force);
				AddForce(-force);
			}
		}

		//次ポインタ取得
		pScene = pScene->GetNext();
	}

	//地面に当たった時の座標
	D3DXVECTOR3 hitPos;

	//地面の高さ
	float fHeight = 0.0f;

	//地面にめり込んだら上に戻す力を加える
	for (int i = 0; i < RB_POINT_NUM; ++i)
	{
		if (m_RBPoint[i].newPos.y < fHeight)
		{
			//当った頂点座標
			hitPos = m_RBPoint[i].newPos;
			hitPos.y = 0.0f;

			//地面抵抗の計算
			force = (hitPos - m_RBPoint[i].newPos) * m_fGroundResist;

			//摩擦の計算
			force += (m_RBPoint[i].newPos - m_RBPoint[i].oldPos) * m_fFricResist;

			AddForce(force, m_RBPoint[i].newPos);
		}
	}
}
//=============================================================================
//描画
//=============================================================================
void CCube::Draw()
{
	//レンダラーの取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//マテリアル関係取得
	D3DMATERIAL9 matDef;
	pDevice->GetMaterial(&matDef);
	m_pD3DXMat = (D3DXMATERIAL*)m_pD3DXBuffMatModel->GetBufferPointer();

	//テクスチャセット
	pDevice->SetTexture(0, m_pD3DTexture1);

	//ファーシェーダー以外の描画
	if (m_UseShaderID != USE_FUR)
	{
		//シェーダー開始
		m_pUseShader->Begin(pDevice);
		m_pUseShader->SetMatrix(pDevice, &m_mtxWorld);

		for (int nCntMat = 0; nCntMat < (int)m_nNumMatModel; nCntMat++)
		{
			D3DXVECTOR4 diffuse = D3DXVECTOR4(m_pD3DXMat[nCntMat].MatD3D.Diffuse.r,
				m_pD3DXMat[nCntMat].MatD3D.Diffuse.g,
				m_pD3DXMat[nCntMat].MatD3D.Diffuse.b,
				m_pD3DXMat[nCntMat].MatD3D.Diffuse.a);

			m_pUseShader->SetMaterial(pDevice, diffuse);

			//ポリゴンの描画
			m_pD3DXMeshModel->DrawSubset(nCntMat);
		}

		//シェーダー終了
		m_pUseShader->End(pDevice);
	}
	//ファーシェーダーでの描画
	else
	{
		CFur *pFur = (CFur*)m_pUseShader;
		pFur->Begin(pDevice);
		pFur->SetMatrix(pDevice, &m_mtxWorld, m_pos);
		for (int i = 0; i < 40; ++i)
		{
			pFur->SetOffset(pDevice, i * 0.02f);
			for (int nCntMat = 0; nCntMat < (int)m_nNumMatModel; nCntMat++)
			{
				D3DXVECTOR4 diffuse = D3DXVECTOR4(m_pD3DXMat[nCntMat].MatD3D.Diffuse.r,
					m_pD3DXMat[nCntMat].MatD3D.Diffuse.g,
					m_pD3DXMat[nCntMat].MatD3D.Diffuse.b,
					m_pD3DXMat[nCntMat].MatD3D.Diffuse.a);

				pFur->SetMaterial(pDevice, diffuse);

				//ポリゴンの描画
				m_pD3DXMeshModel->DrawSubset(nCntMat);
			}
		}
		pFur->End(pDevice);
	}
}
//=============================================================================
//力の加算
//=============================================================================
void CCube::AddForce(D3DXVECTOR3 force)
{
	//加速度の計算(加速度 = 力 / 質量)
	D3DXVECTOR3 acceleration;
	acceleration = force / m_fMass;
	//加速度から速度の加算
	m_MovVelocity += acceleration;
}
//=============================================================================
//力の加算
//=============================================================================
void CCube::AddForce(D3DXVECTOR3 force, D3DXVECTOR3 pos)
{
	//加速度の計算(加速度 = 力 / 質量)
	D3DXVECTOR3 acceleration;
	acceleration = force / m_fMass;
	//加速度から速度の加算
	m_MovVelocity += acceleration;

	//回転の加速度の計算で回転速度を計算(回転加速度 = トルク / 慣性モーメント)
	D3DXVECTOR3 vec, torq, rotacc;
	vec = pos - m_originPoint;
	D3DXVec3Cross(&torq, &vec, &force);
	rotacc = torq / m_fMOI;
	m_RotVelocity += rotacc;
}
//=============================================================================
//その他オブジェクトインスタンス生成
//=============================================================================
CCube *CCube::Create(char *FileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//オブジェクトインスタンス生成	
	CCube *pObject = new CCube();

	//初期化
	pObject->Init(FileName, pos, rot);

	return pObject;
}
//EOF