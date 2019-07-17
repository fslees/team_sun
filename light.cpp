#include "light.h"


// マクロ
#define	NUM_LIGHT		(3)												// ライトの数

// グローバル
D3DLIGHT9			Light[NUM_LIGHT];	// ライト情報

//=============================================================================
// ライトの初期化処理
//=============================================================================
HRESULT InitLight(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXVECTOR3 vecDir;

	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory(&Light[0], sizeof(D3DLIGHT9));

	// ライト0のタイプの設定
	Light[0].Type = D3DLIGHT_DIRECTIONAL;

	// ライト0の拡散光の設定
	Light[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライト0の環境光の設定
	Light[0].Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライト0の方向の設定
	vecDir = D3DXVECTOR3(-0.0f, -0.6f, -1.0f);
	D3DXVec3Normalize((D3DXVECTOR3*)&Light[0].Direction, &vecDir);

	// ライト0をレンダリングパイプラインに設定
	pDevice->SetLight(0, &Light[0]);

	// ライト0を使用使用状態に
	pDevice->LightEnable(0, TRUE);


	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory(&Light[1], sizeof(D3DLIGHT9));

	// ライト1のタイプの設定
	Light[1].Type = D3DLIGHT_DIRECTIONAL;

	// ライト1の拡散光の設定
	Light[1].Diffuse = D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.0f);

	// ライト1の環境光の設定
	Light[1].Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライト1の方向の設定
	vecDir = D3DXVECTOR3(-0.0f, -1.0f, 0.8f);
	D3DXVec3Normalize((D3DXVECTOR3*)&Light[1].Direction, &vecDir);

	// ライト1をレンダリングパイプラインに設定
	pDevice->SetLight(1, &Light[1]);

	// ライト1の設定
	pDevice->LightEnable(1, TRUE);

	// ライティングモードをON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);


	return S_OK;
}

void UninitLight(void)
{





}

void UpdateLight(void)
{



}

void DrawLight(void)
{


}