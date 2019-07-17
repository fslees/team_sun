//=============================================================================
//
// スコア処理 [score.cpp]
// Author : AKIRA TANAKA
//
//=============================================================================
#include "score.h"
#include "input.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TEXTURE_SCORE		"data/TEXTURE/number000.png"	// 読み込むテクスチャファイル名
#define	TEXTURE_FRAME_SCORE	"data/TEXTURE/frame_score.png"	// 読み込むテクスチャファイル名
#define	SCORE_SIZE_X		(20.0f)							// スコアの数字の幅
#define	SCORE_SIZE_Y		(35.0f)							// スコアの数字の高さ
#define	SCORE_INTERVAL_X	(0.0f)							// スコアの数字の表示間隔

#define	NUM_PLACE			(8)								// スコアの桁数

#define	SCORE_POS_X			(SCREEN_WIDTH - (SCORE_INTERVAL_X + SCORE_SIZE_X) * NUM_PLACE - 20.0f)	// スコアの表示基準位置Ｘ座標
#define	SCORE_POS_Y			(25.0f)																	// スコアの表示基準位置Ｙ座標
#define SCORE_ADD			(1)								// スコアがどれだけずつ増えるか
#define INCREASE_SCORE		(100)							// スコアがどれだけ増えるか

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexScore(LPDIRECT3DDEVICE9 pDevice);
void SetTextureScore(int idx, int number);

//*****************************************************************************
// グローバル変数宣言
//*****************************************************************************
LPDIRECT3DTEXTURE9		D3DTextureScore[2] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 D3DVtxBuffScore = NULL;		// 頂点バッファインターフェースへのポインタ

D3DXVECTOR3				posScore;						// 位置
D3DXVECTOR3				rotScore;						// 向き

