#pragma once
#include<windows.h>
#include "myMath6.h"


class Assist3D
{
public:

	//Window�̃T�C�Y,�ʒu
	int win_width = 600;
	int win_height = 500;
	int win_x0 = 500;
	int win_y0 = 100;

	//�o�ߎ���
	LARGE_INTEGER freq, lastTime, currentTime;
	double pTime;
	double elapseTime1, elapseTime2, elapseTime3;

	//�}�E�X����
	int xStart, yStart;
	bool flagMouse = false;
	bool flagSlide = false;
	bool flagStart = false; //�A�j���[�V�����X�^�[�g�t���O
	bool flagFreeze = false;//�ꎞ��~�t���O
	bool flagFreeze0 = false;
	bool flagStep = false;

	//�^�C���X�e�b�v�̋}���ȕω��ɑΉ������邽�߂����Ұ�
	float timeStep, timeStep0, timeStep1;
	//�����e
	int flagOrtho = 0;
	float ortho_scale = 5.0;//�����e�̂Ƃ��̃T�C�Y���W�� 

	//����
	float lightPos[4] = { 0.0, 0.0, 100.0, 1.0 };//�����ʒu
	float lightPos0[4] = { 0.0, 0.0, 100.0, 1.0 };//�����ʒu

	void SetLight()
	{
		float lightAmbient0[] = { 0.7, 0.7, 0.7, 1.0 }; //����
		float lightDiffuse0[] = { 0.9, 0.9, 0.9, 1.0 }; //�g�U��
		float lightSpecular0[] = { 0.9, 0.9, 0.9, 1.0 };//���ʌ�
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse0);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular0);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
	}

	//�J�����Ǝ��̐�
	struct View
	{
		//�J����
		Vector3D vPos;//�ʒu�i���_�j
		Vector3D vCnt;//�����_
		float dist;  //�����_���王�_�܂ł̋���(R)
		float theta; //�p�i�����ʂƂ̕Ίp�j
		float phi;   //���ʊp
					 //���̐�
		float fovY;  //����p
		float nearZ; //�O���N���b�v��(�ߕ���)
		float farZ;  //����N���b�v��(������)
	};
	View view =
	{
		Vector3D(),             //pos(���_,���ݒ�j
		Vector3D(0.0, 0.0, 7.0),//cnt(�����_�j 
		100.0, 15.0, 0.0,       //dist, theta, phi
								//40.0, 0.1, 20.0       //fovY,nearZ,farZ
								30.0, 1.0, 1000.0       //fovY,nearZ,farZ
	};
	View view0 = view;



	void resize(int w, int h)
	{
		//�r���[�|�[�g�ϊ�
		glViewport(0, 0, w, h);
		//�v���W�F�N�V�����s��̎w��
		glMatrixMode(GL_PROJECTION);
		//�v���W�F�N�V�����s��̏�����
		glLoadIdentity();
		//�������e�s��̐ݒ�i���e�ϊ��j
		float aspect = (double)w / (double)h;
		if (flagOrtho) glOrtho(-ortho_scale * aspect, ortho_scale*aspect, -ortho_scale, ortho_scale, view.nearZ, view.farZ);
		else gluPerspective(view.fovY, aspect, view.nearZ, view.farZ);
		glMatrixMode(GL_MODELVIEW);
		//���f���r���[�s��̏�����
		glLoadIdentity();
		//�\���E�B���h�E�̃T�C�Y
		win_width = w;
		win_height = h;
	}





	
	
	
	
	//�C���X�^���X���擾���邽�߂�static�����o�֐�
	static Assist3D& GetInstance()
	{
		if (!Instance)
		{
			Instance = new Assist3D;
		}
		return *Instance;
	}

	static void Destroy()
	{
		delete Instance;
		Instance = nullptr;
	}
	

private:
	Assist3D() {}
	//�R�s�[�֎~���Ă���
	Assist3D(const Assist3D& other);
	Assist3D& operator = (const Assist3D& other);
	Assist3D(Assist3D&&) = delete;

	static Assist3D* Instance;
};

//�O���[�o���ɐ錾
Assist3D* Assist3D::Instance = nullptr;