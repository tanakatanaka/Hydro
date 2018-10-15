varying vec2 texPos;

void main(void)
{
	texPos = gl_Vertex.xy;

	gl_Position = ftransform();
}

