//=============================================================================
//自作フレーム階層処理[MyAllocateHierarchy.cpp]
//Author:HUMITO KIMURA
//=============================================================================
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#define _CRT_SECURE_NO_WARNINGS//警告対策用
#include "MyAllocateHierarchy.h"
#include "manager.h"

//=============================================================================
//コンストラクタ
//=============================================================================
CAllocateHierarchy::CAllocateHierarchy()
{
}
//=============================================================================
//デストラクタ
//=============================================================================
CAllocateHierarchy::~CAllocateHierarchy()
{
}
//=============================================================================
//フレームの生成
//=============================================================================
HRESULT CAllocateHierarchy::CreateFrame(THIS_ LPCSTR Name,LPD3DXFRAME *ppNewFrame)
{
	//フレームを新しく生成
	CMeshFrame *pFrame = new CMeshFrame;

	//フレームがない場合
	if(pFrame==NULL)
	{
		//メモリがないことを返す
		return E_OUTOFMEMORY;
	}

	ZeroMemory(pFrame,sizeof(CMeshFrame));	//初期化
	pFrame->Name=new char[strlen(Name) + 1];//文字列動的確保
	strcpy(pFrame->Name, Name);				//フレーム名をコピー

	//行列の初期化
	D3DXMatrixIdentity(&pFrame->TransformationMatrix);
	//最終ワールド行列の初期化
	D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

	pFrame->pMeshContainer=NULL;	//メッシュコンテナのポインタ
	pFrame->pFrameSibling=NULL;		//兄弟フレーム
	pFrame->pFrameFirstChild=NULL;	//子フレーム

	//初期化したフレームを引数のポインタにセット
	*ppNewFrame = pFrame;

	return D3D_OK;
}
//=============================================================================
//各メッシュ情報の生成
//=============================================================================
HRESULT CAllocateHierarchy::CreateMeshContainer(THIS_
		LPCSTR Name,								//ファイル名
		CONST D3DXMESHDATA *pMeshData,				//メッシュ
		CONST D3DXMATERIAL *pMaterials,				//マテリアル
		CONST D3DXEFFECTINSTANCE *pEffectInstances,	//エフェクト
		DWORD NumMaterials,							//マテリアル数
		CONST DWORD *pAdjacency,					//隣接ポリゴンインデックス
		LPD3DXSKININFO pSkinInfo,					//スキン情報
		LPD3DXMESHCONTAINER *ppNewMeshContainer)	//格納先メッシュコンテナ情報
{
	//マネージャーからレンダラーの取得
	CRenderer *pRenderer = CManager::GetRenderer();
	//レンダラーからデバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//メッシュコンテナオブジェクトの生成
	CMeshContainer *pMeshContainer = new CMeshContainer;

	///////////////////////////////
	//		名前の初期化		//
	/////////////////////////////

	//文字列の生成
	pMeshContainer->Name = new char[ strlen(Name) + 1 ];
	//初期化
	strcpy(pMeshContainer->Name,Name);

	///////////////////////////////////////
	//		メッシュ情報の初期化		//
	/////////////////////////////////////

	//メッシュ情報の代入
	pMeshContainer->MeshData = *pMeshData;

	//メッシュタイプの代入
	pMeshContainer->MeshData.Type=pMeshData->Type;

	//メッシュの種類によってそれぞれのメッシュ情報を受け取る
	switch(pMeshData->Type)
	{
		//通常メッシュ
		case D3DXMESHTYPE_MESH:
			//通常メッシュを受け取る
			pMeshContainer->MeshData.pMesh = pMeshData->pMesh;
			//参照カウンタを加算
			pMeshContainer->MeshData.pMesh->AddRef();
		break;

		//プログレッシブメッシュ
		case D3DXMESHTYPE_PMESH:
			//プログレッシブメッシュを受け取る
			pMeshContainer->MeshData.pPMesh = pMeshData->pPMesh;
			//参照カウンタを加算
			pMeshContainer->MeshData.pPMesh->AddRef();
		break;

		//パッチメッシュ
		case D3DXMESHTYPE_PATCHMESH:
			//パッチメッシュを受け取る
			pMeshContainer->MeshData.pPatchMesh = pMeshData->pPatchMesh;
			//参照カウンタを加算
			pMeshContainer->MeshData.pPatchMesh->AddRef();
		break;
	}

	///////////////////////////////////
	//		マテリアルの初期化		//
	/////////////////////////////////

	//マテリアル数を代入
	pMeshContainer->NumMaterials = NumMaterials;

	//配列の確保
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];

	//テクスチャポインタへのポインタ確保
	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	//テクスチャポインタをマテリアル数分初期化
	memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9)*pMeshContainer->NumMaterials);

	//マテリアル数分ループ
	for(unsigned int i=0;i<pMeshContainer->NumMaterials;i++)
	{
		//マテリアル情報を代入
		pMeshContainer->pMaterials[i].MatD3D = pMaterials[i].MatD3D;

		///////////////////////////////////
		//		テクスチャの設定		//
		/////////////////////////////////

		//マテリアルのテクスチャファイル名が入ってる場合
		if(pMaterials[i].pTextureFilename != NULL)
		{
			//テクスチャファイル名文字列を文字数分確保
			pMeshContainer->pMaterials[i].pTextureFilename = new char[strlen(pMaterials[i].pTextureFilename)+1];
			//テクスチャファイル名をコピー
			strcpy(pMeshContainer->pMaterials[i].pTextureFilename,pMaterials[i].pTextureFilename);
		}
		else
		{
			//テクスチャファイル名を空にする
			pMeshContainer->pMaterials[i].pTextureFilename = NULL;
		}

		//メッシュにテクスチャ名がある場合
		if(pMeshContainer->pMaterials[i].pTextureFilename)
		{
			//テクスチャファイル名をコピー
			TCHAR strTexturePath[MAX_PATH];
			strcpy(strTexturePath,pMeshContainer->pMaterials[i].pTextureFilename);

			//テクスチャを読み込む
			if(FAILED(D3DXCreateTextureFromFile(
				pDevice,
				strTexturePath,
				&pMeshContainer->ppTextures[i])))
			{
				//読込できない場合はNULLを入れる
				pMeshContainer->ppTextures[i]=NULL;
			}
		}
	}

	///////////////////////////////////////
	//		エフェクト情報の初期化		//
	/////////////////////////////////////

	//変数名省略
	const D3DXEFFECTINSTANCE *pEI = pEffectInstances;

	//エフェクト情報を生成
	pMeshContainer->pEffects = new D3DXEFFECTINSTANCE;

	//エラーが起きた場合はCopyStr関数を作成してUnicodeに対応する
	//エフェクト名代入
	pMeshContainer->pEffects->pEffectFilename = pEI->pEffectFilename;
	
	//エフェクト数代入
	pMeshContainer->pEffects->NumDefaults = pEI->NumDefaults;
	
	//デフォルトを確保
	pMeshContainer->pEffects->pDefaults = new D3DXEFFECTDEFAULT[pEI->NumDefaults];

	D3DXEFFECTDEFAULT *pDIST = pEI->pDefaults;						//コピー元
	D3DXEFFECTDEFAULT *pCOPY = pMeshContainer->pEffects->pDefaults;	//コピー先

	//デフォルト数分ループ
	for(unsigned int i=0; i<pEI->NumDefaults; i++)
	{
		//パラメーター名代入
		pCOPY[i].pParamName=pDIST[i].pParamName;

		//バイト数代入
		DWORD NumBytes=pCOPY[i].NumBytes=pDIST[i].NumBytes;

		//コピー元のパラメーター情報を代入
		pCOPY[i].Type=pDIST[i].Type;

		//コピー元のエフェクトの種類がD3DXEDT_FORCEDWORD以外なら
		if(pDIST[i].Type<=D3DXEDT_DWORD)
		{
			//バイト数分確保
			pCOPY[i].pValue=new DWORD[NumBytes];
			//コピー元にあるバイトを指すポインタをコピーする
			memcpy(pCOPY[i].pValue,pDIST[i].pValue,NumBytes);
		}
	}


	///////////////////////////////////////////////////
	//		隣接ポリゴンインデックスの初期化		//
	/////////////////////////////////////////////////

	//ポリゴン数取得
	DWORD NumPolygon=pMeshData->pMesh->GetNumFaces();

	//配列の確保(ポリゴン数 * 隣接する3つのポリゴン)
	pMeshContainer->pAdjacency=new DWORD[ NumPolygon * 3];

	//コピー
	memcpy( pMeshContainer->pAdjacency, pAdjacency, NumPolygon * 3 * sizeof(DWORD));

	///////////////////////////////////
	//		スキン情報の初期化		//
	/////////////////////////////////

	//スキン情報がある場合は初期化
	if(pSkinInfo!=NULL)
	{
		//スキン情報を代入
		pMeshContainer->pSkinInfo=pSkinInfo;

		//参照カウンタを加算
		pMeshContainer->pSkinInfo->AddRef();

		//ボーン数の取得
		DWORD dwBoneAmt=pMeshContainer->pSkinInfo->GetNumBones();

		//ボーン行列をボーン数分確保
		pMeshContainer->pMatOffset=new D3DXMATRIX[dwBoneAmt];

		//ボーン数分ループ
		for(DWORD i=0;i<dwBoneAmt;i++)
		{
			//ボーン行列をコピー
			memcpy(	&pMeshContainer->pMatOffset[i],
					pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i),
					sizeof(D3DMATRIX));
		}

		//頂点の重みとボーンの組み合わせテーブルを適用した新しいメッシュを生成
		if(FAILED(	pMeshContainer->pSkinInfo->ConvertToBlendedMesh(
					pMeshData->pMesh,					//入力メッシュ
					NULL,
					pMeshContainer->pAdjacency,			//入力メッシュの隣接性情報
					NULL, NULL, NULL,
					&pMeshContainer->weight,			//ボーンの重み
					&pMeshContainer->cntBone,			//ボーンの組み合わせテーブルに含まれるボーンの数へのポインタ
					&pMeshContainer->pBoneBuffer,		//ボーンの組み合わせテーブルへのポインタ
					&pMeshContainer->MeshData.pMesh)))	//新しいメッシュへのポインタ
		{
			return E_FAIL;
		}
	}

	//作成したメッシュコンテナ情報を引数のポインタにセット
	*ppNewMeshContainer=pMeshContainer;

	return S_OK;
}
//=============================================================================
//ボーン行列の生成
//=============================================================================
HRESULT CAllocateHierarchy::SetupBoneMatrixPointers(LPD3DXFRAME pFrame)
{
	//フレームが存在する場合
	if(pFrame->pMeshContainer!=NULL)
	{
		//ボーン行列の初期化を行う
		if(FAILED(SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer)))
		{
			return E_FAIL;
		}
	}

	//兄弟フレームが存在する場合
	if(pFrame->pFrameSibling!=NULL)
	{
		//兄弟フレームからボーン行列生成
		if(FAILED(SetupBoneMatrixPointers(pFrame->pFrameSibling)))
		{
			return E_FAIL;
		}
	}

	//子フレームが存在する場合
	if(pFrame->pFrameFirstChild!=NULL)
	{
		//子フレームからボーン行列生成
		if(FAILED(SetupBoneMatrixPointers(pFrame->pFrameFirstChild)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}
//=============================================================================
//ボーン行列の初期化
//=============================================================================
HRESULT CAllocateHierarchy::SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainer)
{
	//メッシュコンテナをキャスト変換
	CMeshContainer *pContainer=(CMeshContainer*)pMeshContainer;

	//スキン情報がない場合は関数から抜ける
	if(pContainer->pSkinInfo==NULL)
	{
		return S_OK;
	}

	//ボーンの数を取得
	DWORD dwBoneAmt=pContainer->pSkinInfo->GetNumBones();

	//ボーン行列を確保
	pContainer->ppMatRoot=new D3DXMATRIX*[dwBoneAmt];

	//ボーン数分ループ
	for(DWORD i=0;i<dwBoneAmt;i++)
	{
		//ルートフレームの子フレームを検索
		CMeshFrame *pFrame=
			(CMeshFrame*)D3DXFrameFind(
			m_pFrameRoot,
			pContainer->pSkinInfo->GetBoneName(i));

		//子フレームが空なら抜ける
		if(pFrame==NULL)
		{
			return E_FAIL;
		}

		//ボーン行列に設定
		pContainer->ppMatRoot[i]=&pFrame->CombinedTransformationMatrix;
	}

	return S_OK;
}
//=============================================================================
//フレームのマトリクス変換
//=============================================================================
void CAllocateHierarchy::MatricesFrame(LPD3DXFRAME pFrameBase,LPD3DXMATRIX pParentMatrix)
{
	//メッシュフレームにキャスト変換
	CMeshFrame *pFrame=(CMeshFrame*)pFrameBase;

	//親行列が存在する場合
	if(pParentMatrix!=NULL)
	{
		//親行列と自身の行列を合成
		D3DXMatrixMultiply(	&pFrame->CombinedTransformationMatrix,
							&pFrame->TransformationMatrix,
							pParentMatrix);
	}
	else
	{
		//親がない場合は自身の行列をそのまま代入
		pFrame->CombinedTransformationMatrix=pFrame->TransformationMatrix;
	}

	//兄弟フレームが存在する場合
	if(pFrame->pFrameSibling!=NULL)
	{
		//兄弟フレームを合成変換
		MatricesFrame(pFrame->pFrameSibling,pParentMatrix);
	}

	//子フレームが存在する場合
	if(pFrame->pFrameFirstChild!=NULL)
	{
		//子フレームを合成変換
		MatricesFrame(	pFrame->pFrameFirstChild,
						&pFrame->CombinedTransformationMatrix);
	}
}
//=============================================================================
//フレームの描画
//=============================================================================
void CAllocateHierarchy::DrawFrame(LPDIRECT3DDEVICE9 device,LPD3DXFRAME pFrameBase)
{
	//フレームのメッシュ情報をメッシュコンテナへキャスト変換
	CMeshContainer *pContainer=(CMeshContainer*)pFrameBase->pMeshContainer;

	//フレームをメッシュフレームへキャスト変換
	CMeshFrame *pFrame=(CMeshFrame*)pFrameBase;

	//メッシュ情報が存在する間ループ
	while(pContainer!=NULL)
	{
		//フレーム内メッシュを描画
		DrawFrameOnMesh(device,pContainer,pFrame);
		//次のメッシュコンテナのポインタへ移動
		pContainer=(CMeshContainer*)pContainer->pNextMeshContainer;
	}

	//兄弟フレームが存在する場合
	if(pFrameBase->pFrameSibling!=NULL)
	{
		//兄弟フレームを描画
		DrawFrame(device,pFrameBase->pFrameSibling);
	}

	//子フレームが存在する場合
	if(pFrameBase->pFrameFirstChild!=NULL)
	{
		//子フレームを描画
		DrawFrame(device,pFrameBase->pFrameFirstChild);
	}
}
//=============================================================================
//フレーム内メッシュ描画
//=============================================================================
void CAllocateHierarchy::DrawFrameOnMesh(LPDIRECT3DDEVICE9 device,CMeshContainer *pContainer,CMeshFrame *pFrame)
{
	//スキン情報に中身がある場合
	if(pContainer->pSkinInfo!=NULL)
	{
		///////////////////////////////////////////
		//		スキンメッシュによる描画		//
		/////////////////////////////////////////

		//ボーン情報
		LPD3DXBONECOMBINATION pBoneCombination=
			(LPD3DXBONECOMBINATION)pContainer->pBoneBuffer->GetBufferPointer();

		//前のボーンID
		DWORD dwPrevBoneID=UINT_MAX;

		//ボーン数分ループ
		for(unsigned int i=0;i<pContainer->cntBone;i++)
		{
			//マトリックスブレンド用
			DWORD dwBlendMatrixAmt=0;

			//ウェイト値の数値分ループ
			for(unsigned int j=0;j<pContainer->weight;j++)
			{
				//各ボーン情報IDが最大値でない場合
				if(pBoneCombination[i].BoneId[j] != UINT_MAX) 
				{
					//ブレンド行列にカウンタを入れる
					dwBlendMatrixAmt=j;
				}
			}

			//ジオメトリブレンディングを実行するために使う行列の個数を設定
			device->SetRenderState(D3DRS_VERTEXBLEND,dwBlendMatrixAmt);

			//ウェイト値の数値分ループ
			for(unsigned int j=0;j<pContainer->weight;j++)
			{
				//ボーンIDをマトリックスのインデックスに格納
				UINT iMatrixIndex = pBoneCombination[i].BoneId[j];
				//マトリックスインデックスが最大値でない場合
				if(iMatrixIndex != UINT_MAX)
				{
					//オフセット行列とボーン行列を掛ける
					D3DXMATRIX matStack=pContainer->pMatOffset[iMatrixIndex]*(*pContainer->ppMatRoot[iMatrixIndex]);
					//行列スタックに格納
					device->SetTransform(D3DTS_WORLDMATRIX(j), &matStack);
				}
			}

			//マテリアルのセット
			device->SetMaterial(&pContainer->pMaterials[pBoneCombination[i].AttribId].MatD3D);
			//テクスチャの設定
			//device->SetTexture(0,pContainer->ppTextures[pBoneCombination[i].AttribId]);
			//前のボーンIDを格納
			dwPrevBoneID=pBoneCombination[i].AttribId;
			//メッシュを描画
			pContainer->MeshData.pMesh->DrawSubset(i);
		}
	}
	else
	{
		///////////////////////////////////
		//		通常メッシュの描画		//
		/////////////////////////////////

		//ワールド座標の設定
		device->SetTransform(D3DTS_WORLD,&pFrame->CombinedTransformationMatrix);

		//マテリアル数分ループ
		for(unsigned int i=0;i<pContainer->NumMaterials;i++)
		{
			device->SetMaterial(&pContainer->pMaterials[i].MatD3D);	//マテリアル
			device->SetTexture(0, pContainer->ppTextures[i]);		//テクスチャ
			pContainer->MeshData.pMesh->DrawSubset(i);				//メッシュのセット
		}
	}
}
//=============================================================================
//フレームの解放
//=============================================================================
HRESULT CAllocateHierarchy::DestroyFrame(THIS_ LPD3DXFRAME pFrameToFree)
{
	///////////////////////////////////////
	//		動的確保した要素を解放		//
	/////////////////////////////////////

	//フレーム名
	if(pFrameToFree->Name!=NULL)
	{
		//解放
		delete[] pFrameToFree->Name;
		pFrameToFree->Name=NULL;
	}

	//メッシュコンテナ情報
	if(pFrameToFree->pMeshContainer)
	{
		//メッシュコンテナ内の情報を解放
		DestroyMeshContainer(pFrameToFree->pMeshContainer);
	}

	//兄弟フレームのポインタ
	if(pFrameToFree->pFrameSibling)
	{
		//再帰関数で兄弟フレームを解放
		DestroyFrame(pFrameToFree->pFrameSibling);
	}

	//子フレームのポインタ
	if(pFrameToFree->pFrameFirstChild)
	{
		//再帰関数で子フレームを解放
		DestroyFrame(pFrameToFree->pFrameFirstChild);
	}

	//自身を解放
	delete pFrameToFree;
	pFrameToFree=NULL;

	return D3D_OK;
}
//=============================================================================
//メッシュ情報の解放
//=============================================================================
HRESULT CAllocateHierarchy::DestroyMeshContainer(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	///////////////////////////////////////////////
	//		メッシュコンテナ内の要素を解放		//
	/////////////////////////////////////////////

	//メッシュコンテナへキャスト変換
	CMeshContainer *pContainer=(CMeshContainer*)pMeshContainerToFree;

	//名前の解放
	delete[] pContainer->Name;
	pContainer->Name=NULL;

	//通常メッシュ解放
	if(pContainer->MeshData.pMesh)
	{
		pContainer->MeshData.pMesh->Release();
		pContainer->MeshData.pMesh=NULL;
	}

	//プログレッシブメッシュ解放
	if(pContainer->MeshData.pPMesh)
	{
		pContainer->MeshData.pPMesh->Release();
		pContainer->MeshData.pPMesh=NULL;
	}

	//パッチメッシュ解放
	if(pContainer->MeshData.pPatchMesh)
	{
		pContainer->MeshData.pPatchMesh->Release();
		pContainer->MeshData.pPatchMesh=NULL;
	}

	//マテリアル数分ループ
	for(unsigned int i=0;i<pContainer->NumMaterials;i++)
	{
		//テクスチャファイル名の解放
		if(pContainer->pMaterials[i].pTextureFilename!=NULL)
		{
			delete[] pContainer->pMaterials[i].pTextureFilename;
		}

		//テクスチャポインタの解放
		if(pContainer->ppTextures[i]!=NULL)
		{
			pContainer->ppTextures[i]->Release();
			pContainer->ppTextures[i]=NULL;
		}
	}

	//マテリアル情報解放
	delete[] pContainer->pMaterials;
	//テクスチャポインタ解放
	delete[] pContainer->ppTextures;

	//デフォルト数分ループ
	for(unsigned int i=0;i<pContainer->pEffects->NumDefaults;i++)
	{
		//バイト数を指すポインタ解放
		delete[] pContainer->pEffects->pDefaults[i].pValue;
	}

	//エフェクト名文字列解放
	delete[] pContainer->pEffects->pEffectFilename;

	//デフォルトを解放
	delete[] pContainer->pEffects->pDefaults;

	//エフェクトを解放
	delete[] pContainer->pEffects;

	//隣接するポリゴンインデックス数解放
	delete[] pContainer->pAdjacency;
	//pContainer->pSkinInfo
	//スキン情報解放
	if(pContainer->pSkinInfo)
	{
		pContainer->pSkinInfo->Release();
		pContainer->pSkinInfo=NULL;
	}

	//ボーンテーブル解放
	if(pContainer->pBoneBuffer!=NULL)
	{
		pContainer->pBoneBuffer->Release();
		pContainer->pBoneBuffer=NULL;
	}

	//ボーンのオフセット行列解放
	delete[] pContainer->pMatOffset;
	//ボーンワールド座標解放
	delete[] pContainer->ppMatRoot;

	delete pContainer;
	return D3D_OK;
}
//=============================================================================
//フレームのトップノードセット
//=============================================================================
void CAllocateHierarchy::SetFrameRoot(LPD3DXFRAME pFrame)
{
	m_pFrameRoot=pFrame;
}
//EOF