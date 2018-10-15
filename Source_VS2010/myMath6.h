//myMath6.h
//�O���t�B�b�N�X�p���w���C�u����
#define M_PI 3.14159265358979323846
double DEG_TO_RAD = M_PI / 180.0;
double RAD_TO_DEG = 180.0 / M_PI;
//--------------------------------------------------------------------------
class Vector2D
{
public:
	float x, y;
	//�R���X�g���N�^
	Vector2D();
	Vector2D(float x0, float y0);
	//���Z�q
	void operator+=(Vector2D a);//���Z
	void operator-=(Vector2D a);//���Z
	void operator*=(float s);//�X�J����Z
	void operator/=(float s);//�X�J�����Z
	friend Vector2D operator*(Vector2D a, float s);//�X�J����Z
	friend Vector2D operator*(float s, Vector2D a);//�X�J����Z
	friend Vector2D operator/(Vector2D a, float s);//�X�J�����Z
	friend Vector2D operator*(Vector2D a, Vector2D b);//�����ǂ����̏�Z
	friend Vector2D operator+(Vector2D a, Vector2D b);//���Z
	friend Vector2D operator-(Vector2D a, Vector2D b);//���Z
	//�����o�֐�
	float magnitude();  //�傫��
	float magnitude2();  //�傫����2��
	float vDist(Vector2D a, Vector2D b);//xy���ʓ��̋���
	float vDist2(Vector2D a, Vector2D b);//xy���ʓ���2�拗��
	void rotZ_deg(float angle); //z����]
	void rotZ_rad(float angle); //z����]
};
//�R���X�g���N�^
Vector2D::Vector2D()
{
    x = y = 0.0;
}
//�R���X�g���N�^
Vector2D::Vector2D(float x0, float y0)
{
    x = x0;
    y = y0;
}
void Vector2D::operator+=(Vector2D a)
{
    x += a.x;
    y += a.y;
}
void Vector2D::operator-=(Vector2D a)
{
    x -= a.x;
    y -= a.y;
}
void Vector2D::operator*=(float s)
{
    x *= s;
    y *= s;
}
void Vector2D::operator/=(float s)
{
    x /= s;
    y /= s;
}
Vector2D operator+(Vector2D a, Vector2D b)
{
    return Vector2D(a.x+b.x, a.y+b.y);
}
Vector2D operator-(Vector2D a, Vector2D b)
{
    return Vector2D(a.x - b.x, a.y - b.y);
}
//�X�J����Z
Vector2D operator*(Vector2D a, float s)
{
    return Vector2D(a.x * s, a.y * s);
}
//�X�J����Z
Vector2D operator*(float s, Vector2D a)
{
    return Vector2D(a.x * s, a.y * s);
}
//�X�J�����Z
Vector2D operator/(Vector2D a, float s)
{
    if(s == 0.0)
			printf("�X�J�����Z�̕��ꂪ�O�ł��I \n");

    return Vector2D(a.x / s, a.y / s);
}
//�����ǂ����̏�Z
Vector2D operator*(Vector2D a, Vector2D b)
{
	return Vector2D( a.x * b.x, a.y * b.y);
}
float Vector2D::magnitude()
{
    return( sqrt(x * x + y * y) );
}
float Vector2D::magnitude2()
{
    return(x * x + y * y);
}
void Vector2D::rotZ_deg(float angle)
{
	float xx, yy;

	xx = x; yy = y;
	x = xx * cos(DEG_TO_RAD * angle) - yy * sin(DEG_TO_RAD * angle);
	y = xx * sin(DEG_TO_RAD * angle) + yy * cos(DEG_TO_RAD * angle);
}
void Vector2D::rotZ_rad(float angle)
{
	float xx, yy;

	xx = x; yy = y;
	x = xx * cos(angle) - yy * sin(angle);
	y = xx * sin(angle) + yy * cos(angle);
}
//�x�N�g���ԋ���(xy���ʁj
float vDist(Vector2D a, Vector2D b)
{
    Vector2D c = b - a;
    return( c.magnitude() );
}
//�x�N�g���ԓ�拗��
float vDist2(Vector2D a, Vector2D b)
{
    Vector2D c = b - a;
    return ( c.x * c.x + c.y * c.y);
}

