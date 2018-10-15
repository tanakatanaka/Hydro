
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerPsi;
uniform sampler2DRect samplerOmg;
uniform int texWidth, texHeight;
uniform float sizeX, sizeY;
uniform int kind;
uniform float adjustH;
uniform float maxOmega, maxPsi, maxVel;

varying vec3 P;
varying vec3 N;
varying vec3 T;//接線ベクトル
varying vec3 B;//従法線ベクトル
varying vec2 grad;
varying float pp;

//テクスチャ座標空間における画素位置の単位
float delta = 1.0;//差分間隔

void main(void)
{	
	//テクスチャ座標の中心を移動
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));
	//辺の長さをsizeX,sizeYに変換
 	gl_Vertex.x *= sizeX /  float(texWidth-1);
	gl_Vertex.y *= sizeY /  float(texHeight-1);
	
	vec2 vel;
	float p1, p2;
	if(kind == 0)//Omega
	{ 
		pp = texture2DRect(samplerOmg, texPos).r / maxOmega + 0.5;
		
		p1 = texture2DRect(samplerOmg, texPos + vec2(-delta, 0.0)).r;
		p2 = texture2DRect(samplerOmg, texPos + vec2( delta, 0.0)).r;
		grad.x = (p2 - p1) / (2.0 * delta);//x方向微分
		p1 = texture2DRect(samplerOmg, texPos + vec2(0.0, -delta)).r;
		p2 = texture2DRect(samplerOmg, texPos + vec2(0.0,  delta)).r;
		grad.y = (p2 - p1) / (2.0 * delta);//y方向微分
	}
	if(kind == 1)//Psi
	{
		pp = texture2DRect(samplerPsi, texPos).r  / maxPsi;
		p1 = texture2DRect(samplerPsi, texPos + vec2(-delta, 0.0)).r;
		p2 = texture2DRect(samplerPsi, texPos + vec2( delta, 0.0)).r;
		grad.x = (p2 - p1) / (2.0 * delta);//x方向微分
		p1 = texture2DRect(samplerPsi, texPos + vec2(0.0, -delta)).r;
		p2 = texture2DRect(samplerPsi, texPos + vec2(0.0,  delta)).r;
		grad.y = (p2 - p1) / (2.0 * delta);//y方向微分
	}
	if(kind == 2)//Velocity
	{
		vel.x = texture2DRect(samplerPsi, texPos).g;
		vel.y = texture2DRect(samplerPsi, texPos).b;
		pp = length(vel) / maxVel;
		p1 = texture2DRect(samplerPsi, texPos + vec2(-delta, 0.0)).g;
		p2 = texture2DRect(samplerPsi, texPos + vec2( delta, 0.0)).g;
		grad.x = (abs(p2) - abs(p1)) / (2.0 * delta);//x方向微分;
		p1 = texture2DRect(samplerPsi, texPos + vec2( 0.0,-delta)).b;
		p2 = texture2DRect(samplerPsi, texPos + vec2( 0.0, delta)).b;	
		grad.y = (abs(p2) - abs(p1)) / (2.0 * delta);//x方向微分;
		if(texPos.x == 0 || texPos.x == texWidth-1) pp = 0;
	}

	//物理量(表示上の高さを調整）
	//gl_Vertex.z = (pp - 0.5) * adjustH;
	gl_Vertex.z = pp * adjustH;
	
	P = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//ｘ方向接線ベクトル
	T = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)).xyz;
	//従法線ベクトル
	B = normalize(gl_NormalMatrix * vec3(0.0, 1.0, 0.0)).xyz;
		
	gl_Position = ftransform();
}

