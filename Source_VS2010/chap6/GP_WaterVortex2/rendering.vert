
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform int texWidth, texHeight;
uniform float sizeX, sizeY;
uniform float adjustH;
uniform float  maxOmg, maxPrs;

varying vec3 P;
varying vec3 N;
varying vec3 T;//接線ベクトル
varying vec3 B;//従法線ベクトル
varying vec2 grad;
varying float pp;

void main(void)
{	
	//テクスチャ座標の中心を移動
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));
	
	vec2 vel;
	float p1, p2;
	float delta = 0.1;//差分間隔

	//液面変動
	pp = -abs(texture2DRect(samplerVelY, texPos).a) / maxOmg;//Omega
	pp += texture2DRect(samplerVelX, texPos).a / maxPrs ;//圧力成分も加える
		
	p1 = -abs(texture2DRect(samplerVelY, texPos + vec2(-delta, 0.0)).a);
	p2 = -abs(texture2DRect(samplerVelY, texPos + vec2( delta, 0.0)).a);
	grad.x = (p2 - p1) * adjustH / (2.0 * delta);//x方向微分
	p1 = -abs(texture2DRect(samplerVelY, texPos + vec2(0.0, -delta)).a);
	p2 = -abs(texture2DRect(samplerVelY, texPos + vec2(0.0,  delta)).a);
	grad.y = (p2 - p1) * adjustH / (2.0 * delta);//y方向微分

	//辺の長さをsizeX,sizeYに変換
 	gl_Vertex.x *= sizeX /  float(texWidth-1);
	gl_Vertex.y *= sizeY /  float(texHeight-1);

	gl_Vertex.z = pp * adjustH;//物理量(表示上の高さを調整）

	P = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//ｘ方向接線ベクトル
	T = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)).xyz;
	//従法線ベクトル
	B = normalize(gl_NormalMatrix * vec3(0.0, 1.0, 0.0)).xyz;
		
	gl_Position = ftransform();
}

