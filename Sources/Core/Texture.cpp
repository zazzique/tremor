
#include <algorithm>

#include "Common.h"

#include "Files.h"
#include "Utils.h"
#include "Renderer.h"
#include "Texture.h"


Texture* Texture::current_texture = nullptr;

Texture::Texture(const std::string& _name, const bool _clamped, const bool _nearest, Renderer* const _renderer)
{
	filename = _name;
	clamped = _clamped;
	nearest = _nearest;
	renderer = _renderer;

	if (textures.capacity() == 0)
		textures.reserve(512);
}

Texture::~Texture()
{
	if (renderer)
		renderer->DeleteTexture(&renderer_texture_id);
        
	current_texture = nullptr;
}

void Texture::Delete(Texture *texture)
{
	textures.erase(std::remove(textures.begin(), textures.end(), texture), textures.end());
}

Texture* Texture::Create(const std::string& name, bool clamped, bool nearest, Renderer* const renderer)
{
	assert(textures.size() >= textures.max_size() - 1);

	for (auto tex : textures)
	{
		if (tex->filename == name)
			return tex;
	}

	Texture* texture = new Texture(name, clamped, nearest, renderer);

	if (texture == nullptr)
		return nullptr;

	if (Texture::Load(*texture))
	{
		textures.push_back(texture);
	}
	else
	{
		if (texture != nullptr)
		{
			delete texture;
			texture = nullptr;
		}
	}
	
	return texture;
}

bool Texture::Load(Texture& texture)
{
	if (texture.renderer == nullptr)
		return false;

	FileHandler tex_file;

	if (!Files_OpenFile(&tex_file, texture.filename.c_str()))
	{
		Log::Print("Error: texture '%s' not found!\n", texture.filename.c_str());
		return false;
	}
	
	if (strcicmp(Files_GetFileExtension(&tex_file), "tga") == 0) // TGA files
	{
		struct TGAHeader
		{
			U8  identsize;          // size of ID field that follows 18 byte header (0 usually)
			U8  colourmaptype;      // type of colour map 0=none, 1=has palette
			U8  imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed
	
			U16 colourmapstart;     // first colour map entry in palette
			U16 colourmaplength;    // number of colours in palette
			U8  colourmapbits;      // number of bits per palette entry 15,16,24,32
	
			U16 xstart;             // image x origin
			U16 ystart;             // image y origin
			U16 width;              // image width in pixels
			U16 height;             // image height in pixels
			U8  bits;               // image bits per pixel 8,16,24,32
			U8  descriptor;         // image descriptor bits (vh flip bits)
		};

		TGAHeader header;
		
		Files_Read(&tex_file, &header.identsize, sizeof(U8));
		Files_Read(&tex_file, &header.colourmaptype, sizeof(U8));
		Files_Read(&tex_file, &header.imagetype, sizeof(U8));
		Files_Read(&tex_file, &header.colourmapstart, sizeof(U16));
		Files_Read(&tex_file, &header.colourmaplength, sizeof(U16));
		Files_Read(&tex_file, &header.colourmapbits, sizeof(U8));
		Files_Read(&tex_file, &header.xstart, sizeof(U16));
		Files_Read(&tex_file, &header.ystart, sizeof(U16));
		Files_Read(&tex_file, &header.width, sizeof(U16));
		Files_Read(&tex_file, &header.height, sizeof(U16));
		Files_Read(&tex_file, &header.bits, sizeof(U8));
		Files_Read(&tex_file, &header.descriptor, sizeof(U8));
		
		if (header.colourmaptype != 0 || (header.imagetype != 2 && header.imagetype != 3))
		{
			Log::Print("Error: unsupported tga format!\n");
			Files_CloseFile(&tex_file);
			return false;
		}

		if (header.width < 2 || header.height < 2)
		{
			Log::Print("Error: image is too small!\n");
			Files_CloseFile(&tex_file);
			return false;
		}
		
		int current_pos;
		
		if (header.bits == 8)
		{
			U8 *image_data = (U8 *)malloc(header.width * header.height);

			if (image_data == nullptr)
			{
				Log::Print("Error: couldn't allocate memory!\n");
				Files_CloseFile(&tex_file);
				return false;
			}
			
			for (int j = 0; j < header.height; j++)
			{
				for (int i = 0; i < header.width; i++)
				{
					if (header.descriptor & 0x20)
						current_pos = (j * header.width + i);
					else
						current_pos = ((header.height - (j + 1)) * header.width + i);
					
					
					Files_Read(&tex_file, &image_data[current_pos], sizeof(U8));
				}
			}
			
	        texture.renderer->CreateTexture(&texture.renderer_texture_id, image_data, header.width, header.height, header.bits, false, 0, texture.clamped, texture.nearest);
			
			free(image_data);
		}
		else if (header.bits == 24)
		{
			U8 *image_data = (U8 *)malloc(header.width * header.height * 3);

			if (image_data == nullptr)
			{
				Log::Print("Error: couldn't allocate memory!\n");
				Files_CloseFile(&tex_file);
				return false;
			}
			
			for (int j = 0; j < header.height; j++)
			{
				for (int i = 0; i < header.width; i++)
				{
					if (header.descriptor & 0x20)
						current_pos = (j * header.width + i) * 3;
					else
						current_pos = ((header.height - (j + 1)) * header.width + i) * 3;
					
					Files_Read(&tex_file, &image_data[current_pos + 2], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 1], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 0], sizeof(U8));
				}
			}
			
            texture.renderer->CreateTexture(&texture.renderer_texture_id, image_data, header.width, header.height, header.bits, false, 0, texture.clamped, texture.nearest);
			
			free(image_data);
		}
		else if (header.bits == 32)
		{
			U8 *image_data = (U8 *)malloc(header.width * header.height * 4);

			if (image_data == nullptr)
			{
				Log::Print("Error: couldn't allocate memory!\n");
				Files_CloseFile(&tex_file);
				return false;
			}
			
			for (int j = 0; j < header.height; j++)
			{
				for (int i = 0; i < header.width; i++)
				{
					if (header.descriptor & 0x20)
						current_pos = (j * header.width + i) * 4;
					else
						current_pos = ((header.height - (j + 1)) * header.width + i) * 4;
					
					Files_Read(&tex_file, &image_data[current_pos + 2], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 1], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 0], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 3], sizeof(U8));
				}
			}
			
			texture.renderer->CreateTexture(&texture.renderer_texture_id, image_data, header.width, header.height, header.bits, false, 0, texture.clamped, texture.nearest);
			
			free(image_data);
		}
		
		texture.width = header.width;
		texture.height = header.height;
	}
	else
	{
		Log::Print("Error: unsupported texture format!\n");
		Files_CloseFile(&tex_file);
		return false;
	}

	Files_CloseFile(&tex_file);
	
	return true;
}

void Texture::Bind()
{
	if (std::find(textures.begin(), textures.end(), this) == textures.end())
		return;

	if (current_texture == this)
		return;
	
	if (renderer != nullptr)
	{
		renderer->BindTexture(renderer_texture_id);
		current_texture = this;
	}
}

