
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerPosP;
uniform float height;

void main()
{
	vec2 texPos = gl_Vertex.xy;

	vec4 pos;
	pos.xyz = texture2DRect(samplerPosP, texPos).xyz ;//ì_ÇÃç¿ïW
	pos.z += height ;//êÖñ ÇÃçÇÇ≥Çí«â¡
	pos.w = 1.0;

	gl_Position = gl_ModelViewProjectionMatrix * pos;
	gl_FrontColor = gl_Color;
}
	