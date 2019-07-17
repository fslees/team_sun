//=============================================================================
//
// 影処理 [sound.cpp]
// Author : SOMEYA KOSUKE
////=============================================================================

#include "main.h"
#include "se.h"

// ダイレクトサウンドのライブラリ
#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "dsound.lib" )
#pragma comment ( lib, "winmm.lib" )

// サウンドファイルのパス（hの通しナンバーと合わせること）
const TCHAR* soundFilename[] = {
	_T("data/SE/hit.wav"),
	_T("data/SE/dorum .wav"),
	_T("data/SE/shimbal.wav"),
	
};

// グローバル変数
static IDirectSound8 *DirectSound = NULL;	// サウンドインターフェース
static LPDIRECTSOUNDBUFFER8         SE[3]; //SE用のサウンドバッファー


// サウンドの初期化
// hWnd:ウィンドウハンドル
HRESULT InitSE( HWND hWnd )
{
	// DirectSoundオブジェクトの作成
	if (FAILED(DirectSoundCreate8(NULL, &DirectSound, NULL)))
		return E_FAIL;
	
	// 協調レベル設定 
	if (FAILED(DirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
		return E_FAIL;

	for (int i = 0; i < SE_MAX; i++)
	{
		SE[i] = LoadSE(i);
	}


	return S_OK;
}

// 後片付け
void UninitSE()
{
	if( DirectSound != NULL )
	{
		DirectSound->Release();
		DirectSound = NULL;
	}
}

// サウンドのロード
// no:サウンドナンバー（ヘッダに定義された列挙型定数）
LPDIRECTSOUNDBUFFER8 LoadSE( int no )
{
	// MMIO = マルチメディア入出力、の略。
	LPDIRECTSOUNDBUFFER  BaseBuffer = NULL;		// 曲データの総合バッファ
	LPDIRECTSOUNDBUFFER8 Buffer =  NULL;			// 曲データのバッファ
	DSBUFFERDESC buff;							// バッファ設定構造体

	HMMIO hMmio = NULL;							// MMIOハンドル
	MMIOINFO mmioInfo;							// マルチメディアデータ構造体

	MMRESULT mmRes;								// MM処理結果格納変数
	MMCKINFO riffChunk, formatChunk, dataChunk;	// チャンクデータ構造体
	DWORD size;									// データサイズ格納用
	WAVEFORMATEX pcm;							// 曲データフォーマット構造体

	LPVOID Block;								// セカンダリバッファのデータ書き込み先アドレス
	DWORD  dwSize;								// セカンダリバッファのサイズ

	// 1.ハンドルをもとにファイルを開く
	memset(&mmioInfo, 0, sizeof(MMIOINFO));
	hMmio = mmioOpen((LPSTR)soundFilename[no], &mmioInfo, MMIO_READ);
	if(!hMmio)
		return NULL;

	// 2.ファイル解析① RIFFチャンク検索
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');				// 検索キーワード
	mmRes = mmioDescend(hMmio, &riffChunk, NULL, MMIO_FINDRIFF);	// キーワードをもとに検索
	
	if(mmRes != MMSYSERR_NOERROR)		// 見つからなかったら異常終了
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 3.ファイル解析② フォーマットチャンク検索
	formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');						// 検索キーワード
	mmRes = mmioDescend(hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);	// キーワードをもとに検索
	
	if(mmRes != MMSYSERR_NOERROR)		// 見つからなかったら異常終了
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	size = mmioRead(hMmio, (HPSTR)&pcm, formatChunk.cksize);	// 検索情報をもとに読み込み
	
	if(size != formatChunk.cksize) 		// 正しく読み込めなかったら異常終了
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	mmioAscend(hMmio, &formatChunk, 0); 

	// 4.ファイル解析③ データチャンク検索
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');					// 検索キーワード
	mmRes = mmioDescend(hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);	// キーワードをもとに検索
	
	if(mmRes != MMSYSERR_NOERROR) 		// 見つからなかったら異常終了
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 5.データ読み込み
	char *Data = new char[dataChunk.cksize];					// 必要な大きさの領域を確保して
	size = mmioRead(hMmio, (HPSTR)Data, dataChunk.cksize);		// データを読み込む
	
	if(size != dataChunk.cksize)  		// 正しく読み込めなかったら異常終了
	{
		delete[] Data;
		return NULL;
	}

	// 6.曲を読み込む「セカンダリバッファ」を用意
	ZeroMemory(&buff, sizeof(DSBUFFERDESC));		// まず初期化
	buff.dwSize = sizeof(DSBUFFERDESC);				// そこから各種設定
	buff.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCDEFER;
	buff.dwBufferBytes = size;
	buff.lpwfxFormat = &pcm;

	// 総合バッファを作る
	if (FAILED(DirectSound->CreateSoundBuffer(&buff, &BaseBuffer, NULL)))
		return NULL;

	// サウンドバッファを取り出す
	if (FAILED(BaseBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&Buffer)))
		return NULL;

	// 取り出したら総合バッファを捨てる
	BaseBuffer->Release();

	// 7.作られたセカンダリバッファにデータを転送
	// まずは書き込みできるようバッファをロック
	if (FAILED(Buffer->Lock(0, 0, &Block, &dwSize, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
		return NULL;

	memcpy(Block, Data, dwSize);	// データ転送
	delete[] Data;					// 元の読み込み領域を消す

	// バッファロック解除
	Buffer->Unlock(Block, dwSize, 0, 0);
	
	// セカンダリバッファを返してようやく完了...
	return Buffer;
}

// 音を鳴らす
// pBuffer:音を鳴らしたいサウンドデータのセカンダリバッファ
// flag   :1(E_DS8_FLALOOP)ならループ再生
void PlaySE(  LPDIRECTSOUNDBUFFER8 Buffer, int flag/*=0*/ )
{	// 続きから鳴らすので、最初から鳴らしたい場合はSetCurrentPosition(0)をすること
	Buffer->SetCurrentPosition(0);
	Buffer->Play(0, 0, flag);
}

// 音を止める
void StopSE( LPDIRECTSOUNDBUFFER8 Buffer )
{
	DWORD status;

	Buffer->GetStatus(&status);
	if( status & DSBSTATUS_PLAYING )	// 鳴っていたら
	{
		Buffer->Stop();	// 意味的にはPauseになる。
	}
}

// 再生中かどうか調べる
// pBuffer:音を鳴らしたいサウンドデータのセカンダリバッファ
bool IsPlayingSE( LPDIRECTSOUNDBUFFER8 Buffer )
{
	DWORD status;

	Buffer->GetStatus(&status);
	if( status & DSBSTATUS_PLAYING )
	{
		return true;
	}
	return false;
}
//引数で鳴らしたいseを指定
void SetSE(int no)
{

	PlaySE(SE[no], 0);

}
