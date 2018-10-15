//myMath6.h
//グラフィックス用数学ライブラリ
#define M_PI 3.14159265358979323846
double DEG_TO_RAD = M_PI / 180.0;
double RAD_TO_DEG = 180.0 / M_PI;
//--------------------------------------------------------------------------
class Vector2D
{
public:
	float x, y;
	//コンストラクタ
	Vector2D();
	Vector2D(float x0, float y0);
	//演算子
	void operator+=(Vector2D a);//加算
	void operator-=(Vector2D a);//減算
	void operator*=(float s);//スカラ乗算
	void operator/=(float s);//スカラ除算
	friend Vector2D operator*(Vector2D a, float s);//スカラ乗算
	friend Vector2D operator*(float s, Vector2D a);//スカラ乗算
	friend Vector2D operator/(Vector2D a, float s);//スカラ除算
	friend Vector2D operator*(Vector2D a, Vector2D b);//成分どうしの乗算
	friend Vector2D operator+(Vector2D a, Vector2D b);//加算
	friend Vector2D operator-(Vector2D a, Vector2D b);//減算
	//メンバ関数
	float magnitude();  //大きさ
	float magnitude2();  //大きさの2乗
	float vDist(Vector2D a, Vector2D b);//xy平面内の距離
	float vDist2(Vector2D a, Vector2D b);//xy平面内の2乗距離
	void rotZ_deg(float angle); //z軸回転
	void rotZ_rad(float angle); //z軸回転
};
//コンストラクタ
Vector2D::Vector2D()
{
    x = y = 0.0;
}
//コンストラクタ
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
//スカラ乗算
Vector2D operator*(Vector2D a, float s)
{
    return Vector2D(a.x * s, a.y * s);
}
//スカラ乗算
Vector2D operator*(float s, Vector2D a)
{
    return Vector2D(a.x * s, a.y * s);
}
//スカラ除算
Vector2D operator/(Vector2D a, float s)
{
    if(s == 0.0)
			printf("スカラ除算の分母が０です！ \n");

    return Vector2D(a.x / s, a.y / s);
}
//成分どうしの乗算
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
//ベクトル間距離(xy平面）
float vDist(Vector2D a, Vector2D b)
{
    Vector2D c = b - a;
    return( c.magnitude() );
}
//ベクトル間二乗距離
float vDist2(Vector2D a, Vector2D b)
{
    Vector2D c = b - a;
    return ( c.x * c.x + c.y * c.y);
}