int						score;							// スコア

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	posScore = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	rotScore = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// スコアの初期化
	score = 0;

	// 頂点情報の作成
	MakeVertexScore(pDevice);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,					// デバイスへのポインタ
								TEXTURE_SCORE,			// ファイルの名前
								&D3DTextureScore[0]);	// 読み込むメモリー

	D3DXCreateTextureFromFile(pDevice,					// デバイスへのポインタ
								TEXTURE_FRAME_SCORE,		// ファイルの名前
								&D3DTextureScore[1]);	// 読み込むメモリー

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScore(void)
{
	if(D3DTextureScore[0] != NULL)
	{// テクスチャの開放
		D3DTextureScore[0]->Release();
		D3DTextureScore[0] = NULL;
	}
	if(D3DTextureScore[1] != NULL)
	{// テクスチャの開放
		D3DTextureScore[1]->Release();
		D3DTextureScore[1] = NULL;
	}

	if(D3DVtxBuffScore != NULL)
	{// 頂点バッファの開放
		D3DVtxBuffScore->Release();
		D3DVtxBuffScore = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateScore(void)
{
	for(int cnt = 0; cnt < NUM_PLACE; cnt++) // cnt スコアの桁数
	{
		int number;
		
		number = (score % (int)(powf(10.0f, (float)(NUM_PLACE - cnt)))) / (int)(powf(10.0f, (float)(NUM_PLACE - cnt - 1)));

		SetTextureScore(cnt, number);
	}


#if 1
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		for (int i = 0; i < INCREASE_SCORE; i++)
		{

			AddScore(SCORE_ADD);

		}

	}

	if (GetKeyboardTrigger(DIK_R))
	{
		score = 0;
	}


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, D3DVtxBuffScore, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, D3DTextureScore[0]);

	// ポリゴンの描画
	for (int cnt = 0; cnt < NUM_PLACE; cnt++)
	{
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (cnt * 4), NUM_POLYGON);
	}

	// テクスチャの設定
	pDevice->SetTexture(0, D3DTextureScore[1]);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (NUM_PLACE * 4), NUM_POLYGON);
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeVertexScore(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
    if( FAILED( pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * (NUM_VERTEX * NUM_PLACE + 4),	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,								// 頂点バッファの使用法　
												FVF_VERTEX_2D,									// 使用する頂点フォーマット
												D3DPOOL_MANAGED,								// リソースのバッファを保持するメモリクラスを指定
												&D3DVtxBuffScore,							// 頂点バッファインターフェースへのポインタ
												NULL)))											// NULLに設定
	{
        return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_2D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		D3DVtxBuffScore->Lock(0, 0, (void**)&pVtx, 0);

		for(int cnt = 0;cnt < NUM_PLACE;cnt++, pVtx += 4)
		{
			// 頂点座標の設定
			pVtx[0].vtx = D3DXVECTOR3(SCORE_POS_X +cnt * SCORE_SIZE_X + SCORE_INTERVAL_X, SCORE_POS_Y, 0.0f);
			pVtx[1].vtx = D3DXVECTOR3(SCORE_POS_X +cnt * (SCORE_INTERVAL_X + SCORE_SIZE_X) + SCORE_SIZE_X, SCORE_POS_Y, 0.0f);
			pVtx[2].vtx = D3DXVECTOR3(SCORE_POS_X +cnt * SCORE_SIZE_X + SCORE_INTERVAL_X, SCORE_POS_Y + SCORE_SIZE_Y, 0.0f);
			pVtx[3].vtx = D3DXVECTOR3(SCORE_POS_X +cnt * (SCORE_INTERVAL_X + SCORE_SIZE_X) + SCORE_SIZE_X, SCORE_POS_Y + SCORE_SIZE_Y, 0.0f);

			// rhwの設定
			pVtx[0].rhw =
			pVtx[1].rhw =
			pVtx[2].rhw =
			pVtx[3].rhw = 1.0f;

			// 反射光の設定
			pVtx[0].diffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
			pVtx[1].diffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
			pVtx[2].diffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
			pVtx[3].diffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(0.1f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(0.1f, 1.0f);
		}

		{
			// 頂点座標の設定
			pVtx[0].vtx = D3DXVECTOR3(SCORE_POS_X - 15, SCORE_POS_Y-25, 0.0f);
			pVtx[1].vtx = D3DXVECTOR3(SCORE_POS_X + (SCORE_INTERVAL_X + SCORE_SIZE_X) * NUM_PLACE + 15, SCORE_POS_Y-25, 0.0f);
			pVtx[2].vtx = D3DXVECTOR3(SCORE_POS_X - 15, SCORE_POS_Y + 55, 0.0f);
			pVtx[3].vtx = D3DXVECTOR3(SCORE_POS_X + (SCORE_INTERVAL_X + SCORE_SIZE_X) * NUM_PLACE + 15, SCORE_POS_Y + 55, 0.0f);

			// rhwの設定
			pVtx[0].rhw =
			pVtx[1].rhw =
			pVtx[2].rhw =
			pVtx[3].rhw = 1.0f;

			// 反射光の設定
			pVtx[0].diffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
			pVtx[1].diffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
			pVtx[2].diffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
			pVtx[3].diffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
		}

		// 頂点データをアンロックする
		D3DVtxBuffScore->Unlock();
	}

	return S_OK;
}

//=============================================================================
// テクスチャ座標の設定
//=============================================================================
void SetTextureScore(int idx, int number)
{
	VERTEX_2D *pVtx;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	D3DVtxBuffScore->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += (idx * 4);

	// 頂点座標の設定
	pVtx[0].tex = D3DXVECTOR2(number * 0.1f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(number * 0.1f + 0.1f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(number * 0.1f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(number * 0.1f + 0.1f, 1.0f);

	// 頂点データをアンロックする
	D3DVtxBuffScore->Unlock();
}

//=============================================================================
// スコアの変更
//=============================================================================
void AddScore(int value)
{
	score += value;

	if(score < 0)
	{
		score = 0;
	}
	else if(score >= (int)(powf(10.0f, (float)(NUM_PLACE + 1))))
	{
		score = (int)(powf(10.0f, (float)(NUM_PLACE + 1))) - 1;
	}
}

