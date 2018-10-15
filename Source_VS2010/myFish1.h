//myFish1.h
//���z��
//�{�̂͂������������Ƃ��č쐬

class CFish1
{
public:
	CFish1(void);
	~CFish1(void) { };
	//�����o�ϐ�
	float* diffuse ;//�g�U�����˗�
	float* specular;//���ʌ����˗�
	float highlight;//����
	float* diffuseS;//�e�̊g�U�����˗�
	//float* specularS;//�e�̋��ʌ����˗�
	bool flagShadow;

	//�ʒu
	float pos[3];
	//�p���p
	float ang[3];
	float radius0;//�~�^���̔��a
	float period0;//�����]�̎���
	float angSwing;//���h�̐U���p�x�i���E�Ώ́j
	float maxAngSwing;//���̍ő�p�x
	float periodSwing;//���h�̐U������

	//����
	float trunk1[3];
	float trunk2[3];
	//�����o�֐�
	void draw();
	void motion(float t);
private:
	void drawTrunk();
	void drawTaile();
	void drawEyes();
	void drawMouth();
};

//----------------------------------------------------------------
CFish1::CFish1()
{

	static float diffuse0[] = {0.9, 0.8, 0.2, 1.0};
	static float specular0[] = {0.4, 0.4, 0.9, 1.0};
	static float diffuseS0[] = {0.2, 0.2, 0.2, 0.2};
	//static float specularS0[] = {0.0, 0.0, 0.0, 1.0};

	//���z�����̃}�e���A������
	diffuse = diffuse0;
	specular = specular0;
	diffuseS = diffuseS0;
	highlight = 100.0;

	pos[0] = 0.0; pos[1] = 3.0; pos[2] = 5.0;
	ang[0] = 0.0; ang[1] = 0.0; ang[2] = 0.0;
	//���h�̊p�x�i���E�Ώ́j
	angSwing = 0.0;
	radius0 = 6.5;
	period0 = 15.0;//�����]�̎���
	periodSwing = 2.0;//���h�̎���
	maxAngSwing = 20.0;//���h�̍ő�p�x
	flagShadow = false;

	//����
	trunk1[0] = 0.5; trunk1[1] = 0.1; trunk1[2] = 0.5; 
	trunk2[0] = 0.5; trunk2[1] = 0.1; trunk2[2] = 0.5; 
}

//------------------------------------------------------------------
void CFish1::draw()
{
	setTextureMatrix();
	//�e�N�X�`�����W�̂��߂̃��f�����O�ϊ�
	glTranslatef(pos[0], pos[1], pos[2]);
	//�̑S�̂̎p��
	glRotatef(ang[2]+90.0, 0.0, 0.0, 1.0); //z����]
	glRotatef(ang[1], 0.0, 1.0, 0.0);      //y����]
	glRotatef(ang[0]+90.0, 1.0, 0.0, 0.0); //x����]
	//�X�P�[�����O
	glScalef(4.0, 4.0, 4.0);
	//�ʏ�̃��f���r���[�ϊ��ɖ߂� 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//���݈ʒu
	glTranslatef(pos[0], pos[1], pos[2]);
	//�̑S�̂̎p��
	glRotatef(ang[2]+90.0, 0.0, 0.0, 1.0); //z����]
	glRotatef(ang[1], 0.0, 1.0, 0.0);      //y����]
	glRotatef(ang[0]+90.0, 1.0, 0.0, 0.0); //x����]
	//�X�P�[�����O
	glScalef(4.0, 4.0, 4.0);

	if(flagShadow) 	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,diffuseS);
	else
	{
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,diffuse);
		glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
		glMaterialf(GL_FRONT,GL_SHININESS,100.0);
	}

	//����
	drawSuper(trunk1, trunk2, 20, 20, 1.5, 1.5, 0.8, angSwing, 5, 0);

	drawTaile();
	drawEyes();
	drawMouth();
	glPopMatrix();
}

