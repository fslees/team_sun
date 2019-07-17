//=============================================================================
//
// ���胉�C���w�b�_�[���� [hitline.h]
// Author : GP12A332_25_����W��
//
//=============================================================================
#ifndef _HITLINE_H_
#define _HITLINE_H_

#include "main.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitHitLine(void);
HRESULT MakeVertexHitLine(LPDIRECT3DDEVICE9 pDevice);
void UninitHitLine(void);
void UpdateHitLine(void);
void DrawHitLine(void);
D3DXVECTOR3 GetHitLinePos(void);
D3DXVECTOR3 GetHitLineScl(void);

#endif

