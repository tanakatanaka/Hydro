#include"SubGLSL.h"
#include "../../myGlsl.h"
//#include"../../Assist3D.h"


void SubGLSL::InitGlsl(GLuint *program, std::string vertexFile, std::string fragmentFile)
{
	std::string vertexPass = "shader/" + vertexFile;

	if (fragmentFile != "")
	{
		std::string fragmentPass = "shader/" + fragmentFile;
		initGlsl(program, vertexPass.c_str(), fragmentPass.c_str());
	}
	else
	{
		initGlsl(program, vertexPass.c_str());
	}

	InitWindow();
}

void SubGLSL::InitWindow()
{
	//Assist3D::GetInstance().SetLight();

	/*
	//�w�i�F
	glClearColor(0.2, 0.5, 0.9, 1.0);
	setCamera();//���_�����߂�
	setLight(); //�����ݒ�@
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	//�J�����Ǝ��̐ς̏����l�̕ύX�iView�̐錾��setup.h)
	view.dist = 30.0;
	view.vCnt = Vector3D(0.0, 0.5, waveHeight);
	view.theta = 30.0;
	view.phi = -90.0;//�E�� +x��
	setCamera();
	view0 = view;
	lightPos[2] = 20.0;
	lightPos0[2] = 20.0;

	//���Ԋ֌W
	memset(&freq, 0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter(&lastTime);
	elapseTime2 = 0.0;//start��̑��o�ߎ���
	elapseTime3 = 0.0;//start��̑��o�ߎ���(�������j

	glGenTextures(6, texID);//�e�N�X�`���I�u�W�F�N�g�̖��O�t��
	glGenFramebuffersEXT(5, fbo);//�t���[���o�b�t�@�I�u�W�F�N�g�̐���

								 //�\���E�B���h�E�ɕ����������_�����O���鏀��
	setFontBitmaps();

	//display list�쐬
	createDisplayLists();

	initData();

	//����L�[�̐���
	printf("�}�E�X����Ŏ��_�ύX�� \n");
	printf("  dolly�F�����t�߂����{�^���N���b�N�ŋ߂Â��C�E�{�^���N���b�N�ŉ������� \n");
	printf("  pan�F��������щE�����E�{�^���N���b�N�Œ����_�����E�ɕω����� \n");
	printf("  tilt�F�^�エ��ѐ^�����E�{�^���N���b�N�Œ����_���㉺�ɕω����� \n");
	printf("  tumble�F���E�Ƀh���b�O����Ǝ��_�����E�ɕω����� \n");
	printf("  crane�F�㉺�Ƀh���b�O����Ǝ��_���㉺�ɕω����� \n");
	printf("  zoom�F�������E�{�^���N���b�N�ŃY�[���C�� \n");
	printf("        �E�����E�{�^���N���b�N�ŃY�[���A�E�g \n");
	*/
}

