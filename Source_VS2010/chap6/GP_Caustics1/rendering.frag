
uniform samplerCube smplRefract;
uniform float transparency;

varying vec4 refCoord;
varying vec3 P;//�ʒu�x�N�g��
varying vec3 N;//�@���x�N�g��

void main(void)
{
	//�����x�N�g��
	vec3 L = normalize(gl_LightSource[0].position.xyz - P);

	vec4 ambient = gl_FrontLightProduct[0].ambient;
	float dotNL = dot(N, L);
	vec4 diffuse = (gl_FrontLightProduct[0].diffuse * max(0.0, dotNL) );
	vec3 V = normalize(-P);
	vec3 H = normalize(L + V);
	float powNH = pow(max(dot(N, H), 0.0), gl_FrontMaterial.shininess);
	if(dotNL <= 0.0) powNH = 0.0;
	vec4 specular = gl_FrontLightProduct[0].specular * powNH;
	
	vec4 refColor = textureCube(smplRefract, refCoord.stp);
	gl_FragColor = mix(ambient + diffuse, refColor, transparency) + specular;
}
