
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerPhi;
uniform int texWidth;//全格子数
uniform int nMesh;
uniform float size0;
uniform float adjustH;

varying float pp;

void main(void)
{
	int texHeight = texWidth;
	//テクスチャ座標の中心を移動
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));
	
	vec4 phi = texture2DRect(samplerPhi, texPos);
	pp = phi.r;//物理量
	
	//1辺の長さをsize0に変換
 	gl_Vertex.x *= size0 /  float(nMesh);
	gl_Vertex.y *= size0 /  float(nMesh);
	gl_Vertex.z = phi.r * adjustH;//物理量
	
	gl_Position = ftransform();
}

