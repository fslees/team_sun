//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
// Author : 
//
//=============================================================================
#include "particle.h"
#include "input.h"
#include "camera.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	TEXTURE_PARTICLE	"data/TEXTURE/effect000.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define	PARTICLE_SIZE_X		(50.0f)							// �r���{�[�h�̕�
#define	PARTICLE_SIZE_Y		(50.0f)							// �r���{�[�h�̍���
#define	VALUE_MOVE_PARTICLE	(2.0f)							// �ړ����x

#define	MAX_PARTICLE			(512)						// �r���{�[�h�ő吔

#define	DISP_SHADOW				// �e�̕\��
//#undef DISP_SHADOW

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PARTICLE
{
	D3DXVECTOR3		pos;		// �ʒu
	D3DXVECTOR3		rot;		// ��]
	D3DXVECTOR3		scale;		// �X�P�[��
	D3DXVECTOR3		move;		// �ړ���
	D3DXCOLOR		col;		// �F
	float			sizeX;		// ��
	float			sizeY;		// ����
	int				life;		// ����
	bool			use;		// �g�p���Ă��邩�ǂ���

};

struct EFFECT
{

	D3DXVECTOR3 pos;

};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice);
void SetVertexParticle(int idxParticle, float sizeX, float sizeY);
void SetColorParticle(int idxParticle, D3DXCOLOR col);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DTEXTURE9		D3DTextureParticle = NULL;	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 D3DVtxBuffParticle = NULL;	// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^

D3DXMATRIX				mtxWorldParticle;				// ���[���h�}�g���b�N�X

PARTICLE				particle[MAX_PARTICLE];		// �p�[�e�B�N�����[�N
D3DXVECTOR3				posBase[4];					// �r���{�[�h�����ʒu
int						alpha;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ���_���̍쐬
	MakeVertexParticle(pDevice);

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,						// �f�o�C�X�ւ̃|�C���^
								TEXTURE_PARTICLE,			// �t�@�C���̖��O
								&D3DTextureParticle);	// �ǂݍ��ރ������[

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
// �I������
//=============================================================================
void UninitParticle(void)
{
	if(D3DTextureParticle != NULL)
	{// �e�N�X�`���̊J��
		D3DTextureParticle->Release();
		D3DTextureParticle = NULL;
	}

	if(D3DVtxBuffParticle != NULL)
	{// ���_�o�b�t�@�̊J��
		D3DVtxBuffParticle->Release();
		D3DVtxBuffParticle = NULL;
	}
}

//=============================================================================
// �X�V����
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
// �`�揈��
//=============================================================================
void DrawParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxView,mtxScale,mtxTranslate;
	CAMERA *camera = GetCamera();

	// ���C�e�B���O�𖳌���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �p�[�e�B�N�����ۂ��ݒ������(���Z���Z�����E���e�X�g�EZ�\�[�g�j
	
		// ���Z����
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// ���� = �]����(DEST) - �]����(SRC)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// Z��r�Ȃ�
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

	
		// ���e�X�g�ݒ�
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, alpha);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	
	

	for(int cnt = 0; cnt < MAX_PARTICLE; cnt++)
	{
		if(particle[cnt].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&mtxWorldParticle);

			// �r���[�}�g���b�N�X���擾
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

			// �X�P�[���𔽉f
			D3DXMatrixScaling(&mtxScale, particle[cnt].scale.x, particle[cnt].scale.y, particle[cnt].scale.z);
			D3DXMatrixMultiply(&mtxWorldParticle, &mtxWorldParticle, &mtxScale);

			// �ړ��𔽉f
			D3DXMatrixTranslation(&mtxTranslate, particle[cnt].pos.x, particle[cnt].pos.y, particle[cnt].pos.z);
			D3DXMatrixMultiply(&mtxWorldParticle, &mtxWorldParticle, &mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &mtxWorldParticle);

			// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
			pDevice->SetStreamSource(0, D3DVtxBuffParticle, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, D3DTextureParticle);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (cnt * 4), NUM_POLYGON);
		}
	}

	// ���C�e�B���O��L����
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);



	// �ʏ�u�����h�ɖ߂�
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// ���\�[�X�J���[�̎w��
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// ���f�X�e�B�l�[�V�����J���[�̎w��
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice)
{
	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
    if( FAILED( pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX * MAX_PARTICLE,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
												D3DUSAGE_WRITEONLY,							// ���_�o�b�t�@�̎g�p�@�@
												FVF_VERTEX_3D,								// �g�p���钸�_�t�H�[�}�b�g
												D3DPOOL_MANAGED,							// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
												&D3DVtxBuffParticle,						// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
												NULL)))										// NULL�ɐݒ�
	{
        return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		D3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		for(int cnt = 0; cnt < MAX_PARTICLE; cnt++, pVtx += 4)
		{
			// ���_���W�̐ݒ�
			pVtx[0].vtx = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[1].vtx = D3DXVECTOR3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[2].vtx = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[3].vtx = D3DXVECTOR3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);

			// �@���̐ݒ�
			pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

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
		}

		// ���_�f�[�^���A�����b�N����
		D3DVtxBuffParticle->Unlock();
	}

	return S_OK;
}

//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void SetVertexParticle(int idxParticle, float sizeX, float sizeY)
{
	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		D3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (idxParticle * 4);

		// ���_���W�̐ݒ�
		pVtx[0].vtx = D3DXVECTOR3(-sizeX / 2, -sizeY / 2, 0.0f);
		pVtx[1].vtx = D3DXVECTOR3(-sizeX / 2, sizeY / 2, 0.0f);
		pVtx[2].vtx = D3DXVECTOR3(sizeX / 2, -sizeY / 2, 0.0f);
		pVtx[3].vtx = D3DXVECTOR3(sizeX / 2, sizeY / 2, 0.0f);

		// ���_�f�[�^���A�����b�N����
		D3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// ���_�J���[�̐ݒ�
//=============================================================================
void SetColorParticle(int idxParticle, D3DXCOLOR col)
{
	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		D3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (idxParticle * 4);

		// ���_���W�̐ݒ�
		pVtx[0].diffuse =
		pVtx[1].diffuse =
		pVtx[2].diffuse =
		pVtx[3].diffuse = col;

		// ���_�f�[�^���A�����b�N����
		D3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// ���_���̍쐬
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

			// ���_���W�̐ݒ�
			SetVertexParticle(cnt, PARTICLE_SIZE_X, PARTICLE_SIZE_Y);

			idxParticle = cnt;


			break;
		}
	}

	return idxParticle;
}



void SetEffect(D3DXVECTOR3* pos, int occurrencePos)	// SetEffect(�z��[]�Ԃ�pos, �����ʒu�̐�);	
{

	// ���S���o��
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

			// �m�[�}���C�Y
			//D3DXVec3Normalize(&norVec, &vec);
			

			SetParticle(center, move, 60);
	}

}