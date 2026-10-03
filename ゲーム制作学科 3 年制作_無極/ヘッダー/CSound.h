//=============================================================================
//サウンド処理[CSound.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _CSOUND_H_
#define _CSOUND_H_
//*****************************************************************************
//インクルードファイル
//*****************************************************************************
#include "renderer.h"
#include <XAudio2.h>

//*****************************************************************************
//構造体定義
//*****************************************************************************
//サウンドラベル
enum SOUND_LABEL
{
	SOUND_LABEL_BGM_TITLE=0,	//タイトルBGM
	SOUND_LABEL_BGM_GAME,		//ゲームBGM
	SOUND_LABEL_BGM_RESULT,		//リザルトBGM
	SOUND_LABEL_SE_SHOT,		//弾発射音
	SOUND_LABEL_SE_ITEM,		//アイテム取得音
	SOUND_LABEL_SE_EXPLOSION,	//爆発音
	SOUND_LABEL_SE_FIREWORKS,	//花火発射音
	SOUND_LABEL_SE_DAMAGE,		//ダメージ音
	SOUND_LABEL_SE_START,		//開始音
	SOUND_LABEL_SE_GAMEOVER,	//ゲームオーバー音
	SOUND_LABEL_MAX,			//ラベル数
};

//*****************************************************************************
//クラス定義
//*****************************************************************************
//サウンドクラス
class CSound
{
	//外部
	public:
		CSound();									//コンストラクタ
		~CSound();									//デストラクタ
		HRESULT InitSound(HWND hWnd);				//初期化
		void UninitSound(void);						//終了
		static HRESULT PlaySound(SOUND_LABEL label);//ラベル指定のサウンド再生
		static void StopSound(SOUND_LABEL label);	//ラベル指定のサウンド停止
		static void StopSound(void);				//全サウンド停止

	//内部
	private:
		//パラメータ構造体定義
		typedef struct
		{
			char *pFilename;	//ファイル名
			bool bLoop;			//ループするかどうか
		}PARAM;

		static IXAudio2 *m_pXAudio2;									//XAudio2オブジェクトへのインターフェイス
		static IXAudio2MasteringVoice *m_pMasteringVoice;				//マスターボイス
		static IXAudio2SourceVoice *m_apSourceVoice[SOUND_LABEL_MAX];	//ソースボイス
		static BYTE *m_apDataAudio[SOUND_LABEL_MAX];					//オーディオデータ
		static DWORD m_aSizeAudio[SOUND_LABEL_MAX];						//オーディオデータサイズ
		static int m_nLoopCnt[SOUND_LABEL_MAX];							//ループカウンタ

		//チャンクのチェック
		HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
		//チャンクデータの読込
		HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
};

#endif
//EOF