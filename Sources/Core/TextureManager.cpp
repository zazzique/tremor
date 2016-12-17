
#include "Common.h"

#include "Files.h"
#include "Utils.h"
#include "FastMath.h"
#include "Vector.h"
#include "Render.h"
#include "TextureManager.h"


#define MAX_TEX_COUNT 512

typedef struct _TGAHeader
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
} TGAHeader;

typedef struct _Texture
{
	char filename[32];
	U32 texture;
	I32 width;
	I32 height;
	U32 flags;
	bool loaded;
	bool valid;
}
Texture;

Texture textures[MAX_TEX_COUNT];
int current_texture_index;

U8 palette[256][3];

void TexManager_Init()
{
	for (int i = 0; i < MAX_TEX_COUNT; i ++)
	{
		textures[i].loaded = false;
		textures[i].valid = false;
	}
	
	current_texture_index = -1;

	FileHandler palette_file;

	if (Files_OpenFile(&palette_file, "CHASM2.PAL"))
	{
		Files_Read(&palette_file, palette, 768);
		Files_CloseFile(&palette_file);
	}

	for (int i = 0; i < 256; i ++)
	{
		palette[i][0] = CLAMP((U32)palette[i][0] * 4, 0, 255);
		palette[i][1] = CLAMP((U32)palette[i][1] * 4, 0, 255);
		palette[i][2] = CLAMP((U32)palette[i][2] * 4, 0, 255);
	}
}


void TexManager_GetTextureResolutionByName(char *name, I32 *width, I32 *height)
{
	int index = TexManager_GetTextureIndexByName(name);
	
	if (index < 0) return;
	
	*width = textures[index].width;
	*height = textures[index].height;
}


int	TexManager_AddTexture(char *filename, U32 user_flags)
{
	int index = TexManager_GetTextureIndexByName(filename);
	if (index >= 0)
		return index;
	
	index = -1;
	
	for (int i = 0; i < MAX_TEX_COUNT; i++)
	{
		if (!textures[i].valid)
		{
			index = i;
			break;
		}
	}
	
	if (index < 0)
	{
		LogPrint("Error: too many textures!\n");
		return -1;
	}

	bool clamped = false;

	if (user_flags & TEXTURE_FLAG_CLAMPED)
		clamped = true;

	bool nearest = false;

	if (user_flags & TEXTURE_FLAG_NEAREST)
		nearest = true;

	if (!TexManager_LoadTexture(index, filename, clamped, nearest))
		return -1;
	
	textures[index].flags = user_flags;
	textures[index].valid = true;

	return index;
}

int	 TexManager_AddTextureFromIndexedData(const U8* data, I32 width, I32 height, U32 user_flags, bool save)
{
	int index = -1;

	if (data == nullptr)
		return -1;
	
	for (int i = 0; i < MAX_TEX_COUNT; i++)
	{
		if (!textures[i].valid)
		{
			index = i;
			break;
		}
	}
	
	if (index < 0)
	{
		LogPrint("Error: too many textures!\n");
		return -1;
	}

	bool clamped = false;

	if (user_flags & TEXTURE_FLAG_CLAMPED)
		clamped = true;

	bool nearest = false;

	if (user_flags & TEXTURE_FLAG_NEAREST)
		nearest = true;

	
	const I32 bits = 32;
	
	U8 *image_data = (U8 *)malloc(width * height * 4);

	if (image_data == nullptr)
		return -1;

	int current_pos = 0;
	
	for (int i = 0; i < width * height; i ++)
	{
		U8 color_index = data[i];

		image_data[current_pos + 0] = palette[color_index][0];
		image_data[current_pos + 1] = palette[color_index][1];
		image_data[current_pos + 2] = palette[color_index][2];
		image_data[current_pos + 3] = (color_index == 0xff) ? 0x00 : 0xff;

		current_pos += 4;
	}
	
    Render_CreateTexture(&textures[index].texture, image_data, width, height, bits, false, 0, clamped, nearest);

	////////////////////////////////

	if (save)
	{
		int current_pos = 0;
	
		for (int i = 0; i < width * height; i ++)
		{
			U8 color_index = data[i];

			image_data[current_pos + 0] = palette[color_index][2];
			image_data[current_pos + 1] = palette[color_index][1];
			image_data[current_pos + 2] = palette[color_index][0];
			image_data[current_pos + 3] = (color_index == 0xff) ? 0x00 : 0xff;

			current_pos += 4;
		}

		TGAHeader tga_header = { 0 };

		tga_header.imagetype = 2;
		tga_header.width = width;
		tga_header.height = height;
		tga_header.bits = 4 * 8;
		tga_header.descriptor = 0x20;

		FILE *rsc_fp = fopen("WALLS_1x_chasm.tga", "wb" );
		if (rsc_fp != nullptr)
		{
			fwrite(&tga_header.identsize, 1, 1, rsc_fp);
			fwrite(&tga_header.colourmaptype, 1, 1, rsc_fp);
			fwrite(&tga_header.imagetype, 1, 1, rsc_fp);

			fwrite(&tga_header.colourmapstart, 2, 1, rsc_fp);
			fwrite(&tga_header.colourmaplength, 2, 1, rsc_fp);
			fwrite(&tga_header.colourmapbits, 1, 1, rsc_fp);

			fwrite(&tga_header.xstart, 2, 1, rsc_fp);
			fwrite(&tga_header.ystart, 2, 1, rsc_fp);
			fwrite(&tga_header.width, 2, 1, rsc_fp);
			fwrite(&tga_header.height, 2, 1, rsc_fp);
			fwrite(&tga_header.bits, 1, 1, rsc_fp);
			fwrite(&tga_header.descriptor, 1, 1, rsc_fp);
 
			fwrite(image_data, tga_header.width * tga_header.height * (tga_header.bits / 8), 1, rsc_fp);

			fclose(rsc_fp);
		}
	}
	///////////////////////////////

	free(image_data);
	
	textures[index].width = width;
	textures[index].height = height;
	
	textures[index].flags = user_flags;
	textures[index].loaded = true; // TODO: do not unload, or should be pointer to data

	textures[index].valid = true;

	return index;
}

