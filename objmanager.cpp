//=============================================================================
//
// オブブジェクトマネージャー処理 [objManagger.cpp]
// Author : GP12A332_26_田中太陽
//
//=============================================================================
#include "main.h"
#include "objmanager.h"
#include "notes.h"

/*****************************************************************************
マクロ定義
*****************************************************************************/
#define CLOSE_FRAME			(1000)	// ファイル読み込み終了フレーム

/*****************************************************************************
グローバル変数
*****************************************************************************/

FILE		*Fp = NULL;		// ポップデータが入ったファイルへのポインタ
ObjManager	PopData;		// マネージャー構造体変数	
/******************************************************************************
初期化処理
******************************************************************************/
HRESULT InitObjManager(void)
{
	if (Fp != NULL)
	{

	}

	Fp = fopen("data/DATA/objData.csv", "r");

	if (Fp == NULL)
	{
		return S_FALSE;
	}

	//ポップデータの読み込み
	PopData.scanResult = fscanf(Fp, "%d,%d,%d", &PopData.nextPopFrame,&PopData.nextLaneNum, &PopData.nextPopNum);

	return S_OK;
}

/******************************************************************************
更新処理
******************************************************************************/
void UpdateObjManager(void)
{
	// フレームの更新
	PopData.frame++;

	// オブジェクトをポップさせる時間であったらポップ
	if (PopData.nextPopFrame < PopData.frame && PopData.scanResult != EOF)
	{ 
		// オブジェクトの発生
		SetObj(PopData.nextLaneNum, PopData.nextPopNum);
		
		//ポップデータの読み込み
		PopData.scanResult = fscanf(Fp, "%d,%d,%d", &PopData.nextPopFrame, &PopData.nextLaneNum, &PopData.nextPopNum);
	}

	// ファイルの終了
	if (PopData.nextPopFrame == CLOSE_FRAME)
	{
		fclose(Fp);
	}
}

/******************************************************************************
 オブジェクトの発生処理
******************************************************************************/
void SetObj(int laneNum, int objNum)
{
	SetNotes(laneNum);
}
