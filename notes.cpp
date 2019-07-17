//=============================================================================
//
// ノーツ処理 [notes.cpp]
// Author : GP12A332_25_染矢晃介
//
//=============================================================================
#include "Notes.h"
#include "input.h"
#include "hitline.h"
#include "particle.h"
#include "score.h"
#include "se.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	NOTES_BALL		"data/MODEL/car000.x"							// 読み込むモデル名
#define NOTES_MAX		(255)                                           //ノーツの数
#define NOTES_SPEED     (5.0f)                                         //速度
#define NOTES_LIMIT     (500.0f)                                       //消えるところ
#define HIT_LIMIT       (50.0f)                                        //当たり判定の発生する限界値
#define LINE_WHIDE      (50.0f)                                       //lineの幅

// ノーツの初期位置
#define NOTES_INIT_POS_X		(0.0f)
#define NOTES_INIT_POS_Y		(50.0f)
#define NOTES_INIT_POS_Z		(100.0f)
// ノーツ発生位置
#define NOTES_SET_POS_X		(50.0f)
#define NOTES_SET_POS_Y		(50.0f)
#define NOTES_SET_POS_Z		(400.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool SetNotesLane(int laneNum, int notesNo);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9	D3DTextureNotes;		 // テクスチャへのポインタ
LPD3DXMESH			D3DXMeshNotes;			 // メッシュ情報へのポインタ
LPD3DXBUFFER		D3DXBuffMatNotes;		 // マテリアル情報へのポインタ
DWORD				NumMatNotes;			 // マテリアル情報の数

NOTES				Notes[NOTES_MAX];		 // Notesの配列
D3DXVECTOR3         LinePos[LANE_MAX];		 // 出現位置のpos

int					SetNotesNo;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitNotes(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	
	for (int i = 0; i < NOTES_MAX; i++)
	{
		// 位置・回転・スケールの初期設定
		Notes[i].pos = D3DXVECTOR3(NOTES_INIT_POS_X, NOTES_INIT_POS_Y, -NOTES_INIT_POS_Z);
		Notes[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		Notes[i].scl = D3DXVECTOR3(2.5f, 2.0f, 0.3f);
		Notes[i].use = false;
	}

	//出現位置のposの初期化
	for (int i = 0; i < LANE_MAX; i++)
	{
		LinePos[i] = D3DXVECTOR3(NOTES_SET_POS_X - LINE_WHIDE * i, NOTES_INIT_POS_Y, -NOTES_SET_POS_Z);
	}

	D3DTextureNotes = NULL;
	D3DXMeshNotes = NULL;
	D3DXBuffMatNotes = NULL;
	NumMatNotes = 0;
	SetNotesNo = 0;

	// Xファイルの読み込み
	if(FAILED(D3DXLoadMeshFromX(NOTES_BALL,				// 読み込むモデルファイル名(Xファイル)
								D3DXMESH_SYSTEMMEM,		// メッシュの作成オプションを指定
								pDevice,				// IDirect3DDevice9インターフェイスへのポインタ
								NULL,					// 隣接性データを含むバッファへのポインタ
								&D3DXBuffMatNotes,	// マテリアルデータを含むバッファへのポインタ
								NULL,					// エフェクトインスタンスの配列を含むバッファへのポインタ
								&NumMatNotes,		// D3DXMATERIAL構造体の数
								&D3DXMeshNotes)))	// ID3DXMeshインターフェイスへのポインタのアドレス
	{
		return E_FAIL;
	}

#if 0
	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,					// デバイスへのポインタ
								TEXTURE_FILENAME,		// ファイルの名前
								&D3DTextureNotes);	// 読み込むメモリー
#endif

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitNotes(void)
{
	if(D3DTextureNotes != NULL)
	{// テクスチャの開放
		D3DTextureNotes->Release();
		D3DTextureNotes = NULL;
	}

	if(D3DXMeshNotes != NULL)
	{// メッシュの開放
		D3DXMeshNotes->Release();
		D3DXMeshNotes = NULL;
	}

	if(D3DXBuffMatNotes != NULL)
	{// マテリアルの開放
		D3DXBuffMatNotes->Release();
		D3DXBuffMatNotes = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateNotes(void)
{

#if 1
	//とりあえずAを押すと左lineに出る
	if (GetKeyboardTrigger(DIK_A))
	{
		
		SetNotes(0);

	}
	//とりあえずAを押すと真ん中lineに出る
	if (GetKeyboardTrigger(DIK_S))
	{

		SetNotes(1);

	}
	//とりあえずAを押すと右lineに出る
	if (GetKeyboardTrigger(DIK_D))
	{

		SetNotes(2);

	}
#endif

	//一定の位置まで進むと消える（ボタンを押せなかったとき）
	for (int i = 0; i < NOTES_MAX;i++)
	{
		if (Notes[i].use == true)
		{
			Notes[i].pos.z += NOTES_SPEED;

			if (Notes[i].pos.z >= NOTES_LIMIT)
			{
				Notes[i].use = false;
			}
		}
	}
	
	//ボタンを押すと当たり判定チェック
	if (GetKeyboardTrigger(DIK_Q))//左
	{
		CheckHitNotes(LEFT_LANE);
	}

	if (GetKeyboardTrigger(DIK_W))//真ん中
	{
		CheckHitNotes(CENTER_LANE);
	}

	if (GetKeyboardTrigger(DIK_E))//右
	{
		CheckHitNotes(RIGHT_LANE);
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawNotes(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	for (int i=0; i < NOTES_MAX; i++)
	{
		if (Notes[i].use == true)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&Notes[i].mtxWorld);

			// スケールを反映
			D3DXMatrixScaling(&mtxScl, Notes[i].scl.x, Notes[i].scl.y, Notes[i].scl.z);
			D3DXMatrixMultiply(&Notes[i].mtxWorld, &Notes[i].mtxWorld, &mtxScl);

			// 回転を反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, Notes[i].rot.y, Notes[i].rot.x, Notes[i].rot.z);
			D3DXMatrixMultiply(&Notes[i].mtxWorld, &Notes[i].mtxWorld, &mtxRot);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, Notes[i].pos.x, Notes[i].pos.y, Notes[i].pos.z);
			D3DXMatrixMultiply(&Notes[i].mtxWorld, &Notes[i].mtxWorld, &mtxTranslate);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &Notes[i].mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアル情報に対するポインタを取得
			pD3DXMat = (D3DXMATERIAL*)D3DXBuffMatNotes->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)NumMatNotes; nCntMat++)
			{
				// マテリアルの設定
				pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

				// テクスチャの設定
				pDevice->SetTexture(0, D3DTextureNotes);

				// 描画
				D3DXMeshNotes->DrawSubset(nCntMat);
			}

			// マテリアルをデフォルトに戻す
			pDevice->SetMaterial(&matDef);
		}
		
	}

}

