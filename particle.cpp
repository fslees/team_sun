//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : 
//
//=============================================================================
#include "particle.h"
#include "input.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TEXTURE_PARTICLE	"data/TEXTURE/effect000.jpg"	// 読み込むテクスチャファイル名
#define	PARTICLE_SIZE_X		(50.0f)							// ビルボードの幅
#define	PARTICLE_SIZE_Y		(50.0f)							// ビルボードの高さ
#define	VALUE_MOVE_PARTICLE	(2.0f)							// 移動速度

#define	MAX_PARTICLE			(512)						// ビルボード最大数

#define	DISP_SHADOW				// 影の表示
//#undef DISP_SHADOW

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PARTICLE
{
	D3DXVECTOR3		pos;		// 位置
	D3DXVECTOR3		rot;		// 回転
	D3DXVECTOR3		scale;		// スケール
	D3DXVECTOR3		move;		// 移動量
	D3DXCOLOR		col;		// 色
	float			sizeX;		// 幅
	float			sizeY;		// 高さ
	int				life;		// 寿命
	bool			use;		// 使用しているかどうか

};

struct EFFECT
{

	D3DXVECTOR3 pos;

};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice);
void SetVertexParticle(int idxParticle, float sizeX, float sizeY);
void SetColorParticle(int idxParticle, D3DXCOLOR col);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9		D3DTextureParticle = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 D3DVtxBuffParticle = NULL;	// 頂点バッファインターフェースへのポインタ

D3DXMATRIX				mtxWorldParticle;				// ワールドマトリックス

PARTICLE				particle[MAX_PARTICLE];		// パーティクルワーク
D3DXVECTOR3				posBase[4];					// ビルボード発生位置
int						alpha;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 頂点情報の作成
	MakeVertexParticle(pDevice);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,						// デバイスへのポインタ
								TEXTURE_PARTICLE,			// ファイルの名前
								&D3DTextureParticle);	// 読み込むメモリー

	posBase[0] = D3DXVECTOR3(-100, 0, 100);
	posBase[1] = D3DXVECTOR3(100, 0, 100);
	posBase[2] = D3DXVECTOR3(-100, 0, -100);
	posBase[3] = D3DXVECTOR3(100, 0, -100);


	for(int cnt = 0; cnt < MAX_PARTICLE; cnt++)
	{
		particle[cnt].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		particle[cnt].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		particle[cnt].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		particle[cnt].move = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		particle[cnt].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		particle[cnt].sizeX = PARTICLE_SIZE_X;
		particle[cnt].sizeY = PARTICLE_SIZE_Y;
		particle[cnt].life = 0;
		particle[cnt].use = false;
	}


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle(void)
{
	if(D3DTextureParticle != NULL)
	{// テクスチャの開放
		D3DTextureParticle->Release();
		D3DTextureParticle = NULL;
	}

	if(D3DVtxBuffParticle != NULL)
	{// 頂点バッファの開放
		D3DVtxBuffParticle->Release();
		D3DVtxBuffParticle = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticle(void)
{
	D3DXVECTOR3 rotCamera;


	if (GetKeyboardTrigger(DIK_SPACE))
	{

		SetEffect(&posBase[0], 4);

	}

	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		if (particle[i].use)
		{
			particle[i].pos.x += particle[i].move.x;
			particle[i].pos.y += particle[i].move.y;
			particle[i].pos.z += particle[i].move.z;

			particle[i].life--;

			if (particle[i].life <= 0)
			{
				particle[i].use = false;
			}

		}

	}



}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxView,mtxScale,mtxTranslate;
	CAMERA *camera = GetCamera();

	// ライティングを無効に
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// パーティクルっぽく設定をする(加算減算合成・αテスト・Zソート）
	
		// 加算合成
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// 結果 = 転送先(DEST) - 転送元(SRC)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// Z比較なし
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

	
		// αテスト設定
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, alpha);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	
	

	for(int cnt = 0; cnt < MAX_PARTICLE; cnt++)
	{
		if(particle[cnt].use)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&mtxWorldParticle);

			// ビューマトリックスを取得
			mtxView = camera->mtxView;

			mtxWorldParticle._11 = mtxView._11;
			mtxWorldParticle._12 = mtxView._21;
			mtxWorldParticle._13 = mtxView._31;
			mtxWorldParticle._21 = mtxView._12;
			mtxWorldParticle._22 = mtxView._22;
			mtxWorldParticle._23 = mtxView._32;
			mtxWorldParticle._31 = mtxView._13;
			mtxWorldParticle._32 = mtxView._23;
			mtxWorldParticle._33 = mtxView._33;

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, particle[cnt].scale.x, particle[cnt].scale.y, particle[cnt].scale.z);
			D3DXMatrixMultiply(&mtxWorldParticle, &mtxWorldParticle, &mtxScale);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, particle[cnt].pos.x, particle[cnt].pos.y, particle[cnt].pos.z);
			D3DXMatrixMultiply(&mtxWorldParticle, &mtxWorldParticle, &mtxTranslate);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &mtxWorldParticle);

			// 頂点バッファをデバイスのデータストリームにバインド
			pDevice->SetStreamSource(0, D3DVtxBuffParticle, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, D3DTextureParticle);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (cnt * 4), NUM_POLYGON);
		}
	}

	// ライティングを有効に
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);



	// 通常ブレンドに戻す
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
    if( FAILED( pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX * MAX_PARTICLE,	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,							// 頂点バッファの使用法　
												FVF_VERTEX_3D,								// 使用する頂点フォーマット
												D3DPOOL_MANAGED,							// リソースのバッファを保持するメモリクラスを指定
												&D3DVtxBuffParticle,						// 頂点バッファインターフェースへのポインタ
												NULL)))										// NULLに設定
	{
        return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		D3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		for(int cnt = 0; cnt < MAX_PARTICLE; cnt++, pVtx += 4)
		{
			// 頂点座標の設定
			pVtx[0].vtx = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[1].vtx = D3DXVECTOR3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[2].vtx = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[3].vtx = D3DXVECTOR3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);

			// 法線の設定
			pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

			// 反射光の設定
			pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
		}

		// 頂点データをアンロックする
		D3DVtxBuffParticle->Unlock();
	}

	return S_OK;
}

