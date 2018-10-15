#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;//ダミーを含む全格子数
uniform int nDummy;
uniform int meshX, meshY;
uniform float sizeX, sizeY;
varying vec3 P;
varying vec3 N;
varying vec3 T;//接線ベクトル
varying vec3 B;//従法線ベクトル
varying vec2 grad;

//テクスチャ座標空間における画素位置の単位
float delta = 0.1;//差分間隔

void main(void)
{
	//テクスチャ座標の中心を移動
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));

	//位置座標
	vec4 wave = texture2DRect(samplerWave, texPos);
	//x方向微分
	vec4 wave1 =  texture2DRect(samplerWave, texPos+vec2(-delta, 0.0));
	vec4 wave2 =  texture2DRect(samplerWave, texPos+vec2( delta, 0.0));
	grad.x = (wave2.g - wave1.g) / (2.0 * delta);
	//y方向微分
	wave1 =  texture2DRect(samplerWave, texPos+vec2(0.0,-delta));
	wave2 =  texture2DRect(samplerWave, texPos+vec2(0.0, delta));
	grad.y =  (wave2.g - wave1.g) / (2.0 * delta);
	//1辺の長さをsize0に変換
 	gl_Vertex.x *= sizeX /  float(meshX);
	gl_Vertex.y *= sizeY /  float(meshY);
	gl_Vertex.z = wave.g;//変位量

	P = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//ｘ方向接線ベクトル
	T = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)).xyz;
	//従法線ベクトル
	B = normalize(gl_NormalMatrix * vec3(0.0, 1.0, 0.0)).xyz;
		
	gl_Position = ftransform();
}