//=============================================================================
// ノーツの設置関数
// 第１引数：int lineNum(ノーツを設置するレーン番号)
// 戻り値　：な　し
//=============================================================================
void SetNotes(int laneNum)
{
	SetNotesLane(laneNum, SetNotesNo);

	SetNotesNo++;
	SetNotesNo = SetNotesNo % NOTES_MAX;

	//for (int i = 0; i < NOTES_MAX; i++)
	//{
	//	if (SetNotesLane(laneNum, i)) return;
	//}
}

//=============================================================================
// ノーツのレーンへの設置関数
//=============================================================================
bool SetNotesLane(int laneNum, int notesNo)
{
	if (!Notes[notesNo].use)
	{
		switch (laneNum)
		{
		case LEFT_LANE:
			Notes[notesNo].pos = LinePos[LEFT_LANE];
			break;
		case CENTER_LANE:
			Notes[notesNo].pos = LinePos[CENTER_LANE];
			break;
		case RIGHT_LANE:
			Notes[notesNo].pos = LinePos[RIGHT_LANE];
			break;
		}

		Notes[notesNo].use = true;

		return true;
	}

	return false;
}

//=============================================================================
// ノーツの当たり判定関数
//=============================================================================
void CheckHitNotes(int laneNum)
{
	D3DXVECTOR3 hitfieldpos;
	float absNotePos, absHitPos, valNoteToHit;

	//absHitPos = fabsf(GetHitLinePos().z);

	// 判定ラインの位置を代入
	//hitfieldpos = GetHitLinePos();

	float tmp = 100.0f;

	for (int i = 0; i < NOTES_MAX; i++)
	{
		if (!Notes[i].use) continue;

		//absNotePos = fabsf(Notes[i].pos.z);

		valNoteToHit = fabsf(Notes[i].pos.z - GetHitLinePos().z - tmp);

		//if (Notes[i].pos.z > hitfieldpos.z && Notes[i].pos.z < hitfieldpos.z + HIT_LIMIT && 
		if (valNoteToHit <  HIT_LIMIT &&
			Notes[i].pos.x == LinePos[laneNum].x)
		{
			Notes[i].use = false;
			AddScore(30);
			SetSE(0);

			break;
		}

	}
}
