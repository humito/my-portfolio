//=============================================================================
// キューブマップ [CubeMap.h]
// Author : 木村 文登
//=============================================================================
#ifndef _CUBE_MAP_H_
#define _CUBE_MAP_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "Shader.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//キューブマップクラス
class CCubeMap : public CShader
{
public:
	CCubeMap();									//コンストラクタ
	~CCubeMap(){}								//デストラクタ
	void Load();								//読込
	void Uninit();								//終了
	void Begin(LPDIRECT3DDEVICE9 pDevice);		//シェーダー開始
	void End(LPDIRECT3DDEVICE9 pDevice);		//シェーダー終了

	void SetMatrix(LPDIRECT3DDEVICE9 pDevice,	//マトリックスの設定
		D3DXMATRIX *pMtxWorld);
private:
	LPDIRECT3DCUBETEXTURE9	m_pD3DCubeMap;		//キューブテクスチャ
};
#endif
//EOF