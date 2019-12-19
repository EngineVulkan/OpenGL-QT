attribute highp vec4 vertex;
attribute mediump vec4 texCoord;
varying mediump vec4 texc;
uniform mediump mat4 matrix;
uniform mediump mat4 model;
uniform mediump mat4 s_view;
uniform mediump mat4 s_project;
void main(void)
{
	gl_Position = s_project*model*s_view*vertex;//matrix * vertex;
}