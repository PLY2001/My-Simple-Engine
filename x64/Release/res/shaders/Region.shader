#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 

layout(std140) uniform Matrices
{
	mat4 u_view;
	mat4 u_projection;
};

out VS_OUT{
	vec4 WorldPosition;
}vs_out;

void main() 
{ 
	vs_out.WorldPosition = vec4(position,1.0f);
	gl_Position = u_projection*u_view*vec4(position,1.0f);

}


#shader fragment
#version 330 core 

out vec4 color; 

in VS_OUT{
	vec4 WorldPosition;
}fs_in;

uniform float time;

void main() 
{
	float ColorR = cos(time+fs_in.WorldPosition.x*0.2f)*0.25f+0.75f;
	float ColorB = sin(time+fs_in.WorldPosition.x*0.2f)*0.25f+0.75f;
	color = vec4(ColorR*0.5f,0.0f,ColorB*0.5f,0.1f);
}