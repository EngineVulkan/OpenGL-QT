attribute highp vec4 vertex;
attribute mediump vec4 texCoord;
attribute mediump vec3 normal;
varying mediump vec4 texc;
uniform mediump mat4 matrix;
void main(void)
{
	gl_Position = matrix * vertex;
    texc = texCoord;
}