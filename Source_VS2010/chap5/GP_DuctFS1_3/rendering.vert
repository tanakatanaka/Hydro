
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform int texWidth, texHeight;
uniform float sizeX, sizeY;
uniform int kind;
uniform float adjustH;
uniform float  maxPrs, maxOmg, maxVel;

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
	
	vec2 vel;
	
	float p1, p2;
	if(kind == 0)//Prs
	{
		pp = texture2DRect(samplerVelX, texPos).a  / maxPrs + 0.5;
		p1 = texture2DRect(samplerVelX, texPos + vec2(-delta, 0.0)).a;
		p2 = texture2DRect(samplerVelX, texPos + vec2( delta, 0.0)).a;
		grad.x = (p2 - p1) / (2.0 * delta);//x方向微分
		p1 = texture2DRect(samplerVelX, texPos + vec2(0.0, -delta)).a;
		p2 = texture2DRect(samplerVelX, texPos + vec2(0.0,  delta)).a;
		grad.y = (p2 - p1) / (2.0 * delta);//y方向微分
	}
	if(kind == 1)//Omega
	{
		pp = texture2DRect(samplerVelY, texPos).a  / maxOmg + 0.5;	
		p1 = texture2DRect(samplerVelY, texPos + vec2(-delta, 0.0)).a;
		p2 = texture2DRect(samplerVelY, texPos + vec2( delta, 0.0)).a;
		grad.x = (p2 - p1) / (2.0 * delta);//x方向微分
		p1 = texture2DRect(samplerVelY, texPos + vec2(0.0, -delta)).a;
		p2 = texture2DRect(samplerVelY, texPos + vec2(0.0,  delta)).a;
		grad.y = (p2 - p1) / (2.0 * delta);//y方向微分
	}
	if(kind == 2)//Velocity
	{
		vel.x = texture2DRect(samplerVelX, texPos).r;
		vel.y = texture2DRect(samplerVelY, texPos).r;
		pp = length(vel) / maxVel;
		vel.x = texture2DRect(samplerVelX, texPos + vec2(-delta, 0.0)).r;
		vel.y = texture2DRect(samplerVelY, texPos + vec2(-delta, 0.0)).r;
		p1 = length(vel) / maxVel;
		vel.x = texture2DRect(samplerVelX, texPos + vec2( delta, 0.0)).r;
		vel.y = texture2DRect(samplerVelY, texPos + vec2( delta, 0.0)).r;
		p2 = length(vel) / maxVel;
		grad.x = (abs(p2) - abs(p1)) / (2.0 * delta);//x方向微分;
		vel.x = texture2DRect(samplerVelX, texPos + vec2( 0.0, -delta)).r;
		vel.y = texture2DRect(samplerVelY, texPos + vec2( 0.0, -delta)).r;
		p1 = length(vel) / maxVel;
		vel.x = texture2DRect(samplerVelX, texPos + vec2( 0.0, delta)).r;
		vel.y = texture2DRect(samplerVelY, texPos + vec2( 0.0, delta)).r;
		p2 = length(vel) / maxVel;
		grad.y = (abs(p2) - abs(p1)) / (2.0 * delta);//y方向微分;
	}
	
	//辺の長さをsizeX,sizeYに変換
 	gl_Vertex.x *= sizeX /  float(texWidth-1);
	gl_Vertex.y *= sizeY /  float(texHeight-1);

	gl_Vertex.z = (pp - 0.5) * adjustH;//物理量(表示上の高さを調整）

	P = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//ｘ方向接線ベクトル
	T = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)).xyz;
	//従法線ベクトル
	B = normalize(gl_NormalMatrix * vec3(0.0, 1.0, 0.0)).xyz;
		
	gl_Position = ftransform();
}

