#version 330
in vec3 vVertex;
in vec4 vColor;
out vec4 Color;
//out vec3 eyePos;
uniform mat4 mvp;
uniform mat4 mv;

void main(void)
{
	Color = vColor;
	gl_Position = mvp * vec4(vVertex,1.0f);
}
