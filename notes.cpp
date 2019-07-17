//=============================================================================
//
// �m�[�c���� [notes.cpp]
// Author : GP12A332_25_����W��
//
//=============================================================================
#include "Notes.h"
#include "input.h"
#include "hitline.h"
#include "particle.h"
#include "score.h"
#include "se.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	NOTES_BALL		"data/MODEL/car000.x"							// �ǂݍ��ރ��f����
#define NOTES_MAX		(255)                                           //�m�[�c�̐�
#define NOTES_SPEED     (5.0f)                                         //���x
#define NOTES_LIMIT     (500.0f)                                       //������Ƃ���
#define HIT_LIMIT       (50.0f)                                        //�����蔻��̔���������E�l
#define LINE_WHIDE      (50.0f)                                       //line�̕�

// �m�[�c�̏����ʒu
#define NOTES_INIT_POS_X		(0.0f)
#define NOTES_INIT_POS_Y		(50.0f)
#define NOTES_INIT_POS_Z		(100.0f)
// �m�[�c�����ʒu
#define NOTES_SET_POS_X		(50.0f)
#define NOTES_SET_POS_Y		(50.0f)
#define NOTES_SET_POS_Z		(400.0f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool SetNotesLane(int laneNum, int notesNo);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DTEXTURE9	D3DTextureNotes;		 // �e�N�X�`���ւ̃|�C���^
LPD3DXMESH			D3DXMeshNotes;			 // ���b�V�����ւ̃|�C���^
LPD3DXBUFFER		D3DXBuffMatNotes;		 // �}�e���A�����ւ̃|�C���^
DWORD				NumMatNotes;			 // �}�e���A�����̐�

NOTES				Notes[NOTES_MAX];		 // Notes�̔z��
D3DXVECTOR3         LinePos[LANE_MAX];		 // �o���ʒu��pos

