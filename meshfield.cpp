//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield.cpp]
// Author : 
//
//=============================================================================
#include "meshfield.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define	LOOP_MAX		(500)		// �t���N�^���̉��Z��
#define	TEXTURE_FIELD	"data/TEXTURE/field000.jpg"		// �ǂݍ��ރe�N�X�`���t�@�C����

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DTEXTURE9 D3DTextureField;			// �e�N�X�`���ǂݍ��ݏꏊ
LPDIRECT3DVERTEXBUFFER9 D3DVtxBuffField;	// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
LPDIRECT3DINDEXBUFFER9 D3DIdxBuffField;		// �C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^

D3DXMATRIX mtxWorldField;					// ���[���h�}�g���b�N�X
D3DXVECTOR3 posField;						// �|���S���\���ʒu�̒��S���W
D3DXVECTOR3 rotField;						// �|���S���̉�]�p

int numBlockXField, numBlockZField;			// �u���b�N��
int numVertexField;							// �����_��	
int numVertexIndexField;					// ���C���f�b�N�X��
int numPolygonField;						// ���|���S����
float blockSizeXField, blockSizeZField;	// �u���b�N�T�C�Y
float xRand, zRand, rRand;
float valField;
float xMax, zMax, rMax;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitMeshField(D3DXVECTOR3 pos, D3DXVECTOR3 rot,
							int numBlockX, int numBlockZ, float blockSizeX, float blockSizeZ)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	xMax = (blockSizeX * numBlockX) / 2;
	zMax = (blockSizeX * numBlockX) / 2;
	rMax = 100;

	valField = (blockSizeX * numBlockX);

	// �|���S���\���ʒu�̒��S���W��ݒ�
	posField = pos;

	rotField = rot;

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
								TEXTURE_FIELD,		// �t�@�C���̖��O
								&D3DTextureField);	// �ǂݍ��ރ������[

	// �u���b�N���̐ݒ�
	numBlockXField = numBlockX;
	numBlockZField = numBlockZ;

	// ���_���̐ݒ�
	numVertexField = (numBlockX + 1) * (numBlockZ + 1);

	// �C���f�b�N�X���̐ݒ�
	numVertexIndexField = (numBlockX + 1) * 2 * numBlockZ + (numBlockZ - 1) * 2;

	// �|���S�����̐ݒ�
	numPolygonField = numBlockX * numBlockZ * 2 + (numBlockZ - 1) * 4;

	// �u���b�N�T�C�Y�̐ݒ�
	blockSizeXField = blockSizeX;
	blockSizeZField = blockSizeZ;

	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
    if(FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * numVertexField,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
												D3DUSAGE_WRITEONLY,					// ���_�o�b�t�@�̎g�p�@�@
												FVF_VERTEX_3D,						// �g�p���钸�_�t�H�[�}�b�g
												D3DPOOL_MANAGED,					// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
												&D3DVtxBuffField,				// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
												NULL)))								// NULL�ɐݒ�
	{
        return E_FAIL;
	}

	// �I�u�W�F�N�g�̃C���f�b�N�X�o�b�t�@�𐶐�
    if(FAILED(pDevice->CreateIndexBuffer(sizeof(WORD) * numVertexIndexField,		// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
												D3DUSAGE_WRITEONLY,					// ���_�o�b�t�@�̎g�p�@�@
												D3DFMT_INDEX16,						// �g�p����C���f�b�N�X�t�H�[�}�b�g
												D3DPOOL_MANAGED,					// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
												&D3DIdxBuffField,				// �C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
												NULL)))								// NULL�ɐݒ�
	{
        return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;
#if 0
		const float texSizeX = 1.0f / nNumBlockX;
		const float texSizeZ = 1.0f / nNumBlockZ;
#else
		const float texSizeX = 1.0f;
		const float texSizeZ = 1.0f;
#endif

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		D3DVtxBuffField->Lock( 0, 0, (void**)&pVtx, 0 );

		for(int cntVtxZ = 0; cntVtxZ < (numBlockZField + 1); cntVtxZ++)
		{
			for(int cntVtxX = 0; cntVtxX < (numBlockXField + 1); cntVtxX++)
			{
				// ���_���W�̐ݒ�
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].vtx.x = -(numBlockXField / 2.0f) * blockSizeXField + cntVtxX * blockSizeXField;
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].vtx.y = 0.0f;		//rand()%100;
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].vtx.z = (numBlockZField / 2.0f) * blockSizeZField - cntVtxZ * blockSizeZField;

				// �@���̐ݒ�
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].nor = D3DXVECTOR3(0.0f, 1.0, 0.0f);

				// ���ˌ��̐ݒ�
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

				// �e�N�X�`�����W�̐ݒ�
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].tex.x = texSizeX * cntVtxX;
				pVtx[cntVtxZ * (numBlockXField + 1) + cntVtxX].tex.y = texSizeZ * cntVtxZ;
			}
		}

		// ���_�f�[�^���A�����b�N����
		D3DVtxBuffField->Unlock();
	}

	{//�C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�
		WORD *pIdx;

		// �C���f�b�N�X�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		D3DIdxBuffField->Lock( 0, 0, (void**)&pIdx, 0 );

		int nCntIdx = 0;
		for(int cntVtxZ = 0; cntVtxZ < numBlockZField; cntVtxZ++)
		{
			if(cntVtxZ > 0)
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
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
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
				pIdx[nCntIdx] = cntVtxZ * (numBlockXField + 1) + numBlockXField;
				nCntIdx++;
			}
		}

		// �C���f�b�N�X�f�[�^���A�����b�N����
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
// �I������
//=============================================================================
void UninitMeshField(void)
{
	if(D3DVtxBuffField)
	{// ���_�o�b�t�@�̊J��
		D3DVtxBuffField->Release();
		D3DVtxBuffField = NULL;
	}

	if(D3DIdxBuffField)
	{// �C���f�b�N�X�o�b�t�@�̊J��
		D3DIdxBuffField->Release();
		D3DIdxBuffField = NULL;
	}

	if(D3DTextureField)
	{// �e�N�X�`���̊J��
		D3DTextureField->Release();
		D3DTextureField = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateMeshField(void)
{

	
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMeshField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXMATRIX mtxRot, mtxTranslate;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorldField);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rotField.y, rotField.x, rotField.z);
	D3DXMatrixMultiply(&mtxWorldField, &mtxWorldField, &mtxRot);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, posField.x, posField.y, posField.z);
	D3DXMatrixMultiply(&mtxWorldField, &mtxWorldField, &mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorldField);

	// ���_�o�b�t�@�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetStreamSource(0, D3DVtxBuffField, 0, sizeof(VERTEX_3D));

	// �C���f�b�N�X�o�b�t�@�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetIndices(D3DIdxBuffField);

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, D3DTextureField);

	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertexField, 0, numPolygonField);
}

