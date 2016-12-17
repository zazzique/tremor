
#pragma once

class Texture
{
private:
	int index = -1;
}

class TextureManager
{
	TextureManager();
	~TextureManager();

	void GetTextureResolutionByName(char *name, I32 *width, I32 *height);
	int	AddTexture(char *filename, bool clamped, bool nearest);
	bool LoadTexture(int index, char *filename, bool clamped, bool nearest);
	bool UnloadTexture(int index);
	int GetTextureIndexByName(char *name);
	void SetTextureByIndex(int index);
	void SetTextureByName(char *name);
	void RemoveTextureByIndex(int index);
	void RemoveTextureByName(char *name);
	void RemoveAllTextures();
};

