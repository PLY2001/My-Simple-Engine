#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord; //贴图坐标


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

uniform mat4 model;

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
uniform samplerCube skybox;

uniform vec4 u_LightPosition;
uniform vec4 u_CameraPosition;


in VS_OUT{
	vec2 v_texcoord;//从顶点着色器传入的变量
	vec4 v_WorldNormal;
	vec4 v_WorldPosition;
}fs_in;


void main() 
{
	vec3 LightDir = normalize(u_LightPosition.xyz - fs_in.v_WorldPosition.xyz);//世界光源方向
	vec3 Normal=normalize(fs_in.v_WorldNormal.xyz);//世界法线
	//漫反射
	float diffuse = max(0.4f,dot(Normal,LightDir));
	vec3 diffuseColor=1.0f*texture(material.texture_diffuse1,fs_in.v_texcoord).xyz*diffuse;
	//高光反射
	vec3 CameraDir = normalize(u_CameraPosition.xyz - fs_in.v_WorldPosition.xyz);
	vec3 ha= normalize(CameraDir+LightDir);
	float specular = pow(max(0.4f,dot(Normal,ha)),100.0f);
	vec3 specularColor =  1.0f*texture(material.texture_specular1,fs_in.v_texcoord).xyz*specular;
	//环境光
	vec3 ambientColor = vec3(0.0f);//vec3(0.05f,0.05f,0.05f);

    //反射环境球
	vec3 R=reflect(-CameraDir,Normal);
	vec3 reflectColor = texture(skybox,R).xyz;


	//color =vec4(diffuseColor/2.0f,1.0f);
	color =vec4((reflectColor*0.05f+ambientColor+diffuseColor+specularColor*reflectColor),1.0f);//*(1.0f-shadowColor) //texColor;//u_color;//vec4(0.2,0.7,0.3,1.0); 
}