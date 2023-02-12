#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord; //贴图坐标
layout(location=3) in mat4 model; 

out VS_OUT{
	vec2 v_texcoord;//传递（vary）给片元着色器的变量
	vec4 v_WorldNormal;
	//vec4 v_ViewNormal;
	vec4 v_WorldPosition;

}vs_out;


layout(std140) uniform Matrices
{
	mat4 u_view;
	mat4 u_projection;
};



void main() 
{ 
	
	

	vs_out.v_texcoord=texcoord;
	vs_out.v_WorldNormal=model*vec4(normalize(normal),0.0f);
	//vs_out.v_ViewNormal=u_view*model*vec4(normalize(normal),0.0f);
	vs_out.v_WorldPosition=model*vec4(position,1.0f);
	gl_Position =u_projection*u_view*model*vec4(position,1.0f); 

}


#shader fragment
#version 330 core 

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	//sampler2D texture_normal1;
};

out vec4 color; 

uniform Material material;

uniform float u_LightPosition[70];
uniform vec4 u_CameraPosition;


in VS_OUT{
	vec2 v_texcoord;//从顶点着色器传入的变量
	vec4 v_WorldNormal;
	vec4 v_WorldPosition;
	//vec4 v_TangentPosition;

}fs_in;



void main() 
{
	vec3 Normal=normalize(fs_in.v_WorldNormal.xyz);//世界法线
	vec3 CameraDir = normalize(u_CameraPosition.xyz - fs_in.v_WorldPosition.xyz);
	vec3 LightDir = vec3(0.0f);
	float diffuse = 0.0f;
	vec3 diffuseColor = vec3(0.0f);
	vec3 ha = vec3(0.0f);
	float specular = 0.0f;
	vec3 specularColor = vec3(0.0f);
	float dis = 0.0f;
	for(int i = 0; i < 35; i++)
	{
		dis = (fs_in.v_WorldPosition.x-u_LightPosition[i*2])*(fs_in.v_WorldPosition.x-u_LightPosition[i*2])+(fs_in.v_WorldPosition.z-u_LightPosition[i*2+1])*(fs_in.v_WorldPosition.z-u_LightPosition[i*2+1]);
		
		if(dis<1600)
		{
			LightDir = normalize(vec3(u_LightPosition[i*2],27.64f,u_LightPosition[i*2+1]) - fs_in.v_WorldPosition.xyz);//世界光源方向
			//漫反射
			diffuse = max(0.4f,dot(Normal,LightDir));
			diffuseColor += (0.4f*(1600.0f-dis)/1600.0f)*texture(material.texture_diffuse1,fs_in.v_texcoord).xyz*diffuse;
			//高光反射
	
			ha = normalize(CameraDir+LightDir);
			specular = pow(max(0.4f,dot(Normal,ha)),100.0f);
			specularColor +=  (0.4f*(1600.0f-dis)/1600.0f)*texture(material.texture_specular1,fs_in.v_texcoord).xyz*specular;
			//环境光
		}
	}
	
	vec3 ambientColor = vec3(0.0f);//vec3(0.05f,0.05f,0.05f);

  

	
	


	//color =vec4(gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z,1.0f);
	//color =vec4(1.0f,1.0f,1.0f,1.0f);
	color =vec4((ambientColor+diffuseColor+specularColor),1.0f);//*(1.0f-shadowColor) //texColor;//u_color;//vec4(0.2,0.7,0.3,1.0); 
}