//=============================================================================
//
// プリミティブ表示処理 [main.cpp]
// Author : 木村　文登
//メモ：mp3をwavに変換する
//背景をつくる
//レベル表示をつくる
//
//=============================================================================
#include "main.h"
//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECT3D9			g_pD3D = NULL;			// Direct3Dオブジェクト
LPDIRECT3DDEVICE9	g_pD3DDevice = NULL;	// Deviceオブジェクト(描画に必要)
MODE g_mode=MODE_TITLE;						//モードの種類
static FADE g_fade=FADE_NONE;				//フェード種類
static int nFadeCount;						//フェードインまでのカウント
static int g_nPause=0;						//ポーズ情報
//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		"カラフルアタック",
		NULL
	};
	HWND hWnd;
	MSG msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindowEx(0,
						"カラフルアタック",
						"カラフルアタック",
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						(SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2),
						(SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION)),
						NULL,
						NULL,
						hInstance,
						NULL);


	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance,hWnd,TRUE)))
	{
		return -1;
	}

	// ウインドウの表示(初期化処理の後に行う)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)	// メッセージを取得しなかった場合"0"を返す
		{// Windowsの処理
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			// DirectXの処理
			// 更新処理
			Update();

			// 描画処理
			Draw();
		}
	}
	
	// ウィンドウクラスの登録を解除
	UnregisterClass("カラフルアタック", wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DPRESENT_PARAMETERS d3dpp;
    D3DDISPLAYMODE d3ddm;


	// Direct3Dオブジェクトの生成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
    if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// ワークをゼロクリア
	d3dpp.BackBufferCount			= 1;						// バックバッファの数
	d3dpp.BackBufferWidth			= SCREEN_WIDTH;				// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight			= SCREEN_HEIGHT;			// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat			= D3DFMT_UNKNOWN;			// バックバッファのフォーマットは現在設定されているものを使う
	d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;	// 映像信号に同期してフリップする
	d3dpp.Windowed					= bWindow;					// ウィンドウモード
	d3dpp.EnableAutoDepthStencil	= TRUE;						// デプスバッファ（Ｚバッファ）とステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;				// デプスバッファとして16bitを使う


	if(bWindow)
	{// ウィンドウモード
		d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;					// バックバッファ
		d3dpp.FullScreen_RefreshRateInHz = 0;								// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// インターバル
	}
	else
	{// フルスクリーンモード
		d3dpp.BackBufferFormat           = D3DFMT_R5G6B5;					// バックバッファ
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;			// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;		// インターバル
	}

	// デバイスオブジェクトの生成
	// [デバイス作成制御]<描画>と<頂点処理>をハードウェアで行なう
	if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,						// ディスプレイアダプタ
									D3DDEVTYPE_HAL,							// ディスプレイタイプ
									hWnd,									// フォーカスするウインドウへのハンドル
									D3DCREATE_HARDWARE_VERTEXPROCESSING,	// デバイス作成制御の組み合わせ
									&d3dpp,									// デバイスのプレゼンテーションパラメータ
									&g_pD3DDevice)))						// デバイスインターフェースへのポインタ
	{
		// 上記の設定が失敗したら
		// [デバイス作成制御]<描画>をハードウェアで行い、<頂点処理>はCPUで行なう
		if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
										D3DDEVTYPE_HAL, 
										hWnd, 
										D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
										&d3dpp,
										&g_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// [デバイス作成制御]<描画>と<頂点処理>をCPUで行なう
			if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
											D3DDEVTYPE_REF,
											hWnd, 
											D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
											&d3dpp,
											&g_pD3DDevice)))
			{
				// 初期化失敗
				return E_FAIL;
			}
		}


	}

	//キーボードの入力初期化
	InitKeyboard(hInstance,hWnd);

	//レンダーステートパラメーターの設定
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	g_pD3DDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	//サンプラーステートの設定
	g_pD3DDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	g_pD3DDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
	g_pD3DDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	//テクスチャーステージステートの設定
	g_pD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_CURRENT);




	//ゲームの初期化
	InitGame();

	//プレイヤーポリゴンの初期化
	InitPolygon();

	//ゲーム背景
	InitBgPolygon();

	//敵ポリゴンの初期化
	InitEPolygon();

	//エフェクト初期化
	InitEfPolygon();

	//スコア初期化
	InitScore();

	//ポーズ画面初期化
	InitPPolygon();

	//カーソル初期化
	InitCuPolygon();

	//燃料メーターの初期化
	InitFuPolygon();

	//コインポリゴンの初期化
	//InitCPolygon();

	//タイトルポリゴンの初期化
	InittPolygon();

	//タイトル背景の初期化
	InittBgPolygon();

	//リザルトポリゴンの初期化
	InitrPolygon();


	//フェードポリゴンの初期化
	InitFade();


	//サウンドの初期化
	InitSound(hWnd);

	//タイトル用BGM再生
	PlaySound(SOUND_LABEL_BGM000);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	//オブジェクトの終了
	if(g_pD3D!=NULL)
	{
		g_pD3D->Release();
		g_pD3D=NULL;
	}

	//デバイスの終了
	if(g_pD3DDevice!=NULL)
	{
		g_pD3DDevice->Release();
		g_pD3DDevice=NULL;
	}

	//キーボードの入力終了
	UninitKeyboard();

	//プレイヤーポリゴンの終了
	UninitPolygon();

	//敵ポリゴンの終了
	UninitEPolygon();

	//エフェクトの終了
	UninitEfPolygon();

	//ゲーム背景の初期化
	UninitBgPolygon();

	//スコアの終了
	UninitScore();

	//ポーズ画面の終了
	UninitPPolygon();

	//カーソルの終了
	UninitCuPolygon();

	//燃料メーターの終了
	UninitFuPolygon();

	//コインポリゴンの終了
	//UninitCPolygon();

	//タイトルポリゴンの終了
	UninittPolygon();
	
	//タイトル背景の終了
	UninittBgPolygon();

	//リザルトポリゴンの終了
	UninitrPolygon();


	//背景の終了
	/*UninitBg1Polygon();
	UninitBg2Polygon();
	UninitBg3Polygon();*/


	//フェードの終了
	UninitFade();

	//サウンドの終了
	UninitSound();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	//キーボードの入力更新
	UpdateKeyboard();

	////////////
	//入力処理//
	////////////

	//タイトルかリザルトでかつエンターキーが押されたらフェードアウト
	if(GetKeyboardTrigger(DIK_RETURN)==true &&
	  (g_mode==MODE_TITLE || g_mode==MODE_RESULT))
	{
		PlaySound(SOUND_LABEL_SE_LASER);//効果音の再生
		g_fade=FADE_OUT;				//フェードアウト代入
	}

	//ゲームモードでかつPキーが押されたらポーズフラグ切替
	if(GetKeyboardTrigger(DIK_P)==true &&
	   g_mode==MODE_GAME)
	{
		//ポーズ情報：0.OFF  1.ON
		g_nPause+=COUNTUP;//ポーズ情報を加算
		g_nPause%=2;//0・1に絞る
	}

	//フェードセット
	SetFade(g_fade);

	//フェードモードがフェードイン以外ならカウントアップ
	if(g_fade!=FADE_IN)
	{
		nFadeCount+=COUNTUP;
	}

	//////////////////////////////////////////
	//				  画面遷移				//
	//////////////////////////////////////////
	switch(g_mode)
	{
		//タイトル画面
		case MODE_TITLE:

			UpdatetBgPolygon();	//タイトル背景の更新
			UpdatetPolygon();	//タイトルポリゴンの更新

			//フェードインに入ったら各ポリゴン初期化処理。ゲームモードの準備
			if(g_fade==FADE_IN && nFadeCount>0)
			{
				g_mode=MODE_GAME;					//ゲームモード
				g_nPause=0;							//ポーズ情報OFF
				nFadeCount=0;						//フェードカウントリセット
				
				InitPolygon();						//プレイヤーポリゴン初期化
				InitEPolygon();						//敵ポリゴン初期化
				InitEfPolygon();					//エフェクトポリゴン初期化
				InitGame();							//ゲーム初期化
				InitScore();						//スコア初期化
				InitPPolygon();						//ポーズポリゴン初期化
				InitCuPolygon();					//カーソルポリゴン初期化
				InitFuPolygon();					//燃料メーターポリゴン初期化

				StopSound(SOUND_LABEL_BGM000);		//タイトル用BGMの停止
				PlaySound(SOUND_LABEL_BGM002);		//ゲーム用BGMの再生
			}

		break;

		//ゲーム画面
		case MODE_GAME:

			//ポーズフラグがOFFならゲーム更新
			if(g_nPause==0)
			{
				UpdatePolygon();	//プレイヤーポリゴン更新
				UpdateEPolygon();	//敵ポリゴン更新
				UpdateEfPolygon();	//エフェクトポリゴン更新
				UpdateFuPolygon();	//燃料メーターポリゴン更新
				UpdateGame();		//ゲーム更新		
			}

			//それ以外は画面を止めてポーズへ
			UpdatePPolygon();		//ポーズ画面更新
			UpdateCuPolygon();		//カーソル更新

			//フェードインしたらリザルトへ
			if(g_fade==FADE_IN && nFadeCount>0)
			{
				g_mode=MODE_RESULT;				//リザルトモードへ
				nFadeCount=0;					//フェードカウントリセット
				StopSound(SOUND_LABEL_BGM002);	//ゲーム用BGM停止
				PlaySound(SOUND_LABEL_BGM001);	//リザルト用BGM再生
			}

		break;

		//リザルト画面
		case MODE_RESULT:

			UpdaterPolygon();//リザルト更新
			ResultEPolygon();//リザルト用敵表示
			ResultSPolygon();

			//フェードインしたらタイトルへ
			if(g_fade==FADE_IN && nFadeCount>0)
			{
				g_mode=MODE_TITLE;				//タイトルモードへ
				g_nPause=0;						//ポーズ情報リセット
				nFadeCount=0;					//フェードカウントリセット
				StopSound(SOUND_LABEL_BGM001);	//リザルト用BGM停止
				PlaySound(SOUND_LABEL_BGM000);	//タイトル用BGM再生
			}

		break;

	}

	//フェードの更新処理
	UpdateFade();

	//フェードゲット
	g_fade=GetFade();

}
//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	//画面のクリア
	g_pD3DDevice->Clear(0,NULL,
						(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
						D3DCOLOR_RGBA(0,0,0,0),
						1.0f,0);

	//描画の開始
	if(SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		//==============================
		//様々なオブジェクトの描画処理
		//==============================
		switch(g_mode)
		{
			//タイトルモード
			case MODE_TITLE:
				//タイトル描画

				//ポーズ情報OFFならタイトル描画
				if(g_nPause==0)
				{
					DrawtBgPolygon();	//タイトル背景の描画
					DrawtPolygon();		//タイトルの描画
				}

			break;

			//ゲームモード
			case MODE_GAME:
				//背景の描画
				DrawBgPolygon();	//背景
				DrawPolygon();		//プレイヤー
				DrawEPolygon();		//エネミー
				DrawEfPolygon();	//エフェクト
				DrawScore();		//スコア
				DrawPPolygon();		//ポーズ
				DrawCuPolygon();	//カーソル
				DrawFuPolygon();	//燃料メーター
			break;

			//リザルトモード
			case MODE_RESULT:
				//リザルトの描画

				//ポーズ情報OFFなら描画
				if(g_nPause==0)
				{
					//DrawBg3Polygon();
					DrawrPolygon();//リザルト描画
					DrawEPolygon();//エネミー
					DrawScore();	//スコア
				}
			break;
		}

		//フェードの描画
		DrawFade();

		//描画終了
		g_pD3DDevice->EndScene();
	}

	//3Dデバイスの中身を空にする
	g_pD3DDevice->Present(NULL,NULL,NULL,NULL);
}
//=============================================================================
//モードのセット
//=============================================================================
void SetMode (MODE mode)
{
	g_mode=mode;
}
//=============================================================================
//ポーズ情報のゲット
//=============================================================================
int GetPause (void)
{
	return g_nPause;
}
//=============================================================================
//ポーズ情報のセット
//=============================================================================
void SetPause (int data)
{
	g_nPause=data;
}
//=============================================================================
//デバイスのゲッター
//=============================================================================
LPDIRECT3DDEVICE9 GetDevice (void)
{
	return g_pD3DDevice;
}
//EOF