//----------------------------------------------------------
//3次元ベクトル演算
class Vector3D{

public:
//メンバ変数
	double x, y, z;
	//コンストラクタ
	Vector3D();
	Vector3D(double x0, double y0, double z0);
	//演算子
	void operator+=(Vector3D a);//加算
	void operator-=(Vector3D a);//減算
	void operator*=(double s);//スカラ乗算
	void operator/=(double s);//スカラ除算
	friend Vector3D operator*(Vector3D a, double s);//スカラ乗算
	friend Vector3D operator*(double s, Vector3D a);//スカラ乗算
	friend Vector3D operator/(Vector3D a, double s);//スカラ除算
	friend Vector3D operator+(Vector3D a, Vector3D b);//加算
	friend Vector3D operator-(Vector3D a, Vector3D b);//減算
	friend Vector3D operator>>(Vector3D a, Vector3D b);//aからbへ向かう単位ベクトル
	//メンバ関数
	double magnitude();  //大きさ
	double magnitudeXY();//大きさ(z成分を無視)
	double magnitude2();  //大きさの2乗
	double magnitudeXY2();//大きさの2乗(z成分を無視)
	void norm(void);//大きさ1のベクトルに変換
	void reverse(void);//反転
	double vDist(Vector3D a, Vector3D b);//距離
	double vDistXY(Vector3D a, Vector3D b);//xy平面内の距離
	double vDist2(Vector3D a, Vector3D b);//2乗距離
	Vector3D reverse2();//反転したベクトルを返す
	Vector3D norm2();//正規化したベクトルを返す
	Vector3D normXY2();//正規化したベクトルを返す(z成分を無視)
	Vector3D normYZ2();//正規化したベクトルを返す(x成分を無視)
	Vector3D normXZ2();//正規化したベクトルを返す(y成分を無視)
	double vDot(Vector3D a, Vector3D b);//内積
	Vector3D vCross(Vector3D a, Vector3D b);//外積
	Vector3D rotate(Vector3D v, Vector3D elr);//オイラー角による回転
	Vector3D rotate(Vector3D v, double angle[3]);//角度配列による回転
	Vector3D rotateReverse(Vector3D v, Vector3D elr);//オイラー角による逆順で負の回転
	void scale(Vector3D vSize);//ｽｹｰﾘﾝｸﾞ
	void rotX(double angle); //x軸回転(deg)
	void rotY(double angle); //y軸回転
	void rotZ(double angle); //z軸回転
	void rotX_rad(double angle); //x軸回転(rad)
	void rotY_rad(double angle); //y軸回転
	void rotZ_rad(double angle); //z軸回転
	void rotX_rad(Vector3D vPos0, double angle); //x軸回転(rad)
	void rotY_rad(Vector3D vPos0, double angle); //y軸回転
	void rotZ_rad(Vector3D vPos0, double angle); //z軸回転
	double getAngle(Vector3D a, Vector3D b);//２つのベクトルのなす角度
	Vector3D getEulerX(Vector3D a, Vector3D b);//aからｂへ向かうベクトルのオイラー角
	Vector3D getEulerZ(Vector3D a, Vector3D b);//aからｂへ向かうベクトルのオイラー角
	Vector3D getEulerX(Vector3D d);//方向ベクトルdのオイラー角
	Vector3D getEulerZ(Vector3D d);//方向ベクトルdのオイラー角
};
//----------------------------------------------------------------------------
//コンストラクタ
Vector3D::Vector3D()
{
    x = y = z = 0.0;
}
//----------------------------------------------------------------------------
//コンストラクタ
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
//スカラ乗算
Vector3D operator*(Vector3D a, double s)
{
    return Vector3D(a.x * s, a.y * s, a.z * s);
}
//------------------------------------------------------------------------------
//スカラ乗算
Vector3D operator*(double s, Vector3D a)
{
    return Vector3D(a.x * s, a.y * s, a.z * s);
}
//------------------------------------------------------------------------------
//スカラ除算
Vector3D operator/(Vector3D a, double s)
{
    if(s == 0.0)
			printf("スカラ除算の分母が０です！ \n");

    return Vector3D(a.x / s, a.y / s, a.z / s);
}

