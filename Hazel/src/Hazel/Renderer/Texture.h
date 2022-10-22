#pragma once
#include <glad/glad.h>
#include "stb_image.h"
class Texture
{
private:
	unsigned int RendererID;
	std::string FilePath;
	unsigned char* LocalBuffer;
	int Width, Height, BBP;//bits per pixel

public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0) const;//Ĭ������Ϊ0����TEXCOORD0��
	void Unbind() const;

	inline int GetWidth() const { return Width; }
	inline int GetHeight() const { return Height; }
};