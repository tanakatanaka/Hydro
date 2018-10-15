
uniform int color;
uniform int flagShading;

varying vec3 P;//位置ベクトル
varying vec3 N;//法線ベクトル
varying vec3 T;//接線ベクトル
varying vec3 B;//従法線ベクトル
varying vec2 grad;
varying float pp;

vec3 getColor1();
vec3 getColor2();

void main(void)
{
	vec4 col;
	
	if(color == 0) col.rgb = getColor1();//連続表示
	else if(color == 1) col.rgb = getColor2();//段差表示
	else col.rgb = vec3(0.8, 0.8, 0.8);//モノクロ
	col.a = 1.0;
	
	if(flagShading == 0) 
	{
		gl_FragColor = col; return;
	}

	//光源ベクトル
	vec3 L = normalize(gl_LightSource[0].position.xyz - P);
	//新法線ベクトル
	N = normalize(N - grad.x * T - grad.y * B);

	vec4 ambient = gl_FrontLightProduct[0].ambient * col;
	float dotNL = dot(N, L);
	vec4 diffuse = (gl_FrontLightProduct[0].diffuse * max(0.0, dotNL) ) * col;
	vec3 V = normalize(-P);
	vec3 H = normalize(L + V);
	float powNH = pow(max(dot(N, H), 0.0), gl_FrontMaterial.shininess);
	if(dotNL <= 0.0) powNH = 0.0;
	vec4 specular = gl_FrontLightProduct[0].specular * powNH;
	
	gl_FragColor = ambient + diffuse + specular;
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
	if(pp < 0.01)
	{
		col.r = 0.0; col.g = 0.0; col.b = 1.0;
	}
	
	else if(pp < 0.1)
	{
		col.r = 0.0; col.g = 0.2; col.b = 0.8;
	}
	else if(pp < 0.2)
	{
		col.r = 0.0; col.g = 0.4; col.b = 0.6;
	}
	else if(pp < 0.3)
	{
		col.r = 0.0; col.g = 0.6; col.b = 0.4;
	}
	else if(pp < 0.4)
	{
		col.r = 0.0; col.g = 0.8; col.b = 0.2;
	}
	else if(pp < 0.5)
	{
		col.r = 0.0; col.g = 1.0; col.b = 0.0;
	}
	else if(pp < 0.6)
	{
		col.r = 0.2; col.g = 0.8; col.b = 0.0;
	}
	else if(pp < 0.7)
	{
		col.r = 0.4; col.g = 0.6; col.b = 0.0;
	}
	
	else if(pp < 0.8)
	{
		col.r = 0.6; col.g = 0.4; col.b = 0.0;
	}
	else if(pp < 0.9)
	{
		col.r = 0.8; col.g = 0.2; col.b = 0.0;
	}
	else
	{
		col.r = 1.0; col.g = 0.0; col.b = 0.0;
	}
	
	return col;
}

