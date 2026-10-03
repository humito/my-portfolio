//=============================================================================
//プレイヤー処理[Player.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Player.h"
#include "../manager.h"
#include "../main.h"
#include "../State/Game.h"
#include "../Object/Mesh/MeshField.h"
#include "../Object/Image.h"
#include "../System/HitCheck.h"
#include "../System/renderer.h"
#include "../System/Camera.h"
#include "../System/Input/InputKeyboard.h"
#include "../Shader/ToonShader.h"
#include "../Shader/Fade.h"

#ifdef _DEBUG
	#include "../System/DebugProc.h"
#endif

//*****************************************************************************
//定数定義
//*****************************************************************************
#define PLAYER_MODEL_NAME ("data/MODEL/forklift.x")	//プレイヤーファイル名
#define POS_MOVE (1.5f)								//プレイヤー移動量
#define PLAYER_HALF_SIZE (18.0f)					//プレイヤーの半径
#define VELOCITY_RATE (0.30f)						//移動量の割合
#define DIFF_ROT_RATE_Y (0.05f)						//回転の差分の割合
#define FIELD_LIMIT_SIZE (270.0f)					//フィールド範囲内のサイズ
#define FIELD_SIZE (FIELD_LIMIT_SIZE * 2)			//フィールド全体のサイズ

