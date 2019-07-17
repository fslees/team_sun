//=============================================================================
//
// �I�u�u�W�F�N�g�}�l�[�W���[���� [objManagger.cpp]
// Author : GP12A332_26_�c�����z
//
//=============================================================================
#include "main.h"
#include "objmanager.h"
#include "notes.h"

/*****************************************************************************
�}�N����`
*****************************************************************************/
#define CLOSE_FRAME			(1000)	// �t�@�C���ǂݍ��ݏI���t���[��

/*****************************************************************************
�O���[�o���ϐ�
*****************************************************************************/

FILE		*Fp = NULL;		// �|�b�v�f�[�^���������t�@�C���ւ̃|�C���^
ObjManager	PopData;		// �}�l�[�W���[�\���̕ϐ�	
/******************************************************************************
����������
******************************************************************************/
HRESULT InitObjManager(void)
{
	if (Fp != NULL)
	{

	}

	Fp = fopen("data/DATA/objData.csv", "r");

	if (Fp == NULL)
	{
		return S_FALSE;
	}

	//�|�b�v�f�[�^�̓ǂݍ���
	PopData.scanResult = fscanf(Fp, "%d,%d,%d", &PopData.nextPopFrame,&PopData.nextLaneNum, &PopData.nextPopNum);

	return S_OK;
}

/******************************************************************************
�X�V����
******************************************************************************/
void UpdateObjManager(void)
{
	// �t���[���̍X�V
	PopData.frame++;

	// �I�u�W�F�N�g���|�b�v�����鎞�Ԃł�������|�b�v
	if (PopData.nextPopFrame < PopData.frame && PopData.scanResult != EOF)
	{ 
		// �I�u�W�F�N�g�̔���
		SetObj(PopData.nextLaneNum, PopData.nextPopNum);
		
		//�|�b�v�f�[�^�̓ǂݍ���
		PopData.scanResult = fscanf(Fp, "%d,%d,%d", &PopData.nextPopFrame, &PopData.nextLaneNum, &PopData.nextPopNum);
	}

	// �t�@�C���̏I��
	if (PopData.nextPopFrame == CLOSE_FRAME)
	{
		fclose(Fp);
	}
}

/******************************************************************************
 �I�u�W�F�N�g�̔�������
******************************************************************************/
void SetObj(int laneNum, int objNum)
{
	SetNotes(laneNum);
}
