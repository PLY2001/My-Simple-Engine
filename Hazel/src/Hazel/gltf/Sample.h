#ifndef _H_SAMPLE_
#define _H_SAMPLE_

//#include "Application.h"
#include <vector>
#include "Pose.h"
#include "Clip.h"
#include "Skeleton.h"
#include "gltfMesh.h"
#include "Texture.h"
#include "gltfShader.h"
#include "Renderer/Camera.h"
#include "Renderer/Light.h"


struct AnimationInstance {
	Pose mAnimatedPose;
	std::vector <mat4> mPosePalette;
	unsigned int mClip;
	float mPlayback;
	Transform mModel;

	inline AnimationInstance() : mClip(0), mPlayback(0.0f) { }
};

class Sample {
protected:
	Texture* mDiffuseTexture;
	Shader* mSkinnedShader;
	std::vector<Mesh> mGPUMeshes;
	Skeleton mSkeleton;
	std::vector<Clip> mClips;

	AnimationInstance mGPUAnimInfo;
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio, std::shared_ptr<Hazel::Camera>& camera, std::shared_ptr<Hazel::Light>& light);
	void Shutdown();
};

#endif