void CFish1::drawEyes()
{
	float eyeDif[] = { 0.3, 0.3, 0.6, 1.0 };
	float eyeSpe[] = { 0.1, 0.1, 0.4, 1.0};
	//����
/*
	setTextureMatrix();
	//�e�N�X�`�����W�̂��߂̃��f�����O�ϊ�
	glTranslatef(pos[0], pos[1], pos[2]);
	//�ʏ�̃��f���r���[�ϊ��ɖ߂� 
	glMatrixMode(GL_MODELVIEW);
*/
	if(flagShadow) 	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,diffuseS);
	else
	{
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, eyeDif);
		glMaterialfv(GL_FRONT,GL_SPECULAR, eyeSpe);
		glMaterialf(GL_FRONT,GL_SHININESS, 50.0);
	}
	glPushMatrix();
	glTranslatef(trunk1[0]/3.0, trunk1[1] /2.0, trunk1[2] * 0.8);
		drawSphere(0.06, 8, 8);
	glPopMatrix();
	//�E��

/*
	setTextureMatrix();
	//�e�N�X�`�����W�̂��߂̃��f�����O�ϊ�
	glTranslatef(pos[0], pos[1], pos[2]);
	//�ʏ�̃��f���r���[�ϊ��ɖ߂� 
	glMatrixMode(GL_MODELVIEW);
*/
	glPushMatrix();
	glTranslatef(-trunk1[0]/3.0, trunk1[1] /2.0, trunk1[2] * 4.0/ 5.0);
		drawSphere(0.06, 8, 8);
	glPopMatrix();
}

void CFish1::drawMouth()
{
	float mouthDif[] = { 0.5, 0.5, 0.2, 1.0};
	float mouthSpe[] = { 0.3, 0.3, 0.3, 1.0};
/*
	setTextureMatrix();
	//�e�N�X�`�����W�̂��߂̃��f�����O�ϊ�
	glTranslatef(pos[0], pos[1], pos[2]);
	//�ʏ�̃��f���r���[�ϊ��ɖ߂� 
	glMatrixMode(GL_MODELVIEW);
*/
	if(flagShadow) 	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,diffuseS);
	else
	{
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, mouthDif);
		glMaterialfv(GL_FRONT,GL_SPECULAR, mouthSpe);
		glMaterialf(GL_FRONT,GL_SHININESS, 20.0);
	}

	glPushMatrix();
	glTranslatef(0.0, -trunk1[1] * 3.0/5.0, trunk1[2]* 3.0/ 5.0);
	glScalef(2.0, 0.2, 1.0);
		drawSphere(0.1, 8, 8);
	glPopMatrix();
}

void CFish1::drawTaile()
{
/*
	setTextureMatrix();
	//�e�N�X�`�����W�̂��߂̃��f�����O�ϊ�
	glTranslatef(pos[0], pos[1], pos[2]);
	//�ʏ�̃��f���r���[�ϊ��ɖ߂� 
	glMatrixMode(GL_MODELVIEW);
*/
	if(flagShadow) 	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,diffuseS);
	else
	{
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT,GL_SPECULAR, specular);
		glMaterialf(GL_FRONT,GL_SHININESS, 100.0);
	}
	glPushMatrix();
	glTranslatef(0.0, 0.0, -trunk2[2] * 3.2/ 2.0);
		drawCylinder(0.01, 0.05, 1.0, 8, 8);
	glPopMatrix();
}

void CFish1::motion(float t)
{
	//������]
	double theta = 360.0 * t / period0 + 180.0;
	pos[0] = radius0 * cos( M_PI * theta / 180.0);
	pos[1] = radius0 * sin( M_PI * theta / 180.0);
	ang[2] = theta + 90.0;//����
	ang[0] = -20.0;
	angSwing = maxAngSwing * sin(2.0 * M_PI * t / periodSwing);
	draw();
}