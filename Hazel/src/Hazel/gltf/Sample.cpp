#include "hzpch.h"
#pragma warning(disable : 26451)
#define _CRT_SECURE_NO_WARNINGS
#include "Sample.h"
#include "GLTFLoader.h"
#include "Uniform.h"
#include <glad/glad.h>


void Sample::Initialize() {
	cgltf_data* gltf = LoadGLTFFile("res/models/Woman.gltf");//加载文件
	mGPUMeshes = LoadMeshes(gltf);//保存mesh的数据
	mSkeleton = LoadSkeleton(gltf);//保存骨骼的数据
	mClips = LoadAnimationClips(gltf);//保存动画片段的数据
	FreeGLTFFile(gltf);//释放文件

	for (unsigned int i = 0, size = (unsigned int)mGPUMeshes.size(); i < size; ++i) {
		mGPUMeshes[i].UpdateOpenGLBuffers();//向GPU Buffer 生成缓冲
	}

	mSkinnedShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");//加载shader
	mDiffuseTexture = new Texture("Assets/Woman.png");//加载贴图

	mGPUAnimInfo.mAnimatedPose = mSkeleton.GetRestPose();//给当前动画实例指定：pose
	mGPUAnimInfo.mPosePalette.resize(mSkeleton.GetRestPose().Size());//给当前动画实例指定：pose的动画变换矩阵数组

	mGPUAnimInfo.mModel.position = vec3(0, 0, 0);//给当前动画实例指定：模型的位置

	unsigned int numUIClips = (unsigned int)mClips.size();
	for (unsigned int i = 0; i < numUIClips; ++i)
	{
		if (mClips[i].GetName() == "Walking") {
			mGPUAnimInfo.mClip = i;//给当前动画实例指定：动画片段的选择
		}
	}
}

void Sample::Update(float deltaTime) {
	mGPUAnimInfo.mPlayback = mClips[mGPUAnimInfo.mClip].Sample(mGPUAnimInfo.mAnimatedPose, mGPUAnimInfo.mPlayback + deltaTime);//给当前动画实例指定：当前动画播放时刻
	mGPUAnimInfo.mAnimatedPose.GetMatrixPalette(mGPUAnimInfo.mPosePalette);//给当前动画实例指定：应用pose的动画变换矩阵数组
}

void Sample::Render(float inAspectRatio, std::shared_ptr<Hazel::Camera>& camera, std::shared_ptr<Hazel::Light>& light) {
	//MVP矩阵
	mat4 projection = perspective(camera->cameraFov, inAspectRatio, 0.1f, 10000.0f);
	mat4 view = lookAt(vec3(camera->cameraPos.x, camera->cameraPos.y, camera->cameraPos.z),vec3(camera->cameraPos.x+camera->cameraFront.x, camera->cameraPos.y + camera->cameraFront.y, camera->cameraPos.z + camera->cameraFront.z), vec3(camera->cameraUp.x, camera->cameraUp.y, camera->cameraUp.z));
	
	mat4 model;


	// GPU Skinned Mesh
	model = transformToMat4(mGPUAnimInfo.mModel);//根据模型的位置指定M矩阵
	mSkinnedShader->Bind();
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mSkinnedShader->GetUniform("light"), vec3(light->Pos.x, light->Pos.y, light->Pos.z));

	Uniform<mat4>::Set(mSkinnedShader->GetUniform("pose"), mGPUAnimInfo.mPosePalette);//向shader传输pose的动画变换矩阵数组
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("invBindPose"), mSkeleton.GetInvBindPose());//向shader传输反转绑定姿势矩阵数组

	mDiffuseTexture->Set(mSkinnedShader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mGPUMeshes.size(); i < size; ++i) {
		mGPUMeshes[i].Bind(mSkinnedShader->GetAttribute("position"), mSkinnedShader->GetAttribute("normal"), mSkinnedShader->GetAttribute("texCoord"), mSkinnedShader->GetAttribute("weights"), mSkinnedShader->GetAttribute("joints"));//向缓冲注入数据
		mGPUMeshes[i].Draw();//绘制
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
