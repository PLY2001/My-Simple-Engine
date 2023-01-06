#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 
layout(location=3) in mat4 model; 

out VS_OUT{
	vec4 v_WorldPosition;
}vs_out;


layout(std140) uniform Matrices
{
	mat4 u_view;
	mat4 u_projection;
};


void main() 
{ 

	vs_out.v_WorldPosition=model*vec4(position,1.0f);
	gl_Position = u_projection*u_view*model*vec4(position,1.0f); 

}



#shader fragment
#version 330 core 

out vec4 color; 

uniform samplerCube shadowcubemap;
uniform float far_plane;
uniform vec3 lightPos;


in VS_OUT{
	vec4 v_WorldPosition;
}fs_in;



void main() 
{
	vec3 sampleOffsetDirections[20] = vec3[]
	(
	vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);


	//������Ӱ
	//vec3 projcoords = fs_in.v_LightSpacePosition.xyz/fs_in.v_LightSpacePosition.w;//��Դ�ӽǱ�׼���ü��ռ�����
	//projcoords = projcoords*0.5f+0.5f;//��-1��1תΪ0��1
	float d_Receiver = length(fs_in.v_WorldPosition.xyz-lightPos);//������Ӱ������
	
	float shadow = 0.0f;//�Ƿ�����Ӱ�жϣ�1Ϊ�ڣ�0Ϊ����
	float shadowColor = 0.0f;//��Ӱ��ǳ
	
	/*��õ��ڹ�Դ�ӽǵ�ƽ�������*/
	float texelSize = 0.5f;// textureSize(shadowcubemap, 0);//�����ڵ���ƽ����ȵľ����С������Խ����ӰԽԲ��
	float d_Block = 0.0f;//�ڵ���ƽ����ȣ���Դ�ӽǱ�׼���ü��ռ����꣩
	int d_BlockCount = 0;//�ڵ������
	//��������������Խ��Ч��Խ��ϸ
	for(int x = 0; x < 20; ++x)
	{
			float pcfDepth = far_plane * texture(shadowcubemap, fs_in.v_WorldPosition.xyz-lightPos+sampleOffsetDirections[x]*texelSize).r;// + vec2(x, y) * texelSize).r; //��������С��ȣ���Դ�ӽǱ�׼���ü��ռ����꣩
			if(d_Receiver > pcfDepth+0.5)//��������С���+0.005�Ͳ�����ʵ����ȣ���Դ�ӽǱ�׼���ü��ռ����꣩�Ƚ�
			{
				shadow ++;
				d_Block += pcfDepth;
				d_BlockCount ++;
			}
		
	}
	shadow /= 20.0f;//����õ��Ƿ�����Ӱ��ƽ������
	d_Block /= d_BlockCount;//�����ڵ����ƽ����ȣ���Դ�ӽǱ�׼���ü��ռ����꣩
	
	
	
	if(shadow>0.0f)
	{
		/*��õ�����ڽ�����Ӱ�ľ���ϵ������Ϊ����ƽ������Ӱ��ǳ�ľ����С*/
		shadowColor = 0.0f;
		float w = (d_Receiver-d_Block)/d_Block;
		float WSize = 1.0f*w;// / textureSize(shadowcubemap, 0); //����ƽ������Ӱ��ǳ�ľ����С������Խ����ӰԽ��
		/*��õ��ڹ�Դ�ӽǵ�ƽ��������Ӱ��ǳ*/
		//��������������Խ��Ч��Խ��ϸ
		for(int x = 0; x <20; ++x)
		{
			float pcfDepth = far_plane * texture(shadowcubemap, fs_in.v_WorldPosition.xyz-lightPos+sampleOffsetDirections[x]*WSize).r;// + vec2(x, y) * WSize).r; ;//��������С��ȣ���Դ�ӽǱ�׼���ü��ռ����꣩
			if(d_Receiver > pcfDepth+0.5)//��������С���+0.005�Ͳ�����ʵ����ȣ���Դ�ӽǱ�׼���ü��ռ����꣩�Ƚ�
			{
				shadowColor ++;
			}		  
		}
		shadowColor /= 20.0f;//����õ��ƽ������Ӱ��ǳ
		//shadowColor = pow(shadowColor,5.0f);//ʹ��Ӱ�߽磨shdaowColor�ӽ�0�ĵط������ӽ�͸��������������Ը��ѣ�����Խ����ӰԽ����
	}
	else
	{
		shadowColor = 0.0f;
	}
	
	
	






	if(d_Receiver>far_plane)
	shadowColor=0.0f;
	
	color =vec4(vec3(0.0f),shadowColor*0.2f); 
	//color =vec4(1.0f,0.0f,0.0f,1.0f); 

}