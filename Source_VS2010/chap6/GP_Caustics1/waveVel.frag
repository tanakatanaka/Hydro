
/*
const float pi = 3.14159265;
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;
uniform float DX, DY;//格子間隔
//uniform float adjustH;
uniform float speed;
uniform float freq;
uniform float amp;
uniform float drag;
uniform float deltaT;
uniform float time;
uniform vec3 posObs; //浮動物体の位置
uniform vec3 velObs; //その速度
uniform vec3 sizeObs;//そのサイズ

varying vec2 texPos;

float makeWave(float v0);

void main(void)
{
	vec4 wave = texture2DRect(samplerWave, texPos);
	
	wave.r = makeWave(wave.r);
	float accel = 0.0;
	float dx2 = DX * DX;//格子間隔2乗	
	float dy2 = DY * DY;//格子間隔2乗
	int im, ip, jm, jp;
	float zmi, zpi, zmj, zpj;
	float coef;

    int i = int(texPos.x);
    int j = int(texPos.y);
	
	im = i-1; ip = i+1; jm = j-1; jp = j+1;
	if(im < 0) zmi = 0.0;
	else zmi = texture2DRect(samplerWave, texPos + vec2(-1.0, 0.0)).g;
	if(ip > texWidth) zpi = 0.0;
	else zpi = texture2DRect(samplerWave, texPos + vec2( 1.0, 0.0)).g;
	if(jm < 0) zmj = 0.0;
	else zmj = texture2DRect(samplerWave, texPos + vec2( 0.0,-1.0)).g;
	if(jp > texHeight) zpj = 0.0;
	else zpj = texture2DRect(samplerWave, texPos + vec2( 0.0, 1.0)).g;

	if(im < 0 || jm < 0 || ip > texWidth || jp > texWidth) coef = 1.5;
	else coef = 2.0;

	accel = (speed * speed) * ((zmi + zpi - coef * wave.g) / dx2
	                             + (zmj + zpj - coef * wave.g) / dy2);

	//粘性抵抗
	accel -= drag * wave.r;
	//速度の更新
	wave.r += accel * deltaT;

	gl_FragColor = wave;
}

//--------------------------------------------------------
float makeWave(float v0)
{
	vec2 pos = texPos.xy - vec2(float(texWidth/2), float(texHeight/2));
	//サイズ変換
	pos.x *= DX;//波の注目点位置
	pos.y *= DY;

	float value = v0;
		
	float R = 1.2 * sizeObs.x / 2.0;
	//前方方向
	vec2 dir = normalize(vec2(velObs.x, velObs.y));
	vec2 posW = posObs + dir * R;//振動源を移動物体の前方に置く

	if(pos.x > posW.x-DX/2.0 &&  pos.x < posW.x+DX/2.0 && pos.y > posW.y-DY/2.0 && pos.y < posW.y+DY/2.0)
	{
		value = sin(2.0 * pi * freq * time);		
	}

	return value;
}
*/

#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;//ダミーを含むX方向全格子数
//uniform float sizeX, sizeY;//実際の1辺の長さ(ダミーを含まず）
uniform float dx, dy;
uniform float amp;
uniform float freq;
uniform float speed;
uniform float drag;
uniform float dt;
uniform float time;
uniform vec3 posObs; //浮動物体の位置
uniform vec3 velObs; //その速度
uniform vec3 sizeObs;//そのサイズ

varying vec2 texPos;

float makeWave2(float v0);

void main(void)
{
	vec4 wave = texture2DRect(samplerWave, texPos);
	wave.r = makeWave2(wave.r);//移動障害物による波を作成(ｚ軸方向速度）

    int i = int(texPos.x);
    int j = int(texPos.y);
	
	int im, ip, jm, jp;
	float zmi, zpi, zmj, zpj;
	float c1, c2;
	im = i-1; ip = i+1; jm = j-1; jp = j+1;
	if(im < 0) zmi = 0.0;
	else zmi = texture2DRect(samplerWave, texPos + vec2(-1.0, 0.0)).g;
	if(ip > texWidth) zpi = 0.0;
	else zpi = texture2DRect(samplerWave, texPos + vec2( 1.0, 0.0)).g;
	if(jm < 0) zmj = 0.0;
	else zmj = texture2DRect(samplerWave, texPos + vec2( 0.0,-1.0)).g;
	if(jp > texHeight) zpj = 0.0;
	else zpj = texture2DRect(samplerWave, texPos + vec2( 0.0, 1.0)).g;

	if(im < 0 || ip > texWidth) c1 = 1.5;
	else c1 = 2.0;
	if(jm < 0 || jp > texHeight) c2 = 1.5;
	else c2 = 2.0;

	float accel = (speed * speed) * ((zmi + zpi - c1 * wave.g) / (dx*dx)
	                             + (zmj + zpj - c2 * wave.g) / (dy*dy));


	//粘性抵抗
	accel -= drag * wave.r;
	//速度の更新
	wave.r += accel * dt;

	gl_FragColor = wave;
}
//----------------------------------------------------------
float makeWave2(float v0)
{
	const float pi = 3.14159265;
	int k;
	//領域の中心を原点に変更
	vec2 pos = texPos.xy
	         - vec2(float((texWidth)/2),float((texHeight)/2));
	pos.x *= dx;//実際のサイズに変更
	pos.y *= dy;

	float value = v0;
	
	float R =  (sizeObs.x + sizeObs.y) / 4.0;
	//前方方向
	vec2 dir = normalize(vec2(velObs.x, velObs.y));
	vec2 posW = posObs.xy + dir * R ;//振動源を移動物体の前方に置く

	if(pos.x > posW.x-dx/2.0 &&  pos.x < posW.x+dx/2.0 
	   && pos.y > posW.y-dy/2.0 && pos.y < posW.y+dy/2.0)
	{
		value = amp * sin(2.0 * pi * freq * time);		
	}
	
	return value;
}
