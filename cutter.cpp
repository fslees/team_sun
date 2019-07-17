//=============================================================================
//
// 地面処理 [cutter.cpp]
// Author : 
//
//=============================================================================
#include "cutter.h"
#include "input.h"
#include "star.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define	TEXTURE_CUTTER	"data/TEXTURE/field000.jpg"	// 読み込むテクスチャファイル名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexCutter(LPDIRECT3DDEVICE9 device);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9		D3DTextureCutter = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 D3DVtxBuffCutter = NULL;	// 頂点バッファへのポインタ

D3DXMATRIX				mtxWorldCutter;			// ワールドマトリックス
D3DXVECTOR3				posCutter;					// 現在の位置
D3DXVECTOR3				rotCutter;					// 現在の向き

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCutter(void)
{
	LPDIRECT3DDEVICE9 device = GetDevice(); 
	HRESULT hr;

	// 位置、向きの初期設定
	posCutter = D3DXVECTOR3(0.0f, 0.0f, -50.0f);
	rotCutter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// テクスチャの読み込み
	//D3DXCreateTextureFromFile(device,					// デバイスへのポインタ
	//							TEXTURE_CUTTER,			// ファイルの名前
	//							&D3DTextureCutter);	// 読み込むメモリー

	// 頂点情報の作成
	hr = MakeVertexCutter(device);

	return hr;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCutter(void)
{
	if(D3DTextureCutter != NULL)
	{// テクスチャの開放
		D3DTextureCutter->Release();
		D3DTextureCutter = NULL;
	}

	if(D3DVtxBuffCutter != NULL)
	{// 頂点バッファの開放
		D3DVtxBuffCutter->Release();
		D3DVtxBuffCutter = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateCutter(void)
{
	//if (GetKeyboardPress(DIK_UP))
	//{
	//	posCutter.y -= 1.0f;
	//}
	//if (GetKeyboardPress(DIK_DOWN))
	//{
	//	posCutter.y += 1.0f;
	//}
	if (GetKeyboardPress(DIK_LEFT))
	{
		rotCutter.z -= 0.02f;
	}
	if (GetKeyboardPress(DIK_RIGHT))
	{
		rotCutter.z += 0.02f;
	}



}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCutter(void)
{

	LPDIRECT3DDEVICE9 device = GetDevice(); 
	D3DXMATRIX mtxRot, mtxTranslate;

	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);				// 裏面をカリング



	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorldCutter);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rotCutter.y, rotCutter.x, rotCutter.z);
	D3DXMatrixMultiply(&mtxWorldCutter, &mtxWorldCutter, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, posCutter.x, posCutter.y, posCutter.z);
	D3DXMatrixMultiply(&mtxWorldCutter, &mtxWorldCutter, &mtxTranslate);

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, &mtxWorldCutter);

	// 頂点バッファをデバイスのデータストリームにバインド
    device->SetStreamSource(0, D3DVtxBuffCutter, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	device->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	device->SetTexture(0, NULL);

	// ポリゴンの描画
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面をカリング


}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeVertexCutter(LPDIRECT3DDEVICE9 device)
{
	// オブジェクトの頂点バッファを生成
    if(FAILED(device->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,			// 頂点バッファの使用法　
												FVF_VERTEX_3D,				// 使用する頂点フォーマット
												D3DPOOL_MANAGED,			// リソースのバッファを保持するメモリクラスを指定
												&D3DVtxBuffCutter,		// 頂点バッファインターフェースへのポインタ
												NULL)))						// NULLに設定
	{
        return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_3D *vtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		D3DVtxBuffCutter->Lock(0, 0, (void**)&vtx, 0);

		// 頂点座標の設定
		vtx[0].vtx = D3DXVECTOR3(150.0f, 0.0f, 150.0f);
		vtx[1].vtx = D3DXVECTOR3(150.0f, 0.0f, 250.0f);
		vtx[2].vtx = D3DXVECTOR3(80.0f, 0.0f, 150.0f);
		vtx[3].vtx = D3DXVECTOR3(80.0f, 0.0f, 250.0f);

		// 法線ベクトルの設定
		vtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 反射光の設定
		vtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		vtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		vtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		vtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// 頂点データをアンロックする
		D3DVtxBuffCutter->Unlock();
	}

	return S_OK;
}

LINE GetCutterLine(void)
{
	LINE wkLine;

	PLANE wkPlane;

	wkLine.vtx[0] = wkPlane.vtx[0] + (wkPlane.vtx[1] - wkPlane.vtx[0]) / 2.0f;
	wkLine.vtx[1] = wkPlane.vtx[2] + (wkPlane.vtx[3] - wkPlane.vtx[2]) / 2.0f;

	return wkLine;
}

D3DXVECTOR3 GetCutterPos(void)
{
	return posCutter;
}

PLANE GetCutterState(void)
{
	PLANE wk;
	LPDIRECT3DDEVICE9 device = GetDevice();
	VERTEX_3D *vtx;
	D3DXMATRIX mtxTranslate, mtxRot, mtxWk;
	D3DXVECTOR3 vec01, vec02, nor;


	D3DXMatrixIdentity(&mtxWk);

	D3DXMatrixRotationYawPitchRoll(&mtxRot, rotCutter.y, rotCutter.x, rotCutter.z);
	D3DXMatrixMultiply(&mtxWk, &mtxWk, &mtxRot);

	D3DXMatrixTranslation(&mtxTranslate, posCutter.x, posCutter.y, posCutter.z);
	D3DXMatrixMultiply(&mtxWk, &mtxWk, &mtxTranslate);

	D3DVtxBuffCutter->Lock(0, 0, (void**)&vtx, 0);
	for (int cntVtx = 0; cntVtx < 4; cntVtx++)
	{// ブロックのローカル座標をワールド座標に変換
		D3DXVec3TransformCoord(&wk.vtx[cntVtx], &vtx[cntVtx].vtx, &mtxWk);
	}
	D3DVtxBuffCutter->Unlock();


	vec01 = wk.vtx[1] - wk.vtx[0];
	vec02 = wk.vtx[2] - wk.vtx[0];

	D3DXVec3Cross(&nor, &vec01, &vec02);
	D3DXVec3Normalize(&wk.nor, &nor);


	return wk;
	
}
