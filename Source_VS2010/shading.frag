
//Blinn-Phong Reflection model
varying vec3 P;
varying vec3 N;
uniform sampler2D image;
uniform int flagWire;
uniform int select;

void main(void)
{
	if(flagWire == 1) gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	else
	{
		vec3 L = normalize(gl_LightSource[0].position.xyz - P);
		N = normalize(N);

		vec4 ambient = gl_FrontLightProduct[0].ambient;
		float dotNL = dot(N, L);
		vec4 diffuse = gl_FrontLightProduct[0].diffuse * max(0.0, dotNL);
		vec3 V = normalize(-P);
		vec3 H = normalize(L + V);
		float powNH = pow(max(dot(N, H), 0.0), gl_FrontMaterial.shininess);
		if(dotNL <= 0.0) powNH = 0.0;
		vec4 specular = gl_FrontLightProduct[0].specular * powNH;
		//テクスチャの色
		vec4 texColor = texture2D(image, gl_TexCoord[0].st);
		if(select == 1)
		{
			if(texColor.r < 0.1 && texColor.g < 0.1 && texColor.b < 0.1) discard;//黒を透明
			gl_FragColor = (ambient + diffuse) * texColor + specular;
		}
		else//テクスチャなし
			gl_FragColor = ambient + diffuse + specular;
  }
}
