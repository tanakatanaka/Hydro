
#define MAX_PARTICLE 50000
int numP0 = 50;//��x�ɕ��o���闱�q��
int countP = 0;//���o���ꂽ���q��
float countInterval = 0.0;

class Particle2D
{
public:
	Vector2D pos;//�ʒu���W(��͋�`�̈���W�n�j
	Vector2D vel;//���x
	float speedC;
	float size;      
	Color color;

	Particle2D();
	void update(float dt);
	void show(Vector2D pos0, Vector2D scale);
};

Particle2D::Particle2D()
{
	pos = Vector2D(1000.0, 1000.0);
	vel = Vector2D();
	size = 5.0;//�_�̑傫��
	speedC = 1.0;
	color = DARK_BLUE;
}

void Particle2D::update(float dt)
{
	pos += vel * dt * speedC;
}

void Particle2D::show(Vector2D pos0, Vector2D scale)
{
	if(color == BLACK) glColor3f(0.0, 0.0, 0.0);
	else if(color == RED) glColor3f(1.0, 0.0, 0.0);
	else if(color == GREEN) glColor3f(0.0, 1.0, 0.0);
	else if(color == BLUE) glColor3f(0.0, 0.0, 1.0);
	else if(color == DARK_RED) glColor3f(0.6, 0.0, 0.0);
	else if(color == DARK_GREEN) glColor3f(0.0, 0.6, 0.0);
	else if(color == DARK_BLUE) glColor3f(0.0, 0.0, 0.6);
	else if(color == YELLOW) glColor3f(1.0, 1.0, 0.0);
	else if(color == CYAN) glColor3f(0.0, 1.0, 1.0);
	else if(color == MAGENTA) glColor3f(1.0, 0.0, 1.0);
	else if(color == DARK_YELLOW) glColor3f(0.7, 0.7, 0.0);
	else if(color == DARK_CYAN) glColor3f(0.0, 0.7, 0.7);
	else if(color == DARK_MAGENTA) glColor3f(0.7, 0.0, 0.7);

	glPointSize(size);
	glBegin(GL_POINTS);
		glVertex2f(pos0.x + pos.x * scale.x, pos0.y + pos.y * scale.y);
	glEnd();
}
