//=============================================================================
//
// �؏��� [colony.h]
// Author : 
//
//=============================================================================
#ifndef _STAR_H_
#define _STAR_H_



//*****************************************************************************
// �}�N����`
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
// �\���̒�`
//*****************************************************************************
typedef struct
{
	WORD					idx[MAX_IDX_SURFACE];

	LPDIRECT3DINDEXBUFFER9	idxBuff;			// �C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^

	//WORD				idx[MAX_IDX_SURFACE];	// ���_�C���f�b�N�X


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
	bool					use;				// �g�p���Ă��邩�ǂ���

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

	LPDIRECT3DVERTEXBUFFER9 vtxBuff = NULL;		// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^

	SURFACE					surface[MAX_SURFACE];

	D3DXMATRIX				mtxWorld;			// ���[���h�}�g���b�N�X
	D3DXMATRIX				mtxPos;				// ���[���h�}�g���b�N�X
	D3DXMATRIX				invMtxPos;			// ���[���h�}�g���b�N�X

	D3DXVECTOR3				pos;				// �ʒu
	D3DXVECTOR3				scl;				// �X�P�[��
	D3DXVECTOR3				rot;
	D3DXVECTOR3				move;				// �X�P�[��

} BLOCK;

typedef struct
{

	float					width;				// ��
	float					height;				// ����
	float					depth;
	bool					use;
	int						numBlock;
	D3DXVECTOR3				pos;				// �ʒu


	BLOCK					block[MAX_BLOCK];
} STAR;




//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitStar(void);
void	UninitStar(void);
void	UpdateStar(void);
void	DrawStar(void);
bool	ClippingStar(PLANE section);

// ���̃Z�b�g�֐�
// �߂�l�F�Z�b�g�������̔ԍ���Ԃ�
// pos	: �����Z�b�g����ʒu
// move : ���̈ړ���
int SetStar(D3DXVECTOR3 pos, D3DXVECTOR3 move);




#endif
