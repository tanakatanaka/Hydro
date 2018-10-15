#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;//ダミーを含むX方向全格子数
uniform int nDummy;
uniform float sizeX, sizeY;//実際の1辺の長さ(ダミーを含まず）
uniform int meshX, meshY;
uniform int nSource;
uniform int sourceX;
uniform int interval;
uniform float amp;
uniform float freq;
uniform float waveSpeed;
uniform float drag0;
uniform float dt;
uniform float time;
uniform int mode;
uniform int boundary;

varying vec2 texPos;

float dx, dy;
float makeWave(float v0);

void main(void)
{
	//格子間隔
	dx = sizeX / float(meshX);//X方向格子間隔	
	dy = sizeY / float(meshY);//Y方向格子間隔	
	
	vec4 wave = texture2DRect(samplerWave, texPos);
	wave.r = makeWave(wave.r);//波源の波を作成(ｚ軸方向速度）

	float accel = 0.0;
	float drag = drag0;
	int nm = nDummy;         //無反射のときの有効領域境界番号	
	int npX = meshX + nDummy;//無反射のときの有効領域境界番号
	int npY = meshY + nDummy;//無反射のときの有効領域境界番号

    int i = int(texPos.x);
    int j = int(texPos.y);

	float dragMax = 5.0;

	if(boundary == 0)//B_NON
	{
		if(j < nm)  drag = drag0 + dragMax * float(nm - j)  / float(nDummy);
		if(j > npY) drag = drag0 + dragMax * float(j - npY) / float(nDummy);
		if(i < nm)  drag = drag0 + dragMax * float(nm - i)  / float(nDummy);
		if(i > npX) drag = drag0 + dragMax * float(i - npX) / float(nDummy);
	}
	
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

	accel = (waveSpeed * waveSpeed) * ((zmi + zpi - c1 * wave.g) / (dx*dx)
	                             + (zmj + zpj - c2 * wave.g) / (dy*dy));

	//粘性抵抗
	accel -= drag * wave.r;
	//速度の更新
	wave.r += accel * dt;

	gl_FragColor = wave;
}
//--------------------------------------------------------
float makeWave(float v0)
{
	float pi = radians(180.0);
	int k;
	vec2 source;//波源の位置
	//領域の中心を原点に変更
	vec2 pos = texPos.xy - vec2(float((texWidth)/2), float((texHeight)/2));

	float value = v0;
	float period = 1.0 / freq;
	for(k = 0; k < nSource; k++)
	{
		source.x = float(sourceX);
		source.y = float(interval) * (-0.5 * float(nSource-1) + float(k));

		if(pos.x == source.x && pos.y == source.y)
		{
			if(mode == 0)//pulse
			{
				if(time < period/2.0) value = amp * sin(2.0 * pi * freq * time);
			}
			else  value = amp * sin(2.0 * pi * freq * time);//continuous
		}
	}
	return value;
}
