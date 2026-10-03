#include "Cursor.h"

LPDIRECT3DTEXTURE9 g_pCuD3DTex=NULL;	//テクスチャへのポインタ
VERTEX_2D g_cuVtx[4];					//頂点情報格納ワーク
static int g_alpha=0;					//α値
static int g_menu=0;					//メニュー
//カーソル初期化
HRESULT InitCuPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pPDevice;

	//ゲッターで返す
	pPDevice=GetDevice();

	//α値最少
	g_alpha=255;

	//頂点座標の代入
	g_cuVtx[0].vtx=D3DXVECTOR3(345,470,0.0f);
	g_cuVtx[1].vtx=D3DXVECTOR3(345,390,0.0f);
	g_cuVtx[2].vtx=D3DXVECTOR3(425,470,0.0f);
	g_cuVtx[3].vtx=D3DXVECTOR3(425,390,0.0f);

		
	//中身
	g_cuVtx[0].rhw=1.0f;
	g_cuVtx[1].rhw=1.0f;
	g_cuVtx[2].rhw=1.0f;
	g_cuVtx[3].rhw=1.0f;
		
	//反射光
	g_cuVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_cuVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_cuVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_cuVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);

	//テクスチャ
	g_cuVtx[0].tex=D3DXVECTOR2(0.0f,1.0f);
	g_cuVtx[1].tex=D3DXVECTOR2(0.0f,0.0f);
	g_cuVtx[2].tex=D3DXVECTOR2(1.0f,1.0f);
	g_cuVtx[3].tex=D3DXVECTOR2(1.0f,0.0f);

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pPDevice,
								"data/TEXTURE/cursor.png",
								&g_pCuD3DTex);

	return S_OK;
}
//カーソル更新
void UpdateCuPolygon (void)
{
	///////////////////
	//	入力処理	//
	/////////////////

	//上入力
	if(GetKeyboardTrigger(DIK_W) ||
	   GetKeyboardTrigger(DIK_UP))
	{
		//メニュー番号を下げる
		g_menu-=1;
	}

	//下入力
	if(GetKeyboardTrigger(DIK_S) ||
	   GetKeyboardTrigger(DIK_DOWN))
	{
		//メニュー番号を上げる
		g_menu+=1;
	}

	//メニュー番号0～2に絞る
	g_menu%=MENU_MAX;

	//メニュー番号のマイナスをなくす
	if(g_menu<0)
	{
		g_menu=-g_menu;
	}
	
	//ポーズメニューで決定が押されたら
	if(GetKeyboardTrigger(DIK_RETURN)==true)
	{
		//各メニューによって処理を分ける
		switch(g_menu)
		{
			//つづけるが選ばれたら
			case 0:
				//メニューにポーズ情報OFFをセット
				SetPause(false);
			break;

			//やりなおしが選ばれたら
			case 1:
				//StopSound(SOUND_LABEL_BGM002);	//ゲーム用BGM停止
				SetPause(false);					//メニューにポーズ情報OFFをセット
				SetMode(MODE_TUTOREAL);				//タイトルモードセット
				SetWipe(WIPE_IN);					//フェードアウトでタイトルモードへ
			break;

			//タイトルへが選ばれたら
			case 2:
				//StopSound(SOUND_LABEL_BGM002);	//ゲーム用BGM停止
				SetPause(false);					//メニューにポーズ情報OFFをセット
				SetMode(MODE_RESULT);				//リザルトモードセット
				SetWipe(WIPE_IN);					//フェードアウトでゲームモードへ
			break;
		}
	}


	//反射光
	g_cuVtx[0].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_cuVtx[1].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_cuVtx[2].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);
	g_cuVtx[3].diffuse=D3DCOLOR_RGBA(255,255,255,g_alpha);

	//頂点座標の代入
	g_cuVtx[0].vtx=D3DXVECTOR3(145,260+(g_menu*110.0f),0.0f);
	g_cuVtx[1].vtx=D3DXVECTOR3(145,180+(g_menu*110.0f),0.0f);
	g_cuVtx[2].vtx=D3DXVECTOR3(325,260+(g_menu*110.0f),0.0f);
	g_cuVtx[3].vtx=D3DXVECTOR3(325,180+(g_menu*110.0f),0.0f);
}
//カーソル描画
void DrawCuPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pPDevice;

	//ゲッターで返す
	pPDevice=GetDevice();

	//頂点フォーマットのセット
	pPDevice->SetFVF(FVF_VERTEX_2D);
	//テクスチャの設定
	pPDevice->SetTexture(0,g_pCuD3DTex);

	//ポリゴンの描画
	pPDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
									2,//ポリゴンの数
									&g_cuVtx[0],
									sizeof(VERTEX_2D));
}
//カーソル終了
void UninitCuPolygon (void)
{
	//テクスチャの開放
	if(g_pCuD3DTex!=NULL)
	{
		g_pCuD3DTex->Release();
		g_pCuD3DTex=NULL;
	}
}