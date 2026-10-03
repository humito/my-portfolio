//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 木村 文登
//
//=============================================================================
//*****************************************************************************
//定数定義
//*****************************************************************************
#define _CRTDBG_MAP_ALLOC//警告対策用
#define FPS (60)

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <stdlib.h>
#include <crtdbg.h>
#include "main.h"
#include "manager.h"
#include "System/renderer.h"

//*****************************************************************************
//定数定義
//*****************************************************************************
#define WINDOW_CLASS_NAME ("ウィンドウクラスの名前")
#define WINDOW_API_NAME ("哲学的なゲーム")

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	//メモリリーク検出
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//メモリリーク出力
	_CrtDumpMemoryLeaks();

	///////////////////////////////////////
	//		フレームカウント初期化		//
	/////////////////////////////////////

	timeBeginPeriod(1);				// 分解能を設定
	CRenderer::m_dwExecLastTime = 
	CRenderer::m_dwFPSLastTime = timeGetTime();
	CRenderer::m_dwCurrentTime = 
	CRenderer::m_dwFrameCount = 0;

	//マネージャーインスタンス生成
	CManager *pManager;
	pManager=new CManager();

	//ウィンドウの生成
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
		WINDOW_CLASS_NAME,
		NULL
	};
	
	HWND hWnd;	//ウィンドウハンドル
	MSG msg;	//メッセージ
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindowEx(0,
						WINDOW_CLASS_NAME,
						WINDOW_API_NAME,
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
	if(FAILED(pManager->Init(hInstance,hWnd,TRUE)))
	{
		return -1;
	}

	// ウインドウの表示(初期化処理の後に行う)
	ShowWindow(hWnd, nCmdShow);
	//ウィンドウの更新
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
			CRenderer::m_dwCurrentTime = timeGetTime();

			if((CRenderer::m_dwCurrentTime-CRenderer::m_dwFPSLastTime)>=500)
			{
#ifdef _DEBUG
				//FPSのカウントを計算
				CRenderer::m_nCountFPS=CRenderer::m_dwFrameCount*1000/(CRenderer::m_dwCurrentTime-CRenderer::m_dwFPSLastTime);
#endif
				CRenderer::m_dwFPSLastTime=CRenderer::m_dwCurrentTime;
				CRenderer::m_dwFrameCount=0;
			}

			if ((CRenderer::m_dwCurrentTime - CRenderer::m_dwExecLastTime) >= (1000 / FPS))
			{
				CRenderer::m_dwExecLastTime=CRenderer::m_dwCurrentTime;

				///////////////////////
				//	DirectXの処理	//
				/////////////////////

				// 更新処理
				pManager->Update();

				// 描画処理
				pManager->Draw();

				CRenderer::m_dwFrameCount++;
			}
		}
	}
	
	//ウィンドウクラスの登録を解除
	UnregisterClass(WINDOW_CLASS_NAME, wcex.hInstance);

	//終了処理
	pManager->Uninit();
	delete pManager;
	pManager=NULL;

	//デバッグ時にメモリリークを出力
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new(_NORMAL_BLOCK,__FILE__,__LINE__);
	#endif
#endif

	// 分解能を戻す
	timeEndPeriod(1);

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
//EOF