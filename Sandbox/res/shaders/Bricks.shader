#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord; //��ͼ����
layout(location=3) in vec3 tangent; //����


out VS_OUT{
	vec2 v_texcoord;//���ݣ�vary����ƬԪ��ɫ���ı���
	vec4 v_TangentPosition;
	vec4 v_TangentLightPos;
	vec4 v_TangentCameraPos;
}vs_out;


layout(std140) uniform Matrices
{
	mat4 u_view;
	mat4 u_projection;
};

uniform vec4 u_LightPosition;
uniform vec4 u_CameraPosition;
uniform mat4 model;

void main() 
{ 
	
	vec3 T = normalize(vec3(model*vec4(tangent,0.0f)));
	vec3 N = normalize(vec3(model*vec4(normal,0.0f)));
	//vec3 g = normalize(vec3(1.0f,0.0f,1.0f));
	T = normalize(T-dot(T,N)*N);//��֤TBN������������
	vec3 B = cross(N,T);
	mat3 TBN = transpose(mat3(T,B,N));

	vs_out.v_texcoord = texcoord;
	vs_out.v_TangentPosition = vec4(TBN*(model*vec4(position,1.0f)).xyz,1.0f);
	vs_out.v_TangentLightPos = vec4(TBN*(u_LightPosition.xyz),1.0f);
	vs_out.v_TangentCameraPos = vec4(TBN*(u_CameraPosition.xyz),1.0f);
	
	gl_Position =u_projection*u_view*model*vec4(position,1.0f); 

}




#shader fragment
#version 330 core 


out vec4 color; 


uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_displacement;



in VS_OUT{
	vec2 v_texcoord;//���ݣ�vary����ƬԪ��ɫ���ı���
	vec4 v_TangentPosition;
	vec4 v_TangentLightPos;
	vec4 v_TangentCameraPos;
}fs_in;


void main() 
{
	//���Ӳ���ͼ��ƫ�ƺ����������
	vec3 CameraDir = normalize(fs_in.v_TangentCameraPos.xyz - fs_in.v_TangentPosition.xyz);//ָ������ķ���
	
	const float numLayers = 10;//ƫ�ƴ���
	float deltaDepth = 1.0/numLayers;//ÿ��ƫ�����ӵ����

	float currentDepth = 0.0;//��ǰ��ƫ�Ƶ����
	float lastDepth = currentDepth;//��һ��ƫ�Ƶ����

	vec2 p1 = CameraDir.xy;//ƫ�Ʒ���
	vec2 deltaTexcoords = p1 / numLayers*0.1f;//ÿ��ƫ�Ƶĵ�λƫ�������С��Խ����͹Խ����

	vec2 currentTexcoords = fs_in.v_texcoord;//��ǰ�����������
	vec2 lastTexcoords = currentTexcoords;//��һ�����������

	float currentDepthMapValue = texture(texture_displacement,currentTexcoords).r;//��ǰ��������ͼ��ʵ�����
	float lastDepthMapValue = currentDepthMapValue;//��һ��������ͼ��ʵ�����

	while(currentDepthMapValue>currentDepth)//��δԽ��Ӧ��ƫ�Ƶ���������ʱ
	{
		//����һ���������ֵ
		lastTexcoords = currentTexcoords;
		lastDepthMapValue = currentDepthMapValue;
		lastDepth = currentDepth;

		currentTexcoords -= deltaTexcoords;//ƫ�Ƶ�ǰ����������꣨��ָ������ķ������෴�ģ�������-��
		currentDepthMapValue = texture(texture_displacement,currentTexcoords).r;//��ǰ��������ͼ��ʵ�����
		currentDepth += deltaDepth;//��ǰ��ƫ�Ƶ����

		if(currentDepthMapValue<currentDepth)//�ո�Խ��Ӧ��ƫ�Ƶ���������ʱ
		{
			vec2 backOffset =  deltaTexcoords*(currentDepth-currentDepthMapValue)/(lastDepthMapValue-lastDepth+currentDepth-currentDepthMapValue);//��ֵ
			currentTexcoords = currentTexcoords + backOffset;//�õ���ȷ��ƫ�ƺ����������
		}
	}
	vec2 finalTexcoords = currentTexcoords;//ƫ�ƺ����������
	
	if(finalTexcoords.x>1.0||finalTexcoords.y>1.0||finalTexcoords.x<0.0||finalTexcoords.y<0.0)//���ƫ�Ƶó������������귶Χ���Ͳ���Ⱦ
		discard;

	vec3 LightDir = normalize(fs_in.v_TangentLightPos.xyz - fs_in.v_TangentPosition.xyz);//��Դ����
	vec3 Normal= texture(texture_normal,finalTexcoords).xyz;
	Normal = Normal*2.0f - vec3(1.0f);
	//������
	float diffuse = max(0.0f,dot(Normal,LightDir));
	vec3 diffuseColor=2.0f*texture(texture_diffuse,finalTexcoords).xyz*diffuse;
	//�߹ⷴ��
	//vec3 CameraDir = normalize(fs_in.v_TangentCameraPos.xyz - fs_in.v_TangentPosition.xyz);
	vec3 ha= normalize(CameraDir+LightDir);
	float specular = pow(max(0.0f,dot(Normal,ha)),100.0f);
	vec3 specularColor =  vec3(2.0f)*specular;
	//������
	vec3 ambientColor = vec3(0.05f,0.05f,0.05f);
	

	
	color =vec4((ambientColor+diffuseColor+specularColor),1.0f);//*(1.0f-shadowColor) //texColor;//u_color;//vec4(0.2,0.7,0.3,1.0); 
}