//----------------------------------------------------------
//3�����x�N�g�����Z
class Vector3D{

public:
//�����o�ϐ�
	double x, y, z;
	//�R���X�g���N�^
	Vector3D();
	Vector3D(double x0, double y0, double z0);
	//���Z�q
	void operator+=(Vector3D a);//���Z
	void operator-=(Vector3D a);//���Z
	void operator*=(double s);//�X�J����Z
	void operator/=(double s);//�X�J�����Z
	friend Vector3D operator*(Vector3D a, double s);//�X�J����Z
	friend Vector3D operator*(double s, Vector3D a);//�X�J����Z
	friend Vector3D operator/(Vector3D a, double s);//�X�J�����Z
	friend Vector3D operator+(Vector3D a, Vector3D b);//���Z
	friend Vector3D operator-(Vector3D a, Vector3D b);//���Z
	friend Vector3D operator>>(Vector3D a, Vector3D b);//a����b�֌������P�ʃx�N�g��
	//�����o�֐�
	double magnitude();  //�傫��
	double magnitudeXY();//�傫��(z�����𖳎�)
	double magnitude2();  //�傫����2��
	double magnitudeXY2();//�傫����2��(z�����𖳎�)
	void norm(void);//�傫��1�̃x�N�g���ɕϊ�
	void reverse(void);//���]
	double vDist(Vector3D a, Vector3D b);//����
	double vDistXY(Vector3D a, Vector3D b);//xy���ʓ��̋���
	double vDist2(Vector3D a, Vector3D b);//2�拗��
	Vector3D reverse2();//���]�����x�N�g����Ԃ�
	Vector3D norm2();//���K�������x�N�g����Ԃ�
	Vector3D normXY2();//���K�������x�N�g����Ԃ�(z�����𖳎�)
	Vector3D normYZ2();//���K�������x�N�g����Ԃ�(x�����𖳎�)
	Vector3D normXZ2();//���K�������x�N�g����Ԃ�(y�����𖳎�)
	double vDot(Vector3D a, Vector3D b);//����
	Vector3D vCross(Vector3D a, Vector3D b);//�O��
	Vector3D rotate(Vector3D v, Vector3D elr);//�I�C���[�p�ɂ���]
	Vector3D rotate(Vector3D v, double angle[3]);//�p�x�z��ɂ���]
	Vector3D rotateReverse(Vector3D v, Vector3D elr);//�I�C���[�p�ɂ��t���ŕ��̉�]
	void scale(Vector3D vSize);//����ݸ�
	void rotX(double angle); //x����](deg)
	void rotY(double angle); //y����]
	void rotZ(double angle); //z����]
	void rotX_rad(double angle); //x����](rad)
	void rotY_rad(double angle); //y����]
	void rotZ_rad(double angle); //z����]
	void rotX_rad(Vector3D vPos0, double angle); //x����](rad)
	void rotY_rad(Vector3D vPos0, double angle); //y����]
	void rotZ_rad(Vector3D vPos0, double angle); //z����]
	double getAngle(Vector3D a, Vector3D b);//�Q�̃x�N�g���̂Ȃ��p�x
	Vector3D getEulerX(Vector3D a, Vector3D b);//a���炂�֌������x�N�g���̃I�C���[�p
	Vector3D getEulerZ(Vector3D a, Vector3D b);//a���炂�֌������x�N�g���̃I�C���[�p
	Vector3D getEulerX(Vector3D d);//�����x�N�g��d�̃I�C���[�p
	Vector3D getEulerZ(Vector3D d);//�����x�N�g��d�̃I�C���[�p
};
//----------------------------------------------------------------------------
//�R���X�g���N�^
Vector3D::Vector3D()
{
    x = y = z = 0.0;
}
//----------------------------------------------------------------------------
//�R���X�g���N�^
Vector3D::Vector3D(double x0, double y0, double z0)
{
    x = x0;
    y = y0;
    z = z0;
}
//------------------------------------------------------------------------------
void Vector3D::operator+=(Vector3D a)
{
    x += a.x;
    y += a.y;
    z += a.z;
}
//------------------------------------------------------------------------------
void Vector3D::operator-=(Vector3D a)
{
    x -= a.x;
    y -= a.y;
    z -= a.z;
}
//------------------------------------------------------------------------------
void Vector3D::operator*=(double s)
{
    x *= s;
    y *= s;
    z *= s;
}
//------------------------------------------------------------------------------
void Vector3D::operator/=(double s)
{
    x /= s;
    y /= s;
    z /= s;
}
//------------------------------------------------------------------------------
Vector3D operator+(Vector3D a, Vector3D b)
{
    return Vector3D(a.x+b.x, a.y+b.y, a.z+b.z);
}
//------------------------------------------------------------------------------
Vector3D operator-(Vector3D a, Vector3D b)
{
    return Vector3D(a.x - b.x, a.y - b.y, a.z - b.z);
}
//------------------------------------------------------------------------------
//�X�J����Z
Vector3D operator*(Vector3D a, double s)
{
    return Vector3D(a.x * s, a.y * s, a.z * s);
}
//------------------------------------------------------------------------------
//�X�J����Z
Vector3D operator*(double s, Vector3D a)
{
    return Vector3D(a.x * s, a.y * s, a.z * s);
}
//------------------------------------------------------------------------------
//�X�J�����Z
Vector3D operator/(Vector3D a, double s)
{
    if(s == 0.0)
			printf("�X�J�����Z�̕��ꂪ�O�ł��I \n");

    return Vector3D(a.x / s, a.y / s, a.z / s);
}

