#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform sampler2DRect samplerWave;
uniform sampler2DRect samplerPosP;
uniform int texWidth, texHeight;
uniform float adjustH;
uniform float sizeX;
uniform float sizeY;
uniform float dt;
varying vec2 texPos;

void main(void)
{
	float pp = texture2DRect(samplerWave, texPos).b * adjustH;//水位の変動
	pp = clamp(pp, -0.5, 0.5);//表示上の振幅制限

	//粒子の位置座標計算
	vec3 posP;
	float deltaX = sizeX / float(texWidth-1);
	float deltaY = sizeY / float(texHeight-1);

	posP = texture2DRect(samplerPosP, texPos).xyz;//粒子の位置
	//その位置に最も近い格子点
	float I = (posP.x + sizeX/2.0) / deltaX;
	float J = (posP.y + sizeY/2.0) / deltaY;
	//格子点の流体速度
	float velX = texture2DRect(samplerVelX, vec2(I, J)).r;	
	float velY = texture2DRect(samplerVelY, vec2(I, J)).r;
	vec3 velP = vec3(velX, velY, 0.0);

	//粒子位置の更新
	posP.xy += velP.xy * dt;
	posP.z = texture2DRect(samplerPosP, vec2(I, J)).a;

	gl_FragColor = vec4(posP, pp);
}



