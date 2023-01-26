#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 
layout(location=1) in vec2 aTexcoords;

out vec2 Texcoords;

void main() 
{ 
	gl_Position = vec4(position,1.0f);
	Texcoords=aTexcoords;
}


#shader fragment
#version 330 core 

in vec2 Texcoords;

out vec4 color; 
uniform sampler2D screenTexture;
uniform sampler2D cameramap;

//float kernel[25]=float[](2,4,5,4,2,
//						 4,9,12,9,4,
//						 5,12,15,12,5,
//						 4,9,12,9,4,
//						 2,4,5,4,2);
uniform float width0;
uniform float height0;
uniform float width1;
uniform float height1;


void main() 
{
	vec4 texColor=vec4(0.0f);
	float near = 0.1f;
	float far = 10000.0f;
	float LinearZ = (2.0f * near * far)/(far + near - ((texture(cameramap, Texcoords).r) * 2.0f - 1.0f) * (far - near));
	float width = (LinearZ - near)/far * (width1-width0) + width0;
	float height = (LinearZ - near)/far * (height1-height0) + height0;
	float WOff = 1.0f/width;
	float HOff = 1.0f/height;
	for(int i=-2;i<3;i++)
	{
		for(int j=2;j>-3;j--)
		{
			texColor+=texture(screenTexture,vec2(Texcoords.x+i*WOff,Texcoords.y+j*HOff));
		}

		//texColor+=kernel[i]*texture(screenTexture,vec2(Texcoords.x+offsetsW[i],Texcoords.y));
	}
	vec4 CenterColor = texture(screenTexture,Texcoords);
	//if(CenterColor>0.0f)
		CenterColor = texColor/25;
	//else
	//	CenterColor = CenterColor;

	color = CenterColor;
}