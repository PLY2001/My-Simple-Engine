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
uniform vec4 u_LightPosition;
uniform float bias;
uniform float radius;
uniform float bias1;
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
	float cos_bias = sqrt(1.0f-pow(max(dot(WorldLightDir,fs_in.v_WorldNormal.xyz),0.0f),2.0f));//世界光线和世界法线的夹角
	
	float shadow = 0.0f;//是否在阴影判断，1为在，0为不在
	float shadowColor = 0.0f;//阴影深浅
	
	/*求该点在光源视角的平均化深度*/
	vec2 texelSize = 5.0f / textureSize(shadowmap, 0);//采样遮挡物平均深度的矩阵大小，矩阵越大，阴影越圆润
	float d_Block = 0.0f;//遮挡物平均深度（光源视角标准化裁剪空间坐标）
	int d_BlockCount = 0;//遮挡物计数
	//迭代次数，次数越高效果越精细
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowmap, projcoords.xy + vec2(x, y) * texelSize).r; //采样点最小深度（光源视角标准化裁剪空间坐标）
			if(projcoords.z > pcfDepth+bias*cos_bias)//采样点最小深度+0.005和采样点实际深度（光源视角标准化裁剪空间坐标）比较
			{
				shadow ++;
				d_Block += pcfDepth;
				d_BlockCount ++;
			}
		}    
	}
	shadow /= 9.0f;//计算该点是否在阴影（平均化）
	d_Block /= d_BlockCount;//计算遮挡物的平均深度（光源视角标准化裁剪空间坐标）
	
	
	float d_Receiver = projcoords.z;//接收阴影物的深度（光源视角标准化裁剪空间坐标）
	if(shadow>0.0f)
	{
		/*求该点相对于接收阴影的距离系数，作为采样平均化阴影深浅的矩阵大小*/
		shadowColor = 0.0f;
		float w = (d_Receiver-d_Block)/d_Block*10.0f;
		vec2 WSize = w / textureSize(shadowmap, 0); //采样平均化阴影深浅的矩阵大小，矩阵越大，阴影越淡
		/*求该点在光源视角的平均化的阴影深浅*/
		//迭代次数，次数越高效果越精细
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
			
				float pcfDepth = texture(shadowmap, projcoords.xy + vec2(x, y) * WSize).r;//采样点最小深度（光源视角标准化裁剪空间坐标）
				if(projcoords.z > pcfDepth+bias*cos_bias)//采样点最小深度+0.005和采样点实际深度（光源视角标准化裁剪空间坐标）比较
				{
					shadowColor ++;
				}
				
			}    
		}
		shadowColor /= 9.0f;//计算该点的平均化阴影深浅
		//shadowColor = pow(shadowColor,5.0f);//使阴影边界（shdaowColor接近0的地方）更接近透明，不会出现明显割裂，数字越大，阴影越内缩
	}
	else
	{
		shadowColor = 0.0f;
	}
	

	if(projcoords.z>1.0f)
	shadowColor=0.0f;



	//HBAO
	/*
	vec3 WorldCameraDir = normalize(u_CameraPosition.xyz - fs_in.v_WorldPosition.xyz);
	float sin_hbao_bias = sqrt(1.0f-pow(max(dot(WorldCameraDir,fs_in.v_WorldNormal.xyz),0.0f),2.0f));//世界光线和世界法线的夹角
	float near = 0.1f;
	float far = 10000.0f;
	float LinearZ = (2.0f * near * far)/(far + near - (gl_FragCoord.z * 2.0f - 1.0f) * (far - near));

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
	if(LinearZ<20.0f)
	{
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<6;j++)
			{
				float theata1 = i*PI/6.0f;
				float theata2 = j*PI/3.0f;
				float x = radius*sin(theata1)*cos(theata2);
				float y = radius*sin(theata1)*sin(theata2);
				float z = radius*cos(theata1);
				//vec3 thisTangentPosition = vec3(x,y,z);
				vec4 thisViewPosition = u_view*vec4(TBN*vec3(x,y,z)+fs_in.v_WorldPosition.xyz,1.0f);//
				float ViewPositionZ = -thisViewPosition.z;
			
				if(LinearZ<(ViewPositionZ-bias1*sin_hbao_bias))
				{
					hbao++;
				}
			}
		}
	}

	float hbaoShadowColor = hbao/24.0f;
	*/
	
	color = vec4(0.0f,0.0f,0.0f,shadowColor*0.2f);//+hbaoShadowColor*0.8f); 
	//color = vec4(0.0f,0.0f,0.0f,hbaoShadowColor); 
	//color =vec4(1.0f,0.0f,0.0f,1.0f); 

}