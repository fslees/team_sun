//=============================================================================
//
// 木処理 [colony.h]
// Author : 
//
//=============================================================================
#ifndef _STAR_H_
#define _STAR_H_



//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_STAR			(128)
#define	MAX_BLOCK			(16)	
#define MAX_SURFACE			(16)
#define MAX_IDX_SURFACE		(16)
#define MAX_VTX_SURFACE		(16)	
#define VAL_BLOCK_MOVE		(1)

#define FALSE_FLAG			(-1)

#define VAL_ALPHA_DIS		(60000.0f)

typedef enum 
{
	NEW_SURFACE,
	OLD_SURFACE,
	TWO_SURFACE,
	START_POS = 0,
	END_POS,
	TWO_POS
}USE_CREATE_SURFACE;


//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	WORD					idx[MAX_IDX_SURFACE];

	LPDIRECT3DINDEXBUFFER9	idxBuff;			// インデックスバッファインターフェースへのポインタ

	//WORD				idx[MAX_IDX_SURFACE];	// 頂点インデックス


	//D3DXVECTOR3				cp[2];
	int						numIdx;
	int						numNewIdx;
	int						numPolygon;
	bool					use;

} SURFACE;


typedef struct
{
	int						numSurface;
	int						numVtx;
	bool					use;				// 使用しているかどうか

	int						numVtxNewBlock;
	int						numVtxOldBlock;

	int						numCp;
	int						cntCrossLineSp[MAX_SURFACE];
	int						cntCrossLineEp[MAX_SURFACE];
	int						cntNewVtxToSp[MAX_SURFACE];
	int						cntNewVtxToEp[MAX_SURFACE];

	D3DXVECTOR3				cp[MAX_SURFACE];


	D3DXVECTOR3				transPos[MAX_SURFACE * MAX_VTX_SURFACE];
	double					dotVtx[MAX_SURFACE * MAX_VTX_SURFACE];

	LPDIRECT3DVERTEXBUFFER9 vtxBuff = NULL;		// 頂点バッファインターフェースへのポインタ

	SURFACE					surface[MAX_SURFACE];

	D3DXMATRIX				mtxWorld;			// ワールドマトリックス
	D3DXMATRIX				mtxPos;				// ワールドマトリックス
	D3DXMATRIX				invMtxPos;			// ワールドマトリックス

	D3DXVECTOR3				pos;				// 位置
	D3DXVECTOR3				scl;				// スケール
	D3DXVECTOR3				rot;
	D3DXVECTOR3				move;				// スケール

} BLOCK;

typedef struct
{

	float					width;				// 幅
	float					height;				// 高さ
	float					depth;
	bool					use;
	int						numBlock;
	D3DXVECTOR3				pos;				// 位置


	BLOCK					block[MAX_BLOCK];
} STAR;




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitStar(void);
void	UninitStar(void);
void	UpdateStar(void);
void	DrawStar(void);
bool	ClippingStar(PLANE section);

// 星のセット関数
// 戻り値：セットした星の番号を返す
// pos	: 星をセットする位置
// move : 星の移動量
int SetStar(D3DXVECTOR3 pos, D3DXVECTOR3 move);




#endif
