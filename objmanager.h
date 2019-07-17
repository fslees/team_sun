//=============================================================================
//
// オブジェクト発生マネージャーヘッダ [objmanager.h]
// Author : GP12A332_26_田中太陽
//
//=============================================================================
#ifndef _OBJMANAGER_H_
#define _OBJMANAGER_H_

typedef struct 
{
	int nextPopFrame;	// オブジェクト発生フレーム
	int nextLaneNum;	// オブジェクト発生レーン番号
	int nextPopNum;		// オブジェクトの発生個数
	int scanResult;		// データの読み込み
	int frame;			// 発生フレームカウント

}ObjManager;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitObjManager(void);
void UpdateObjManager(void);
void SetObj(int laneNum, int objNum);

#endif
