#version 330
uniform mat4 mvp;
in vec4 vVertex;

void main(void)
{
	gl_Position = mvp*vVertex;


}
