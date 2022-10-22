#include "hzpch.h"
#pragma warning(disable : 26451)
#define _CRT_SECURE_NO_WARNINGS
#include "Sample.h"
#include "GLTFLoader.h"
#include "Uniform.h"
#include <glad/glad.h>


void Sample::Initialize() {
	cgltf_data* gltf = LoadGLTFFile("res/models/Woman.gltf");//�����ļ�
	mGPUMeshes = LoadMeshes(gltf);//����mesh������
	mSkeleton = LoadSkeleton(gltf);//�������������
	mClips = LoadAnimationClips(gltf);//���涯��Ƭ�ε�����
	FreeGLTFFile(gltf);//�ͷ��ļ�

	for (unsigned int i = 0, size = (unsigned int)mGPUMeshes.size(); i < size; ++i) {
		mGPUMeshes[i].UpdateOpenGLBuffers();//��GPU Buffer ���ɻ���
	}

	mSkinnedShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");//����shader
	mDiffuseTexture = new Texture("Assets/Woman.png");//������ͼ

	mGPUAnimInfo.mAnimatedPose = mSkeleton.GetRestPose();//����ǰ����ʵ��ָ����pose
	mGPUAnimInfo.mPosePalette.resize(mSkeleton.GetRestPose().Size());//����ǰ����ʵ��ָ����pose�Ķ����任��������

	mGPUAnimInfo.mModel.position = vec3(0, 0, 0);//����ǰ����ʵ��ָ����ģ�͵�λ��

	unsigned int numUIClips = (unsigned int)mClips.size();
	for (unsigned int i = 0; i < numUIClips; ++i)
	{
		if (mClips[i].GetName() == "Walking") {
			mGPUAnimInfo.mClip = i;//����ǰ����ʵ��ָ��������Ƭ�ε�ѡ��
		}
	}
}

void Sample::Update(float deltaTime) {
	mGPUAnimInfo.mPlayback = mClips[mGPUAnimInfo.mClip].Sample(mGPUAnimInfo.mAnimatedPose, mGPUAnimInfo.mPlayback + deltaTime);//����ǰ����ʵ��ָ������ǰ��������ʱ��
	mGPUAnimInfo.mAnimatedPose.GetMatrixPalette(mGPUAnimInfo.mPosePalette);//����ǰ����ʵ��ָ����Ӧ��pose�Ķ����任��������
}

void Sample::Render(float inAspectRatio, std::shared_ptr<Hazel::Camera>& camera, std::shared_ptr<Hazel::Light>& light) {
	//MVP����
	mat4 projection = perspective(camera->cameraFov, inAspectRatio, 0.1f, 10000.0f);
	mat4 view = lookAt(vec3(camera->cameraPos.x, camera->cameraPos.y, camera->cameraPos.z),vec3(camera->cameraPos.x+camera->cameraFront.x, camera->cameraPos.y + camera->cameraFront.y, camera->cameraPos.z + camera->cameraFront.z), vec3(camera->cameraUp.x, camera->cameraUp.y, camera->cameraUp.z));
	
	mat4 model;


	// GPU Skinned Mesh
	model = transformToMat4(mGPUAnimInfo.mModel);//����ģ�͵�λ��ָ��M����
	mSkinnedShader->Bind();
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mSkinnedShader->GetUniform("light"), vec3(light->Pos.x, light->Pos.y, light->Pos.z));

	Uniform<mat4>::Set(mSkinnedShader->GetUniform("pose"), mGPUAnimInfo.mPosePalette);//��shader����pose�Ķ����任��������
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("invBindPose"), mSkeleton.GetInvBindPose());//��shader���䷴ת�����ƾ�������

	mDiffuseTexture->Set(mSkinnedShader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mGPUMeshes.size(); i < size; ++i) {
		mGPUMeshes[i].Bind(mSkinnedShader->GetAttribute("position"), mSkinnedShader->GetAttribute("normal"), mSkinnedShader->GetAttribute("texCoord"), mSkinnedShader->GetAttribute("weights"), mSkinnedShader->GetAttribute("joints"));//�򻺳�ע������
		mGPUMeshes[i].Draw();//����
		mGPUMeshes[i].UnBind(mSkinnedShader->GetAttribute("position"), mSkinnedShader->GetAttribute("normal"), mSkinnedShader->GetAttribute("texCoord"), mSkinnedShader->GetAttribute("weights"), mSkinnedShader->GetAttribute("joints"));
	}
	mDiffuseTexture->UnSet(0);
	mSkinnedShader->UnBind();
}

void Sample::Shutdown() {

	delete mDiffuseTexture;
	delete mSkinnedShader;
	mClips.clear();
	mGPUMeshes.clear();
}