bool TexManager_LoadTexture(int index, char *filename, bool clamped, bool nearest)
{
	if (index < 0 || index >= MAX_TEX_COUNT)
		return false;

	if (textures[index].loaded)
		return true;

	int i, j;
	
	if (filename != textures[index].filename)
		strncpy(textures[index].filename, filename, 31);

	FileHandler tex_file;

	if (!Files_OpenFileAltType(&tex_file, filename, "crt"))
	{
		if (!Files_OpenFileAltType(&tex_file, filename, "pkm"))
		{
			if (!Files_OpenFileAltType(&tex_file, filename, "cel"))
			{
				LogPrint("Error: texture '%s' not found!\n", filename);
				return false;
			}
		}
	}
	
	if (strcicmp(Files_GetFileExtension(&tex_file), "tga") == 0) // TGA files
	{
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
			LogPrint("Error: unsupported tga format!\n");
			Files_CloseFile(&tex_file);
			return false;
		}

		if (header.width < 2 || header.height < 2)
		{
			LogPrint("Error: image is too small!\n");
			Files_CloseFile(&tex_file);
			return false;
		}
		
		int current_pos;
		
		if (header.bits == 8)
		{
			U8 *image_data = (U8 *)malloc(header.width * header.height);

			if (image_data == nullptr)
			{
				LogPrint("Error: couldn't allocate memory!\n");
				Files_CloseFile(&tex_file);
				return false;
			}
			
			for (j = 0; j < header.height; j++)
			{
				for (i = 0; i < header.width; i++)
				{
					if (header.descriptor & 0x20)
						current_pos = (j * header.width + i);
					else
						current_pos = ((header.height - (j + 1)) * header.width + i);
					
					
					Files_Read(&tex_file, &image_data[current_pos], sizeof(U8));
				}
			}
			
	        Render_CreateTexture(&textures[index].texture, image_data, header.width, header.height, header.bits, false, 0, clamped, nearest);
			
			free(image_data);
		}
		else if (header.bits == 24)
		{
			U8 *image_data = (U8 *)malloc(header.width * header.height * 3);

			if (image_data == nullptr)
			{
				LogPrint("Error: couldn't allocate memory!\n");
				Files_CloseFile(&tex_file);
				return false;
			}
			
			for (j = 0; j < header.height; j++)
			{
				for (i = 0; i < header.width; i++)
				{
					if (header.descriptor & 0x20)
						current_pos = (j * header.width + i) * 3;
					else
						current_pos = ((header.height - (j + 1)) * header.width + i) * 3;
					
					Files_Read(&tex_file, &image_data[current_pos + 2], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 1], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 0], sizeof(U8));
					
					//ApplyColorModifications(&image_data[current_pos + 2], &image_data[current_pos + 1], &image_data[current_pos + 0]);
				}
			}
			
            Render_CreateTexture(&textures[index].texture, image_data, header.width, header.height, header.bits, false, 0, clamped, nearest);
			
			free(image_data);
		}
		else if (header.bits == 32)
		{
			U8 *image_data = (U8 *)malloc(header.width * header.height * 4);

			if (image_data == nullptr)
			{
				LogPrint("Error: couldn't allocate memory!\n");
				Files_CloseFile(&tex_file);
				return false;
			}
			
			for (j = 0; j < header.height; j++)
			{
				for (i = 0; i < header.width; i++)
				{
					if (header.descriptor & 0x20)
						current_pos = (j * header.width + i) * 4;
					else
						current_pos = ((header.height - (j + 1)) * header.width + i) * 4;
					
					Files_Read(&tex_file, &image_data[current_pos + 2], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 1], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 0], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 3], sizeof(U8));
					
					//ApplyColorModifications(&image_data[current_pos + 2], &image_data[current_pos + 1], &image_data[current_pos + 0]);
				}
			}
			
			Render_CreateTexture(&textures[index].texture, image_data, header.width, header.height, header.bits, false, 0, clamped, nearest);
			
			free(image_data);
		}
		
		textures[index].width = header.width;
		textures[index].height = header.height;
	}
	else if (strcicmp(Files_GetFileExtension(&tex_file), "pkm") == 0) // PKM files
	{
		U8 header[4];
		U16 width, height;

		Files_Read(&tex_file, &header[0], sizeof(header));

		if (header[0] != 'P' || header[1] != 'K' || header[2] != 'M' || header[3] != ' ')
		{
			LogPrint("Error: unsupported texture format!\n");
			Files_CloseFile(&tex_file);
			return false;
		}

		Files_Skip(&tex_file, sizeof(U16));
		Files_Skip(&tex_file, sizeof(U16));

		Files_Read(&tex_file, &header[0], sizeof(U16));
		width = (((U16)header[0]) << 8) | (U16)header[1];
		Files_Read(&tex_file, &header[0], sizeof(U16));
		height = (((U16)header[0]) << 8) | (U16)header[1];

		Files_Skip(&tex_file, sizeof(U16));
		Files_Skip(&tex_file, sizeof(U16));

		U8 *image_data = nullptr;
		Files_GetData(&tex_file, (void **)&image_data, nullptr);

		if (image_data == nullptr)
		{
			LogPrint("Error: couldn't get texture data!\n");
			Files_CloseFile(&tex_file);
			return false;
		}

		U32 data_size = ((width / 4) * (height / 4)) * 8;

		I32 current_pos = Files_GetCurrentPos(&tex_file);

		Render_CreateTexture(&textures[index].texture, &image_data[current_pos], width, height, 24, true, data_size, clamped, nearest);

		textures[index].width = width;
		textures[index].height = height;
	}
	else if (strcicmp(Files_GetFileExtension(&tex_file), "cel") == 0) // CEL files
	{
		const I32 bits = 32;

		U16 width, height;

		Files_Skip(&tex_file, sizeof(U16));
		Files_Read(&tex_file, &width, sizeof(U16));
		Files_Read(&tex_file, &height, sizeof(U16));
		Files_Skip(&tex_file, 26);
		Files_Skip(&tex_file, 768);

		U8 *image_data = (U8 *)malloc(width * height * 4);

		if (image_data == nullptr)
		{
			LogPrint("Error: couldn't allocate memory!\n");
			Files_CloseFile(&tex_file);
			return false;
		}

		int current_pos = 0;
		
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				U8 color_index;
				Files_Read(&tex_file, &color_index, sizeof(U8));

				image_data[current_pos + 0] = palette[color_index][0] * 4;
				image_data[current_pos + 1] = palette[color_index][1] * 4;
				image_data[current_pos + 2] = palette[color_index][2] * 4;
				image_data[current_pos + 3] = (color_index == 0xff) ? 0x00 : 0xff;

				current_pos += 4;
			}
		}
		
        Render_CreateTexture(&textures[index].texture, image_data, width, height, bits, false, 0, clamped, nearest);
		
		free(image_data);
		
		textures[index].width = width;
		textures[index].height = height;
	}
	else if (strcicmp(Files_GetFileExtension(&tex_file), "crt") == 0) // CRT files
	{
		U16 width, height;
		U8 bits;
		
		Files_Read(&tex_file, &width, sizeof(U16));
		Files_Read(&tex_file, &height, sizeof(U16));
		Files_Read(&tex_file, &bits, sizeof(U8));

		U8 *image_data = nullptr;
		Files_GetData(&tex_file, (void **)&image_data, nullptr);

		if (image_data == nullptr)
		{
			LogPrint("Error: couldn't get texture data!\n");
			Files_CloseFile(&tex_file);
			return false;
		}

		if (bits & 128)
		{
			bits &= 127;

			U32 pvrtc_rawsize = 0;
			Files_Read(&tex_file, &pvrtc_rawsize, sizeof(U32));
			
			I32 current_pos = Files_GetCurrentPos(&tex_file);

			Render_CreateTexture(&textures[index].texture, &image_data[current_pos], width, height, bits, true, pvrtc_rawsize, clamped, nearest);
		}
		else
		{
			I32 current_pos = Files_GetCurrentPos(&tex_file);

            Render_CreateTexture(&textures[index].texture, &image_data[current_pos], width, height, bits, false, 0, clamped, nearest);
		}
		
		textures[index].width = width;
		textures[index].height = height;
	}
	else
	{
		LogPrint("Error: unsupported texture format!\n");
		Files_CloseFile(&tex_file);
		return false;
	}

	current_texture_index = index;
	
	textures[index].loaded = true;

	Files_CloseFile(&tex_file);
	
	return true;
}

