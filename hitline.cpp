//=============================================================================
//
// ���胉�C������ [hitline.cpp]
// Author : GP12A332_26_�c�����z
//
//=============================================================================
#include "main.h"
#include "hitline.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	TEXTURE_FIELD	"data/TEXTURE/field001.jpg"		// �ǂݍ��ރe�N�X�`���t�@�C����
#define INIT_POS_X			(0.0f)						// X���W�̏����ʒu
#define INIT_POS_Y			(00.0f)						// Y���W
#define INIT_POS_Z			(100.0f)						// Z���W

#define	INIT_SIZE_X			(1.0f)						// ���胉�C���̃T�C�Y X����
#define	INIT_SIZE_Y			(1.0f)						// Y����
#define	INIT_SIZE_Z			(1.0f)						// Z����

#define VTX_X				(200.0f)					// ���_X���W
#define VTX_Z				(20.0f)					// ���_Z���W

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DTEXTURE9		D3DTextureLine = NULL;	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 D3DVtxBuffLine = NULL;	// ���_�o�b�t�@�ւ̃|�C���^
D3DXVECTOR3				PosHitLine;			// �n�ʂ̈ʒu
D3DXVECTOR3				RotHitLine;			// �n�ʂ̌���(��])
D3DXVECTOR3				SclHitLine;			// �n�ʂ̑傫��(�X�P�[��)
D3DXMATRIX				MtxWorld;				// ���[���h�}�g���b�N�X

//=============================================================================
// ����������
//=============================================================================
HRESULT InitHitLine(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	
	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	PosHitLine = D3DXVECTOR3(0.0f, 0.0f, INIT_POS_Z);
	RotHitLine = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	SclHitLine = D3DXVECTOR3(INIT_SIZE_X, INIT_SIZE_Y, INIT_SIZE_Z);

	// ���_���̍쐬
	MakeVertexHitLine(pDevice);

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		TEXTURE_FIELD,			// �t�@�C���̖��O
		&D3DTextureLine);	// �ǂݍ��ރ������[
	
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitHitLine(void)
{
	if (D3DTextureLine != NULL)
	{// �e�N�X�`���̊J��
		D3DTextureLine->Release();
		D3DTextureLine = NULL;
	}

	if (D3DVtxBuffLine != NULL)
	{// ���_�o�b�t�@�̊J��
		D3DVtxBuffLine->Release();
		D3DVtxBuffLine = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateHitLine(void)
{


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawHitLine(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&MtxWorld);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, SclHitLine.x, SclHitLine.y, SclHitLine.z);
	D3DXMatrixMultiply(&MtxWorld, &MtxWorld, &mtxScl);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, RotHitLine.y, RotHitLine.x, RotHitLine.z);
	D3DXMatrixMultiply(&MtxWorld, &MtxWorld, &mtxRot);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, PosHitLine.x, PosHitLine.y, PosHitLine.z);
	D3DXMatrixMultiply(&MtxWorld, &MtxWorld, &mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &MtxWorld);


	// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
	pDevice->SetStreamSource(0, D3DVtxBuffLine, 0, sizeof(VERTEX_3D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, D3DTextureLine);

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
}

//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeVertexHitLine(LPDIRECT3DDEVICE9 pDevice)
{
	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		D3DUSAGE_WRITEONLY,			// ���_�o�b�t�@�̎g�p�@�@
		FVF_VERTEX_3D,				// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,			// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&D3DVtxBuffLine,			// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))						// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		D3DVtxBuffLine->Lock(0, 0, (void**)&pVtx, 0);

		// ���_���W�̐ݒ�
		pVtx[0].vtx = D3DXVECTOR3(VTX_X, 0.0f, 0);
		pVtx[1].vtx = D3DXVECTOR3(-VTX_X, 0.0f, 0);
		pVtx[2].vtx = D3DXVECTOR3(VTX_X, 0.0f, VTX_Z);
		pVtx[3].vtx = D3DXVECTOR3(-VTX_X, 0.0f, VTX_Z);

		// �@���x�N�g���̐ݒ�
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// ���ˌ��̐ݒ�
		pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// ���_�f�[�^���A�����b�N����
		D3DVtxBuffLine->Unlock();
	}

	return S_OK;
}

//=============================================================================
// ���胉�C���̈ʒu�擾�֐�
// ���@���F�ȁ@��
// �߂�l�FD3DXVECTRO3�^�̔��胉�C���ʒu
//=============================================================================
D3DXVECTOR3 GetHitLinePos()
{
	return PosHitLine;
}

//=============================================================================
// ���胉�C���̃X�P�[���擾�֐�
// ���@���F�ȁ@��
// �߂�l�FD3DXVECTRO3�^�̔��胉�C���X�P�[��
//=============================================================================
D3DXVECTOR3 GetHitLineScl()
{
	return SclHitLine;
}
