
varying vec3 P;//位置ベクトル
varying vec3 N;//法線ベクトル
varying vec3 T;//接線ベクトル
varying vec3 B;//従法線ベクトル
varying vec2 grad;
varying float pp;

void main(void)
{
	
	//光源ベクトル
	vec3 L = normalize(gl_LightSource[0].position.xyz - P);
	//新法線ベクトル
	N = normalize(N - grad.x * T - grad.y * B);

	vec4 ambient = gl_FrontLightProduct[0].ambient;
	float dotNL = dot(N, L);
	vec4 diffuse = (gl_FrontLightProduct[0].diffuse * max(0.0, dotNL) );// * col;
	vec3 V = normalize(-P);
	vec3 H = normalize(L + V);
	float powNH = pow(max(dot(N, H), 0.0), gl_FrontMaterial.shininess);
	if(dotNL <= 0.0) powNH = 0.0;
	vec4 specular = gl_FrontLightProduct[0].specular * powNH;
	
	gl_FragColor = ambient + diffuse + specular;
}
