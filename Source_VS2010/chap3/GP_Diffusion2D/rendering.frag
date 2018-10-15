
uniform int color;

varying float pp;

vec3 getColor1();
vec3 getColor2();

void main(void)
{
	vec4 col;
	
	if(color == 0) col.rgb = getColor1();//連続表示
	else if(color == 1) col.rgb = getColor2();//段階表示
	col.a = 1.0;

	gl_FragColor = col;
}

vec3 getColor1()
{
	vec3 col;
	if(pp < 0.5)
	{
		col.r = 0.0; col.g = 2.0 * pp; col.b = 1.0 - 2.0 * pp;
	}
	else
	{
		col.r = 2.0 * pp - 1.0; col.g = 2.0 * (1.0 - pp); col.b = 0.0;
	}
	
	return col;
}

vec3 getColor2()
{
	vec3 col;
	if(pp <= 0.01)
	{
		col.r = 0.0; col.g = 0.0; col.b = 1.0;
	}
	
	else if(pp <= 0.1)
	{
		col.r = 0.0; col.g = 0.2; col.b = 0.8;
	}
	else if(pp <= 0.2)
	{
		col.r = 0.0; col.g = 0.4; col.b = 0.6;
	}
	else if(pp <= 0.3)
	{
		col.r = 0.0; col.g = 0.6; col.b = 0.4;
	}
	else if(pp <= 0.4)
	{
		col.r = 0.0; col.g = 0.8; col.b = 0.2;
	}
	else if(pp <= 0.5)
	{
		col.r = 0.0; col.g = 1.0; col.b = 0.0;
	}
	else if(pp <= 0.6)
	{
		col.r = 0.2; col.g = 0.8; col.b = 0.0;
	}
	else if(pp <= 0.7)
	{
		col.r = 0.4; col.g = 0.6; col.b = 0.0;
	}
	
	else if(pp <= 0.8)
	{
		col.r = 0.6; col.g = 0.4; col.b = 0.0;
	}
	else if(pp <= 0.9)
	{
		col.r = 0.8; col.g = 0.2; col.b = 0.0;
	}
	else
	{
		col.r = 1.0; col.g = 0.0; col.b = 0.0;
	}
	
	return col;
}

