uniform sampler2D texture;
varying mediump vec4 texc;
varying mediump vec4 v_normal;
void main(void)
{
    vec3 amination=vec3(0.1,0.1,0.1);
    vec3 lightDirection=vec3 (1,-1,0);
	vec3 noraml=vec3(v_normal.x,v_normal.y,v_normal.z);
	//noraml=normalize(noraml);
	float result=dot(lightDirection,noraml);
    gl_FragColor = texture2D(texture, texc.st)*result+vec4(amination,1.0);

}