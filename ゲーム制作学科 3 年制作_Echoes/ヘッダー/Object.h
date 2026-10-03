//=============================================================================
// 表示用オブジェクト [Object.h]
// Author : HUMITO KIMURA
//=============================================================================
#ifndef _OBJECT_H_
#define _OBJECT_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "SceneX.h"

//*****************************************************************************
//クラス定義
//*****************************************************************************
//表示用オブジェクトクラス
class CObject : public CSceneX
{
	//外部
	public:
		CObject(){}
		~CObject(){}
		HRESULT Init(char *pFileName,
					D3DXVECTOR3 pos,
					D3DXVECTOR3 rot);
		void Uninit();
		void Update();
		void Draw();
		static void Create(	char *pFileName,
							D3DXVECTOR3 pos,
							D3DXVECTOR3 rot);

	//内部
	private:
		LPDIRECT3DTEXTURE9 m_pD3DTexture;
};

#endif
//EOF