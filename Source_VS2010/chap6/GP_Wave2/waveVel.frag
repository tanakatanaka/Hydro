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

varying vec2 texPos;

float dx, dy;
float makeWave(float v0);

void main(void)
{
	//格子間隔
	dx = sizeX / float(meshX);	
	dy = sizeY / float(meshY);	
	
	vec4 wave = texture2DRect(samplerWave, texPos);
	wave.r = makeWave(wave.r);//波源の波を作成(ｚ軸方向速度）

	float drag = drag0;
	int nm = nDummy;      //無反射のときの有効領域境界番号	
	int npX = meshX + nm;//無反射のときの有効領域境界番号
	int npY = meshY + nm;//無反射のときの有効領域境界番号

    int i = int(texPos.x);
    int j = int(texPos.y);

	//有効領域境界では無反射境界
	float dragMax = 5.0;
	if(j < nm) drag = drag0 + dragMax * float(nm - j) / float(nm);
	if(j > npY)drag = drag0 + dragMax * float(j - npY) / float(nm);
	if(i < nm) drag = drag0 + dragMax * float(nm - i) / float(nm);
	if(i > npX)drag = drag0 + dragMax * float(i - npX) / float(nm);
	
	//加速度計算に必要な係数を求める
	vec4 cA, cB, cC, cD;//水面であれば1.0，障害物であれば0.0
	cA = texture2DRect(samplerWave, texPos + vec2(-1.0, 0.0));
	cB = texture2DRect(samplerWave, texPos + vec2( 1.0, 0.0));
	cC = texture2DRect(samplerWave, texPos + vec2( 0.0,-1.0));
	cD = texture2DRect(samplerWave, texPos + vec2( 0.0, 1.0));
	
	float c1, c2;

	float accel = 0.0;
	if(wave.a == 1.0 ||       //注目点が水面
	  (wave.a == 0.0 && (cA.a == 1.0 || cB.a == 1.0 
		                || cC.a == 1.0 || cD.a == 1.0)))//表面
	{
		if(cA.a * cB.a == 1.0) c1 = 2.0;//どちらも水面
		else c1 = 1.5;                  //どちらかが障害物表面
		if(cC.a * cD.a == 1.0) c2 = 2.0;//どちらも水面
		else c2 = 1.5;                  //どちらかが障害物表面
	  
		accel = (waveSpeed * waveSpeed) * 
		 ((cA.a * cA.g + cB.a * cB.g - c1 * wave.g) / (dx*dx) 
		+ (cC.a * cC.g + cD.a * cD.g - c2 * wave.g) / (dy*dy));
	}

	//粘性抵抗
	accel -= drag * wave.r;
	//速度の更新
	wave.r += accel * dt;

	gl_FragColor = wave;
}
//----------------------------------------------------------
float makeWave(float v0)
{
	float pi = radians(180.0);

	int k;
	//領域の中心を原点に変更
	vec2 pos = texPos.xy
	         - vec2(float((texWidth)/2),float((texHeight)/2));
	vec2 source;//波源の位置

	float value = v0;
	for(k = 0; k < nSource; k++)
	{
		source.x = float(sourceX) ;
		source.y = float(interval) *
		           (-0.5 * float(nSource-1) + float(k));

		if(pos.x == source.x && pos.y == source.y)
		{//連続波だけ
			value = amp * sin(2.0 * pi * freq * time);
		}
	}
	return value;
}