//=============================================================================
//コンストラクタ
//=============================================================================
CPlayer::CPlayer()
{
	m_type = PLAYER_TYPE;
}
//=============================================================================
//初期化
//=============================================================================
HRESULT CPlayer::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//座標・角度・スケール設定
	m_pos = pos;
	m_rot = rot;
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//プレイヤー移動量
	m_velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//目的の角度
	m_fDestRotY = 0.0f;

	//シェーダーキャストの種類
	m_castType = CAST_NONE;

	//Xファイルの生成
	if (FAILED(CreateModel(PLAYER_MODEL_NAME)))
		return E_FAIL;

	//マテリアル関係取得
	m_pD3DXMat = (D3DXMATERIAL*)m_pD3DXBuffMatModel->GetBufferPointer();

	return S_OK;
}
//=============================================================================
//終了
//=============================================================================
void CPlayer::Uninit()
{
	//終了処理
	CSceneX::Uninit();
}
//=============================================================================
//更新
//=============================================================================
void CPlayer::Update()
{
	//モデルの移動
	MoveModel();

	//当たり判定
	HitCheck();
}
//=============================================================================
//描画
//=============================================================================
void CPlayer::Draw()
{
	//レンダラーの取得
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスのゲット
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//マテリアルプロパティ
	D3DMATERIAL9 matDef;
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;//サイズ,回転,位置

	//プロジェクションマトリックスを反映
	D3DXMatrixIdentity(&m_mtxWorld);

	//サイズ設定
	D3DXMatrixScaling(&mtxScl,
					m_scl.x,
					m_scl.y,
					m_scl.z);

	//サイズ反映
	D3DXMatrixMultiply(&m_mtxWorld,
					&m_mtxWorld,
					&mtxScl);

	//回転設定
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
									m_rot.y,
									m_rot.x,
									m_rot.z);
	//回転反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
						&mtxRot);

	//位置設定
	D3DXMatrixTranslation(&mtxTranslate,
							m_pos.x,
							m_pos.y,
							m_pos.z);

	//位置反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld,
						&mtxTranslate);

	//トゥーンシェーダー使用
	m_pUseShader = pRenderer->GetToon();
	m_pUseShader->SetMatrix(pDevice, &m_mtxWorld);
	m_pUseShader->Begin(pDevice);

	//マテリアルプロパティの取得
	pDevice->GetMaterial(&matDef);

	//テクスチャセット
	pDevice->SetTexture(0, m_pD3DTexture1);

	//ポリゴンの描画ループ
	for (int nCntMat = 0; nCntMat < (int)m_nNumMatModel; ++nCntMat)
	{
		//シェーダーにマテリアル情報のセット
		D3DXVECTOR4 color = D3DXVECTOR4(m_pD3DXMat[nCntMat].MatD3D.Diffuse.r,
										m_pD3DXMat[nCntMat].MatD3D.Diffuse.g,
										m_pD3DXMat[nCntMat].MatD3D.Diffuse.b,
										m_pD3DXMat[nCntMat].MatD3D.Diffuse.a);

		m_pUseShader->SetMaterial(pDevice, color);

		//ポリゴンの描画
		m_pD3DXMeshModel->DrawSubset(nCntMat);
	}

	//シェーダー完了
	m_pUseShader->End(pDevice);

	//前のワールドマトリクスへのコピー
	CopyMemory(&m_mtxWorldOld, &m_mtxWorld, sizeof(D3DXMATRIX));
}
//=============================================================================
//モデルの移動
//=============================================================================
void CPlayer::MoveModel()
{
	//カメラの向き取得
	D3DXVECTOR3 rot = CManager::GetCamera()->GetRotCamera();

	///////////////////////////////////
	//			操作処理			//
	/////////////////////////////////

	//前
	if (CInputKeyboard::GetKeyPress(DIK_W))
	{
		//移動量加算
		m_velocity.x -= cosf(rot.y + D3DX_PI / 2) * POS_MOVE;
		m_velocity.z += sinf(rot.y + D3DX_PI / 2) * POS_MOVE;
		//目的の向き
		m_fDestRotY = rot.y + D3DX_PI;

		//前左
		if (CInputKeyboard::GetKeyPress(DIK_A))
		{
			m_velocity.x -= sinf(rot.y + D3DX_PI / 2) * POS_MOVE;
			m_velocity.z -= cosf(rot.y + D3DX_PI / 2) * POS_MOVE;

			m_fDestRotY = rot.y + (D3DX_PI / 4) * 3;
		}
		//前右
		else if (CInputKeyboard::GetKeyPress(DIK_D))
		{
			m_velocity.x += sinf(rot.y + D3DX_PI / 2) * POS_MOVE;
			m_velocity.z += cosf(rot.y + D3DX_PI / 2) * POS_MOVE;

			m_fDestRotY = rot.y - (D3DX_PI / 4) * 3;
		}
	}

	//後
	else if (CInputKeyboard::GetKeyPress(DIK_S))
	{
		//移動量加算
		m_velocity.x += cosf(rot.y + D3DX_PI / 2) * POS_MOVE;
		m_velocity.z -= sinf(rot.y + D3DX_PI / 2) * POS_MOVE;
		//目的の向き
		m_fDestRotY = rot.y;

		//後左
		if (CInputKeyboard::GetKeyPress(DIK_A))
		{
			m_velocity.x -= sinf(rot.y + D3DX_PI / 2) * POS_MOVE;
			m_velocity.z -= cosf(rot.y + D3DX_PI / 2) * POS_MOVE;

			m_fDestRotY = rot.y + D3DX_PI / 4;
		}
		//後右
		else if (CInputKeyboard::GetKeyPress(DIK_D))
		{
			m_velocity.x += sinf(rot.y + D3DX_PI / 2) * POS_MOVE;
			m_velocity.z += cosf(rot.y + D3DX_PI / 2) * POS_MOVE;

			m_fDestRotY = rot.y - D3DX_PI / 4;
		}
	}

	//左
	else if (CInputKeyboard::GetKeyPress(DIK_A))
	{
		//移動量加算
		m_velocity.x -= sinf(rot.y + D3DX_PI / 2) * POS_MOVE;
		m_velocity.z -= cosf(rot.y + D3DX_PI / 2) * POS_MOVE;
		//目的の向き
		m_fDestRotY = rot.y + D3DX_PI / 2;
	}

	//右
	else if (CInputKeyboard::GetKeyPress(DIK_D))
	{
		//移動量加算
		m_velocity.x += sinf(rot.y + D3DX_PI / 2) * POS_MOVE;
		m_velocity.z += cosf(rot.y + D3DX_PI / 2) * POS_MOVE;
		//目的の向き
		m_fDestRotY = rot.y - D3DX_PI / 2;
	}

	///////////////////////////////
	//		座標・角度更新		//
	/////////////////////////////

	//向きの旋回
	//目的の向きと現在の向きの角度の差
	float fDiffRotY = m_fDestRotY - m_rot.y;

	//差分補正
	//右上 270°
	if (fDiffRotY > D3DX_PI)
	{
		fDiffRotY = fDiffRotY - D3DX_PI * 2;//360°減算
	}
	//右上 -270°
	if (fDiffRotY < -D3DX_PI)
	{
		fDiffRotY = fDiffRotY + D3DX_PI * 2;//360°加算
	}

	//現在の角度を加算
	m_rot.y += fDiffRotY * DIFF_ROT_RATE_Y;

	//現在の角度補正
	if (m_rot.y > D3DX_PI)
	{
		m_rot.y = -D3DX_PI;
	}

	if (m_rot.y < -D3DX_PI)
	{
		m_rot.y = D3DX_PI;
	}

	//プレイヤーの座標更新
	m_pos += m_velocity;
	m_velocity -= m_velocity * VELOCITY_RATE;
}
//=============================================================================
//モデルの当たり判定
//=============================================================================
void CPlayer::HitCheck()
{
	//箱との当たり判定
	CSceneX *pModel = (CSceneX*)CScene::GetListTop(PRIORITY_MODEL);
	while (pModel)
	{
		if (pModel->GetType() == RB_BOX_TYPE
		&&	EllipsCheck(m_pos, pModel->GetPos(), PLAYER_HALF_SIZE, 50.0f))
		{
			//メッセージ画像の生成
			D3DXVECTOR3 pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2.0f, 0.0f);
			CImage::Create("data/TEXTURE/hit_message.png", pos,
							SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
			//リザルトへ遷移
			CManager::GetRenderer()->GetFade()->StartFade(RESULT_STATE);
		}

		//次ポインタへ移動
		pModel = (CSceneX*)pModel->GetNext();
	}

	//フィールドの範囲外との当たり判定
	//右と左
	if (m_pos.x > FIELD_LIMIT_SIZE)
		m_pos.x = FIELD_LIMIT_SIZE;
	else if (m_pos.x < -FIELD_LIMIT_SIZE)
		m_pos.x = -FIELD_LIMIT_SIZE;

	//前と後ろ
	if (m_pos.z > FIELD_LIMIT_SIZE)
		m_pos.z = FIELD_LIMIT_SIZE;
	else if (m_pos.z < -FIELD_LIMIT_SIZE)
		m_pos.z = -FIELD_LIMIT_SIZE;

	//Y座標をフィールドの高さに合わせる
	m_pos.y = CGame::GetField()->GetHeight(m_pos);
}
//=============================================================================
//インスタンスの生成
//=============================================================================
CPlayer *CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//インスタンス生成して初期化
	CPlayer *pPlayer = new CPlayer();
	pPlayer->Init(pos, rot);

	return pPlayer;
}
//EOF