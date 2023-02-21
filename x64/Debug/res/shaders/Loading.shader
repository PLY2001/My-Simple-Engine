#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord; //贴图坐标
//layout(location=3) in mat4 model; 

out VS_OUT{
	vec2 v_texcoord;//传递（vary）给片元着色器的变量

}vs_out;


void main() 
{ 


	vs_out.v_texcoord=texcoord;
	gl_Position =vec4(position,1.0f); 

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

}fs_in;


void main() 
{
	
	color = texture(material.texture_diffuse1,fs_in.v_texcoord);

	
}