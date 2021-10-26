#version 330
in vec3 vVertex;
in vec4 vColor;
out vec4 Color;
uniform mat4 mvp;

void main(void)
{
  Color = vColor;
  gl_Position = mvp * vec4(vVertex,1.0f);
}
