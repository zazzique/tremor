
#pragma once

#include "Common.h"
#include "Renderer.h"
#include <string>
#include <vector>

class Texture
{
private:
	static std::vector<Texture *> textures;
	static Texture* current_texture;

	std::string filename;
	U32 renderer_texture_id = -1;
	int width = 0;
	int height = 0;
	bool clamped = false;
	bool nearest = false;

	Renderer* renderer = nullptr;

private:
	Texture(const std::string& _name, const bool _clamped, const bool _nearest, Renderer* const _renderer);
	~Texture();

public:
	static Texture* Create(const std::string& name, bool clamped, bool nearest, Renderer* const _renderer);
	static void Delete(Texture *texture);
	
	void Bind();

private:
	static bool Load(Texture& texture);
};

