//=============================================================================
//
// 木処理 [star.cpp]
// Author : 
//
//=============================================================================

#include "main.h"
#include "star.h"
#include "camera.h"
#include "cutter.h"
#include "input.h"
#include "se.h"
#include "score.h"
//#include "rainbowLane.h"
//#include "player.h"
//#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


#define INIT_BLOCK_SCALE		D3DXVECTOR3(0.5f, 0.5f, 0.5f)

#define	BLOCK_WIDTH				(48.0f)						// 弾の半径幅
#define	BLOCK_HEIGHT			(32.0f)						// 弾の半径幅
#define BLOCK_DEPTH				(32.0f)
#define INIT_NUM_SURFACE_BLOCK	(6)
#define INIT_NUM_VTX_BLOCK		(8)
#define INIT_NUM_BLOCK			(1)
#define NUM_SECTION_VTX			(4)
#define NUM_NEW_IDX				(2)
#define COLOR_VTX				D3DXCOLOR(1.0f, 1.0f, 0.50f, 0.8f)
//#define NEW_SURFACE_COLOR		D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f)
//#define CENTER_BLOCK			D3DXVECTOR3(0.0f, BLOCK_HEIGHT / 2.0f, 0.0f)


#define SWITCH_SPHERE           (0)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//HRESULT MakeVertexStar(LPDIRECT3DDEVICE9 device);
//void	SetVertexStar(int idxStar, float width, float height);
//void	SetColorStar(int idxStar, D3DXCOLOR col);
HRESULT InitBlock(LPDIRECT3DDEVICE9 device, STAR* wkStar);
HRESULT InitSurface(LPDIRECT3DDEVICE9 device, BLOCK* wkBlock);

void	UninitBlock(STAR* wkStar);
void	UninitSurface(BLOCK* wkBlock);
void	DrawBlock(LPDIRECT3DDEVICE9 device, BLOCK* block);
void	DrawSurface(LPDIRECT3DDEVICE9 device, SURFACE* surface);


bool	ClippingBlock(PLANE section, STAR* star, BLOCK* block);
bool	HitCheckSectionLine(BLOCK* block, PLANE section);
bool	CheckBlockDot(PLANE section, BLOCK* block);
void	ClippingLine(PLANE section, BLOCK* block, SURFACE* surface);
void	CreateNewVtxPos(BLOCK* block, int cntCp, VERTEX_3D* vtx);
void	CreateNewSurface(BLOCK* block, PLANE section);
void	CategorizeBlock(BLOCK* block);
void	SetBlockData(BLOCK* oldBlock, BLOCK* newBlock);
void	SetSurfaceIdxBuff(BLOCK* oldBlock, BLOCK* newBlock);
void	SetMoveBlock(STAR* wkStar, PLANE section);


int		CheckCalculatedHitCheck(BLOCK* block, int sp, int ep);
int		GetEndPointOutLine(int sp, int numVtx);
bool	HitCheckSurface(D3DXVECTOR3* cp, PLANE section, D3DXVECTOR3 pos0, D3DXVECTOR3 pos1);
bool	HitCheckTriangle(TRIANGLE p, D3DXVECTOR3 pos0, D3DXVECTOR3 pos1);
//void	InitNewSurface(SURFACE* oldBlockSurface, SURFACE* newBlockSurface);

void	SetMoveStar(STAR* star, D3DXVECTOR3 pos, D3DXVECTOR3 move);

void	ResetStar(STAR* wkStar);

double DotProduct(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2);







//*****************************************************************************
// グローバル変数
//*****************************************************************************
//LPDIRECT3DVERTEXBUFFER9 D3DVtxBuffStar = NULL;		// 頂点バッファインターフェースへのポインタ
static bool	debug;
static bool	useWire = false;
static bool	useMove = true;

static int		cntFPS = 0;
static float	anglePos = 0.0f;
static int		interval = 60;

static STAR	star[MAX_STAR];				// 木ワーク

#if SWITCH_SPHERE

#define INIT_SPHERE_DVD_A			(12)
#define INIT_SPHERE_DVD_S			(INIT_SPHERE_DVD_A * 2)
#define NUM_VTX_SPHERE				((INIT_SPHERE_DVD_A-1) * INIT_SPHERE_DVD_S + 2)
#define NUM_SRUFACE_SPHERE          (INIT_SPHERE_DVD_S * INIT_SPHERE_DVD_A)
#define RADIUS_SPHERE				(50.0f)

D3DXVECTOR3		vtxS[NUM_VTX_SPHERE];
WORD			idxS[NUM_SRUFACE_SPHERE][4];
int				numIdxS[NUM_SRUFACE_SPHERE];

#endif

int		blockIdxNo[6][4] =
{
	//{ 0, 4, 1 },{ 0, 1, 3 },{ 0, 2, 4 },{ 0, 3, 2 },{ 1, 4, 2 },{ 1, 2, 3 }
	//{2, 0, 3, 1}, {4, 5, 6, 7}, {1, 4, 3, 6}, {3, 6, 2, 7}, {2, 7, 0, 5}, {0, 5, 1, 4}
	{ 0, 1, 2, 3 },{ 1, 4, 7, 2 },{ 2, 7, 6, 3 },{ 3, 6, 5, 0 },{ 0, 5, 4, 1 },{ 4, 5, 6, 7 }
};

