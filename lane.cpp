//=============================================================================
//
// レーンの処理 [lane.cpp]
// Author : 
//
//=============================================================================
#include "lane.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	LANE_MODEL		"data/MODEL/car000.x"	// 読み込むモデル名
#define LANE_MAX		(4)						// レーンの最大数
#define LANE_SPACE		(50.0f)					// レーン間隔の調整値
// 位置の初期値
#define INIT_POS_X		(75.0f)					// レーンの初期位置X座標
#define INIT_POS_Y		(50.0f)					// Y座標
#define INIT_POS_Z		(100.0f)				// Z座標
// スケールの初期値
#define INIT_SCL_X		(1.0f)					
#define INIT_SCL_Y		(1.0f)
#define INIT_SCL_Z		(100000.0f)



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9	D3DTextureLane;		// テクスチャへのポインタ
LPD3DXMESH			D3DXMeshLane;		// メッシュ情報へのポインタ
LPD3DXBUFFER		D3DXBuffMatLane;	// マテリアル情報へのポインタ
DWORD				NumMatLane;			// マテリアル情報の数

LANE				Lane[LANE_MAX];
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitLane(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DTextureLane = NULL;
	D3DXMeshLane = NULL;
	D3DXBuffMatLane = NULL;
	NumMatLane = 0;
	
	for (int cnt = 0; cnt < LANE_MAX; cnt++)
	{
		// 位置・スケールの初期設定
		Lane[cnt].pos = D3DXVECTOR3(-INIT_POS_X + LANE_SPACE * cnt, INIT_POS_Y, -INIT_POS_Z);
		Lane[cnt].scl = D3DXVECTOR3(INIT_SCL_X, INIT_SCL_Y, INIT_SCL_Z);
	}

	// Xファイルの読み込み
	if (FAILED(D3DXLoadMeshFromX(LANE_MODEL,				// 読み込むモデルファイル名(Xファイル)
								D3DXMESH_SYSTEMMEM,		// メッシュの作成オプションを指定
								pDevice,				// IDirect3DDevice9インターフェイスへのポインタ
								NULL,					// 隣接性データを含むバッファへのポインタ
								&D3DXBuffMatLane,	// マテリアルデータを含むバッファへのポインタ
								NULL,					// エフェクトインスタンスの配列を含むバッファへのポインタ
								&NumMatLane,		// D3DXMATERIAL構造体の数
								&D3DXMeshLane)))	// ID3DXMeshインターフェイスへのポインタのアドレス
	{
		return E_FAIL;
	}

#if 0
	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,					// デバイスへのポインタ
		TEXTURE_FILENAME,		// ファイルの名前
		&D3DTextureLane);	// 読み込むメモリー
#endif

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitLane(void)
{
	if (D3DTextureLane != NULL)
	{// テクスチャの開放
		D3DTextureLane->Release();
		D3DTextureLane = NULL;
	}

	if (D3DXMeshLane != NULL)
	{// メッシュの開放
		D3DXMeshLane->Release();
		D3DXMeshLane = NULL;
	}

	if (D3DXBuffMatLane != NULL)
	{// マテリアルの開放
		D3DXBuffMatLane->Release();
		D3DXBuffMatLane = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateLane(void)
{

	


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawLane(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	for (int cnt = 0; cnt < LANE_MAX; cnt++)
	{
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&Lane[cnt].mtxWorld);

		// スケールを反映
		D3DXMatrixScaling(&mtxScl, Lane[cnt].scl.x, Lane[cnt].scl.y, Lane[cnt].scl.z);
		D3DXMatrixMultiply(&Lane[cnt].mtxWorld, &Lane[cnt].mtxWorld, &mtxScl);

		// 移動を反映
		D3DXMatrixTranslation(&mtxTranslate, Lane[cnt].pos.x, Lane[cnt].pos.y, Lane[cnt].pos.z);
		D3DXMatrixMultiply(&Lane[cnt].mtxWorld, &Lane[cnt].mtxWorld, &mtxTranslate);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &Lane[cnt].mtxWorld);

		// 現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		// マテリアル情報に対するポインタを取得
		pD3DXMat = (D3DXMATERIAL*)D3DXBuffMatLane->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)NumMatLane; nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, D3DTextureLane);

			// 描画
			D3DXMeshLane->DrawSubset(nCntMat);
		}

		// マテリアルをデフォルトに戻す
		pDevice->SetMaterial(&matDef);
	}

}

