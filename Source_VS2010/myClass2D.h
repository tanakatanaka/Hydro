
#define M_PI 3.14159265358979323846
float DEG_TO_RAD = M_PI / 180.0;
float RAD_TO_DEG = 180.0 / M_PI;


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

//------------------------------------------------------------------------------
//一様乱数
float getRandom(float fMin, float fMax)
{
  return fMin + (fMax - fMin) * (float)rand() / (float)RAND_MAX;
}
//正規乱数
float getNormalRandom(float mean, float sigma)
{
  double ran = 0.0;
  for(int i=0; i < 12; i++) ran += (double)rand() / (double)RAND_MAX;
  ran -= 6.0;
  ran *= sigma;
  return mean + (float)ran;
}
//XY平面における放射状の一様乱数(x軸，y軸の確率が高い）
Vector2D getRandomVectorXY(float rad)
{
	Vector2D vPos;
	double theta = getRandom(-M_PI, M_PI);
	vPos.x = (float)cos(theta) * getRandom(-rad, rad);
	vPos.y = (float)sin(theta) * getRandom(-rad, rad);
	return vPos;
}

//-------------------------------------------------------------------------

//---------------------------------------------------------------------------
class Particle2D
{
public:
	Vector2D pos;//位置座標(解析矩形領域座標系）
	Vector2D vel;//速度
	float speedC;
	float size;      
	Particle2D();
	void update(float dt);
	void draw(Vector2D pos0, Vector2D scale);
};

Particle2D::Particle2D()
{
	pos = Vector2D(1000.0, 1000.0);
	vel = Vector2D();
	size = 10.0;//点の大きさ
}

void Particle2D::update(float dt)
{
	pos += vel * dt * speedC;
}

void Particle2D::draw(Vector2D pos0, Vector2D scale)
{
	glColor3f(0.0, 0.0, 0.5);
	glPointSize(size);
	glBegin(GL_POINTS);
		glVertex2f(pos0.x + pos.x * scale.x, pos0.y + pos.y * scale.y);
	glEnd();
}
