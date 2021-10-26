#version 330
out vec4 vFragColor;

void main(void)
{
        vFragColor = vec4(1.0,1.0,1.0,1.0)*(1 - gl_FragCoord.z);

}
