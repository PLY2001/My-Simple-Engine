#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 
layout(location=3) in mat4 model; 

out VS_OUT{
	vec4 v_LightSpacePosition;
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
	gl_Position = u_projection*u_view*model*vec4(position,1.0f); 

}



#shader fragment
#version 330 core 

out vec4 color; 

uniform sampler2D shadowmap;


in VS_OUT{
	vec4 v_LightSpacePosition;
}fs_in;


void main() 
{
	//������Ӱ
	vec3 projcoords = fs_in.v_LightSpacePosition.xyz/fs_in.v_LightSpacePosition.w;//��Դ�ӽǱ�׼���ü��ռ�����
	projcoords = projcoords*0.5f+0.5f;//��-1��1תΪ0��1
	
	float shadow = 0.0f;//�Ƿ�����Ӱ�жϣ�1Ϊ�ڣ�0Ϊ����
	float shadowColor = 0.0f;//��Ӱ��ǳ
	
	/*��õ��ڹ�Դ�ӽǵ�ƽ�������*/
	vec2 texelSize = 5.0f / textureSize(shadowmap, 0);//�����ڵ���ƽ����ȵľ����С������Խ����ӰԽԲ��
	float d_Block = 0.0f;//�ڵ���ƽ����ȣ���Դ�ӽǱ�׼���ü��ռ����꣩
	int d_BlockCount = 0;//�ڵ������
	//��������������Խ��Ч��Խ��ϸ
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowmap, projcoords.xy + vec2(x, y) * texelSize).r; //��������С��ȣ���Դ�ӽǱ�׼���ü��ռ����꣩
			if(projcoords.z > pcfDepth+0.005)//��������С���+0.005�Ͳ�����ʵ����ȣ���Դ�ӽǱ�׼���ü��ռ����꣩�Ƚ�
			{
				shadow ++;
				d_Block += pcfDepth;
				d_BlockCount ++;
			}
		}    
	}
	shadow /= 9.0f;//����õ��Ƿ�����Ӱ��ƽ������
	d_Block /= d_BlockCount;//�����ڵ����ƽ����ȣ���Դ�ӽǱ�׼���ü��ռ����꣩
	
	
	float d_Receiver = projcoords.z;//������Ӱ�����ȣ���Դ�ӽǱ�׼���ü��ռ����꣩
	if(shadow>0.0f)
	{
		/*��õ�����ڽ�����Ӱ�ľ���ϵ������Ϊ����ƽ������Ӱ��ǳ�ľ����С*/
		shadowColor = 0.0f;
		float w = (d_Receiver-d_Block)/d_Block*10.0f;
		vec2 WSize = w / textureSize(shadowmap, 0); //����ƽ������Ӱ��ǳ�ľ����С������Խ����ӰԽ��
		/*��õ��ڹ�Դ�ӽǵ�ƽ��������Ӱ��ǳ*/
		//��������������Խ��Ч��Խ��ϸ
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
			
				float pcfDepth = texture(shadowmap, projcoords.xy + vec2(x, y) * WSize).r;//��������С��ȣ���Դ�ӽǱ�׼���ü��ռ����꣩
				if(projcoords.z > pcfDepth+0.005)//��������С���+0.005�Ͳ�����ʵ����ȣ���Դ�ӽǱ�׼���ü��ռ����꣩�Ƚ�
				{
					shadowColor ++;
				}
				
			}    
		}
		shadowColor /= 9.0f;//����õ��ƽ������Ӱ��ǳ
		//shadowColor = pow(shadowColor,5.0f);//ʹ��Ӱ�߽磨shdaowColor�ӽ�0�ĵط������ӽ�͸��������������Ը��ѣ�����Խ����ӰԽ����
	}
	else
	{
		shadowColor = 0.0f;
	}
	
	
	






	if(projcoords.z>1.0f)
	shadowColor=0.0f;
	
	color =vec4(vec3(0.0f,0.0f,0.0f),shadowColor); 
	//color =vec4(1.0f,0.0f,0.0f,1.0f); 

}