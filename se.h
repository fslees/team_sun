#ifndef ___SE_H___
#define ___SE_H___

#include <windows.h>
#include <tchar.h>
#include <dsound.h>
#include <mmsystem.h>

// �}�N����`
enum
{	// �T�E���h�ʂ��i���o�[
	SE_00,
	SE_01,
	SE_02,
	SE_MAX
};

enum
{	// �Đ��p�t���O
	E_DS8_FLANONE,
	E_DS8_FLALOOP,	// DSBPLAY_LOOPING=1
	E_DS8_FLAMAX
};

HRESULT					InitSE( HWND hWnd );	// ������
void					UninitSE();			// ��Еt��
LPDIRECTSOUNDBUFFER8	LoadSE( int no );	// �T�E���h�̃��[�h
void                    SetSE(int no);          //se���w�肵�Ăяo��
void					PlaySE( LPDIRECTSOUNDBUFFER8 Buffer, int flag = 0 );	// �����ƂɍĐ�
bool					IsPlaying( LPDIRECTSOUNDBUFFER8 Buffer );					// �Đ������ǂ���

#endif