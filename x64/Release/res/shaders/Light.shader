#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord; //贴图坐标
//layout(location=3) in mat4 model; 

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

uniform vec4 u_CameraPosition;
uniform mat4 model;

void main() 
{ 
	vec4 ModelCameraPosition = inverse(model)*u_CameraPosition;
	vec3 ModelNormalDir = normalize(ModelCameraPosition.xyz);
	vec3 UpDir = vec3(0.0f);
	if(abs(ModelNormalDir.y)>0.999)
	{
		UpDir = vec3(0.0f,0.0f,1.0f);
	}
	else
	{
		UpDir = vec3(0.0f,1.0f,0.0f);
	}
	vec3 RightDir = normalize(cross(UpDir,ModelNormalDir));
	UpDir = normalize(cross(ModelNormalDir,RightDir));
	vec3 centerOff = position.xyz;
	vec3 NewPosition = RightDir * centerOff.x + UpDir * centerOff.y + ModelNormalDir * centerOff.z;

	vs_out.v_texcoord=texcoord;
	vs_out.v_WorldNormal=model*vec4(ModelNormalDir,0.0f);
	//vs_out.v_ViewNormal=u_view*model*vec4(normalize(normal),0.0f);
	vs_out.v_WorldPosition=model*vec4(NewPosition,1.0f);
	gl_Position =u_projection*u_view*model*vec4(NewPosition,1.0f); 

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


in VS_OUT{
	vec2 v_texcoord;//从顶点着色器传入的变量
	vec4 v_WorldNormal;
	vec4 v_WorldPosition;
	//vec4 v_TangentPosition;

}fs_in;



void main() 
{
	
	
	vec4 diffuseColor=texture(material.texture_diffuse1,fs_in.v_texcoord);
	color = diffuseColor;//vec4(1.0,1.0,1.0,1.0); //diffuseColor;
}