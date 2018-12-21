

#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;
uniform float sizeX, sizeY;
uniform float height;//水面の高さ
uniform float nRatio;
uniform int flagInverse;

varying vec4 refCoord;
varying vec3 P;
varying vec3 N;

vec3 calcRefract(vec3 I, vec3 N, int flagInverse);

void main(void)
{	
	vec3 T;//接線ベクトル
	vec3 B;//従法線ベクトル
	vec2 grad;

	//テクスチャ座標の中心を移動
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));
	
	float p1, p2;
	float delta = 0.1;//差分間隔
	float pp = texture2DRect(samplerWave, texPos).b;//合成された変位量
	pp = clamp(pp, -0.5, 0.5);//表示上の振幅制限

	//勾配
	p1 = texture2DRect(samplerWave, texPos + vec2(-delta, 0.0)).b;
	p2 = texture2DRect(samplerWave, texPos + vec2( delta, 0.0)).b;
	grad.x = (p2 - p1) / (2.0 * delta);//x方向微分
	p1 = texture2DRect(samplerWave, texPos + vec2(0.0, -delta)).b;
	p2 = texture2DRect(samplerWave, texPos + vec2(0.0,  delta)).b;
	grad.y = (p2 - p1) / (2.0 * delta);//y方向微分

	//辺の長さをsizeX,sizeYに変換
 	gl_Vertex.x *= sizeX /  float(texWidth-1);
	gl_Vertex.y *= sizeY /  float(texHeight-1);

	gl_Vertex.z = height + pp;//物理量(表示上の高さを調整）

	P = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//ｘ方向接線ベクトル
	T = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)).xyz;
	//従法線ベクトル
	B = normalize(gl_NormalMatrix * vec3(0.0, 1.0, 0.0)).xyz;
	//新法線ベクトル
	N = normalize(N - grad.x * T - grad.y * B);

	//屈折ベクトルの計算
	vec3 incident = normalize(P);//入射ベクトル
	vec3 TT = calcRefract(incident, N, flagInverse) ;//その屈折ベクトル
	//vec3 TT;
	//if(flagInverse == 0) TT = refract(incident, N, 1.0/nRatio);
	//else TT = refract(incident, N, nRatio);
//	refCoord = ViewTranspose * vec4(TT, 0.0) ;//屈折ベクトルによるテクスチャ座標
	refCoord = gl_ModelViewMatrixTranspose * vec4(TT, 0.0) ;//屈折ベクトルによるテクスチャ座標
 
	gl_Position = ftransform();
}

//------------------------------------------------------------------------------
vec3 calcRefract(vec3 I, vec3 N, int flagInverse)
{//屈折ベクトルの計算

	if(flagInverse == 1){//視点が水面以下
		N = - N;
		nRatio = 1.0 / nRatio;
	}
	float cosIN = dot(-I, N);
	float a = nRatio * nRatio - 1.0 + cosIN * cosIN;
	//if(a < 0.0) return reflect(I, N);//全反射
	if(a < 0.0) return I + 2.0*cosIN * N;
	else 
	return (I - N *(sqrt(a) - cosIN)) / nRatio;
}
