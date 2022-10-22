#shader vertex
#version 330 core 

layout(location=0) in vec3 position; 
layout(location=3) in mat4 model; 

void main() 
{ 
	gl_Position = model*vec4(position,1.0f);//��ֻM�任
}

#shader geometry
#version 330 core 
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];
out vec4 FragPos; // �ɼ�����ɫ�������õ����������

void main()
{
    for(int face = 0; face < 6; ++face)//��6������ĵ�
    {
        gl_Layer = face; //ͨ�����ڽ�����gl_Layer��ֵ��ѡ��ǰ����
        for(int i = 0; i < 3; ++i) //��ǰ����������ε�3��
        {
            FragPos = gl_in[i].gl_Position;//�÷���ÿ�������������
            gl_Position = shadowMatrices[face] * FragPos;//��ɸ÷���ÿ�����VP�任
            EmitVertex();
        }    
        EndPrimitive();
    }
}

#shader fragment
#version 330 core

in vec4 FragPos;
uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);//������������ϵ�й�Դ���õ�ľ���
    lightDistance = lightDistance / far_plane;//�������׼��Ϊ��Χ0��1
    gl_FragDepth = lightDistance;//���ڽ�����gl_FragDepth��ֵ����������ı�׼��������Ϊ�õ����
}
