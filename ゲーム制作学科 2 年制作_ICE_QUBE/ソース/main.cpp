//=============================================================================
// メイン処理 [main.cpp]
// Author : HUMITO KIMURA
//=============================================================================
#include "main.h"
//*****************************************************************************
// マクロ定義
//******************************************************************************
#define CLASS_NAME		"AppClass"		// ウインドウのクラス名
#define WINDOW_NAME		"ICE QUBE"	// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
#ifdef _DEBUG
void DrawFPS(void);
#endif
//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECT3D9			g_pD3D = NULL;			// Direct3D オブジェクト
LPDIRECT3DDEVICE9	g_pD3DDevice = NULL;	// Deviceオブジェクト(描画に必要)
static MODE			g_mode=MODE_TITLE;		//モード種類
static WIPE			g_Wipe=WIPE_NONE;		//ワイプ種類
static int			g_nWipeCnt=0;			//ワイプカウント
#ifdef _DEBUG
LPD3DXFONT			g_pD3DXFont = NULL;		// フォントへのポインタ
int					g_nCountFPS;			// FPSカウンタ
#endif
static bool			g_bStop=false;			//一時停止フラグ
static bool			g_bWire=false;			//ワイヤーフレームフラグ
//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX wcex =
	{
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
		CLASS_NAME,
		NULL
	};
	HWND hWnd;
	MSG msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindowEx(0,
						CLASS_NAME,
						WINDOW_NAME,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
						NULL,
						NULL,
						hInstance,
						NULL);

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	//フレームカウント初期化
	timeBeginPeriod(1);				// 分解能を設定
	dwExecLastTime = 
	dwFPSLastTime = timeGetTime();
	dwCurrentTime =
	dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();
			if((dwCurrentTime - dwFPSLastTime) >= 500)	// 0.5秒ごとに実行(1000=1.0秒)
			{
#ifdef _DEBUG
				g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
#endif
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}

			if((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;

				// 更新処理
				Update();

				// 描画処理
				Draw();

				dwFrameCount++;
			}
		}
	}
	
	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	timeEndPeriod(1);				// 分解能を戻す

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

	// Direct3Dオブジェクトの作成
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
	d3dpp.BackBufferFormat			= d3ddm.Format;				// バックバッファフォーマットはディスプレイモードに合わせて使う
	d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;	// 映像信号に同期してフリップする
	d3dpp.Windowed					= bWindow;					// ウィンドウモード
	d3dpp.EnableAutoDepthStencil	= TRUE;						// デプスバッファ（Ｚバッファ）とステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;				// デプスバッファとして16bitを使う

	if(bWindow)
	{// ウィンドウモード
		d3dpp.FullScreen_RefreshRateInHz = 0;								// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// インターバル
	}
	else
	{// フルスクリーンモード
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;			// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;		// インターバル
	}

	// デバイスオブジェクトの生成
	// [デバイス作成制御]<描画>と<頂点処理>をハードウェアで行なう
	if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
									D3DDEVTYPE_HAL, 
									hWnd, 
									D3DCREATE_HARDWARE_VERTEXPROCESSING, 
									&d3dpp, &g_pD3DDevice)))
	{
		// 上記の設定が失敗したら
		// [デバイス作成制御]<描画>をハードウェアで行い、<頂点処理>はCPUで行なう
		if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
										D3DDEVTYPE_HAL, 
										hWnd, 
										D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
										&d3dpp, &g_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// [デバイス作成制御]<描画>と<頂点処理>をCPUで行なう
			if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
											D3DDEVTYPE_REF,
											hWnd, 
											D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
											&d3dpp, &g_pD3DDevice)))
			{
				// 初期化失敗
				return E_FAIL;
			}
		}
	}

	// レンダーステートパラメータの設定
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);				// 裏面をカリング
	g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						// Zバッファを使用
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドを行う
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定

	// サンプラーステートパラメータの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(U値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(V値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// テクスチャ縮小フィルタモードを設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// テクスチャ拡大フィルタモードを設定

	// テクスチャステージステートの設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// アルファブレンディング処理
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 最初のアルファ引数
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// ２番目のアルファ引数
#ifdef _DEBUG
	// 情報表示用フォントを設定
	D3DXCreateFont(g_pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
					OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Terminal", &g_pD3DXFont);
#endif

	//キーボードの入力初期化
	InitKeyboard(hInstance,hWnd);

	//サウンド初期化
	InitSound(hWnd);
	
	//ワイプの初期化
	InitWipe();

	//背景の初期化
	InitBg();

	//タイトルの初期化
	InitTitle();

	//ゲームの初期化
	InitGame();

	//リザルトの初期化
	InitResult();

	//タイトルサウンドの再生
	PlaySound(SOUND_LABEL_BGM000);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
#ifdef _DEBUG
	if(g_pD3DXFont != NULL)
	{// 情報表示用フォントの開放
		g_pD3DXFont->Release();
		g_pD3DXFont = NULL;
	}
#endif

	if(g_pD3DDevice != NULL)
	{// デバイスの開放
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	if(g_pD3D != NULL)
	{// Direct3Dオブジェクトの開放
		g_pD3D->Release();
		g_pD3D = NULL;
	}

	//キーボードの終了処理
	UninitKeyboard();

	//サウンドの終了
	UninitSound();

	//ワイプの終了
	UninitWipe();

	//背景の終了
	UninitBg();

	//タイトルの終了
	UninitTitle();

	//ゲームの終了
	UninitGame();

	//リザルトの終了
	UninitResult();
}
//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	//ワイプ状態の取得
	g_Wipe=GetWipe();

	//キーボードの更新
	UpdateKeyboard();

	if(g_Wipe!=WIPE_OUT)
	{
		g_nWipeCnt++;
	}

	if(GetKeyboardTrigger(DIK_G) && g_bWire==false)
	{
		g_bWire=true;	
		g_pD3DDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	}
	else if(GetKeyboardTrigger(DIK_G) && g_bWire==true)
	{
		g_bWire=false;
		g_pD3DDevice->SetRenderState(D3DRS_FILLMODE,NULL);
	}

	//Oキーによって一時停止フラグ切替
	if(GetKeyboardTrigger(DIK_O) &&g_bStop==false)
	{
		//フラグtrue
		g_bStop=true;
	}
	else if(GetKeyboardTrigger(DIK_O) &&g_bStop==true)
	{
		//フラグfalse
		g_bStop=false;
	}

	//一時停止フラグチェック
	if(g_bStop!=true)
	{

		////////////////////////////////////////////////////////////////////////
		//							ゲームモード							　//
		////////////////////////////////////////////////////////////////////////
		switch(g_mode)
		{
			//////////////////////////////////////
			//				タイトル		   //
			////////////////////////////////////
			case MODE_TITLE:
					//タイトルの更新
					UpdateTitle();

					//ワイプアウト開始時
					if(g_Wipe==WIPE_OUT && g_nWipeCnt!=0)
					{
						SetBg(TUTOREAL_BG);					//チュートリアル背景セット
						g_mode=MODE_TUTOREAL;				//チュートリアルモード変更
						g_nWipeCnt=0;						//ワイプカウントリセット
						StopSound(SOUND_LABEL_BGM000);		//タイトルサウンド停止
						PlaySound(SOUND_LABEL_BGM001);		//チュートリアルサウンド再生
					}
			break;

			//////////////////////////////////////
			//			チュートリアル		   //
			////////////////////////////////////
			case MODE_TUTOREAL:

					//エンターキーでワイプセット
					if(GetKeyboardTrigger(DIK_RETURN))
					{
						SetWipe(WIPE_IN);
					}

					//ワイプアウト開始時
					if(g_Wipe==WIPE_OUT && g_nWipeCnt!=0)
					{
						
						InitGame();							//ゲームの初期化
						SetBg(GAME_BG);						//ゲーム用背景セット
						g_mode=MODE_GAME;					//ゲームモード変更
						g_nWipeCnt=0;						//ワイプカウントリセット
						StopSound(SOUND_LABEL_BGM001);		//チュートリアルサウンドの停止
						PlaySound(SOUND_LABEL_BGM003);		//ゲームサウンドの再生
					}
			break;

			////////////////////////////////////////
			//				ゲーム			     //
			//////////////////////////////////////
			case MODE_GAME:
					//ゲームの更新
					UpdateGame();

					//ワイプアウト開始時
					if(g_Wipe==WIPE_OUT && g_nWipeCnt!=0)
					{
						SetBg(RESULT_BG);				//リザルト背景セット
						g_mode=MODE_RESULT;				//リザルトモード変更
						g_nWipeCnt=0;					//ワイプカウントリセット
						StopSound(SOUND_LABEL_BGM003);	//ゲームサウンド停止
						PlaySound(SOUND_LABEL_BGM002);	//リザルトサウンド再生
					}
			break;
			//////////////////////////////////////
			//				リザルト		   //
			////////////////////////////////////
			case MODE_RESULT:

					//リザルトの更新
					UpdateResult();

					ResultSPolygon();

					//ワイプアウト開始時
					if(g_Wipe==WIPE_OUT && g_nWipeCnt!=0)
					{
						SetBg(TITLE_BG);				//タイトル背景セット
						g_mode=MODE_TITLE;				//タイトルモード変更
						g_nWipeCnt=0;					//ワイプカウントリセット
						StopSound(SOUND_LABEL_BGM002);	//リザルトサウンド停止
						PlaySound(SOUND_LABEL_BGM000);	//タイトルサウンド再生
					}
			break;
		}
	}

	//ワイプの更新
	UpdateWipe();

	//背景の更新
	UpdateBg();
}
//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファ＆Ｚバッファのクリア
	g_pD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// Direct3Dによる描画の開始
	if(SUCCEEDED(g_pD3DDevice->BeginScene()))
	{

		////////////////////////////////////////////////////////////////////////
		//						モードによる描画分け						  //
		////////////////////////////////////////////////////////////////////////
		switch(g_mode)
		{
		///////////////////////////
		//	タイトル画面の描画	//
		/////////////////////////
		case MODE_TITLE:

			//背景の描画
			DrawBg();

			//タイトルの描画
			DrawTitle();
		break;
		///////////////////////////////////
		//	チュートリアル画面の描画	//
		/////////////////////////////////
		case MODE_TUTOREAL:

			//背景の描画
			DrawBg();
		break;
		///////////////////////////
		//	ゲーム画面の描画	//
		//////////////////////////
		case MODE_GAME:

			//ゲームの描画
			DrawGame();

		break;
		///////////////////////////
		//	リザルト画面の描画	//
		//////////////////////////
		case MODE_RESULT:

			//背景の描画
			DrawBg();

			//リザルトの描画
			DrawResult();

			//スコア描画
			DrawScore();
		break;
		}

		//ワイプ可動時にみ描画
		if(g_Wipe!=WIPE_NONE)
		{
			//ワイプの描画
			DrawWipe();
		}

#ifdef _DEBUG
		// FPS表示
		DrawFPS();
#endif

		// Direct3Dによる描画の終了
		g_pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}
//=============================================================================
// デバイスの取得
//=============================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}
//=============================================================================
// モードのセット
//=============================================================================
void SetMode(MODE mode)
{
	g_mode=mode;
}
#ifdef _DEBUG
//=============================================================================
// FPS表示
//=============================================================================
void DrawFPS(void)
{
	RECT rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	char str[256];
	wsprintf(str, "FPS:%d\n", g_nCountFPS);


	// テキスト描画
	g_pD3DXFont->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));
}
#endif
//EOF