//=============================================================================
//
// 判定ラインヘッダー処理 [hitline.h]
// Author : GP12A332_25_染矢晃介
//
//=============================================================================
#ifndef _HITLINE_H_
#define _HITLINE_H_

#include "main.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitHitLine(void);
HRESULT MakeVertexHitLine(LPDIRECT3DDEVICE9 pDevice);
void UninitHitLine(void);
void UpdateHitLine(void);
void DrawHitLine(void);
D3DXVECTOR3 GetHitLinePos(void);
D3DXVECTOR3 GetHitLineScl(void);

#endif

