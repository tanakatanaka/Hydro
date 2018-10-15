
//�A��1��������(Simultaneous linear equation)�̉�@
#define DIM  1001

class Sle
{
public:

	Sle();
	void Gauss(float a[][DIM], float b[],  int n);
	void Thomas(float a[], float b[], int n, int boundary);
private:
	void Pivoting(float a[][DIM], float b[], int n, int k);
};

Sle::Sle()
{
}

void Sle::Gauss(float a[][DIM], float b[],  int n)
{
    //���́Fa[i][j]=�W���s��(�v�Z��ω�),b[n]=�萔�x�N�g��, n=����(0����n-1)
    //�o�́Fb[i]=�������̉�(i=0,1,2,---,n-1)
    int i, j, k;
    float akk, p1, s;

    //�O�i����
    for (k = 0; k < n - 1; k++)
    {
        Pivoting(a, b, n, k);
        akk = a[k][k];
        for (i = k + 1; i < n; i++)
        {
            p1 = a[i][k] / akk;
            b[i] -= p1 * b[k];
            for (j = k; j < n; j++) a[i][j] -= p1 * a[k][j];
        }
    }

    //��ޑ��
    b[n - 1] /= a[n - 1][n - 1];
    for (i = n - 2; i >= 0; i--)
    {
        s = 0.0;
        for (j = i + 1; j < n; j++) s += a[i][j] * b[j];
        b[i] = (b[i] - s) / a[i][i];
    }
}
void Sle::Pivoting(float a[][DIM], float b[], int n, int k)
{
	float piv, ch;
	int i, j, kk;

	piv = 0.0f;//pivoting
	kk = k;
	for (i = k; i < n; i++)
	{
		if (piv < fabs(a[i][k]))
		{
			piv = fabs(a[i][k]);
			kk = i;
		}
	}
	if (piv == 0.0)
	{
		printf("�������Ƃ��ł��܂���(Gauss,Pivoting) \n");
		return;
	}
	if (kk != k)
	{
		for (j = k; j < n; j++)
		{
			ch = a[k][j]; a[k][j] = a[kk][j]; a[kk][j] = ch;
		}
		ch = b[k]; b[k] = b[kk]; b[kk] = ch;
	}
}
/*
void Sle::Thomas(float a[], float b[], int n)
{
	//a[3]:3�̌W��
    //���́Fa[3]=3�̊m�肵�Ă���W��,b[i]=�萔�x�N�g��, n=����(0����n�Cn���܂ށj
    //�o�́Fb[i]=�������̉�(0,1,2,...,n)
	float p[DIM], q[DIM];
	int i;

	//�O�i����
	p[0] = a[1];
	q[0] = b[0];//���E�l
	for(i = 1; i <= n; i++)
	{
		p[i] = a[1] - a[0] * a[2] / p[i-1];
		q[i] = b[i] - a[0] * q[i-1] / p[i-1];
	}
	b[n] = q[n] / p[n];
	//��ޑ��
	for(i = n-1; i >= 0; i--)
	{
		b[i] = (q[i] - a[2] * b[i+1]) / p[i];
	}
}
*/
void Sle::Thomas(float a[], float b[], int n, int boundary)
{
	//a[3]:3�̌W��
    //���́Fa[3]=3�̊m�肵�Ă���W��,b[n]=�萔�x�N�g��, n=����
    //�o�́Fb[n]=�������̉�(0,1,2,...,n)
	float p[DIM], q[DIM];
	int i;

	if(boundary == 0)//Dirichlet
	{
		//�O�i����
		p[0] = a[1];
		q[0] = 0.0;//���E�l
		for(i = 1; i <= n; i++)
		{
			p[i] = a[1] - a[0] * a[2] / p[i-1];
			q[i] = b[i] - a[0] * q[i-1] / p[i-1];
		}
		//b[n] = 0.0;//���E�l q[n] / p[n];
		//��ޑ��
		for(i = n-1; i >= 1; i--)
		{
			b[i] = (q[i] - a[2] * b[i+1]) / p[i];
		}
	}
	else//Neumann
	{
		//�O�i����
		p[1] = a[0] + a[1];
		q[1] = b[1];
		for(i = 2; i <= n-1; i++)
		{
			p[i] = a[1] - a[0] * a[2] / p[i-1];
			q[i] = b[i] - a[0] * q[i-1] / p[i-1];
		}
		b[n] = b[n-1];//���E�l
		//b[n-1] = q[n-1] / p[n-1];
		
		//��ޑ��
		for(i = n-1; i >= 1; i--)
		{
			b[i] = (q[i] - a[2] * b[i+1]) / p[i];
		}
		b[0] = b[1];// b[n] = b[n-1];
	}
}