int					SetNotesNo;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitNotes(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	
	for (int i = 0; i < NOTES_MAX; i++)
	{
		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		Notes[i].pos = D3DXVECTOR3(NOTES_INIT_POS_X, NOTES_INIT_POS_Y, -NOTES_INIT_POS_Z);
		Notes[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		Notes[i].scl = D3DXVECTOR3(2.5f, 2.0f, 0.3f);
		Notes[i].use = false;
	}

	//�o���ʒu��pos�̏�����
	for (int i = 0; i < LANE_MAX; i++)
	{
		LinePos[i] = D3DXVECTOR3(NOTES_SET_POS_X - LINE_WHIDE * i, NOTES_INIT_POS_Y, -NOTES_SET_POS_Z);
	}

	D3DTextureNotes = NULL;
	D3DXMeshNotes = NULL;
	D3DXBuffMatNotes = NULL;
	NumMatNotes = 0;
	SetNotesNo = 0;

	// X�t�@�C���̓ǂݍ���
	if(FAILED(D3DXLoadMeshFromX(NOTES_BALL,				// �ǂݍ��ރ��f���t�@�C����(X�t�@�C��)
								D3DXMESH_SYSTEMMEM,		// ���b�V���̍쐬�I�v�V�������w��
								pDevice,				// IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
								NULL,					// �אڐ��f�[�^���܂ރo�b�t�@�ւ̃|�C���^
								&D3DXBuffMatNotes,	// �}�e���A���f�[�^���܂ރo�b�t�@�ւ̃|�C���^
								NULL,					// �G�t�F�N�g�C���X�^���X�̔z����܂ރo�b�t�@�ւ̃|�C���^
								&NumMatNotes,		// D3DXMATERIAL�\���̂̐�
								&D3DXMeshNotes)))	// ID3DXMesh�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
	{
		return E_FAIL;
	}

#if 0
	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
								TEXTURE_FILENAME,		// �t�@�C���̖��O
								&D3DTextureNotes);	// �ǂݍ��ރ������[
#endif

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitNotes(void)
{
	if(D3DTextureNotes != NULL)
	{// �e�N�X�`���̊J��
		D3DTextureNotes->Release();
		D3DTextureNotes = NULL;
	}

	if(D3DXMeshNotes != NULL)
	{// ���b�V���̊J��
		D3DXMeshNotes->Release();
		D3DXMeshNotes = NULL;
	}

	if(D3DXBuffMatNotes != NULL)
	{// �}�e���A���̊J��
		D3DXBuffMatNotes->Release();
		D3DXBuffMatNotes = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateNotes(void)
{

#if 1
	//�Ƃ肠����A�������ƍ�line�ɏo��
	if (GetKeyboardTrigger(DIK_A))
	{
		
		SetNotes(0);

	}
	//�Ƃ肠����A�������Ɛ^��line�ɏo��
	if (GetKeyboardTrigger(DIK_S))
	{

		SetNotes(1);

	}
	//�Ƃ肠����A�������ƉEline�ɏo��
	if (GetKeyboardTrigger(DIK_D))
	{

		SetNotes(2);

	}
#endif

	//���̈ʒu�܂Ői�ނƏ�����i�{�^���������Ȃ������Ƃ��j
	for (int i = 0; i < NOTES_MAX;i++)
	{
		if (Notes[i].use == true)
		{
			Notes[i].pos.z += NOTES_SPEED;

			if (Notes[i].pos.z >= NOTES_LIMIT)
			{
				Notes[i].use = false;
			}
		}
	}
	
	//�{�^���������Ɠ����蔻��`�F�b�N
	if (GetKeyboardTrigger(DIK_Q))//��
	{
		CheckHitNotes(LEFT_LANE);
	}

	if (GetKeyboardTrigger(DIK_W))//�^��
	{
		CheckHitNotes(CENTER_LANE);
	}

	if (GetKeyboardTrigger(DIK_E))//�E
	{
		CheckHitNotes(RIGHT_LANE);
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawNotes(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	for (int i=0; i < NOTES_MAX; i++)
	{
		if (Notes[i].use == true)
		{
			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&Notes[i].mtxWorld);

			// �X�P�[���𔽉f
			D3DXMatrixScaling(&mtxScl, Notes[i].scl.x, Notes[i].scl.y, Notes[i].scl.z);
			D3DXMatrixMultiply(&Notes[i].mtxWorld, &Notes[i].mtxWorld, &mtxScl);

			// ��]�𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRot, Notes[i].rot.y, Notes[i].rot.x, Notes[i].rot.z);
			D3DXMatrixMultiply(&Notes[i].mtxWorld, &Notes[i].mtxWorld, &mtxRot);

			// �ړ��𔽉f
			D3DXMatrixTranslation(&mtxTranslate, Notes[i].pos.x, Notes[i].pos.y, Notes[i].pos.z);
			D3DXMatrixMultiply(&Notes[i].mtxWorld, &Notes[i].mtxWorld, &mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &Notes[i].mtxWorld);

			// ���݂̃}�e���A�����擾
			pDevice->GetMaterial(&matDef);

			// �}�e���A�����ɑ΂���|�C���^���擾
			pD3DXMat = (D3DXMATERIAL*)D3DXBuffMatNotes->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)NumMatNotes; nCntMat++)
			{
				// �}�e���A���̐ݒ�
				pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

				// �e�N�X�`���̐ݒ�
				pDevice->SetTexture(0, D3DTextureNotes);

				// �`��
				D3DXMeshNotes->DrawSubset(nCntMat);
			}

			// �}�e���A�����f�t�H���g�ɖ߂�
			pDevice->SetMaterial(&matDef);
		}
		
	}

}

//=============================================================================
// �m�[�c�̐ݒu�֐�
// ��P�����Fint lineNum(�m�[�c��ݒu���郌�[���ԍ�)
// �߂�l�@�F�ȁ@��
//=============================================================================
void SetNotes(int laneNum)
{
	SetNotesLane(laneNum, SetNotesNo);

	SetNotesNo++;
	SetNotesNo = SetNotesNo % NOTES_MAX;

	//for (int i = 0; i < NOTES_MAX; i++)
	//{
	//	if (SetNotesLane(laneNum, i)) return;
	//}
}

//=============================================================================
// �m�[�c�̃��[���ւ̐ݒu�֐�
//=============================================================================
bool SetNotesLane(int laneNum, int notesNo)
{
	if (!Notes[notesNo].use)
	{
		switch (laneNum)
		{
		case LEFT_LANE:
			Notes[notesNo].pos = LinePos[LEFT_LANE];
			break;
		case CENTER_LANE:
			Notes[notesNo].pos = LinePos[CENTER_LANE];
			break;
		case RIGHT_LANE:
			Notes[notesNo].pos = LinePos[RIGHT_LANE];
			break;
		}

		Notes[notesNo].use = true;

		return true;
	}

	return false;
}

//=============================================================================
// �m�[�c�̓����蔻��֐�
//=============================================================================
void CheckHitNotes(int laneNum)
{
	D3DXVECTOR3 hitfieldpos;
	float absNotePos, absHitPos, valNoteToHit;

	//absHitPos = fabsf(GetHitLinePos().z);

	// ���胉�C���̈ʒu����
	//hitfieldpos = GetHitLinePos();

	float tmp = 100.0f;

	for (int i = 0; i < NOTES_MAX; i++)
	{
		if (!Notes[i].use) continue;

		//absNotePos = fabsf(Notes[i].pos.z);

		valNoteToHit = fabsf(Notes[i].pos.z - GetHitLinePos().z - tmp);

		//if (Notes[i].pos.z > hitfieldpos.z && Notes[i].pos.z < hitfieldpos.z + HIT_LIMIT && 
		if (valNoteToHit <  HIT_LIMIT &&
			Notes[i].pos.x == LinePos[laneNum].x)
		{
			Notes[i].use = false;
			AddScore(30);
			SetSE(0);

			break;
		}

	}
}
