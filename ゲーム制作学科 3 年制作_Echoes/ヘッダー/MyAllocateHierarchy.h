//=============================================================================
//自作フレーム階層処理[MyAllocateHierarchy.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _MYALLOCATEHIERARCHY_H_
#define _MYALLOCATEHIERARCHY_H_
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <Windows.h>
#include "renderer.h"
//*****************************************************************************
//クラス定義
//*****************************************************************************
//フレーム階層クラス
class CAllocateHierarchy : public ID3DXAllocateHierarchy
{
	//外部
	public:
		CAllocateHierarchy();	//コンストラクタ
		~CAllocateHierarchy();	//デストラクタ

		//フレームの生成
		STDMETHOD(CreateFrame)(THIS_ LPCSTR Name,LPD3DXFRAME *ppNewFrame);

		//各メッシュ情報の生成
		STDMETHOD(CreateMeshContainer)(THIS_
		LPCSTR Name,
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,
		CONST D3DXEFFECTINSTANCE *pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD *pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER *ppNewMeshContainer);

		//ボーン行列の生成
		HRESULT SetupBoneMatrixPointers(LPD3DXFRAME pFrame);

		//フレームの解放
		STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);

		//メッシュ情報の解放
		STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree);

		//フレームの描画
		void DrawFrame(LPDIRECT3DDEVICE9 device,LPD3DXFRAME pFrameBase);

		//フレームのマトリクス変換
		void MatricesFrame(LPD3DXFRAME pFrameBase,LPD3DXMATRIX pParentMatrix);

		//フレームのトップノードセット
		void SetFrameRoot(LPD3DXFRAME pFrame);

	//内部
	private:

		//フレームのトップノード
		LPD3DXFRAME m_pFrameRoot;

		//メッシュフレーム構造体
		struct CMeshFrame : public D3DXFRAME
		{
			//結合後の変換行列
			D3DXMATRIX CombinedTransformationMatrix;
		};

		//メッシュコンテナ構造体
		struct CMeshContainer : public D3DXMESHCONTAINER
		{
			LPDIRECT3DTEXTURE9* ppTextures;		//テクスチャポインタ
			DWORD weight;						//重みの個数（重みとは頂点への影響。）
			DWORD cntBone;						//ボーン数
			LPD3DXBUFFER pBoneBuffer;			//ボーンテーブル
			D3DXMATRIX** ppMatRoot;				//全てのボーンのワールド行列の先頭ポインタ
			D3DXMATRIX* pMatOffset;				//ボーンのオフセット行列
		};

		//ボーン行列の初期化
		HRESULT SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainer);

		//フレーム内メッシュ描画
		void DrawFrameOnMesh(LPDIRECT3DDEVICE9 device,CMeshContainer *pContainer,CMeshFrame *pFrame);
};
#endif
//EOF