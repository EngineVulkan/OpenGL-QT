attribute highp vec4 vertex;
attribute mediump vec4 texCoord;
attribute mediump vec3 normal;
varying mediump vec4 texc;
varying mediump vec4 v_normal;
uniform mediump mat4 matrix;
void main(void)
{
	gl_Position = matrix * vertex;
	v_normal=matrix*vec4(normal,1.0);
	v_normal=normalize(v_normal);
    texc = texCoord;
}