//=============================================================================
// 更新処理
//=============================================================================
void UpdateStar(void)
{

	D3DXVECTOR3 vec, wkPos;

	anglePos += 0.01f;

	if (GetKeyboardTrigger(DIK_M))
	{
		interval = (interval == 60) ? 5 : 60;
	}
	
	const float length = 100.0f;
	
	if (!(cntFPS++ % interval))
	{
		SetStar(D3DXVECTOR3(cosf(anglePos) * length, 70.0f + fabsf(sinf(anglePos) * length/2.0f), -500.0f), 
			D3DXVECTOR3(0.0f, 0.0f, 2.0f));
	}

	D3DXMATRIX viewMtx;
	D3DXVECTOR3 viewPos;

	viewMtx = GetMtxViewData();
	
	for (int cntStar = 0; cntStar < MAX_STAR; cntStar++)
	{// 画面外に出たの星を初期化する
		if (!star[cntStar].use)
			continue;

		STAR *wkStar = &star[cntStar];

		for (int cntBlock = 0; cntBlock < MAX_BLOCK; cntBlock++)
		{
			if (!wkStar->block[cntBlock].use)
			{
				continue;
			}
			if (wkStar->block[cntBlock].pos.z > GetCamera()->pos.z)
			{
				ResetStar(wkStar);
			}

		}
	}


	for (int i = 0; i < MAX_STAR; i++)
	{
		if (!star[i].use)
		{
			continue;
		}
		for (int j = 0; j < MAX_BLOCK; j++)
		{
			if (!star[i].block[j].use || star[i].block[j].vtxBuff == NULL)
			{
				continue;
			}


			PLANE wkCutter = GetCutterState();
			if (star[i].block[0].pos.z > GetCutterPos().z && star[i].numBlock == 1)
				//if (GetKeyboardTrigger(DIK_SPACE))
			{
				ClippingStar(wkCutter);
				//continue;
			}

			if (/*star[i].block[j].pos.y + vtx[0].vtx.y > 0.0f &&*/ useMove)
			{
				star[i].block[j].pos += star[i].block[j].move;
			}


		}
	}

}



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStar(void)
{
	LPDIRECT3DDEVICE9 device = GetDevice();
	useMove = true;


	for (int i = 0; i < MAX_STAR; i++)
	{

		star[i].width = BLOCK_WIDTH;
		star[i].height = BLOCK_HEIGHT;
		star[i].depth = BLOCK_DEPTH;
		star[i].use = false;
		star[i].numBlock = INIT_NUM_BLOCK;
		star[i].pos = D3DXVECTOR3(0.0f, 100.0f, 0.0f);


		InitBlock(device, &star[i]);
	}



	return S_OK;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBlock(LPDIRECT3DDEVICE9 device, STAR* wkStar)
{
	debug = false;

	VERTEX_3D *vtx;

	for (int i = 0; i < MAX_BLOCK; i++)
	{
		wkStar->block[i].numSurface = 0;
		wkStar->block[i].numVtx = 0;
		wkStar->block[i].use = false;

		wkStar->block[i].numVtxNewBlock = 0;
		wkStar->block[i].numVtxOldBlock = 0;

		wkStar->block[i].numCp = 0;

		for (int j = 0; j < MAX_SURFACE; j++)
		{
			wkStar->block[i].cntCrossLineSp[j] = FALSE_FLAG;
			wkStar->block[i].cntCrossLineEp[j] = FALSE_FLAG;
			wkStar->block[i].cntNewVtxToSp[j] = FALSE_FLAG;
			wkStar->block[i].cntNewVtxToEp[j] = FALSE_FLAG;

			wkStar->block[i].cp[j] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		}
		for (int j = 0; j < MAX_SURFACE * MAX_VTX_SURFACE; j++)
		{
			wkStar->block[i].transPos[j] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			wkStar->block[i].dotVtx[j] = 0;
		}

		wkStar->block[i].pos = D3DXVECTOR3(0.0f, 100.0f, 0.0f);

		//switch (cntStar % 2)
		//{
		//case 0:
		//	wkStar->block[i].pos = D3DXVECTOR3(0.0f - 60.0f * (MAX_STAR - 1) + cntStar * 120.0f, 100.0f, 0.0f);
		//	break;

		//case 1:
		//	wkStar->block[i].pos = D3DXVECTOR3(0.0f - 60.0f * (MAX_STAR - 1) + cntStar * 120.0f, 100.0f, 0.0f);
		//	break;
		//default:
		//	break;
		//}

		wkStar->block[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		wkStar->block[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		wkStar->block[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);


		for (int j = 0; j < MAX_SURFACE; j++)
		{
			wkStar->block[i].surface[j].numIdx = 0;
			wkStar->block[i].surface[j].numNewIdx = 0;
			wkStar->block[i].surface[j].numPolygon = 0;
			wkStar->block[i].surface[j].use = false;
			for (int k = 0; k < MAX_IDX_SURFACE; k++)
			{
				wkStar->block[i].surface[j].idx[k] = FALSE_FLAG;
			}

		}

	}

	//for (int i = 0; i < wkStar->numBlock; i++)
	for (int i = 0; i < INIT_NUM_BLOCK; i++)
	{
		// オブジェクトの頂点バッファを生成
		if (FAILED(device->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_SURFACE * MAX_VTX_SURFACE,	// 頂点データ用に確保するバッファサイズ(バイト単位)
			D3DUSAGE_WRITEONLY,												// 頂点バッファの使用法
			FVF_VERTEX_3D,															// 使用する頂点フォーマット
			D3DPOOL_MANAGED,												// リソースのバッファを保持するメモリクラスを指定
			&wkStar->block[i].vtxBuff,							// 頂点バッファインターフェースへのポインタ
			NULL)))															// NULLに設定
		{
			return E_FAIL;
		}

		{
			wkStar->block[i].vtxBuff->Lock(0, 0, (void**)&vtx, 0);
			// 頂点座標の設定
			vtx[0].vtx = D3DXVECTOR3(-wkStar->width / 2.0f, -wkStar->height / 2.0f, wkStar->depth / 2.0f);
			vtx[1].vtx = D3DXVECTOR3(-wkStar->width / 2.0f, -wkStar->height / 2.0f, -wkStar->depth / 2.0f);
			vtx[2].vtx = D3DXVECTOR3(wkStar->width / 2.0f, -wkStar->height / 2.0f, -wkStar->depth / 2.0f);
			vtx[3].vtx = D3DXVECTOR3(wkStar->width / 2.0f, -wkStar->height / 2.0f, wkStar->depth / 2.0f);
			vtx[4].vtx = D3DXVECTOR3(-wkStar->width / 2.0f, wkStar->height / 2.0f, -wkStar->depth / 2.0f);
			vtx[5].vtx = D3DXVECTOR3(-wkStar->width / 2.0f, wkStar->height / 2.0f, wkStar->depth / 2.0f);
			vtx[6].vtx = D3DXVECTOR3(wkStar->width / 2.0f, wkStar->height / 2.0f, wkStar->depth / 2.0f);
			vtx[7].vtx = D3DXVECTOR3(wkStar->width / 2.0f, wkStar->height / 2.0f, -wkStar->depth / 2.0f);


			D3DXCOLOR R = COLOR_VTX;//D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.8f);
									//	// 反射光の設定
			vtx[0].diffuse = R;//D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.5f);
			vtx[1].diffuse = R;//
			vtx[2].diffuse = R;
			vtx[3].diffuse = R;//D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.5f);
			vtx[4].diffuse = R;
			vtx[5].diffuse = R;
			vtx[6].diffuse = R;
			vtx[7].diffuse = R;


			D3DXVECTOR3 nor;
			for (int j = 0; j < INIT_NUM_VTX_BLOCK; j++)
			{
				D3DXVec3Normalize(&nor, &vtx[j].vtx);
				vtx[j].nor = nor;
			}

			wkStar->block[i].vtxBuff->Unlock();
		}

		wkStar->block[i].scl = INIT_BLOCK_SCALE;
		wkStar->block[i].rot = D3DXVECTOR3(0.0f, 0.0f,0.0f);

		wkStar->block[i].numSurface = INIT_NUM_SURFACE_BLOCK;
		wkStar->block[i].numVtx = INIT_NUM_VTX_BLOCK;
		wkStar->block[i].use = true;

		InitSurface(device, &wkStar->block[i]);

	}

	return S_OK;

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSurface(LPDIRECT3DDEVICE9 device, BLOCK* wkBlock)
{
	WORD *idx;
	for (int i = 0; i < MAX_SURFACE; i++)
	{
		wkBlock->surface[i].numIdx = 0;
		wkBlock->surface[i].numNewIdx = 0;
		wkBlock->surface[i].numPolygon = 0;
		wkBlock->surface[i].use = false;
		for (int j = 0; j < MAX_IDX_SURFACE; j++)
		{
			wkBlock->surface[i].idx[j] = FALSE_FLAG;
		}
	}


	for (int i = 0; i < wkBlock->numSurface; i++)
	{
		// オブジェクトのインデックスバッファを生成
		if (FAILED(device->CreateIndexBuffer(sizeof(WORD) * MAX_VTX_SURFACE * 3,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&wkBlock->surface[i].idxBuff,
			NULL)))
		{
			return E_FAIL;
		}

		{//インデックスバッファの中身を埋める

		 // インデックスデータの範囲をロックし、頂点バッファへのポインタを取得
			wkBlock->surface[i].idxBuff->Lock(0, 0, (void**)&idx, 0);

			idx[0] = blockIdxNo[i][0];
			idx[1] = blockIdxNo[i][1];
			idx[2] = blockIdxNo[i][2];
			idx[3] = blockIdxNo[i][3];


			// インデックスデータをアンロックする
			wkBlock->surface[i].idxBuff->Unlock();
		}
		wkBlock->surface[i].numIdx = NUM_SECTION_VTX;
		wkBlock->surface[i].numPolygon = 2;
		wkBlock->surface[i].use = true;


	}
	return S_OK;


}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStar(void)
{
	for (int i = 0; i < MAX_STAR; i++)
	{
		UninitBlock(&star[i]);
	}
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBlock(STAR* wkStar)
{
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (wkStar->block[i].vtxBuff != NULL)
		{// 頂点バッファの開放
			wkStar->block[i].vtxBuff->Release();
			wkStar->block[i].vtxBuff = NULL;
		}
		UninitSurface(&wkStar->block[i]);
	}

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSurface(BLOCK* wkBlock)
{
	for (int i = 0; i < MAX_SURFACE; i++)
	{
		if (wkBlock->surface[i].idxBuff != NULL)
		{// インデックスバッファの開放
			wkBlock->surface[i].idxBuff->Release();
			wkBlock->surface[i].idxBuff = NULL;
		}
	}
}

//=============================================================================
// ★の再初期化の処理
//=============================================================================
void ResetStar(STAR* wkStar)
{
	LPDIRECT3DDEVICE9 device = GetDevice();

	wkStar->use = false;
	wkStar->numBlock = INIT_NUM_BLOCK;
	wkStar->pos = D3DXVECTOR3(0.0f, 100.0f, 0.0f);
	UninitBlock(wkStar);
	InitBlock(device, wkStar);
}



//=============================================================================
// 描画処理
//=============================================================================
void DrawStar(void)
{
	LPDIRECT3DDEVICE9 device = GetDevice();

	//device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);				// Z比較あり

	// ラインティングを無効にする
	//device->SetRenderState(D3DRS_LIGHTING, FALSE);
	//device->SetRenderState(D3DRS_LIGHTING, TRUE);
	//device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);			// Z比較なし
	// テクスチャの設定
	//if (useWire)
	//{
	//	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	//}
	//else
	//{
	//	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//}

	//device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	for (int cntStar = 0; cntStar < MAX_STAR; cntStar++)
	{
		if (!star[cntStar].use)
		{
			continue;
		}

		for (int cntBlock = 0; cntBlock < MAX_BLOCK; cntBlock++)
		{
			DrawBlock(device, &star[cntStar].block[cntBlock]);
		}
	}

	// ラインティングを有効にする
	//device->SetRenderState(D3DRS_LIGHTING, TRUE);
	//device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);				// Z比較あり
	//device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);


	//device->SetRenderState(D3DRS_FOGENABLE, TRUE);

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBlock(LPDIRECT3DDEVICE9 device, BLOCK* block)
{
	if (!block->use)
	{
		return;
	}

	D3DXMATRIX mtxView, mtxScale, mtxTranslate, mtxRot;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&block->mtxWorld);

	// ビューマトリックスを取得
	mtxView = GetMtxViewData();

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, block->scl.x, block->scl.y, block->scl.z);
	D3DXMatrixMultiply(&block->mtxWorld, &block->mtxWorld, &mtxScale);

	// 移動を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, block->rot.y, block->rot.x, block->rot.z);
	D3DXMatrixMultiply(&block->mtxWorld, &block->mtxWorld, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, block->pos.x, block->pos.y, block->pos.z);
	D3DXMatrixMultiply(&block->mtxWorld, &block->mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, &block->mtxWorld);

	// 頂点バッファをデバイスのデータストリームにバインド
	device->SetStreamSource(0, block->vtxBuff, 0, sizeof(VERTEX_3D));

	// テクスチャの設定
	device->SetTexture(0, NULL);

	for (int cntSurface = 0; cntSurface < block->numSurface; cntSurface++)
	{
		DrawSurface(device, &block->surface[cntSurface]);
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSurface(LPDIRECT3DDEVICE9 device, SURFACE* surface)
{
	device->SetIndices(surface->idxBuff);

	// 頂点フォーマットの設定
	device->SetFVF(FVF_VERTEX_3D);

	// ポリゴンの描画
	device->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, surface->numIdx, 0, surface->numPolygon);
}



//=============================================================================
// 頂点情報の作成
//=============================================================================
//HRESULT MakeVertexStar(LPDIRECT3DDEVICE9 device)
//{
//	// オブジェクトの頂点バッファを生成
//	if(FAILED(device->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX * MAX_BLOCK,	// 頂点データ用に確保するバッファサイズ(バイト単位)
//												D3DUSAGE_WRITEONLY,						// 頂点バッファの使用法　
//												FVF_VERTEX_3D,							// 使用する頂点フォーマット
//												D3DPOOL_MANAGED,						// リソースのバッファを保持するメモリクラスを指定
//												&D3DVtxBuffStar,						// 頂点バッファインターフェースへのポインタ
//												NULL)))									// NULLに設定
//	{
//		return E_FAIL;
//	}
//
//	{//頂点バッファの中身を埋める
//		VERTEX_3D *vtx;
//
//		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
//		D3DVtxBuffStar->Lock(0, 0, (void**)&vtx, 0);
//
//		for(int cntStar = 0; cntStar < MAX_BLOCK; cntStar++, vtx += 4)
//		{
//			// 頂点座標の設定
//			vtx[0].vtx = D3DXVECTOR3(-BLOCK_WIDTH / 2.0f, 0.0f, 0.0f);
//			vtx[1].vtx = D3DXVECTOR3(-BLOCK_WIDTH / 2.0f, BLOCK_HEIGHT, 0.0f);
//			vtx[2].vtx = D3DXVECTOR3(BLOCK_WIDTH / 2.0f, 0.0f, 0.0f);
//			vtx[3].vtx = D3DXVECTOR3(BLOCK_WIDTH / 2.0f, BLOCK_HEIGHT, 0.0f);
//
//			// 反射光の設定
//			vtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//			vtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//			vtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//			vtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//
//			// テクスチャ座標の設定
//			vtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
//			vtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
//			vtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
//			vtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);
//		}
//
//		// 頂点データをアンロックする
//		D3DVtxBuffStar->Unlock();
//	}
//
//	return S_OK;
//}

//=============================================================================
// 頂点座標の設定
//=============================================================================
//void SetVertexStar(int idxStar, float width, float height)
//{
//	{//頂点バッファの中身を埋める
//		VERTEX_3D *vtx;
//
//		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
//		D3DVtxBuffStar->Lock(0, 0, (void**)&vtx, 0);
//
//		vtx += (idxStar * 4);
//
//		// 頂点座標の設定
//		vtx[0].vtx = D3DXVECTOR3(-width / 2.0f, 0.0f, 0.0f);
//		vtx[1].vtx = D3DXVECTOR3(-width / 2.0f, height, 0.0f);
//		vtx[2].vtx = D3DXVECTOR3(width / 2.0f, 0.0f, 0.0f);
//		vtx[3].vtx = D3DXVECTOR3(width / 2.0f, height, 0.0f);
//
//		// 頂点データをアンロックする
//		D3DVtxBuffStar->Unlock();
//	}
//}

//=============================================================================
// 頂点カラーの設定
//=============================================================================
//void SetColorStar(int idxStar, D3DXCOLOR col)
//{
//	{//頂点バッファの中身を埋める
//		VERTEX_3D *vtx;
//
//		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
//		D3DVtxBuffStar->Lock(0, 0, (void**)&vtx, 0);
//
//		vtx += (idxStar * 4);
//
//		// 頂点座標の設定
//		vtx[0].diffuse = col;
//		vtx[1].diffuse = col;
//		vtx[2].diffuse = col;
//		vtx[3].diffuse = col;
//
//		// 頂点データをアンロックする
//		D3DVtxBuffStar->Unlock();
//	}
//}

//=============================================================================
// ★を設定
//=============================================================================
int SetStar(D3DXVECTOR3 pos, D3DXVECTOR3 move)
{
	int idxStar = -1;

	for (int cntStar = 0; cntStar < MAX_STAR; cntStar++)
	{
		if (!star[cntStar].use)
		{
			star[cntStar].use = true;
			star[cntStar].numBlock = INIT_NUM_BLOCK;
			//star[cntStar].pos = pos;

			SetMoveStar(&star[cntStar], pos, move);


			//Star[cntStar].pos = pos;
			//Star[cntStar].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			//Star[cntStar].width = width;
			//Star[cntStar].height = height;
			//Star[cntStar].bUse = true;
			//// 頂点座標の設定
			//SetVertexStar(cntStar, width, height);
			//// 頂点カラーの設定
			//SetColorStar(cntStar, col);
			//// 影の設定
			//Star[cntStar].nIdxShadow = CreateShadow(Star[cntStar].pos, Star[cntStar].width, Star[cntStar].width);

			idxStar = cntStar;

			break;
		}
	}

	return idxStar;
}

//=============================================================================
// ★の移動設定
//=============================================================================
void SetMoveStar(STAR* star, D3DXVECTOR3 pos, D3DXVECTOR3 move)
{
	for (int cntBlock = 0; cntBlock < INIT_NUM_BLOCK; cntBlock++)
	{
		//if (!star->block[cntBlock].use) { continue; }

		star->block[cntBlock].pos = pos;
		//star->block[cntBlock].pos.y -= 5.0f;
		//star->block[cntBlock].pos.x += 1.0f;


		star->block[cntBlock].move = move;

	}
}




//=============================================================================
// ブロック群体と切断面の衝突判定
// section	:切断面の４頂点の座標
//=============================================================================
bool ClippingStar(PLANE section)
{
	if (section.vtx[0] == section.vtx[2])
	{
		return true;;
	}

	//D3DXMATRIX mtxPos, mtxPosInverse;

	int numOldBlock;
	bool useMove, useCheck;
	float lengthSection, distance;

	D3DXVECTOR3 wkLength = section.vtx[1] - section.vtx[0];
	D3DXVECTOR3 wkPos, tmpVec, checkPos;


	lengthSection = D3DXVec3LengthSq(&wkLength);

	//lengthSection -= 50.0f;

	tmpVec = section.vtx[1] - section.vtx[0];
	tmpVec /= 2.0f;

	wkPos = section.vtx[0] + tmpVec;


	for (int cntStar = 0; cntStar < MAX_STAR; cntStar++)
	{
		if (!star[cntStar].use)
		{
			continue;
		}

		useMove = false;
		useCheck = false;
		numOldBlock = star[cntStar].numBlock;

		checkPos = star[cntStar].block[0].pos;
		for (int cntBlock = 0; cntBlock < numOldBlock; cntBlock++)
		{
			if (checkPos != star[cntStar].block[cntBlock].pos)
			{
				useCheck = true;
			}
		}

		if (useCheck)
		{
			continue;
		}
		else
		{
			wkLength = checkPos - wkPos;

			distance = D3DXVec3LengthSq(&wkLength);

			if (distance > lengthSection)
			{
				continue;
			}
		}


		for (int cntBlock = 0; cntBlock < numOldBlock; cntBlock++)
		{
			if (star[cntStar].numBlock >= MAX_BLOCK)
			{
				break;
			}
			if (ClippingBlock(section, &star[cntStar], &star[cntStar].block[cntBlock]))
			{
				SetSE(1);

				AddScore(1);

				useMove = true;
			}
		}

		if (useMove)
		{// 切断されたブロックの動きを決める

			SetMoveBlock(&star[cntStar], section);

			//return true;
		}
		//star[cntStar].numBlock += numNewBlock;

	}

	return false;
}

//=============================================================================
// 切断後のブロックの動きをセット
// star	: ブロック群体のポインター
//=============================================================================
void SetMoveBlock(STAR* wkStar, PLANE section)
{
	VERTEX_3D *vtxBuff;
	D3DXMATRIX mtxTranslate, mtxRot, mtxScale, mtxTmp;
	D3DXVECTOR3 wkVtx, wkVec;
	float dot;
	int cntNew, cntOld;

	for (int cntBlock = 0; cntBlock < wkStar->numBlock; cntBlock++)
	{
		cntNew = 0;
		cntOld = 0;

		D3DXMatrixIdentity(&mtxTmp);

		// スケールを反映
		D3DXMatrixScaling(&mtxScale, wkStar->block[cntBlock].scl.x, wkStar->block[cntBlock].scl.y, wkStar->block[cntBlock].scl.z);
		D3DXMatrixMultiply(&mtxTmp, &mtxTmp, &mtxScale);

		D3DXMatrixRotationYawPitchRoll(&mtxRot, wkStar->block[cntBlock].rot.y, wkStar->block[cntBlock].rot.x, wkStar->block[cntBlock].rot.z);
		D3DXMatrixMultiply(&mtxTmp, &mtxTmp, &mtxRot);

		D3DXMatrixTranslation(&mtxTranslate, wkStar->block[cntBlock].pos.x, wkStar->block[cntBlock].pos.y, wkStar->block[cntBlock].pos.z);
		D3DXMatrixMultiply(&mtxTmp, &mtxTmp, &mtxTranslate);

		wkStar->block[cntBlock].vtxBuff->Lock(0, 0, (void**)&vtxBuff, 0);

		for (int cntVtx = 0; cntVtx < wkStar->block[cntBlock].numVtx; cntVtx++)
		{
			D3DXVec3TransformCoord(&wkVtx, &vtxBuff[cntVtx].vtx, &mtxTmp);

			wkVec = wkVtx - section.vtx[0];

			dot = D3DXVec3Dot(&section.nor, &wkVec);

			if (dot >= 0)
			{
				cntOld++;
			}
			else
			{
				cntNew++;
			}

		}

		wkStar->block[cntBlock].vtxBuff->Unlock();

		wkStar->block[cntBlock].move *= 1.5f;
		if (cntOld > cntNew)
		{
			wkStar->block[cntBlock].move += section.nor * VAL_BLOCK_MOVE;
		}
		else
		{
			wkStar->block[cntBlock].move -= section.nor * VAL_BLOCK_MOVE;
		}

	}

}


//=============================================================================
// ブロックと切断面の衝突判定
// star	: ブロック群体のポインター
// section	:切断面の４頂点の座標
//=============================================================================
bool ClippingBlock(PLANE section, STAR* star, BLOCK* block)
{
	if (!block->use || star->numBlock >= MAX_BLOCK)
	{
		return false;
	}

	D3DXMATRIX mtxTranslate, mtxRot, mtxScale;

	D3DXMatrixIdentity(&block->mtxPos);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, block->scl.x, block->scl.y, block->scl.z);
	D3DXMatrixMultiply(&block->mtxPos, &block->mtxPos, &mtxScale);

	D3DXMatrixRotationYawPitchRoll(&mtxRot, block->rot.y, block->rot.x, block->rot.z);
	D3DXMatrixMultiply(&block->mtxPos, &block->mtxPos, &mtxRot);

	D3DXMatrixTranslation(&mtxTranslate, block->pos.x, block->pos.y, block->pos.z);
	D3DXMatrixMultiply(&block->mtxPos, &block->mtxPos, &mtxTranslate);

	D3DXMatrixInverse(&block->invMtxPos, NULL, &block->mtxPos);

	VERTEX_3D *vtxBuff;
	WORD *idxBuff;

	block->numVtxOldBlock = 0;
	block->numVtxNewBlock = 0;


	// ブロックの頂点座標とインデックスを取る
	block->vtxBuff->Lock(0, 0, (void**)&vtxBuff, 0);
	for (int cntVtx = 0; cntVtx < block->numVtx; cntVtx++)
	{// ブロックのローカル座標をワールド座標に変換
		D3DXVec3TransformCoord(&block->transPos[cntVtx], &vtxBuff[cntVtx].vtx, &block->mtxPos);
	}
	block->vtxBuff->Unlock();

	for (int cntSurface = 0; cntSurface < block->numSurface; cntSurface++)
	{// ブロックの各面のインデックス番号をコピーする
		block->surface[cntSurface].idxBuff->Lock(0, 0, (void**)&idxBuff, 0);
		for (int cntIdx = 0; cntIdx < block->surface[cntSurface].numIdx; cntIdx++)
		{
			block->surface[cntSurface].idx[cntIdx] = idxBuff[cntIdx];
		}
		block->surface[cntSurface].idxBuff->Unlock();
	}

	// 切断面がブロックを貫通してなかったら処理終了
	if (HitCheckSectionLine(block, section))
	{
		return false;
	}

	// 切断面でブロックが二分されていなかったら終了
	if (CheckBlockDot(section, block))
	{
		return false;
	}

	block->numCp = 0;
	// 切断面で二分された線分の交点を求める
	for (int cntSurface = 0; cntSurface < block->numSurface; cntSurface++)
	{
		ClippingLine(section, block, &block->surface[cntSurface]);
	}

	if (block->numCp == 0)
	{
		return false;
	}

	VERTEX_3D *vtx;
	block->vtxBuff->Lock(0, 0, (void**)&vtx, 0);
	// 求められた交点をもとに新しい頂点座標を生成する
	for (int cntVtx = 0; cntVtx < block->numVtx; cntVtx++)
	{
		vtx[cntVtx].diffuse = COLOR_VTX;

	}
	for (int cntCp = 0; cntCp < block->numCp; cntCp++)
	{
		CreateNewVtxPos(block, cntCp, vtx);
	}
	block->vtxBuff->Unlock();



	// 交点同士を繋げて切断面を生成
	CreateNewSurface(block, section);

	//int idx[MAX_SURFACE][MAX_IDX_SURFACE];
	//for (int i = 0; i < MAX_SURFACE; i++)
	//{
	//	for (int j = 0; j < MAX_IDX_SURFACE; j++)
	//	{
	//		idx[i][j] = block->surface[i].idx[j];
	//	}
	//}

	// 既存のインデックスを修正する
	CategorizeBlock(block);

	SetBlockData(block, &star->block[star->numBlock]);
	SetSurfaceIdxBuff(block, &star->block[star->numBlock]);


	//block->move = section.nor / VAL_BLOCK_MOVE;
	star->block[star->numBlock].move = block->move;
	star->block[star->numBlock].pos = block->pos;
	star->block[star->numBlock].rot = block->rot;
	star->block[star->numBlock].scl = block->scl;

	//block->pos += section.nor * 20.0f;
	//star->block[star->numBlock].pos = block->pos - section.nor * 20.0f;

	star->numBlock++;


	return true;

	// block構造体を既存のブロックと新しいブロックに分離する


}

//=============================================================================
// 面ごとのインデックスバッファを再入力する
// oldBlock	:元のブロックのポインター
// newBlock	:新しく生成するブロックのポインター
//=============================================================================
void SetSurfaceIdxBuff(BLOCK* oldBlock, BLOCK* newBlock)
{
	WORD *idx;
	LPDIRECT3DDEVICE9 device = GetDevice();

	for (int cntSurface = 0; cntSurface < oldBlock->numSurface + newBlock->numSurface; cntSurface++)
	{
		if (cntSurface < oldBlock->numSurface)
		{
			if (oldBlock->surface[cntSurface].idxBuff != NULL)
			{
				oldBlock->surface[cntSurface].idxBuff->Release();
				oldBlock->surface[cntSurface].idxBuff = NULL;
			}

			if (oldBlock->surface[cntSurface].idxBuff == NULL)
			{
				device->CreateIndexBuffer(sizeof(WORD) * oldBlock->surface[cntSurface].numIdx + 4,
					D3DUSAGE_WRITEONLY,
					D3DFMT_INDEX16,
					D3DPOOL_MANAGED,
					&oldBlock->surface[cntSurface].idxBuff,
					NULL);
			}

			oldBlock->surface[cntSurface].idxBuff->Lock(0, 0, (void**)&idx, 0);
			for (int cntIdx = 0; cntIdx < oldBlock->surface[cntSurface].numIdx; cntIdx++)
			{
				idx[cntIdx] = oldBlock->surface[cntSurface].idx[cntIdx];
			}
			oldBlock->surface[cntSurface].idxBuff->Unlock();

			oldBlock->surface[cntSurface].numNewIdx = 0;
			oldBlock->surface[cntSurface].use = true;
		}
		else
		{
			if (oldBlock->surface[cntSurface].idxBuff != NULL)
			{
				oldBlock->surface[cntSurface].idxBuff->Release();
				oldBlock->surface[cntSurface].idxBuff = NULL;

			}
			oldBlock->surface[cntSurface].numIdx = 0;
			oldBlock->surface[cntSurface].numPolygon = 0;
			oldBlock->surface[cntSurface].numNewIdx = 0;
			oldBlock->surface[cntSurface].use = false;

		}
	}

	for (int cntSurface = 0; cntSurface < newBlock->numSurface; cntSurface++)
	{
		device->CreateIndexBuffer(sizeof(WORD) * newBlock->surface[cntSurface].numIdx * 2,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&newBlock->surface[cntSurface].idxBuff,
			NULL);

		newBlock->surface[cntSurface].idxBuff->Lock(0, 0, (void**)&idx, 0);

		for (int cntIdx = 0; cntIdx < newBlock->surface[cntSurface].numIdx; cntIdx++)
		{
			idx[cntIdx] = newBlock->surface[cntSurface].idx[cntIdx];
		}
		newBlock->surface[cntSurface].idxBuff->Unlock();

		newBlock->surface[cntSurface].use = true;
	}
}



//=============================================================================
// 切断面を基準に左右に分かれた頂点を二つのブロックに再編する
// oldBlock	:元のブロックのポインター
// newBlock	:新しく生成するブロックのポインター
//=============================================================================
void SetBlockData(BLOCK* oldBlock, BLOCK* newBlock)
{
	LPDIRECT3DDEVICE9 device = GetDevice();
	VERTEX_3D *oldVtx, *newVtx;
	VERTEX_3D tmpVtxOld[MAX_SURFACE * MAX_VTX_SURFACE];
	WORD tmpCntVtx[MAX_SURFACE * MAX_VTX_SURFACE];

	//WORD *idx;

	int	cntOldVtx = 0;
	int cntNewVtx = 0;

	// オブジェクトの頂点バッファを生成
	device->CreateVertexBuffer(sizeof(VERTEX_3D) * oldBlock->numVtxNewBlock * 4,	// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,												// 頂点バッファの使用法
		FVF_VERTEX_3D,															// 使用する頂点フォーマット
		D3DPOOL_MANAGED,												// リソースのバッファを保持するメモリクラスを指定
		&newBlock->vtxBuff,												// 頂点バッファインターフェースへのポインタ
		NULL);															// NULLに設定


	newBlock->numSurface = 0;
	newBlock->numVtx = 0;
	newBlock->use = true;

	//int numOldSurface = 0;
	//int numNewSurface = 0;

	// 頂点バッファのデータを分離
	newBlock->vtxBuff->Lock(0, 0, (void**)&newVtx, 0);
	oldBlock->vtxBuff->Lock(0, 0, (void**)&oldVtx, 0);

	for (int cntVtx = 0; cntVtx < oldBlock->numVtx; cntVtx++)
	{
		if (oldBlock->dotVtx[cntVtx] >= 0)
		{
			tmpCntVtx[cntVtx] = cntOldVtx;
			tmpVtxOld[cntOldVtx] = oldVtx[cntVtx];
			cntOldVtx++;
		}
		else
		{
			tmpCntVtx[cntVtx] = cntNewVtx;
			cntNewVtx++;
			newVtx[newBlock->numVtx] = oldVtx[cntVtx];
			newBlock->numVtx++;
		}

	}

	for (int cntVtx = 0; cntVtx < cntOldVtx; cntVtx++)
	{
		oldVtx[cntVtx] = tmpVtxOld[cntVtx];
	}
	oldBlock->numVtx = cntOldVtx;

	newBlock->vtxBuff->Unlock();
	oldBlock->vtxBuff->Unlock();

	int tmpIdx;
	int cntOldSurface = 0;
	int cntNewSurface = 0;
	int numOldSurfaceIdx[MAX_SURFACE];
	WORD idxOldSurface[MAX_SURFACE][MAX_IDX_SURFACE];

	for (int cntSurface = 0; cntSurface < oldBlock->numSurface; cntSurface++)
	{
		tmpIdx = oldBlock->surface[cntSurface].idx[0];

		if (oldBlock->dotVtx[tmpIdx] >= 0)
		{
			numOldSurfaceIdx[cntOldSurface] = 0;

			for (int cntIdx = 0; cntIdx < oldBlock->surface[cntSurface].numIdx; cntIdx++)
			{
				idxOldSurface[cntOldSurface][cntIdx] = tmpCntVtx[oldBlock->surface[cntSurface].idx[cntIdx]];
				numOldSurfaceIdx[cntOldSurface]++;
			}
			cntOldSurface++;
		}
		else
		{
			newBlock->surface[cntNewSurface].numIdx = oldBlock->surface[cntSurface].numIdx;
			newBlock->surface[cntNewSurface].numPolygon = oldBlock->surface[cntSurface].numPolygon;
			newBlock->surface[cntNewSurface].use = true;

			for (int cntIdx = 0; cntIdx < newBlock->surface[cntNewSurface].numIdx; cntIdx++)
			{
				//インデックスバッファの中身を埋める
				newBlock->surface[cntNewSurface].idx[cntIdx] = tmpCntVtx[oldBlock->surface[cntSurface].idx[cntIdx]];

				//idx[cntIdx] = tmpCntVtx[oldBlock->surface[cntSurface].idx[cntIdx]];

			}

			cntNewSurface++;

		}
	}

	for (int cntSurface = 0; cntSurface < cntOldSurface; cntSurface++)
	{
		for (int cntIdx = 0; cntIdx < numOldSurfaceIdx[cntSurface]; cntIdx++)
		{
			oldBlock->surface[cntSurface].idx[cntIdx] = idxOldSurface[cntSurface][cntIdx];
		}
		oldBlock->surface[cntSurface].numIdx = numOldSurfaceIdx[cntSurface];
		oldBlock->surface[cntSurface].numPolygon = numOldSurfaceIdx[cntSurface] - 2;

	}

	oldBlock->numSurface = cntOldSurface;
	newBlock->numSurface = cntNewSurface;


	for (int cntCp = 0; cntCp < oldBlock->numCp; cntCp++)
	{
		oldBlock->cntCrossLineSp[cntCp] = 0;
		oldBlock->cntCrossLineEp[cntCp] = 0;
		oldBlock->cntNewVtxToSp[cntCp] = 0;
		oldBlock->cntNewVtxToEp[cntCp] = 0;
		oldBlock->cp[cntCp] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	oldBlock->numVtxNewBlock = 0;
	oldBlock->numVtxOldBlock = 0;
	oldBlock->numCp = 0;


}

//
////=============================================================================
//// 新しく作るブロックの初期化
//// newBlock	:初期化するブロックのポインター
////=============================================================================
//void InitNewBlock(BLOCK* newBlock)
//{
//
//}



//=============================================================================
// 切断面を基準に左右に分かれた頂点を二つのブロックに再編する
// block	:ブロックのポインター
//=============================================================================
void CategorizeBlock(BLOCK* block)
{
	SURFACE *surface;
	int ep;
	int cntVtx;

	int idx, cntNewSurface, cntNewIdx, tmpIdx[TWO_SURFACE][TWO_POS];

	int numNewSurface = 0;

	bool checkContinue;

	//int asdf[20][20];


	for (int cntSurface = 0; cntSurface < block->numSurface; cntSurface++)
	{
		checkContinue = true;
		cntVtx = FALSE_FLAG;

		surface = &block->surface[cntSurface];

		for (int cntIdx = 0; cntIdx < surface->numIdx; cntIdx++)
		{
			ep = (cntIdx + 1) % surface->numIdx;
			cntVtx = CheckCalculatedHitCheck(block, surface->idx[cntIdx], surface->idx[ep]);
			if (cntVtx != FALSE_FLAG)
			{
				if (block->dotVtx[surface->idx[cntIdx]] < 0)
				{// 始点がnewの場合
					if (block->cntCrossLineSp[cntVtx] == surface->idx[cntIdx])
					{
						tmpIdx[NEW_SURFACE][START_POS] = block->cntCrossLineSp[cntVtx];
						tmpIdx[NEW_SURFACE][END_POS] = block->cntNewVtxToSp[cntVtx];
						tmpIdx[OLD_SURFACE][START_POS] = block->cntNewVtxToEp[cntVtx];
						tmpIdx[OLD_SURFACE][END_POS] = block->cntCrossLineEp[cntVtx];
					}
					else
					{
						tmpIdx[NEW_SURFACE][START_POS] = block->cntCrossLineEp[cntVtx];
						tmpIdx[NEW_SURFACE][END_POS] = block->cntNewVtxToEp[cntVtx];
						tmpIdx[OLD_SURFACE][START_POS] = block->cntNewVtxToSp[cntVtx];
						tmpIdx[OLD_SURFACE][END_POS] = block->cntCrossLineSp[cntVtx];
					}

				}
				else if (block->dotVtx[surface->idx[cntIdx]] >= 0)
				{
					if (block->cntCrossLineSp[cntVtx] == surface->idx[cntIdx])
					{
						tmpIdx[OLD_SURFACE][START_POS] = block->cntCrossLineSp[cntVtx];
						tmpIdx[OLD_SURFACE][END_POS] = block->cntNewVtxToSp[cntVtx];
						tmpIdx[NEW_SURFACE][START_POS] = block->cntNewVtxToEp[cntVtx];
						tmpIdx[NEW_SURFACE][END_POS] = block->cntCrossLineEp[cntVtx];
					}
					else
					{
						tmpIdx[OLD_SURFACE][START_POS] = block->cntCrossLineEp[cntVtx];
						tmpIdx[OLD_SURFACE][END_POS] = block->cntNewVtxToEp[cntVtx];
						tmpIdx[NEW_SURFACE][START_POS] = block->cntNewVtxToSp[cntVtx];
						tmpIdx[NEW_SURFACE][END_POS] = block->cntCrossLineSp[cntVtx];
					}
				}

				checkContinue = false;

				break;
			}

		}

		if (checkContinue)
		{
			continue;
		}

		cntNewSurface = block->numSurface + numNewSurface;
		block->surface[cntNewSurface].numIdx = 0;
		// 新しい面に移動

		int wkIdx[MAX_IDX_SURFACE];
		int numIdxOldSurface = 0;
		for (int cntIdx = 0; cntIdx < surface->numIdx + surface->numNewIdx; cntIdx++)
		{
			idx = surface->idx[cntIdx];
			cntNewIdx = block->surface[cntNewSurface].numIdx;

			if (block->dotVtx[idx] < 0)
			{
				block->surface[cntNewSurface].idx[cntNewIdx] = idx;
				block->surface[cntNewSurface].numIdx++;
			}
			else
			{
				wkIdx[numIdxOldSurface] = idx;
				numIdxOldSurface++;
			}

		}

		surface->numIdx = numIdxOldSurface;
		surface->numNewIdx = block->surface[cntNewSurface].numIdx;

		for (int cntIdx = 0; cntIdx < surface->numIdx; cntIdx++)
		{
			surface->idx[cntIdx] = wkIdx[cntIdx];
		}


		D3DXVECTOR3 standardVec, vec01, vec02;
		float sortDotVal[MAX_IDX_SURFACE];
		WORD sortDotIdx[MAX_IDX_SURFACE], tmpNumIdx;

		float tmpDotVal;
		WORD tmpDotIdx;
		int sortCnt;// , tmpCnt;

		SURFACE *adrSurface;

		int tmpCnt;
		//// インデックスの順番を直す
		for (int cntSurface = 0; cntSurface < TWO_SURFACE; cntSurface++)
		{
			if (cntSurface == NEW_SURFACE)
			{
				adrSurface = &block->surface[cntNewSurface];

			}
			else
			{
				adrSurface = surface;

			}

			vec01 = block->transPos[tmpIdx[cntSurface][END_POS]] - block->transPos[tmpIdx[cntSurface][START_POS]];
			D3DXVec3Normalize(&vec01, &vec01);

			tmpCnt = 0;
			for (int cntVtx = 0; cntVtx < adrSurface->numIdx; cntVtx++)
			{
				if (adrSurface->idx[cntVtx] == tmpIdx[cntSurface][START_POS])
				{
					continue;
				}

				tmpNumIdx = adrSurface->idx[cntVtx];

				vec02 = block->transPos[tmpNumIdx] - block->transPos[tmpIdx[cntSurface][START_POS]];
				D3DXVec3Normalize(&vec02, &vec02);

				sortDotVal[tmpCnt] = D3DXVec3Dot(&vec01, &vec02);
				sortDotIdx[tmpCnt] = tmpNumIdx;
				tmpCnt++;

			}

			for (int cntVtx = 0; cntVtx < adrSurface->numIdx - 2; cntVtx++)
			{
				sortCnt = cntVtx + 1;
				for (int i = cntVtx; i >= 0; i--)
				{
					if (sortDotVal[sortCnt] > sortDotVal[i])
					{
						tmpDotVal = sortDotVal[i];
						tmpDotIdx = sortDotIdx[i];

						sortDotVal[i] = sortDotVal[sortCnt];
						sortDotIdx[i] = sortDotIdx[sortCnt];

						sortDotVal[sortCnt] = tmpDotVal;
						sortDotIdx[sortCnt] = tmpDotIdx;
						sortCnt--;
					}

				}

			}

			adrSurface->idx[0] = tmpIdx[cntSurface][START_POS];
			for (int cntIdx = 0; cntIdx < adrSurface->numIdx - 1; cntIdx++)
			{
				adrSurface->idx[cntIdx + 1] = sortDotIdx[cntIdx];
			}

			adrSurface->numPolygon = adrSurface->numIdx - 2;

		}


		numNewSurface++;

	}

	block->numSurface += numNewSurface;



}

//=============================================================================
// 交点から生成した頂点同士を繋げて新しい面を生成する
// block	:ブロックのポインター
// section	:切断面の構造体
//=============================================================================
void CreateNewSurface(BLOCK* block, PLANE section)
{
	D3DXVECTOR3 tmpVec, tmpVtx;
	int cntNewVtx;
	int cntNewSurfaceOldBlock = block->numSurface;
	int cntNewSurfaceNewBlock = block->numSurface + 1;

	for (int cntVtx = 0; cntVtx < block->numCp * 2; cntVtx++)
	{
		cntNewVtx = block->numVtx + cntVtx;

		tmpVec = block->transPos[cntNewVtx] - section.vtx[0];
		D3DXVec3Normalize(&tmpVec, &tmpVec);

		block->dotVtx[cntNewVtx] = DotProduct(section.nor, tmpVec);

		if (block->dotVtx[cntNewVtx] >= 0)
		{
			block->surface[cntNewSurfaceOldBlock].idx[block->surface[cntNewSurfaceOldBlock].numIdx] = cntNewVtx;
			block->surface[cntNewSurfaceOldBlock].numIdx++;
			block->numVtxOldBlock++;
		}
		else if (block->dotVtx[cntNewVtx] < 0)
		{
			block->surface[cntNewSurfaceNewBlock].idx[block->surface[cntNewSurfaceNewBlock].numIdx] = cntNewVtx;
			block->surface[cntNewSurfaceNewBlock].numIdx++;
			block->numVtxNewBlock++;
		}
		//else if (block->dotVtx[cntNewVtx] == 0)
		//{
		//	break;
		//}
	}

	D3DXVECTOR3 cross, vec01, vec02;
	float dot;
	WORD sp01, ep01, ep02;

	float tmpDotVal;
	WORD tmpDotIdx;
	int sortCnt;

	float sortDotVal[MAX_IDX_SURFACE];
	WORD sortDotIdx[MAX_IDX_SURFACE];
	{
		sp01 = block->surface[cntNewSurfaceOldBlock].idx[0];
		ep01 = block->surface[cntNewSurfaceOldBlock].idx[1];
		ep02 = block->surface[cntNewSurfaceOldBlock].idx[2];

		vec01 = block->transPos[ep01] - block->transPos[sp01];
		vec02 = block->transPos[ep02] - block->transPos[sp01];

		D3DXVec3Normalize(&vec01, &vec01);

		D3DXVec3Cross(&cross, &vec01, &vec02);
		D3DXVec3Normalize(&cross, &cross);

		dot = D3DXVec3Dot(&cross, &section.nor);


		D3DXVec3Normalize(&vec01, &vec01);

		for (int cntVtx = 0; cntVtx < block->numCp; cntVtx++)
		{
			if (cntVtx == 0)
			{
				continue;
			}
			ep02 = block->surface[cntNewSurfaceOldBlock].idx[cntVtx];

			vec02 = block->transPos[ep02] - block->transPos[sp01];
			D3DXVec3Normalize(&vec02, &vec02);

			sortDotVal[cntVtx - 1] = D3DXVec3Dot(&vec01, &vec02);
			sortDotIdx[cntVtx - 1] = ep02;

		}

		for (int cntVtx = 0; cntVtx < block->numCp - 2; cntVtx++)
		{
			sortCnt = cntVtx + 1;
			for (int i = cntVtx; i >= 0; i--)
			{
				if ((dot < 0 && (sortDotVal[sortCnt] > sortDotVal[i])) ||
					(dot > 0 && (sortDotVal[sortCnt] < sortDotVal[i])))
				{
					tmpDotVal = sortDotVal[i];
					tmpDotIdx = sortDotIdx[i];

					sortDotVal[i] = sortDotVal[sortCnt];
					sortDotIdx[i] = sortDotIdx[sortCnt];

					sortDotVal[sortCnt] = tmpDotVal;
					sortDotIdx[sortCnt] = tmpDotIdx;
					sortCnt--;
				}

			}

		}

		WORD *idx;
		// オブジェクトのインデックスバッファを生成

		GetDevice()->CreateIndexBuffer(sizeof(WORD) * MAX_IDX_SURFACE * 3,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&block->surface[cntNewSurfaceOldBlock].idxBuff,
			NULL);

		for (int cntIdx = 0; cntIdx < block->numCp - 1; cntIdx++)
		{
			block->surface[cntNewSurfaceOldBlock].idx[cntIdx + 1] = sortDotIdx[cntIdx];
		}

		block->surface[cntNewSurfaceOldBlock].idxBuff->Lock(0, 0, (void**)&idx, 0);
		for (int cntIdx = 0; cntIdx < block->numCp; cntIdx++)
		{
			idx[cntIdx] = block->surface[cntNewSurfaceOldBlock].idx[cntIdx];
		}

		block->surface[cntNewSurfaceOldBlock].idxBuff->Unlock();

		block->surface[cntNewSurfaceOldBlock].numIdx = block->numCp;
		block->surface[cntNewSurfaceOldBlock].numPolygon = block->numCp - 2;
		block->surface[cntNewSurfaceOldBlock].use = true;
		block->numSurface++;
		block->numVtx += block->numCp;
	}

	{
		sp01 = block->surface[cntNewSurfaceNewBlock].idx[0];
		ep01 = block->surface[cntNewSurfaceNewBlock].idx[1];
		ep02 = block->surface[cntNewSurfaceNewBlock].idx[2];

		vec01 = block->transPos[ep01] - block->transPos[sp01];
		vec02 = block->transPos[ep02] - block->transPos[sp01];


		D3DXVec3Cross(&cross, &vec01, &vec02);
		D3DXVec3Normalize(&cross, &cross);

		dot = D3DXVec3Dot(&cross, &section.nor);

		D3DXVec3Normalize(&vec01, &vec01);

		for (int cntVtx = 0; cntVtx < block->numCp; cntVtx++)
		{
			if (cntVtx == 0)
			{
				continue;
			}
			ep02 = block->surface[cntNewSurfaceNewBlock].idx[cntVtx];

			vec02 = block->transPos[ep02] - block->transPos[sp01];
			D3DXVec3Normalize(&vec02, &vec02);

			sortDotVal[cntVtx - 1] = D3DXVec3Dot(&vec01, &vec02);
			sortDotIdx[cntVtx - 1] = ep02;

		}

		for (int cntVtx = 0; cntVtx < block->numCp - 2; cntVtx++)
		{
			sortCnt = cntVtx + 1;
			for (int i = cntVtx; i >= 0; i--)
			{
				if ((dot < 0 && (sortDotVal[sortCnt] < sortDotVal[i])) ||
					(dot > 0 && (sortDotVal[sortCnt] > sortDotVal[i])))
				{
					tmpDotVal = sortDotVal[i];
					tmpDotIdx = sortDotIdx[i];

					sortDotVal[i] = sortDotVal[sortCnt];
					sortDotIdx[i] = sortDotIdx[sortCnt];

					sortDotVal[sortCnt] = tmpDotVal;
					sortDotIdx[sortCnt] = tmpDotIdx;
					sortCnt--;
				}

			}

		}

		//WORD *idx;
		//// オブジェクトのインデックスバッファを生成

		//GetDevice()->CreateIndexBuffer(sizeof(WORD) * MAX_IDX_SURFACE,
		//	D3DUSAGE_WRITEONLY,
		//	D3DFMT_INDEX16,
		//	D3DPOOL_MANAGED,
		//	&block->surface[cntNewSurfaceNewBlock].idxBuff,
		//	NULL);

		for (int cntIdx = 0; cntIdx < block->numCp - 1; cntIdx++)
		{
			block->surface[cntNewSurfaceNewBlock].idx[cntIdx + 1] = sortDotIdx[cntIdx];
		}

		//block->surface[cntNewSurfaceNewBlock].idxBuff->Lock(0, 0, (void**)&idx, 0);
		//for (int cntIdx = 0; cntIdx < block->numCp; cntIdx++)
		//{
		//	idx[cntIdx] = block->surface[cntNewSurfaceNewBlock].idx[cntIdx];
		//}

		//block->surface[cntNewSurfaceNewBlock].idxBuff->Unlock();


		block->surface[cntNewSurfaceNewBlock].numIdx = block->numCp;
		block->surface[cntNewSurfaceNewBlock].numPolygon = block->numCp - 2;
		block->surface[cntNewSurfaceNewBlock].use = false;
		block->numSurface++;

		block->numVtx += block->numCp;

	}


}

//=============================================================================
// ブロックを構成する線分と切断面との交点を求めてblock構造体内に保存
// block:ブロックのポインター
// cntCp:交点の番号
//=============================================================================
void CreateNewVtxPos(BLOCK* block, int cntCp, VERTEX_3D* vtx)
{
	D3DXVECTOR3 tmpSp, tmpEp, length, nor;
	int sp = block->cntCrossLineSp[cntCp];
	int ep = block->cntCrossLineEp[cntCp];
	int cntVtx = block->numVtx + (cntCp * 2);

	tmpSp = block->transPos[sp] - block->cp[cntCp];
	//tmpEp = block->transPos[ep] - block->cp[cntCp];
	D3DXVec3Normalize(&length, &tmpSp);
	length /= 2.0f;

	if (D3DXVec3LengthSq(&length) > D3DXVec3LengthSq(&tmpSp))
	{
		length = tmpSp / 2.0f;
	}
	else
	{
		length /= 2.0f;
	}
	//tmpが長すぎるときの例外設定

	block->transPos[cntVtx] = block->cp[cntCp] + length;
	block->cntNewVtxToSp[cntCp] = cntVtx;

	D3DXVec3TransformCoord(&vtx[cntVtx].vtx, &block->transPos[cntVtx], &block->invMtxPos);
	vtx[cntVtx].diffuse = COLOR_VTX;

	nor = vtx[cntVtx].vtx;
	D3DXVec3Normalize(&nor, &nor);
	vtx[cntVtx].nor = nor;



	tmpEp = block->transPos[ep] - block->cp[cntCp];
	D3DXVec3Normalize(&length, &tmpEp);
	length /= 2.0f;

	if (D3DXVec3LengthSq(&length) > D3DXVec3LengthSq(&tmpEp))
	{
		length = tmpEp / 2.0f;
	}
	else
	{
		length /= 2.0f;
	}


	cntVtx++;

	block->transPos[cntVtx] = block->cp[cntCp] + length;
	block->cntNewVtxToEp[cntCp] = cntVtx;


	D3DXVec3TransformCoord(&vtx[cntVtx].vtx, &block->transPos[cntVtx], &block->invMtxPos);
	vtx[cntVtx].diffuse = COLOR_VTX;

	nor = vtx[cntVtx].vtx;
	D3DXVec3Normalize(&nor, &nor);
	vtx[cntVtx].nor = nor;

	//vtx[cntVtx].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);


}

//=============================================================================
// ブロックを構成する線分と切断面との交点を求めてblock構造体内に保存
// section	:切断面の構造体
// block	:ブロックのポインター
// surface	:ブロックの構成面のポインター
//=============================================================================
void ClippingLine(PLANE section, BLOCK* block, SURFACE* surface)
{
	int sp, ep;
	int cntIdx = surface->numIdx;
	int checkHit;
	surface->numNewIdx = 0;


	for (int cntVtx = 0; cntVtx < surface->numIdx; cntVtx++)
	{
		sp = surface->idx[cntVtx];
		ep = surface->idx[(cntVtx + 1) % surface->numIdx];
		cntIdx = surface->numIdx + surface->numNewIdx;

		checkHit = CheckCalculatedHitCheck(block, sp, ep);

		if (block->dotVtx[sp] * block->dotVtx[ep] >= 0)
		{
			continue;
		}
		if (checkHit != FALSE_FLAG)
		{
			surface->idx[cntIdx] = block->numVtx + (checkHit * 2);
			surface->idx[cntIdx + 1] = block->numVtx + (checkHit * 2) + 1;

			surface->numNewIdx += NUM_NEW_IDX;
			continue;
		}
		if (HitCheckSurface(&block->cp[block->numCp], section, block->transPos[sp], block->transPos[ep]))
		{
			block->cntCrossLineSp[block->numCp] = sp;
			block->cntCrossLineEp[block->numCp] = ep;

			surface->idx[cntIdx] = block->numVtx + (block->numCp * 2);
			surface->idx[cntIdx + 1] = block->numVtx + (block->numCp * 2) + 1;

			surface->numNewIdx += NUM_NEW_IDX;
			block->numCp++;
		}
	}
}

//=============================================================================
// 渡された線分の交点がすでに求められているかをかくにん
// cntNumCp		:保存されている線分の指標変数の添え字を返す
// FALSE_FLAG	:未確認
// block		:ブロックのポインター
// sp			:線分の始点
// ep			:線分の終点
//=============================================================================
int CheckCalculatedHitCheck(BLOCK* block, int sp, int ep)
{
	for (int cntNumCp = 0; cntNumCp < block->numCp; cntNumCp++)
	{
		if ((sp == block->cntCrossLineSp[cntNumCp] || sp == block->cntCrossLineEp[cntNumCp]) &&
			(ep == block->cntCrossLineSp[cntNumCp] || ep == block->cntCrossLineEp[cntNumCp]))
		{
			return cntNumCp;
		}
	}

	return FALSE_FLAG;
}

//=============================================================================
// 切断面でブロックが両断されるかを確認
// section	:切断面の４頂点の座標
// block	:ブロックのポインター
//=============================================================================
bool CheckBlockDot(PLANE section, BLOCK* block)
{
	D3DXVECTOR3 tmpVec;

	block->numVtxOldBlock = 0;
	block->numVtxNewBlock = 0;

	for (int cntVtx = 0; cntVtx < block->numVtx; cntVtx++)
	{
		tmpVec = block->transPos[cntVtx] - section.vtx[0];
		D3DXVec3Normalize(&tmpVec, &tmpVec);

		block->dotVtx[cntVtx] = DotProduct(section.nor, tmpVec);

		if (block->dotVtx[cntVtx] > 0)
		{
			block->numVtxOldBlock++;
		}
		else if (block->dotVtx[cntVtx] < 0)
		{
			block->numVtxNewBlock++;
		}

#ifdef _DEBUG

		float tmp = D3DXVec3Dot(&section.nor, &tmpVec);
		if (tmp == 0.0f)
		{
			debug = true;
		}
#endif
	}

	if (block->numVtxOldBlock == 0 || block->numVtxNewBlock == 0)
	{
		return true;
	}

	return false;
}

//=============================================================================
// 切断面の線分とブロックの構成面の衝突判定
// block	:ブロックのポインター
// section	:切断面の４頂点の座標
//=============================================================================
bool HitCheckSectionLine(BLOCK* block, PLANE section)
{
	int ep;
	TRIANGLE polygon;

	for (int cntSurface = 0; cntSurface < block->numSurface; cntSurface++)
	{// ブロックを構成する全ての面を確認する
		for (int cntPolygon = 0; cntPolygon < block->surface[cntSurface].numPolygon; cntPolygon++)
		{// 面を構成するすべてのポリゴンを確認する

			polygon.vtx[0] = block->transPos[block->surface[cntSurface].idx[0]];
			polygon.vtx[1] = block->transPos[block->surface[cntSurface].idx[cntPolygon + 1]];
			polygon.vtx[2] = block->transPos[block->surface[cntSurface].idx[cntPolygon + 2]];

			for (int cntLine = 0; cntLine < NUM_SECTION_VTX; cntLine++)
			{// ポリゴン一枚と切断面の四つの輪郭線の衝突判定を行う

				switch (cntLine)
				{
				case 0:
					ep = 1;
					break;
				case 1:
					ep = 3;
					break;
				case 2:
					ep = 0;
					break;
				case 3:
					ep = 2;
					break;
				default:
					ep = 0;
					break;
				}
				//ep = GetEndPointOutLine(cntLine, NUM_SECTION_VTX);

				if (HitCheckTriangle(polygon, section.vtx[cntLine], section.vtx[ep]))
				{
					return true;
				}
			}
		}
	}

	return false;
}

//=============================================================================
// 図形の始点からつながる枠線の終点を求める
// sp : 線分の始点
//=============================================================================
int GetEndPointOutLine(int sp, int numVtx)
{
	if (sp == 0)
	{
		return 1;
	}
	else if (!(numVtx % 2) && sp == numVtx - 1)
	{
		return (sp - 1);
	}
	else if (numVtx % 2 && sp == numVtx - 2)
	{
		return (sp + 1);
	}
	else if (sp % 2)
	{
		return (sp + 2);
	}
	else if (!(sp % 2))
	{
		return (sp - 2);
	}
	return -1;
}

//=============================================================================
// 平面（三角）と線分との当たり判定
// p	:当たり判定を行う3頂点
// pos0	:始点（移動前）
// pos1	:終点（移動後）
//=============================================================================
bool HitCheckTriangle(TRIANGLE p, D3DXVECTOR3 pos0, D3DXVECTOR3 pos1)
{
	D3DXVECTOR3		nor;		// ポリゴンの法線

	{	// ポリゴンの外積をとって法線を求める。そして正規化しておく。
		D3DXVECTOR3		vec01;
		D3DXVECTOR3		vec02;

		vec01 = p.vtx[1] - p.vtx[0];
		vec02 = p.vtx[2] - p.vtx[0];

		D3DXVec3Cross(&nor, &vec01, &vec02);

		D3DXVec3Normalize(&nor, &nor);
	}

	{	// 平面と線分の内積とって衝突している可能性を調べる
		// 求めた法線とベクトル２つ（線分の両端とポリゴン上の任意の点）の内積とって衝突している可能性を調べる
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;
		float a;
		float b;

		vec1 = pos0 - p.vtx[0];
		vec2 = pos1 - p.vtx[0];

		a = D3DXVec3Dot(&vec1, &nor);
		b = D3DXVec3Dot(&vec2, &nor);


		if ((a * b) > 0)
		{
			// 当たっている可能性は無い
			return false;
		}
	}


	{	// ポリゴンと線分の交点を求める
		D3DXVECTOR3		vec1 = pos0 - p.vtx[0];
		D3DXVECTOR3		vec2 = pos1 - p.vtx[0];
		float			d1 = fabsf(D3DXVec3Dot(&nor, &vec1));					// 内分比を求める為の点とポリゴンとの距離
		float			d2 = fabsf(D3DXVec3Dot(&nor, &vec2));					// 内分比を求める為の点とポリゴンとの距離
		float			a = d1 / (d1 + d2);					// 内分比
		D3DXVECTOR3		vec3 = (1 - a) * vec1 + a * vec2;
		D3DXVECTOR3		p3 = p.vtx[0] + vec3;					// 交点




		{	// 求めた交点がポリゴンの中にあるか調べる

			// ポリゴンの各辺のベクトル
			D3DXVECTOR3		v1 = p.vtx[1] - p.vtx[0];
			D3DXVECTOR3		v2 = p.vtx[2] - p.vtx[1];
			D3DXVECTOR3		v3 = p.vtx[0] - p.vtx[2];

			// 各頂点と交点とのベクトル
			D3DXVECTOR3		v4 = p3 - p.vtx[0];
			D3DXVECTOR3		v5 = p3 - p.vtx[1];
			D3DXVECTOR3		v6 = p3 - p.vtx[2];

			// 各辺との外積で法線を求める
			D3DXVECTOR3		n1, n2, n3;

			D3DXVec3Cross(&n1, &v1, &v4);
			D3DXVec3Cross(&n2, &v2, &v5);
			D3DXVec3Cross(&n3, &v3, &v6);

			// それぞれのベクトルの向きを内積でチェック


			if (D3DXVec3Dot(&n1, &n3) < 0 || D3DXVec3Dot(&n2, &n3) < 0)
			{
				return false;//伊藤達治
			}
		}
	}

	return true;
}

//=============================================================================
// 平面（四角）と線分との当たり判定
// cp	:交点の座標を収納する変数のポインター
// p	:当たり判定を行う４頂点
// pos0	:始点（移動前）
// pos1	:終点（移動後）
//=============================================================================
bool HitCheckSurface(D3DXVECTOR3* cp, PLANE section, D3DXVECTOR3 pos0, D3DXVECTOR3 pos1)
{

	{	// ポリゴンと線分の交点を求める
		D3DXVECTOR3		vec1 = pos0 - section.vtx[0];
		D3DXVECTOR3		vec2 = pos1 - section.vtx[0];
		float			d1 = fabsf(D3DXVec3Dot(&section.nor, &vec1));					// 内分比を求める為の点とポリゴンとの距離
		float			d2 = fabsf(D3DXVec3Dot(&section.nor, &vec2));					// 内分比を求める為の点とポリゴンとの距離
		float			a = d1 / (d1 + d2);					// 内分比
		D3DXVECTOR3		vec3 = (1 - a) * vec1 + a * vec2;
		D3DXVECTOR3		p3 = section.vtx[0] + vec3;					// 交点




		{	// 求めた交点がポリゴンの中にあるか調べる

			// ポリゴンの各辺のベクトル
			D3DXVECTOR3		v1 = section.vtx[1] - section.vtx[0];
			D3DXVECTOR3		v2 = section.vtx[3] - section.vtx[1];
			D3DXVECTOR3		v3 = section.vtx[0] - section.vtx[3];

			// 各頂点と交点とのベクトル
			D3DXVECTOR3		v4 = p3 - section.vtx[0];
			D3DXVECTOR3		v5 = p3 - section.vtx[1];
			D3DXVECTOR3		v6 = p3 - section.vtx[3];

			// 各辺との外積で法線を求める
			D3DXVECTOR3		n1, n2, n3;

			D3DXVec3Cross(&n1, &v1, &v4);
			D3DXVec3Cross(&n2, &v2, &v5);
			D3DXVec3Cross(&n3, &v3, &v6);

			// それぞれのベクトルの向きを内積でチェック


			if (D3DXVec3Dot(&n1, &n3) < 0 || D3DXVec3Dot(&n2, &n3) < 0)
			{
				// ポリゴンの各辺のベクトル
				v1 = section.vtx[3] - section.vtx[0];
				v2 = section.vtx[2] - section.vtx[3];
				v3 = section.vtx[0] - section.vtx[2];

				// 各頂点と交点とのベクトル
				v4 = p3 - section.vtx[0];
				v5 = p3 - section.vtx[3];
				v6 = p3 - section.vtx[2];

				D3DXVec3Cross(&n1, &v1, &v4);
				D3DXVec3Cross(&n2, &v2, &v5);
				D3DXVec3Cross(&n3, &v3, &v6);

				if (D3DXVec3Dot(&n1, &n3) < 0 || D3DXVec3Dot(&n2, &n3) < 0)
				{
					return false;//伊藤達治
				}
			}

			*cp = p3;
		}
	}
	return true;
}

//=============================================================================
// 桁落ち回避用のdouble型の内積計算関数
//=============================================================================
double DotProduct(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2)
{
	double dot;

	dot = (double)vec1.x * (double)vec2.x + (double)vec1.y * (double)vec2.y + (double)vec1.z * (double)vec2.z;

	return dot;
}