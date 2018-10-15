
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerPhi;
uniform int texWidth;//ダミーを含む全格子数
uniform int nMesh;
uniform float size0;
uniform float adjustH;
varying vec3 P;
varying vec3 N;
varying vec3 T;//接線ベクトル
varying vec3 B;//従法線ベクトル
varying vec2 grad;
varying float pp;

int texHeight = texWidth;

void main(void)
{
	//テクスチャ座標の中心を移動
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));
	//位置座標
	vec4 phi = texture2DRect(samplerPhi, texPos);
	pp = phi.r;
	//1辺の長さをsize0に変換
 	gl_Vertex.x *= size0 /  float(nMesh);
	gl_Vertex.y *= size0 /  float(nMesh);
	gl_Vertex.z = phi.r * adjustH;//物理量(高さ調整）
	gl_Position = ftransform();
}

