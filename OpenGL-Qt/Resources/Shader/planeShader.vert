attribute highp vec4 vertex;
attribute mediump vec4 texCoord;
attribute mediump vec3 normal;
varying mediump vec4 texc;
varying mediump vec4 v_normal;

uniform mediump mat4 matrix;
uniform mediump mat4 s_view;
uniform mediump mat4 s_project;
uniform mediump mat4 model;

varying vec3 FragPos;
varying vec4 FragPosLightSpace;

void main(void)
{
	v_normal=matrix*vec4(normal,1.0);
	v_normal=normalize(v_normal);
    texc =texCoord;
	//vs_out.TexCoords=texCoord.st;

    FragPos = vec3(s_project*model*s_view*vertex);//vec3(model * vec4(aPos, 1.0));
    FragPosLightSpace = vec4(FragPos,1.0);   //在光源空间中的位置
    gl_Position = matrix * vertex;
}