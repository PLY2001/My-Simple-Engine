#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 
layout(location=1) in vec3 normal;
layout(location=3) in mat4 model; 



layout(std140) uniform Matrices
{
	mat4 u_view;
	mat4 u_projection;
};

uniform mat4 view;
uniform mat4 projection;

void main() 
{ 

	
	gl_Position = u_projection*u_view*model*vec4(position,1.0f); 

}



#shader fragment
#version 330 core 

out vec4 color; 







void main() 
{

	color = vec4(0.0f,0.0f,0.0f,0.0f); 

}