bool TexManager_UnloadTexture(int index)
{
	if (index < 0 || index >= MAX_TEX_COUNT)
		return false;

	Render_DeleteTexture(&textures[index].texture);
	textures[index].loaded = false;
        
	return true;
}

void TexManager_LoadAll()
{
	for (int i = 0; i < MAX_TEX_COUNT; i++)
		if (textures[i].valid)
			if (!textures[i].loaded)
				TexManager_LoadTexture(i, textures[i].filename, (textures[i].flags & TEXTURE_FLAG_CLAMPED), (textures[i].flags & TEXTURE_FLAG_NEAREST));
}

void TexManager_UnloadAll()
{
	for (int i = 0; i < MAX_TEX_COUNT; i++)
		if (textures[i].valid)
			if (textures[i].loaded)
				TexManager_UnloadTexture(i);
}

int TexManager_GetTextureIndexByName(char *name)
{
	for (int i = 0; i < MAX_TEX_COUNT; i++)
		if (textures[i].valid)
			if (textures[i].filename[0] == name[0])
			{
				if (strcicmp(textures[i].filename, name) == 0)
					return i;
			}
	
	return -1;
}


void TexManager_SetTextureByIndex(int index)
{
	if (index == current_texture_index)
		return;
	
	if (index >= 0 && index < MAX_TEX_COUNT)
	{
		if (textures[index].valid)
		{
			if (!textures[index].loaded)
			{
				TexManager_LoadTexture(index, textures[index].filename, (textures[index].flags & TEXTURE_FLAG_CLAMPED), (textures[index].flags & TEXTURE_FLAG_NEAREST));
			}
			else
			{
				Render_BindTexture(textures[index].texture);
				current_texture_index = index;
			}
		}
	}
	else
	{
		current_texture_index = -1;
	}
}


void TexManager_SetTextureByName(char *name)
{
	int index = TexManager_GetTextureIndexByName(name);
	
	TexManager_SetTextureByIndex(index);
}


void TexManager_RemoveTextureByIndex(int index)
{
	if (index >= 0 && index < MAX_TEX_COUNT)
	{
		if (textures[index].loaded)
			Render_DeleteTexture(&textures[index].texture);
        
		textures[index].loaded = false;
		textures[index].valid = false;
	
		if (index == current_texture_index)
			current_texture_index = -1;
	}
}


void TexManager_RemoveTextureByName(char *name)
{
	int index = TexManager_GetTextureIndexByName(name);
	
	TexManager_RemoveTextureByIndex(index);
}


void TexManager_RemoveTexturesByFlag(U32 user_flag)
{
	for (int i = 0; i < MAX_TEX_COUNT; i++)
		if (textures[i].valid)
			if (textures[i].flags & user_flag)
				TexManager_RemoveTextureByIndex(i);
}


void TexManager_RemoveAllTextures()
{
	for (int i = 0; i < MAX_TEX_COUNT; i++)
		if (textures[i].valid)
			TexManager_RemoveTextureByIndex(i);
}


void TexManager_Release()
{
	TexManager_RemoveAllTextures();
}

