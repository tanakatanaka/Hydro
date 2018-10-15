
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;
uniform float sizeX, sizeY;
uniform int meshX, meshY;
uniform float adjustH;
uniform float  maxOmg, maxPrs;

varying vec3 P;
varying vec3 N;
varying vec3 T;//接線ベクトル
varying vec3 B;//従法線ベクトル
varying vec2 grad;

void main(void)
{	
	//テクスチャ座標の中心を移動
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));

	float p1, p2;
	float delta = 0.1;//差分間隔
	float pp = texture2DRect(samplerWave, texPos).b;//合成された変位量

	//勾配
	p1 = texture2DRect(samplerWave, texPos + vec2(-delta, 0.0)).b;
	p2 = texture2DRect(samplerWave, texPos + vec2( delta, 0.0)).b;
	grad.x = (p2 - p1) * adjustH / (2.0 * delta);//x方向微分
	p1 = texture2DRect(samplerWave, texPos + vec2(0.0, -delta)).b;
	p2 = texture2DRect(samplerWave, texPos + vec2(0.0,  delta)).b;
	grad.y = (p2 - p1) * adjustH / (2.0 * delta);//y方向微分

	//辺の長さをsizeX,sizeYに変換
 	gl_Vertex.x *= sizeX /  float(meshX);
	gl_Vertex.y *= sizeY /  float(meshY);

	gl_Vertex.z = pp * adjustH;//物理量(表示上の高さを調整）

	P = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//ｘ方向接線ベクトル
	T = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)).xyz;
	//従法線ベクトル
	B = normalize(gl_NormalMatrix * vec3(0.0, 1.0, 0.0)).xyz;
		
	gl_Position = ftransform();
}