//------------------------------------------------------------------------------
//aからbへ向かう単位ベクトル
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
//内積
double vDot(Vector3D a, Vector3D b)
{
    return( a.x*b.x + a.y*b.y + a.z*b.z );
}
//------------------------------------------------------------------------------
//外積
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
	//vPos0を中心とした回転
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
    //逆順で負の回転
    v.rotZ(-elr.z);
    v.rotY(-elr.y);
    v.rotX(-elr.x);
    return v;
}
//------------------------------------------------------------------------------
//ベクトル間距離
double vDist(Vector3D a, Vector3D b)
{
    Vector3D c = b - a;
    return( c.magnitude() );
}
//------------------------------------------------------------------------------
//ベクトル間距離(xy平面）
double vDistXY(Vector3D a, Vector3D b)
{
    Vector3D c = b - a;
    return( c.magnitudeXY() );
}
//------------------------------------------------------------------------------
//ベクトル間二乗距離
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
    return ang;//rad単位で返す
}
//-----------------------------------------------------------------------------
Vector3D getEulerX(Vector3D a, Vector3D b)
{//基本姿勢で中心軸がｘ軸方向である棒状オブジェクトのオイラー角(deg)
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
{//基本姿勢で中心軸がｘ軸方向である棒状オブジェクトのオイラー角(deg)
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
{//基本姿勢で中心軸がz軸方向である棒状オブジェクトのオイラー角(deg)
		//2011.3.14改良
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
{//基本姿勢で中心軸がz軸方向である棒状オブジェクトのオイラー角(deg)
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
//3*3のマトリクス
//------------------------------------------------------------------------------
class CMatrix{

public:
    //メンバ変数
    double e11, e12, e13, e21, e22, e23, e31, e32, e33;
    //コンストラクタ
    CMatrix();
    CMatrix(double a11, double a12, double a13,
            double a21, double a22, double a23,
            double a31, double a32, double a33);
    //演算子
    friend Vector3D operator*(CMatrix m, Vector3D v);
    friend Vector3D operator*(Vector3D v, CMatrix m);
    //メンバ関数
    double det();
    CMatrix inverse();
};
//----------------------------------------------------------------------------
//コンストラクタ
CMatrix::CMatrix()
{
    e11 = e12 = e13 = 0.0;
    e21 = e22 = e23 = 0.0;
    e31 = e32 = e33 = 0.0;
}
//-----------------------------------------------------------------------------
//コンストラクタ
CMatrix::CMatrix(double a11, double a12, double a13,
                 double a21, double a22, double a23,
                 double a31, double a32, double a33)
{
    e11 = a11; e12 = a12; e13 = a13;
    e21 = a21; e22 = a22; e23 = a23;
    e31 = a31; e32 = a32; e33 = a33;
}
//----------------------------------------------------------------------------
//行列式
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
			 printf("逆行列を求めることができません！ \n");
       d = 1.0;
    }
    return CMatrix(
        (e22*e33-e23*e32)/d, -(e12*e33-e13*e32)/d,  (e12*e23-e13*e22)/d,
       -(e21*e33-e23*e31)/d,  (e11*e33-e13*e31)/d, -(e11*e23-e13*e21)/d,
        (e21*e32-e22*e31)/d, -(e11*e32-e12*e31)/d,  (e11*e22-e12*e21)/d );
}
//----------------------------------------------------------------------------
//ベクトル乗算
Vector3D operator*(CMatrix m, Vector3D v)
{
    return Vector3D(
        m.e11*v.x + m.e12*v.y + m.e13*v.z,    //x成分
        m.e21*v.x + m.e22*v.y + m.e23*v.z,    //y成分
        m.e31*v.x + m.e32*v.y + m.e33*v.z );  //z成分
}
//----------------------------------------------------------------------------
//ベクトル乗算
Vector3D operator*(Vector3D v, CMatrix m)
{
    return Vector3D(
        v.x*m.e11 + v.y*m.e21 + v.z*m.e31,
        v.x*m.e12 + v.y*m.e22 + v.z*m.e32,
        v.x*m.e13 + v.y*m.e23 + v.z*m.e33 );
}

//----------------------------------------------------------------------------
//四元数：quaternion
// 定義式 q = s + xi + yj + zk;
//----------------------------------------------------------------------------
class CQuaternion{

public:
	//メンバ変数
	double s;//スカラ値
	Vector3D v;//ベクトル
	//コンストラクタ
	CQuaternion();
	CQuaternion(double a0, double a1, double a2, double a3);
	CQuaternion(double a0, Vector3D v);

	//演算子
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
	//メンバ関数
	double magnitude(void);
	void norm(void);//大きさ1のquaternionに変換
	Vector3D getVector();
	double getScalar();
	Vector3D qvRotate(CQuaternion q, Vector3D v);
	CQuaternion getQFromEulerXYZ(Vector3D elr);
	CQuaternion getQFromEulerZYX(Vector3D elr);
	CQuaternion getQFromAxis(double angle, Vector3D axis);
};
//-----------------------------------------------------------------------------
//コンストラクタ
CQuaternion::CQuaternion()
{
    s = 1.0;
    v.x = v.y = v.z = 0.0;
}
//-----------------------------------------------------------------------------
//コンストラクタ
CQuaternion::CQuaternion(double a0, double a1, double a2, double a3)
{
    s = a0;
    v.x = a1;
    v.y = a2;
    v.z = a3;
}
//-----------------------------------------------------------------------------
//コンストラクタ
CQuaternion::CQuaternion(double a0, Vector3D u)
{
    s = a0;
		v = u;
}
//-----------------------------------------------------------------------------
//共役四元数
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
//四元数どうしの和
CQuaternion operator+(CQuaternion p, CQuaternion q)
{
    return CQuaternion( p.s + q.s,
           p.v.x + q.v.x, p.v.y + q.v.y, p.v.z + q.v.z);
}
//------------------------------------------------------------------------------
//四元数どうしの差
CQuaternion operator-(CQuaternion p, CQuaternion q)
{
    return CQuaternion( p.s - q.s,
         p.v.x - q.v.x, p.v.y - q.v.y, p.v.z - q.v.z);
}
//------------------------------------------------------------------------------
//四元数どうしの乗算
CQuaternion operator*(CQuaternion p, CQuaternion q)
{
    CQuaternion qq;
	qq.s = p.s * q.s - vDot(p.v , q.v) ; //２項目はベクトル内積
	qq.v = p.s * q.v + q.s * p.v + vCross(p.v , q.v); //３項目はベクトル外積
    return qq;
}
//-----------------------------------------------------------------------------
//スカラ乗算
CQuaternion operator*(CQuaternion p, double a)
{
    return CQuaternion(p.s * a, p.v.x * a, p.v.y * a, p.v.z * a);
}
//-----------------------------------------------------------------------------
//スカラ乗算
CQuaternion operator*(double a, CQuaternion p)
{
    return CQuaternion(p.s * a, p.v.x * a, p.v.y * a, p.v.z * a);
}
//------------------------------------------------------------------------------
//ベクトルと四元数の乗算
CQuaternion operator*(Vector3D v, CQuaternion q)
{
    CQuaternion p, pp;
    p.s = 0.0; p.v = v; //スカラ部が0のクオータニオン
    pp = p * q;
    return pp; 
}
//------------------------------------------------------------------------------
//ベクトルと四元数の乗算
CQuaternion operator*(CQuaternion p, Vector3D v)
{
    CQuaternion q, pp;
    q.s = 0.0; q.v = v;
    pp = p * q;
    return pp;
}

//-----------------------------------------------------------------------------
//大きさを返す
double CQuaternion::magnitude()
{
    return sqrt( s * s + v.x * v.x + v.y * v.y + v.z * v.z);
}
//-----------------------------------------------------------------------------
//正規化
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
//ベクトル部分を返す
Vector3D CQuaternion::getVector()
{
    return Vector3D(v.x, v.y, v.z);
}
//-----------------------------------------------------------------------------
//スカラ部を返す
double CQuaternion::getScalar()
{
    return s;
}
//-----------------------------------------------------------------------------
//ベクトルvを４元数qで回転
Vector3D qvRotate(CQuaternion q, Vector3D v)
{
    CQuaternion p = q * v * (~q);
    return Vector3D(p.v.x, p.v.y, p.v.z);
}
//------------------------------------------------------------------------------
//任意の回転軸から４元数を取得(axisは慣性座標, angle[deg]）
CQuaternion getQFromAxis(double angle, Vector3D axis)
{
	CQuaternion q0;
	if(fabs(angle) > 90.0) {//fabs(angle > 90.0)に対応するように変更（2011.3.10)
		q0 = getQFromAxis(angle/2.0, axis);
		angle /= 2.0;
	}

	axis.norm();
	double a2 = (double)(angle * DEG_TO_RAD / 2.0);//rad単位でθ/2
	if(a2 > 1.0) a2 = 1.0;
	if(a2 < -1.0) a2 = -1.0;
	return CQuaternion( cos(a2) , axis * sin(a2)) * q0;
}
//------------------------------------------------------------------------
CQuaternion getQFromEulerXYZ(Vector3D elr)
{//x,y,zの順番で回転
	CQuaternion q;
	q = getQFromAxis(elr.z, Vector3D(0.0, 0.0, 1.0)) 
		* getQFromAxis(elr.y, Vector3D(0.0, 1.0, 0.0))
		* getQFromAxis(elr.x, Vector3D(1.0, 0.0, 0.0));
	return q;
}

//------------------------------------------------------------------------
CQuaternion getQFromEulerZYX(Vector3D elr)
{//z,y,xの順番で回転
	CQuaternion q;
	q = getQFromAxis(elr.x, Vector3D(1.0, 0.0, 0.0)) 
		* getQFromAxis(elr.y, Vector3D(0.0, 1.0, 0.0))
		* getQFromAxis(elr.z, Vector3D(0.0, 0.0, 1.0));
	return q;
}

//---------------------------------------------------------------------------------------
//------------------------------------------------------
//乱数
double getRandom(double fMin, double fMax)
{//一様乱数
  return fMin + (fMax - fMin) * (double)rand() / (double)RAND_MAX;
}

//XY平面における放射状の一様乱数(x軸，y軸の確率が高い）
Vector2D getRandomVectorXY(float rad)
{
	Vector2D vPos;
	double theta = getRandom(-M_PI, M_PI);
	vPos.x = (float)cos(theta) * getRandom(0.0, rad);
	vPos.y = (float)sin(theta) * getRandom(0.0, rad);
	return vPos;
}

//XY平面における放射状の一様乱数
Vector3D getRandomVectorXY3(float rad)
{
	Vector3D vPos;
	//double r = getRandom(0.0, 1.0);
	double theta = getRandom(-M_PI, M_PI);
	vPos.x = (double)cos(theta) * getRandom(0.0, rad);
	vPos.y = (double)sin(theta) * getRandom(0.0, rad);
	return vPos;
}
//XY平面における放射状の一様乱数(x軸，y軸の確率が高い）
Vector3D getRandomVectorXY2()
{
	Vector3D vPos;
	vPos.z = 0.0;
	double theta = getRandom(-M_PI, M_PI);
	vPos.x = (double)cos(theta) * getRandom(-1.0, 1.0);
	vPos.y = (double)sin(theta) * getRandom(-1.0, 1.0);
	return vPos;
}

//正規乱数
double getNormalRandom(double mean, double sigma)
{
	double ran = 0.0;
	for(int i = 0; i < 12; i++) ran += (double)rand() / (double)RAND_MAX;
	ran -= 6.0;
	ran *= sigma;
	return mean + ran;
}

//3D空間における球状の一様乱数
//空間密度がほぼ一定
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
//3D空間におけ放射状の乱数
//中心ほど密度が高い
Vector3D getRandomVector()
{
	Vector3D vPos;
	Vector3D vDir = Vector3D(getRandom(-1.0, 1.0), getRandom(-1.0, 1.0), getRandom(-1.0, 1.0));
	double r = getRandom(0.0, 1.0);
	double len = vDir.magnitude();
	vPos = r * vDir / len;
	return vPos;
}

//リング状に分布する乱数(中心ほど密度は高い)
Vector3D getRandomRingVectorXY(double minR, double maxR)
{
	Vector3D vPos;
	double r = getRandom(minR, maxR);
	double theta = getRandom(-M_PI, M_PI);
	vPos.x = (double)cos(theta) * r;
	vPos.y = (double)sin(theta) * r;
	return vPos;
}
