//=============================================================================
//
// �I�u�W�F�N�g�����}�l�[�W���[�w�b�_ [objmanager.h]
// Author : GP12A332_26_�c�����z
//
//=============================================================================
#ifndef _OBJMANAGER_H_
#define _OBJMANAGER_H_

typedef struct 
{
	int nextPopFrame;	// �I�u�W�F�N�g�����t���[��
	int nextLaneNum;	// �I�u�W�F�N�g�������[���ԍ�
	int nextPopNum;		// �I�u�W�F�N�g�̔�����
	int scanResult;		// �f�[�^�̓ǂݍ���
	int frame;			// �����t���[���J�E���g

}ObjManager;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitObjManager(void);
void UpdateObjManager(void);
void SetObj(int laneNum, int objNum);

#endif
