//=============================================================================
//
// ���[���̏��� [lane.cpp]
// Author : 
//
//=============================================================================
#include "lane.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	LANE_MODEL		"data/MODEL/car000.x"	// �ǂݍ��ރ��f����
#define LANE_MAX		(4)						// ���[���̍ő吔
#define LANE_SPACE		(50.0f)					// ���[���Ԋu�̒����l
// �ʒu�̏����l
#define INIT_POS_X		(75.0f)					// ���[���̏����ʒuX���W
#define INIT_POS_Y		(50.0f)					// Y���W
#define INIT_POS_Z		(100.0f)				// Z���W
// �X�P�[���̏����l
#define INIT_SCL_X		(1.0f)					
#define INIT_SCL_Y		(1.0f)
#define INIT_SCL_Z		(100000.0f)



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DTEXTURE9	D3DTextureLane;		// �e�N�X�`���ւ̃|�C���^
LPD3DXMESH			D3DXMeshLane;		// ���b�V�����ւ̃|�C���^
LPD3DXBUFFER		D3DXBuffMatLane;	// �}�e���A�����ւ̃|�C���^
DWORD				NumMatLane;			// �}�e���A�����̐�

LANE				Lane[LANE_MAX];
//=============================================================================
// ����������
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
		// �ʒu�E�X�P�[���̏����ݒ�
		Lane[cnt].pos = D3DXVECTOR3(-INIT_POS_X + LANE_SPACE * cnt, INIT_POS_Y, -INIT_POS_Z);
		Lane[cnt].scl = D3DXVECTOR3(INIT_SCL_X, INIT_SCL_Y, INIT_SCL_Z);
	}

	// X�t�@�C���̓ǂݍ���
	if (FAILED(D3DXLoadMeshFromX(LANE_MODEL,				// �ǂݍ��ރ��f���t�@�C����(X�t�@�C��)
								D3DXMESH_SYSTEMMEM,		// ���b�V���̍쐬�I�v�V�������w��
								pDevice,				// IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
								NULL,					// �אڐ��f�[�^���܂ރo�b�t�@�ւ̃|�C���^
								&D3DXBuffMatLane,	// �}�e���A���f�[�^���܂ރo�b�t�@�ւ̃|�C���^
								NULL,					// �G�t�F�N�g�C���X�^���X�̔z����܂ރo�b�t�@�ւ̃|�C���^
								&NumMatLane,		// D3DXMATERIAL�\���̂̐�
								&D3DXMeshLane)))	// ID3DXMesh�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
	{
		return E_FAIL;
	}

#if 0
	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		TEXTURE_FILENAME,		// �t�@�C���̖��O
		&D3DTextureLane);	// �ǂݍ��ރ������[
#endif

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitLane(void)
{
	if (D3DTextureLane != NULL)
	{// �e�N�X�`���̊J��
		D3DTextureLane->Release();
		D3DTextureLane = NULL;
	}

	if (D3DXMeshLane != NULL)
	{// ���b�V���̊J��
		D3DXMeshLane->Release();
		D3DXMeshLane = NULL;
	}

	if (D3DXBuffMatLane != NULL)
	{// �}�e���A���̊J��
		D3DXBuffMatLane->Release();
		D3DXBuffMatLane = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateLane(void)
{

	


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawLane(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	for (int cnt = 0; cnt < LANE_MAX; cnt++)
	{
		// ���[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&Lane[cnt].mtxWorld);

		// �X�P�[���𔽉f
		D3DXMatrixScaling(&mtxScl, Lane[cnt].scl.x, Lane[cnt].scl.y, Lane[cnt].scl.z);
		D3DXMatrixMultiply(&Lane[cnt].mtxWorld, &Lane[cnt].mtxWorld, &mtxScl);

		// �ړ��𔽉f
		D3DXMatrixTranslation(&mtxTranslate, Lane[cnt].pos.x, Lane[cnt].pos.y, Lane[cnt].pos.z);
		D3DXMatrixMultiply(&Lane[cnt].mtxWorld, &Lane[cnt].mtxWorld, &mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		pDevice->SetTransform(D3DTS_WORLD, &Lane[cnt].mtxWorld);

		// ���݂̃}�e���A�����擾
		pDevice->GetMaterial(&matDef);

		// �}�e���A�����ɑ΂���|�C���^���擾
		pD3DXMat = (D3DXMATERIAL*)D3DXBuffMatLane->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)NumMatLane; nCntMat++)
		{
			// �}�e���A���̐ݒ�
			pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, D3DTextureLane);

			// �`��
			D3DXMeshLane->DrawSubset(nCntMat);
		}

		// �}�e���A�����f�t�H���g�ɖ߂�
		pDevice->SetMaterial(&matDef);
	}

}

