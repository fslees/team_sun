#ifndef ___SE_H___
#define ___SE_H___

#include <windows.h>
#include <tchar.h>
#include <dsound.h>
#include <mmsystem.h>

// マクロ定義
enum
{	// サウンド通しナンバー
	SE_00,
	SE_01,
	SE_02,
	SE_MAX
};

enum
{	// 再生用フラグ
	E_DS8_FLANONE,
	E_DS8_FLALOOP,	// DSBPLAY_LOOPING=1
	E_DS8_FLAMAX
};

HRESULT					InitSE( HWND hWnd );	// 初期化
void					UninitSE();			// 後片付け
LPDIRECTSOUNDBUFFER8	LoadSE( int no );	// サウンドのロード
void                    SetSE(int no);          //seを指定し呼び出す
void					PlaySE( LPDIRECTSOUNDBUFFER8 Buffer, int flag = 0 );	// 音ごとに再生
bool					IsPlaying( LPDIRECTSOUNDBUFFER8 Buffer );					// 再生中かどうか

#endif