//------------------------------------------------------------------------------
//a����b�֌������P�ʃx�N�g��
Vector3D operator>>(Vector3D a, Vector3D b)
{
    Vector3D c = b - a;
    c.norm();
    return c ;
}

//-----------------------------------------------------------------------------
double Vector3D::magnitude()
{
    return( sqrt(x * x + y * y + z * z) );
}
//-----------------------------------------------------------------------------
double Vector3D::magnitudeXY()
{
    return( sqrt(x * x + y * y) );
}
//----------------------------------------------------------------------------
double Vector3D::magnitude2()
{
    return(x * x + y * y + z * z);
}
//-----------------------------------------------------------------------------
double Vector3D::magnitudeXY2()
{
    return(x * x + y * y);
}
//-----------------------------------------------------------------------------
void Vector3D::norm()
{
    double eps = 0.000001;
    double mag = sqrt(x * x + y * y + z * z) ;

	if(mag <= eps){ mag = 1.0;}
    x /= mag;
    y /= mag;
    z /= mag;
    if(fabs(x) < eps) x = 0.0;
    if(fabs(y) < eps) y = 0.0;
    if(fabs(z) < eps) z = 0.0;
}
//-----------------------------------------------------------------------------
Vector3D Vector3D::norm2()
{
    double eps = 0.000001;
    double xx, yy, zz;
    double mag = sqrt(x * x + y * y + z * z) ;
    if(mag < eps) mag = 1.0;
    xx = x / mag;
    yy = y / mag;
    zz = z / mag;
    if(fabs(x) < eps) xx = 0.0;
    if(fabs(y) < eps) yy = 0.0;
    if(fabs(z) < eps) zz = 0.0;
    return(Vector3D(xx, yy, zz));
}
//-----------------------------------------------------------------------------
Vector3D Vector3D::normXY2()
{
    double eps = 0.000001;
    double xx, yy;
    double mag = sqrt(x * x + y * y) ;
    if(mag < eps) mag = 1.0;
    xx = x / mag;
    yy = y / mag;
    if(fabs(x) < eps) xx = 0.0;
    if(fabs(y) < eps) yy = 0.0;
    return(Vector3D(xx, yy, 0.0));
}
//-----------------------------------------------------------------------------
Vector3D Vector3D::normYZ2()
{
    double eps = 0.000001;
    double yy, zz;
    double mag = sqrt(y * y + z * z) ;
    if(mag < eps) mag = 1.0;
    zz = z / mag;
    yy = y / mag;
    if(fabs(z) < eps) zz = 0.0;
    if(fabs(y) < eps) yy = 0.0;
    return(Vector3D(0.0, yy, zz));
}
//-----------------------------------------------------------------------------
Vector3D Vector3D::normXZ2()
{
    double eps = 0.000001;
    double xx, zz;
    double mag = sqrt(x * x + z * z) ;
    if(mag < eps) mag = 1.0;
    xx = x / mag;
    zz = z / mag;
    if(fabs(x) < eps) xx = 0.0;
    if(fabs(z) < eps) zz = 0.0;
    return(Vector3D(xx, 0.0, zz));
}

//-----------------------------------------------------------------------------
void Vector3D::reverse()
{
    x = -x;
    y = -y;
    z = -z;
}
//-----------------------------------------------------------------------------
Vector3D Vector3D::reverse2()
{
	Vector3D v = Vector3D();
	v.x = -x;
	v.y = -y;
	v.z = -z;
	return(v);
}
//------------------------------------------------------------------------------
//����
double vDot(Vector3D a, Vector3D b)
{
    return( a.x*b.x + a.y*b.y + a.z*b.z );
}
//------------------------------------------------------------------------------
//�O��
Vector3D vCross(Vector3D a, Vector3D b)
{
    double x = a.y * b.z - a.z * b.y;
    double y = a.z * b.x - a.x * b.z;
    double z = a.x * b.y - a.y * b.x;
    return( Vector3D(x, y, z) );
}

