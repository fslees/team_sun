//=============================================================================
//
// メッシュ地面の処理 [meshfield.cpp]
// Author : 
//
//=============================================================================
#include "meshfield.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define	LOOP_MAX		(500)		// フラクタルの演算数
#define	TEXTURE_FIELD	"data/TEXTURE/field000.jpg"		// 読み込むテクスチャファイル名

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 D3DTextureField;			// テクスチャ読み込み場所
LPDIRECT3DVERTEXBUFFER9 D3DVtxBuffField;	// 頂点バッファインターフェースへのポインタ
LPDIRECT3DINDEXBUFFER9 D3DIdxBuffField;		// インデックスバッファインターフェースへのポインタ

D3DXMATRIX mtxWorldField;					// ワールドマトリックス
D3DXVECTOR3 posField;						// ポリゴン表示位置の中心座標
D3DXVECTOR3 rotField;						// ポリゴンの回転角

int numBlockXField, numBlockZField;			// ブロック数
int numVertexField;							// 総頂点数	
int numVertexIndexField;					// 総インデックス数
int numPolygonField;						// 総ポリゴン数
float blockSizeXField, blockSizeZField;	// ブロックサイズ
float xRand, zRand, rRand;
float valField;
float xMax, zMax, rMax;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshField(D3DXVECTOR3 pos, D3DXVECTOR3 rot,
							int numBlockX, int numBlockZ, float blockSizeX, float blockSizeZ)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	xMax = (blockSizeX * numBlockX) / 2;
	zMax = (blockSizeX * numBlockX) / 2;
	rMax = 100;

	valField = (blockSizeX * numBlockX);

	// ポリゴン表示位置の中心座標を設定
	posField = pos;

	rotField = rot;

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,					// デバイスへのポインタ
								TEXTURE_FIELD,		// ファイルの名前
								&D3DTextureField);	// 読み込むメモリー

	// ブロック数の設定
	numBlockXField = numBlockX;
	numBlockZField = numBlockZ;

	// 頂点数の設定
	numVertexField = (numBlockX + 1) * (numBlockZ + 1);

	// インデックス数の設定
	numVertexIndexField = (numBlockX + 1) * 2 * numBlockZ + (numBlockZ - 1) * 2;

	// ポリゴン数の設定
	numPolygonField = numBlockX * numBlockZ * 2 + (numBlockZ - 1) * 4;

	// ブロックサイズの設定
	blockSizeXField = blockSizeX;
	blockSizeZField = blockSizeZ;

	// オブジェクトの頂点バッファを生成
    if(FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * numVertexField,	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,					// 頂点バッファの使用法　
												FVF_VERTEX_3D,						// 使用する頂点フォーマット
												D3DPOOL_MANAGED,					// リソースのバッファを保持するメモリクラスを指定
												&D3DVtxBuffField,				// 頂点バッファインターフェースへのポインタ
												NULL)))								// NULLに設定
	{
        return E_FAIL;
	}

	// オブジェクトのインデックスバッファを生成
    if(FAILED(pDevice->CreateIndexBuffer(sizeof(WORD) * numVertexIndexField,		// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,					// 頂点バッファの使用法　
												D3DFMT_INDEX16,						// 使用するインデックスフォーマット
												D3DPOOL_MANAGED,					// リソースのバッファを保持するメモリクラスを指定
												&D3DIdxBuffField,				// インデックスバッファインターフェースへのポインタ
												NULL)))								// NULLに設定
	{
        return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;
#if 0
		const float texSizeX = 1.0f / nNumBlockX;
		const float texSizeZ = 1.0f / nNumBlockZ;
#else
		const float texSizeX = 1.0f;
		const float texSizeZ = 1.0f;
#endif

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		D3DVtxBuffField->Lock( 0, 0, (void**)&pVtx, 0 );

		for(int cntVtxZ = 0; cntVtxZ < (numBlockZField + 1); cntVtxZ++)
		{
			for(int cntVtxX = 0; cntVtxX < (numBlockXField + 1); cntVtxX++)
			{
				// 頂点座標の設定
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].vtx.x = -(numBlockXField / 2.0f) * blockSizeXField + cntVtxX * blockSizeXField;
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].vtx.y = 0.0f;		//rand()%100;
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].vtx.z = (numBlockZField / 2.0f) * blockSizeZField - cntVtxZ * blockSizeZField;

				// 法線の設定
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].nor = D3DXVECTOR3(0.0f, 1.0, 0.0f);

				// 反射光の設定
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

				// テクスチャ座標の設定
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].tex.x = texSizeX * cntVtxX;
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].tex.y = texSizeZ * cntVtxZ;
			}
		}

		// 頂点データをアンロックする
		D3DVtxBuffField->Unlock();
	}

	{//インデックスバッファの中身を埋める
		WORD *pIdx;

		// インデックスデータの範囲をロックし、頂点バッファへのポインタを取得
		D3DIdxBuffField->Lock( 0, 0, (void**)&pIdx, 0 );

		int nCntIdx = 0;
		for(int cntVtxZ = 0; cntVtxZ < numBlockZField; cntVtxZ++)
		{
			if(cntVtxZ > 0)
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = (cntVtxZ + 1) * (numBlockXField + 1);
				nCntIdx++;
			}

			for(int cntVtxX = 0; cntVtxX < (numBlockXField + 1); cntVtxX++)
			{
				pIdx[nCntIdx] = (cntVtxZ + 1) * (numBlockXField + 1) + cntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = cntVtxZ * (numBlockXField + 1) + cntVtxX;
				nCntIdx++;
			}

			if(cntVtxZ < (numBlockZField - 1))
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = cntVtxZ * (numBlockXField + 1) + numBlockXField;
				nCntIdx++;
			}
		}

		// インデックスデータをアンロックする
		D3DIdxBuffField->Unlock();
	}

	//for (int i = 0; i < LOOP_MAX; i++)
	//{


	//	//if (GetKeyboardPress(DIK_B))
	//	//{

	//	xRand = (float)(rand() % (int)(valField)) - xMax;
	//	zRand = (float)(rand() % (int)(valField)) - zMax;
	//	rRand = (float)(rand() % 200 * (D3DX_PI)) / 100.0f;

	//	D3DXVECTOR3 vecRand, vecMesh, cross;
	//	D3DXVECTOR3 tmp;
	//	VERTEX_3D	*pVtx;
	//	D3DVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);


	//	vecRand.x = cosf(rRand);
	//	vecRand.z = sinf(rRand);
	//	vecRand.y = 0.0f;

	//	for (int z = 0; z < (numBlockZField + 1); z++)
	//	{
	//		for (int x = 0; x < (numBlockXField + 1); x++)
	//		{
	//			tmp.x = pVtx[z*(numBlockXField + 1) + x].vtx.x;
	//			tmp.z = pVtx[z*(numBlockXField + 1) + x].vtx.z;

	//			vecMesh.x = tmp.x - xRand;
	//			vecMesh.z = tmp.z - zRand;
	//			vecMesh.y = 0.0f;

	//			D3DXVec3Cross(&cross, &vecRand, &vecMesh);

	//			if (cross.y > 0.0f)
	//			{
	//				pVtx[z*(numBlockXField + 1) + x].vtx.y += 5.0f;
	//			}
	//			else
	//			{
	//				pVtx[z*(numBlockXField + 1) + x].vtx.y -= 5.0f;

	//			}

	//		}

	//	}

	//	D3DIdxBuffField->Unlock();

	//	//}
	//}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
	if(D3DVtxBuffField)
	{// 頂点バッファの開放
		D3DVtxBuffField->Release();
		D3DVtxBuffField = NULL;
	}

	if(D3DIdxBuffField)
	{// インデックスバッファの開放
		D3DIdxBuffField->Release();
		D3DIdxBuffField = NULL;
	}

	if(D3DTextureField)
	{// テクスチャの開放
		D3DTextureField->Release();
		D3DTextureField = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{

	
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXMATRIX mtxRot, mtxTranslate;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorldField);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rotField.y, rotField.x, rotField.z);
	D3DXMatrixMultiply(&mtxWorldField, &mtxWorldField, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, posField.x, posField.y, posField.z);
	D3DXMatrixMultiply(&mtxWorldField, &mtxWorldField, &mtxTranslate);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorldField);

	// 頂点バッファをレンダリングパイプラインに設定
	pDevice->SetStreamSource(0, D3DVtxBuffField, 0, sizeof(VERTEX_3D));

	// インデックスバッファをレンダリングパイプラインに設定
	pDevice->SetIndices(D3DIdxBuffField);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, D3DTextureField);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertexField, 0, numPolygonField);
}

