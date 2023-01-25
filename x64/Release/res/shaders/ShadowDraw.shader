#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 
layout(location=1) in vec3 normal;
layout(location=3) in mat4 model; 

out VS_OUT{
	vec4 v_LightSpacePosition;
	vec4 v_WorldNormal;
	vec4 v_WorldPosition;
	//vec3 TangentPosition;
	//float ProjPositionZ[24];
	//mat3 TBN;
}vs_out;


layout(std140) uniform Matrices
{
	mat4 u_view;
	mat4 u_projection;
};

uniform mat4 view;
uniform mat4 projection;

void main() 
{ 

	

	vs_out.v_LightSpacePosition=projection*view*model*vec4(position,1.0f);
	vs_out.v_WorldNormal=model*vec4(normalize(normal),0.0f);
	vs_out.v_WorldPosition=model*vec4(position,1.0f);
	gl_Position = u_projection*u_view*model*vec4(position,1.0f); 

}



#shader fragment
#version 330 core 

out vec4 color; 

uniform sampler2D shadowmap;
uniform sampler2D shadowcolormap;
uniform sampler2D cameramap;
//uniform sampler2D hbaoPos;
uniform vec4 u_LightPosition;
uniform float bias;
uniform float radius;
uniform float bias1;
uniform float bias3;
uniform vec4 u_CameraPosition;


in VS_OUT{
	vec4 v_LightSpacePosition;
	vec4 v_WorldNormal;
	vec4 v_WorldPosition;
	//float ProjPositionZ[24];
	//vec3 TangentPosition;
	//mat3 TBN;
}fs_in;

layout(std140) uniform Matrices
{
	mat4 u_view;
	mat4 u_projection;
};


void main() 
{
	//计算阴影

	vec3 projcoords = fs_in.v_LightSpacePosition.xyz/fs_in.v_LightSpacePosition.w;//光源视角标准化裁剪空间坐标
	projcoords = projcoords*0.5f+0.5f;//由-1到1转为0到1
	vec3 WorldLightDir = normalize(u_LightPosition.xyz - fs_in.v_WorldPosition.xyz);
	float sin_bias = sqrt(1.0f-pow(max(dot(WorldLightDir,fs_in.v_WorldNormal.xyz),0.0f),2.0f));//世界光线和世界法线的夹角
	float pcfDepth = texture(shadowmap, projcoords.xy).r;
	float shadowColor = 0.0f;//阴影深浅
	if(projcoords.z > pcfDepth+bias*sin_bias)//采样点最小深度+0.005和采样点实际深度（光源视角标准化裁剪空间坐标）比较
	{
		shadowColor = texture(shadowcolormap, projcoords.xy).r;
	}
	
	


	//HBAO
	
	vec3 WorldCameraDir = normalize(u_CameraPosition.xyz - fs_in.v_WorldPosition.xyz);
	float sin_hbao_bias = sqrt(1.0f-pow(max(dot(WorldCameraDir,fs_in.v_WorldNormal.xyz),0.0f),2.0f));//世界光线和世界法线的夹角
	float near = 0.1f;
	float far = 10000.0f;
	

	
	vec3 nnormal = fs_in.v_WorldNormal.xyz;
	vec3 tangent = vec3(0.0f);
	

	if(abs(nnormal.x) > abs(nnormal.y))
	{
		tangent.x = nnormal.z;
		tangent.z = -nnormal.x;
	}
	else
	{
		tangent.y = nnormal.z;
		tangent.z = -nnormal.y;
	}
	 
	vec3 T = normalize(tangent);
	vec3 N = normalize(nnormal);
	T = normalize(T-dot(T,N)*N);//保证TBN矩阵是正交化
	vec3 B = cross(N,T);
	mat3 TBN = mat3(T,B,N);
	

	float PI = 3.1415926f;
	float hbao = 0.0f;
	vec3 hbaoPosition = vec3(0.0f);
	vec4 thisViewPosition = vec4(0.0f);
	vec4 thisProjPosition = vec4(0.0f);
	vec3 thisProjPosition3 = vec3(0.0f);
	float ViewPositionZ = 0.0f;
	float LinearZ = 0.0f;
	float dis = 0.0f;
	
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<(2*i+1);j++)
		{
				
			float theata1 = i*PI/6.0f;
			float theata2 = j*2*PI/(2*i+1);
			hbaoPosition = radius*vec3(sin(theata1)*cos(theata2),sin(theata1)*sin(theata2),cos(theata1));
			//vec3 thisTangentPosition = vec3(x,y,z);
			thisViewPosition = u_view*vec4(TBN*hbaoPosition+fs_in.v_WorldPosition.xyz,1.0f);
			thisProjPosition = u_projection*thisViewPosition;
			thisProjPosition = thisProjPosition/thisProjPosition.w;
			thisProjPosition3 = thisProjPosition.xyz*0.5f + 0.5f;
			ViewPositionZ = -thisViewPosition.z;
			LinearZ = (2.0f * near * far)/(far + near - ((texture(cameramap, thisProjPosition3.xy).r) * 2.0f - 1.0f) * (far - near));
			dis = (ViewPositionZ-bias1*sin_hbao_bias) - LinearZ;
			if(dis>0&&dis<bias3)
			{
				hbao ++;
			}
		}
	}
	

	float hbaoShadowColor = hbao/24.0f;
	
	
	color = vec4(0.0f,0.0f,0.0f,shadowColor*0.2f+hbaoShadowColor*0.8f); 
	//color = vec4(0.0f,0.0f,0.0f,hbaoShadowColor); 
	//color =vec4(1.0f,0.0f,0.0f,1.0f); 

}