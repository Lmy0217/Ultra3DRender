#version 330
uniform mat4 mvp;
uniform mat4 mv;
uniform float distance;
in vec4 vVertex;
in vec3 vNormal;

out vec3 normal;
out vec3 lightV;
void main(void)
{
    vec3 lightPos = normalize(vec3(mv*vec4(0.0,0.0,1.0,1.0)))*distance;
	normal = vNormal;
	lightV = lightPos - vVertex.xyz;
        gl_Position = mvp*vVertex;

}