//-----------------------------------------------------------------------------
void Vector3D::scale(Vector3D vSize)
{
	double xx, yy, zz;

	xx = x; yy = y; zz = z;
	x = xx * vSize.x;
	y = yy * vSize.y;
	z = zz * vSize.z;
}

//------------------------------------------------------------------------------------
void Vector3D::rotX(double angle)
{//angle:deg
	double xx, yy, zz;

	xx = x; yy = y; zz = z;
	x = xx;
	y = yy * cos(DEG_TO_RAD * angle) - zz * sin(DEG_TO_RAD * angle);
	z = yy * sin(DEG_TO_RAD * angle) + zz * cos(DEG_TO_RAD * angle);
}
//------------------------------------------------------------------------------------
void Vector3D::rotY(double angle)
{
	double xx, yy, zz;

	xx = x; yy = y; zz = z;
	x = xx * cos(DEG_TO_RAD * angle) + zz * sin(DEG_TO_RAD * angle);
	y = yy;
	z = - xx * sin(DEG_TO_RAD * angle) + zz * cos(DEG_TO_RAD * angle);
}
//------------------------------------------------------------------------------------
void Vector3D::rotZ(double angle)
{
	double xx, yy, zz;

	xx = x; yy = y; zz = z;
	x = xx * cos(DEG_TO_RAD * angle) - yy * sin(DEG_TO_RAD * angle);
	y = xx * sin(DEG_TO_RAD * angle) + yy * cos(DEG_TO_RAD * angle);
	z = zz;
}
//-----------------------------------------------------------------------------
Vector3D rotate(Vector3D v, Vector3D elr)
{
    v.rotX(elr.x);
    v.rotY(elr.y);
    v.rotZ(elr.z);
    return v;
}

Vector3D rotate(Vector3D v, double angle[3])
{
    v.rotX(angle[0]);
    v.rotY(angle[1]);
    v.rotZ(angle[2]);
    return v;
}
//------------------------------------------------------------------------------------
void Vector3D::rotX_rad(double angle)
{//angle:rad
	double xx, yy, zz;

	xx = x; yy = y; zz = z;
	x = xx;
	y = yy * cos(angle) - zz * sin(angle);
	z = yy * sin(angle) + zz * cos(angle);
}
//------------------------------------------------------------------------------------
void Vector3D::rotY_rad(double angle)
{
	double xx, yy, zz;

	xx = x; yy = y; zz = z;
	x = xx * cos(angle) + zz * sin(angle);
	y = yy;
	z = - xx * sin(angle) + zz * cos(angle);
}
//------------------------------------------------------------------------------------
void Vector3D::rotZ_rad(double angle)
{
	double xx, yy, zz;

	xx = x; yy = y; zz = z;
	x = xx * cos(angle) - yy * sin(angle);
	y = xx * sin(angle) + yy * cos(angle);
	z = zz;
}
//------------------------------------------------------------------------------------
void Vector3D::rotX_rad(Vector3D vPos0, double angle)
{//angle:rad
	//vPos0�𒆐S�Ƃ�����]
	double xx, yy, zz;

	xx = x; yy = y - vPos0.y; zz = z - vPos0.z;
	x = xx;
	y = yy * cos(angle) - zz * sin(angle) + vPos0.y;
	z = yy * sin(angle) + zz * cos(angle) + vPos0.z;
}
//------------------------------------------------------------------------------------
void Vector3D::rotY_rad(Vector3D vPos0, double angle)
{
	double xx, yy, zz;

	xx = x - vPos0.x; yy = y; zz = z - vPos0.z;
	x = xx * cos(angle) + zz * sin(angle) + vPos0.x;
	y = yy;
	z = - xx * sin(angle) + zz * cos(angle) + vPos0.z;
}
//------------------------------------------------------------------------------------
void Vector3D::rotZ_rad(Vector3D vPos0, double angle)
{
	double xx, yy, zz;

	xx = x - vPos0.x; yy = y - vPos0.y; zz = z;
	x = xx * cos(angle) - yy * sin(angle) + vPos0.x;
	y = xx * sin(angle) + yy * cos(angle) + vPos0.y;
	z = zz;
}

Vector3D rotateReverse(Vector3D v, Vector3D elr)
{
    //�t���ŕ��̉�]
    v.rotZ(-elr.z);
    v.rotY(-elr.y);
    v.rotX(-elr.x);
    return v;
}
//------------------------------------------------------------------------------
//�x�N�g���ԋ���
double vDist(Vector3D a, Vector3D b)
{
    Vector3D c = b - a;
    return( c.magnitude() );
}
//------------------------------------------------------------------------------
//�x�N�g���ԋ���(xy���ʁj
double vDistXY(Vector3D a, Vector3D b)
{
    Vector3D c = b - a;
    return( c.magnitudeXY() );
}
//------------------------------------------------------------------------------
//�x�N�g���ԓ�拗��
double vDist2(Vector3D a, Vector3D b)
{
    Vector3D c = b - a;
    return ( c.x * c.x + c.y * c.y + c.z * c.z);
}

