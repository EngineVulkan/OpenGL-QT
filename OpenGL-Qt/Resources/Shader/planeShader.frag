uniform sampler2D texture;
varying mediump vec4 texc;
varying mediump vec4 v_normal;

varying vec3 FragPos;
varying vec4 FragPosLightSpace;


void main(void)
{
    vec3 lightDirection=vec3 (1,0,1);
	vec3 noraml=vec3(v_normal.x,v_normal.y,v_normal.z);
	//noraml=normalize(noraml);
	float result=dot(lightDirection,noraml);
	vec3 lightColor=vec3(1.0);
    

	vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    //ת����[0,1]��Χ
    projCoords = (projCoords + 1.0) * 0.5;
    //���������
    float closestDepth = texture2D(texture, projCoords.xy).r;
    //��ǰ�����
    float currentDepth = projCoords.z;
    //�Ƚϵ�ǰ��Ⱥ�������������
    float shadow = currentDepth > closestDepth? 1.0 : 0.0;

	gl_FragColor = vec4(lightColor,1.0)*(1.0-shadow);//*result;

}