//=============================================================================
// 頂点座標の設定
//=============================================================================
void SetVertexParticle(int idxParticle, float sizeX, float sizeY)
{
	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		D3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (idxParticle * 4);

		// 頂点座標の設定
		pVtx[0].vtx = D3DXVECTOR3(-sizeX / 2, -sizeY / 2, 0.0f);
		pVtx[1].vtx = D3DXVECTOR3(-sizeX / 2, sizeY / 2, 0.0f);
		pVtx[2].vtx = D3DXVECTOR3(sizeX / 2, -sizeY / 2, 0.0f);
		pVtx[3].vtx = D3DXVECTOR3(sizeX / 2, sizeY / 2, 0.0f);

		// 頂点データをアンロックする
		D3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// 頂点カラーの設定
//=============================================================================
void SetColorParticle(int idxParticle, D3DXCOLOR col)
{
	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		D3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (idxParticle * 4);

		// 頂点座標の設定
		pVtx[0].diffuse =
		pVtx[1].diffuse =
		pVtx[2].diffuse =
		pVtx[3].diffuse = col;

		// 頂点データをアンロックする
		D3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
int SetParticle(D3DXVECTOR3 pos, D3DXVECTOR3 move, int life)
{
	int idxParticle = -1;

	for(int cnt = 0; cnt < MAX_PARTICLE; cnt++)
	{
		if(!particle[cnt].use)
		{
			particle[cnt].pos = pos;
			particle[cnt].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			particle[cnt].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			particle[cnt].move = move;
			particle[cnt].col = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
			particle[cnt].sizeX = PARTICLE_SIZE_X;
			particle[cnt].sizeY = PARTICLE_SIZE_Y;
			particle[cnt].life = life;
			particle[cnt].use = true;

			// 頂点座標の設定
			SetVertexParticle(cnt, PARTICLE_SIZE_X, PARTICLE_SIZE_Y);

			idxParticle = cnt;


			break;
		}
	}

	return idxParticle;
}



void SetEffect(D3DXVECTOR3* pos, int occurrencePos)	// SetEffect(配列[]番のpos, 発生位置の数);	
{

	// 中心を出す
	D3DXVECTOR3	center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	for (int i=0; i < occurrencePos; i++)
	{
		center += *(pos + i);
	}
	center /= (float)occurrencePos;

	for (int i = 0; i < occurrencePos; i++)
	{
			D3DXVECTOR3 move;

			move = *(pos + i) - center;
			move /= 100.0f;

			// ノーマライズ
			//D3DXVec3Normalize(&norVec, &vec);
			

			SetParticle(center, move, 60);
	}

}