//-----------------------------------------------------------------------------
double getAngle(Vector3D a, Vector3D b)
{
    double ang = (a.x*b.x+a.y*b.y+a.z*b.z)/(a.magnitude()*b.magnitude());
    if(ang >= 1.0) ang = 0.0;
    else if (ang <= -1.0) ang = M_PI;
    else ang = acos(ang);
    return ang;//rad�P�ʂŕԂ�
}
//-----------------------------------------------------------------------------
Vector3D getEulerX(Vector3D a, Vector3D b)
{//��{�p���Œ��S�������������ł���_��I�u�W�F�N�g�̃I�C���[�p(deg)
    double cx, cy, cz, len;
    Vector3D e;
    cx = b.x - a.x;
    cy = b.y - a.y;
    cz = b.z - a.z;
    len = vDist(a, b);
    e.x = 0.0;
    if(cz >= len) e.y = -90.0;
    else if(cz <= -len) e.y = 90.0;
    else e.y = - asin(cz / len) * RAD_TO_DEG;
    if(fabs(cx) <= 0.0001 && fabs(cy) <= 0.0001) e.z = 0.0;
		else e.z = atan2(cy, cx) * RAD_TO_DEG;
    return e;
}
//-----------------------------------------------------------------------------
Vector3D getEulerX(Vector3D d)
{//��{�p���Œ��S�������������ł���_��I�u�W�F�N�g�̃I�C���[�p(deg)
    double cx, cy, cz, len;
    Vector3D e;
    cx = d.x;
    cy = d.y;
    cz = d.z;
    len = d.magnitude();
    e.x = 0.0;
    if(cz >= len) e.y = -90.0;
    else if(cz <= -len) e.y = 90.0;
    else e.y = - asin(cz / len) * RAD_TO_DEG;
    if(fabs(cx) <= 0.0001 && fabs(cy) <= 0.0001) e.z = 0.0;
		else e.z = atan2(cy, cx) * RAD_TO_DEG;
    return e;
}
//------------------------------------------------------------------------------
Vector3D getEulerZ(Vector3D a, Vector3D b)
{//��{�p���Œ��S����z�������ł���_��I�u�W�F�N�g�̃I�C���[�p(deg)
		//2011.3.14����
    double cx, cy, cz, len;
    Vector3D e;
    cx = b.x - a.x;
    cy = b.y - a.y;
    cz = b.z - a.z;
    len = vDist(a, b);
    e.z = 0.0;
    if(cy >= len) e.x = -90.0;
    else if(cy <= -len) e.x = 90.0;
    else e.x = -asin(cy / len) * RAD_TO_DEG;
    if(fabs(cx) <= 0.0001 && fabs(cz) <= 0.0001) e.y = 0.0;
    else e.y = atan2(cx, cz) * RAD_TO_DEG;
    return e;
}
//------------------------------------------------------------------------------
Vector3D getEulerZ(Vector3D d)
{//��{�p���Œ��S����z�������ł���_��I�u�W�F�N�g�̃I�C���[�p(deg)
    double cx, cy, cz, len;
    Vector3D e;
    cx = d.x;
    cy = d.y;
    cz = d.z;
    len = d.magnitude();
    e.z = 0.0;
    if(cy >= len) e.x = -90.0;
    else if(cy <= -len) e.x = 90.0;
    else e.x = -asin(cy / len) * RAD_TO_DEG;
    if(fabs(cx) <= 0.0001 && fabs(cz) <= 0.0001) e.y = 0.0;
    else e.y = atan2(cx, cz) * RAD_TO_DEG;
    return e;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//3*3�̃}�g���N�X
//------------------------------------------------------------------------------
class CMatrix{

public:
    //�����o�ϐ�
    double e11, e12, e13, e21, e22, e23, e31, e32, e33;
    //�R���X�g���N�^
    CMatrix();
    CMatrix(double a11, double a12, double a13,
            double a21, double a22, double a23,
            double a31, double a32, double a33);
    //���Z�q
    friend Vector3D operator*(CMatrix m, Vector3D v);
    friend Vector3D operator*(Vector3D v, CMatrix m);
    //�����o�֐�
    double det();
    CMatrix inverse();
};
//----------------------------------------------------------------------------
//�R���X�g���N�^
CMatrix::CMatrix()
{
    e11 = e12 = e13 = 0.0;
    e21 = e22 = e23 = 0.0;
    e31 = e32 = e33 = 0.0;
}
//-----------------------------------------------------------------------------
//�R���X�g���N�^
CMatrix::CMatrix(double a11, double a12, double a13,
                 double a21, double a22, double a23,
                 double a31, double a32, double a33)
{
    e11 = a11; e12 = a12; e13 = a13;
    e21 = a21; e22 = a22; e23 = a23;
    e31 = a31; e32 = a32; e33 = a33;
}
//----------------------------------------------------------------------------
//�s��
double CMatrix::det()
{
    return( e11*e22*e33 + e12*e23*e31 + e13*e32*e21
          - e11*e32*e23 - e12*e21*e33 - e31*e22*e13 );
}
//----------------------------------------------------------------------------
CMatrix CMatrix::inverse()
{
    double d = e11*e22*e33 + e12*e23*e31 + e13*e32*e21
             - e11*e32*e23 - e12*e21*e33 - e31*e22*e13 ;
    if( d == 0.0 ) {
			 printf("�t�s������߂邱�Ƃ��ł��܂���I \n");
       d = 1.0;
    }
    return CMatrix(
        (e22*e33-e23*e32)/d, -(e12*e33-e13*e32)/d,  (e12*e23-e13*e22)/d,
       -(e21*e33-e23*e31)/d,  (e11*e33-e13*e31)/d, -(e11*e23-e13*e21)/d,
        (e21*e32-e22*e31)/d, -(e11*e32-e12*e31)/d,  (e11*e22-e12*e21)/d );
}
//----------------------------------------------------------------------------
//�x�N�g����Z
Vector3D operator*(CMatrix m, Vector3D v)
{
    return Vector3D(
        m.e11*v.x + m.e12*v.y + m.e13*v.z,    //x����
        m.e21*v.x + m.e22*v.y + m.e23*v.z,    //y����
        m.e31*v.x + m.e32*v.y + m.e33*v.z );  //z����
}
//----------------------------------------------------------------------------
//�x�N�g����Z
Vector3D operator*(Vector3D v, CMatrix m)
{
    return Vector3D(
        v.x*m.e11 + v.y*m.e21 + v.z*m.e31,
        v.x*m.e12 + v.y*m.e22 + v.z*m.e32,
        v.x*m.e13 + v.y*m.e23 + v.z*m.e33 );
}

//----------------------------------------------------------------------------
//�l�����Fquaternion
// ��`�� q = s + xi + yj + zk;
//----------------------------------------------------------------------------
class CQuaternion{

public:
	//�����o�ϐ�
	double s;//�X�J���l
	Vector3D v;//�x�N�g��
	//�R���X�g���N�^
	CQuaternion();
	CQuaternion(double a0, double a1, double a2, double a3);
	CQuaternion(double a0, Vector3D v);

	//���Z�q
	CQuaternion operator~();
	void operator+=(CQuaternion q);
	void operator-=(CQuaternion q);
	void operator*=(double a);
	void operator/=(double a);
	friend CQuaternion operator+(CQuaternion p, CQuaternion q);
	friend CQuaternion operator-(CQuaternion p, CQuaternion q);
	friend CQuaternion operator*(CQuaternion p, CQuaternion q);
	friend CQuaternion operator*(CQuaternion p, double s);
	friend CQuaternion operator*(double s, CQuaternion p);
	friend CQuaternion operator*(Vector3D v, CQuaternion q);
	friend CQuaternion operator*(CQuaternion q, Vector3D v);
	//�����o�֐�
	double magnitude(void);
	void norm(void);//�傫��1��quaternion�ɕϊ�
	Vector3D getVector();
	double getScalar();
	Vector3D qvRotate(CQuaternion q, Vector3D v);
	CQuaternion getQFromEulerXYZ(Vector3D elr);
	CQuaternion getQFromEulerZYX(Vector3D elr);
	CQuaternion getQFromAxis(double angle, Vector3D axis);
};
//-----------------------------------------------------------------------------
//�R���X�g���N�^
CQuaternion::CQuaternion()
{
    s = 1.0;
    v.x = v.y = v.z = 0.0;
}
//-----------------------------------------------------------------------------
//�R���X�g���N�^
CQuaternion::CQuaternion(double a0, double a1, double a2, double a3)
{
    s = a0;
    v.x = a1;
    v.y = a2;
    v.z = a3;
}
//-----------------------------------------------------------------------------
//�R���X�g���N�^
CQuaternion::CQuaternion(double a0, Vector3D u)
{
    s = a0;
		v = u;
}
//-----------------------------------------------------------------------------
//�����l����
CQuaternion CQuaternion::operator~()
{
    return CQuaternion( s, -v.x, -v.y, -v.z);
}
//-----------------------------------------------------------------------------
void CQuaternion::operator+=(CQuaternion q)
{
    s += q.s;
    v.x += q.v.x;
    v.y += q.v.y;
    v.z += q.v.z;
}
//-----------------------------------------------------------------------------
void CQuaternion::operator-=(CQuaternion q)
{
    s -= q.s;
    v.x -= q.v.x;
    v.y -= q.v.y;
    v.z -= q.v.z;
}
//-----------------------------------------------------------------------------
void CQuaternion::operator*=(double a)
{
    s *= a;
    v.x *= a;
    v.y *= a;
    v.z *= a;
}
//-----------------------------------------------------------------------------
void CQuaternion::operator/=(double a)
{
    s /= a;
    v.x /= a;
    v.y /= a;
    v.z /= a;
}
//------------------------------------------------------------------------------
//�l�����ǂ����̘a
CQuaternion operator+(CQuaternion p, CQuaternion q)
{
    return CQuaternion( p.s + q.s,
           p.v.x + q.v.x, p.v.y + q.v.y, p.v.z + q.v.z);
}
//------------------------------------------------------------------------------
//�l�����ǂ����̍�
CQuaternion operator-(CQuaternion p, CQuaternion q)
{
    return CQuaternion( p.s - q.s,
         p.v.x - q.v.x, p.v.y - q.v.y, p.v.z - q.v.z);
}
//------------------------------------------------------------------------------
//�l�����ǂ����̏�Z
CQuaternion operator*(CQuaternion p, CQuaternion q)
{
    CQuaternion qq;
	qq.s = p.s * q.s - vDot(p.v , q.v) ; //�Q���ڂ̓x�N�g������
	qq.v = p.s * q.v + q.s * p.v + vCross(p.v , q.v); //�R���ڂ̓x�N�g���O��
    return qq;
}
//-----------------------------------------------------------------------------
//�X�J����Z
CQuaternion operator*(CQuaternion p, double a)
{
    return CQuaternion(p.s * a, p.v.x * a, p.v.y * a, p.v.z * a);
}
//-----------------------------------------------------------------------------
//�X�J����Z
CQuaternion operator*(double a, CQuaternion p)
{
    return CQuaternion(p.s * a, p.v.x * a, p.v.y * a, p.v.z * a);
}
//------------------------------------------------------------------------------
//�x�N�g���Ǝl�����̏�Z
CQuaternion operator*(Vector3D v, CQuaternion q)
{
    CQuaternion p, pp;
    p.s = 0.0; p.v = v; //�X�J������0�̃N�I�[�^�j�I��
    pp = p * q;
    return pp; 
}
//------------------------------------------------------------------------------
//�x�N�g���Ǝl�����̏�Z
CQuaternion operator*(CQuaternion p, Vector3D v)
{
    CQuaternion q, pp;
    q.s = 0.0; q.v = v;
    pp = p * q;
    return pp;
}

//-----------------------------------------------------------------------------
//�傫����Ԃ�
double CQuaternion::magnitude()
{
    return sqrt( s * s + v.x * v.x + v.y * v.y + v.z * v.z);
}
//-----------------------------------------------------------------------------
//���K��
void CQuaternion::norm()
{
    double eps = 0.000001;
    double mag = sqrt(s * s + v.x * v.x + v.y * v.y + v.z * v.z) ;
		if(mag <= eps) mag = 1.0;
		s /= mag;
    v.x /= mag;
    v.y /= mag;
    v.z /= mag;
		if(fabs(s) < eps) s = 0.0;
    if(fabs(v.x) < eps) v.x = 0.0;
    if(fabs(v.y) < eps) v.y = 0.0;
    if(fabs(v.z) < eps) v.z = 0.0;
}

//-----------------------------------------------------------------------------
//�x�N�g��������Ԃ�
Vector3D CQuaternion::getVector()
{
    return Vector3D(v.x, v.y, v.z);
}
//-----------------------------------------------------------------------------
//�X�J������Ԃ�
double CQuaternion::getScalar()
{
    return s;
}
//-----------------------------------------------------------------------------
//�x�N�g��v���S����q�ŉ�]
Vector3D qvRotate(CQuaternion q, Vector3D v)
{
    CQuaternion p = q * v * (~q);
    return Vector3D(p.v.x, p.v.y, p.v.z);
}
//------------------------------------------------------------------------------
//�C�ӂ̉�]������S�������擾(axis�͊������W, angle[deg]�j
CQuaternion getQFromAxis(double angle, Vector3D axis)
{
	CQuaternion q0;
	if(fabs(angle) > 90.0) {//fabs(angle > 90.0)�ɑΉ�����悤�ɕύX�i2011.3.10)
		q0 = getQFromAxis(angle/2.0, axis);
		angle /= 2.0;
	}

	axis.norm();
	double a2 = (double)(angle * DEG_TO_RAD / 2.0);//rad�P�ʂŃ�/2
	if(a2 > 1.0) a2 = 1.0;
	if(a2 < -1.0) a2 = -1.0;
	return CQuaternion( cos(a2) , axis * sin(a2)) * q0;
}
//------------------------------------------------------------------------
CQuaternion getQFromEulerXYZ(Vector3D elr)
{//x,y,z�̏��Ԃŉ�]
	CQuaternion q;
	q = getQFromAxis(elr.z, Vector3D(0.0, 0.0, 1.0)) 
		* getQFromAxis(elr.y, Vector3D(0.0, 1.0, 0.0))
		* getQFromAxis(elr.x, Vector3D(1.0, 0.0, 0.0));
	return q;
}

//------------------------------------------------------------------------
CQuaternion getQFromEulerZYX(Vector3D elr)
{//z,y,x�̏��Ԃŉ�]
	CQuaternion q;
	q = getQFromAxis(elr.x, Vector3D(1.0, 0.0, 0.0)) 
		* getQFromAxis(elr.y, Vector3D(0.0, 1.0, 0.0))
		* getQFromAxis(elr.z, Vector3D(0.0, 0.0, 1.0));
	return q;
}

//---------------------------------------------------------------------------------------
//------------------------------------------------------
//����
double getRandom(double fMin, double fMax)
{//��l����
  return fMin + (fMax - fMin) * (double)rand() / (double)RAND_MAX;
}

//XY���ʂɂ�������ˏ�̈�l����(x���Cy���̊m���������j
Vector2D getRandomVectorXY(float rad)
{
	Vector2D vPos;
	double theta = getRandom(-M_PI, M_PI);
	vPos.x = (float)cos(theta) * getRandom(0.0, rad);
	vPos.y = (float)sin(theta) * getRandom(0.0, rad);
	return vPos;
}

//XY���ʂɂ�������ˏ�̈�l����
Vector3D getRandomVectorXY3(float rad)
{
	Vector3D vPos;
	//double r = getRandom(0.0, 1.0);
	double theta = getRandom(-M_PI, M_PI);
	vPos.x = (double)cos(theta) * getRandom(0.0, rad);
	vPos.y = (double)sin(theta) * getRandom(0.0, rad);
	return vPos;
}
//XY���ʂɂ�������ˏ�̈�l����(x���Cy���̊m���������j
Vector3D getRandomVectorXY2()
{
	Vector3D vPos;
	vPos.z = 0.0;
	double theta = getRandom(-M_PI, M_PI);
	vPos.x = (double)cos(theta) * getRandom(-1.0, 1.0);
	vPos.y = (double)sin(theta) * getRandom(-1.0, 1.0);
	return vPos;
}

//���K����
double getNormalRandom(double mean, double sigma)
{
	double ran = 0.0;
	for(int i = 0; i < 12; i++) ran += (double)rand() / (double)RAND_MAX;
	ran -= 6.0;
	ran *= sigma;
	return mean + ran;
}

//3D��Ԃɂ����鋅��̈�l����
//��Ԗ��x���قڈ��
Vector3D getRandomXYZ()
{
	Vector3D vPos;
	vPos.z = getRandom(-1.0, 1.0);
	double r = (double)sqrt(1.0 - vPos.z * vPos.z);
	double theta = getRandom(-M_PI, M_PI);
	vPos.x = (double)cos(theta) * r;
	vPos.y = (double)sin(theta) * r;
	return vPos;
}
//3D��Ԃɂ������ˏ�̗���
//���S�قǖ��x������
Vector3D getRandomVector()
{
	Vector3D vPos;
	Vector3D vDir = Vector3D(getRandom(-1.0, 1.0), getRandom(-1.0, 1.0), getRandom(-1.0, 1.0));
	double r = getRandom(0.0, 1.0);
	double len = vDir.magnitude();
	vPos = r * vDir / len;
	return vPos;
}

//�����O��ɕ��z���闐��(���S�قǖ��x�͍���)
Vector3D getRandomRingVectorXY(double minR, double maxR)
{
	Vector3D vPos;
	double r = getRandom(minR, maxR);
	double theta = getRandom(-M_PI, M_PI);
	vPos.x = (double)cos(theta) * r;
	vPos.y = (double)sin(theta) * r;
	return vPos;
}
