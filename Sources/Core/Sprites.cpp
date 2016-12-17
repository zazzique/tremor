
#include "Common.h"

#include "Utils.h"
#include "FastMath.h"
#include "Vector.h"
#include "Render.h"
#include "TextureManager.h"
#include "Sprites.h"


#define MAX_SPRITES_IN_GROUP 128
#define MAX_SPRITES_IN_ARRAY 256
#define MAX_SPRITE_GROUPS    24

typedef struct _Sprite
{
	float width;
	float height;
	float tc_x;
	float tc_y;
	float tc_width;
	float tc_height;
}
Sprite;

typedef struct _SpriteGroup
{
	char texture[32];
	float scale_x;
	float scale_y;
	Sprite sprites[MAX_SPRITES_IN_GROUP];
	int sprites_count;
	float sprites_vertices[MAX_SPRITES_IN_ARRAY * 8];
	U32 sprites_colors[MAX_SPRITES_IN_ARRAY * 4];
	float sprites_tex_coords[MAX_SPRITES_IN_ARRAY * 8];
	int sprites_to_draw_count;
}
SpriteGroup;

SpriteGroup sprite_groups[MAX_SPRITE_GROUPS];
int sprite_groups_count;


U16 sprites_index_array[MAX_SPRITES_IN_GROUP * 6];


void Sprites_Init()
{
	sprite_groups_count = 0;
	
	for (int i = 0; i < MAX_SPRITE_GROUPS; i++)
	{
		sprite_groups[i].sprites_count = 0;
		sprite_groups[i].sprites_to_draw_count = 0;
	}

	for (int i = 0; i < MAX_SPRITES_IN_GROUP; i++)
	{
		sprites_index_array[i * 6 + 0] = (i * 4) + 0;
		sprites_index_array[i * 6 + 1] = (i * 4) + 1;
		sprites_index_array[i * 6 + 2] = (i * 4) + 2;
		sprites_index_array[i * 6 + 3] = (i * 4) + 2;
		sprites_index_array[i * 6 + 4] = (i * 4) + 3;
		sprites_index_array[i * 6 + 5] = (i * 4) + 0;
	}
}

void Sprites_AddSprite(SpriteHandler *sprite_handler, char *texture, float tc_x, float tc_y, float tc_width, float tc_height, float scale)
{
	if (sprite_handler == nullptr)
		return;
	
	int current_sprite_group = -1;
	int current_sprite = -1;
	I32 width = 0;
	I32 height = 0;

	for (int i = 0; i < sprite_groups_count; i++)
	{
		if (strcicmp(sprite_groups[i].texture, texture) == 0)
			current_sprite_group = i;
	}
	
	if (current_sprite_group < 0)
	{
		if (sprite_groups_count < MAX_SPRITE_GROUPS)
		{
			current_sprite_group = sprite_groups_count;
			
			strcpy(sprite_groups[current_sprite_group].texture, texture);
			TexManager_AddTexture(sprite_groups[current_sprite_group].texture, 0);
			
			TexManager_GetTextureResolutionByName(sprite_groups[current_sprite_group].texture, &width, &height);
			
			sprite_groups[current_sprite_group].scale_x = 1.0f / (float)width;
			sprite_groups[current_sprite_group].scale_y = 1.0f / (float)height;
			
			sprite_groups[current_sprite_group].sprites_count = 0;
			sprite_groups[current_sprite_group].sprites_to_draw_count = 0;
			
			sprite_groups_count ++;
		}
		else
		{
			LogPrint("Error: too many sprite groups!\n");
			sprite_handler = nullptr;
			return;
		}
	}
	
	current_sprite = sprite_groups[current_sprite_group].sprites_count;
	
	if (current_sprite >= MAX_SPRITES_IN_GROUP)
	{
		LogPrint("Error: too many sprites in group!\n");
		sprite_handler = nullptr;
		return;
	}
	
	if (tc_width <= 0)
		tc_width = (float)width;
	if (tc_height <= 0)
		tc_height = (float)height;
	
	sprite_groups[current_sprite_group].sprites[current_sprite].width = tc_width * scale;
	sprite_groups[current_sprite_group].sprites[current_sprite].height = tc_height * scale;
	sprite_groups[current_sprite_group].sprites[current_sprite].tc_x = tc_x * sprite_groups[current_sprite_group].scale_x;
	sprite_groups[current_sprite_group].sprites[current_sprite].tc_y = tc_y * sprite_groups[current_sprite_group].scale_y;
	sprite_groups[current_sprite_group].sprites[current_sprite].tc_width = tc_width * sprite_groups[current_sprite_group].scale_x;
	sprite_groups[current_sprite_group].sprites[current_sprite].tc_height = tc_height * sprite_groups[current_sprite_group].scale_y;
	
	sprite_groups[current_sprite_group].sprites_count ++;
	
	sprite_handler->sprite_id = current_sprite;
	sprite_handler->group_id = current_sprite_group;
}

void Sprites_DrawSprite(SpriteHandler *sprite_handler, float x, float y, float scale, float angle, U32 color, U32 flags)
{
	if (sprite_handler == nullptr)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	if (sprite_handler->sprite_id < 0 || sprite_handler->group_id < 0 || sprite_handler->group_id >= sprite_groups_count)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	if (sprite_handler->sprite_id >= sprite_groups[sprite_handler->group_id].sprites_count)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	SpriteGroup *gptr = &sprite_groups[sprite_handler->group_id];
	Sprite *sptr = &gptr->sprites[sprite_handler->sprite_id];
	
	if (gptr->sprites_to_draw_count >= MAX_SPRITES_IN_ARRAY)
	{
		LogPrint("Error: too many sprites in array!\n");
		return;
	}
	
	const float alpha = DEG2RAD(-angle);
	
	float sin_alpha;
	float cos_alpha;
	
	if (alpha != 0.0f)
	{
		sin_alpha = sinf(alpha);
		cos_alpha = cosf(alpha);
	}
	else
	{
		sin_alpha = 0.0f;
		cos_alpha = 1.0f;
	}
	
	float sprite_rect_left;
	float sprite_rect_right;
	float sprite_rect_top;
	float sprite_rect_bottom;
	
	float sprite_tc_rect_left;
	float sprite_tc_rect_right;
	float sprite_tc_rect_top;
	float sprite_tc_rect_bottom;
	
	if (flags & SPRITE_ALIGN_LEFT)
	{
		sprite_rect_left = 0.0f;
		sprite_rect_right = sptr->width * scale;
	}
	else if (flags & SPRITE_ALIGN_RIGHT)
	{
		sprite_rect_left = - sptr->width * scale;
		sprite_rect_right = 0.0f;
	}
	else
	{
		sprite_rect_right = sptr->width * 0.5f * scale;
		sprite_rect_left = - sprite_rect_right;
	}
	
	if (flags & SPRITE_ALIGN_DOWN)
	{
		sprite_rect_bottom = 0.0f;
		sprite_rect_top = sptr->height * scale;
	}
	else if (flags & SPRITE_ALIGN_UP)
	{
		sprite_rect_bottom = - sptr->height * scale;
		sprite_rect_top = 0.0f;
	}
	else
	{
		sprite_rect_top = sptr->height * 0.5f * scale;
		sprite_rect_bottom = - sprite_rect_top;
	}
	
	if (flags & SPRITE_FLIP_X)
	{
		sprite_tc_rect_right = sptr->tc_x;
		sprite_tc_rect_left = sptr->tc_x + sptr->tc_width;
	}
	else
	{
		sprite_tc_rect_left = sptr->tc_x;
		sprite_tc_rect_right = sptr->tc_x + sptr->tc_width;
	}
	
	if (flags & SPRITE_FLIP_Y)
	{
		sprite_tc_rect_bottom = sptr->tc_y;
		sprite_tc_rect_top = sptr->tc_y + sptr->tc_height;
	}
	else
	{
		sprite_tc_rect_top = sptr->tc_y;
		sprite_tc_rect_bottom = sptr->tc_y + sptr->tc_height;
	}
	
	
	float *vptr = &gptr->sprites_vertices[gptr->sprites_to_draw_count * 8];
	float *tcptr = &gptr->sprites_tex_coords[gptr->sprites_to_draw_count * 8];
	U32 *cptr = &gptr->sprites_colors[gptr->sprites_to_draw_count * 4];
		
	*vptr = sprite_rect_left * cos_alpha - sprite_rect_bottom * sin_alpha + x;		vptr++;
	*vptr = sprite_rect_left * sin_alpha + sprite_rect_bottom * cos_alpha + y;		vptr++;
	*vptr = sprite_rect_right * cos_alpha - sprite_rect_bottom * sin_alpha + x;		vptr++;
	*vptr = sprite_rect_right * sin_alpha + sprite_rect_bottom * cos_alpha + y;		vptr++;
	*vptr = sprite_rect_right * cos_alpha - sprite_rect_top * sin_alpha + x;		vptr++;
	*vptr = sprite_rect_right * sin_alpha + sprite_rect_top * cos_alpha + y;		vptr++;
	*vptr = sprite_rect_left * cos_alpha - sprite_rect_top * sin_alpha + x;			vptr++;
	*vptr = sprite_rect_left * sin_alpha + sprite_rect_top * cos_alpha + y;
		
	*tcptr = sprite_tc_rect_left;	tcptr++;
	*tcptr = sprite_tc_rect_bottom;	tcptr++;
	*tcptr = sprite_tc_rect_right;	tcptr++;
	*tcptr = sprite_tc_rect_bottom;	tcptr++;
	*tcptr = sprite_tc_rect_right;	tcptr++;
	*tcptr = sprite_tc_rect_top;	tcptr++;
	*tcptr = sprite_tc_rect_left;	tcptr++;
	*tcptr = sprite_tc_rect_top;
		
	*cptr = color;					cptr++;
	*cptr = color;					cptr++;
	*cptr = color;					cptr++;
	*cptr = color;

	gptr->sprites_to_draw_count ++;
}

void Sprites_DrawSpriteEx(SpriteHandler *sprite_handler, float x, float y, float scale_x, float scale_y, float tc_bias_x, float tc_bias_y, float tc_scale_x, float tc_scale_y, float angle, U32 color, U32 flags)
{
	if (sprite_handler == nullptr)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	if (sprite_handler->sprite_id < 0 || sprite_handler->group_id < 0 || sprite_handler->group_id >= sprite_groups_count)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	if (sprite_handler->sprite_id >= sprite_groups[sprite_handler->group_id].sprites_count)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	SpriteGroup *gptr = &sprite_groups[sprite_handler->group_id];
	Sprite *sptr = &gptr->sprites[sprite_handler->sprite_id];
	
	if (gptr->sprites_to_draw_count >= MAX_SPRITES_IN_ARRAY)
	{
		LogPrint("Error: too many sprites in array!\n");
		return;
	}
	
	const float alpha = DEG2RAD(-angle);
	
	float sin_alpha;
	float cos_alpha;
	
	if (alpha != 0.0f)
	{
		sin_alpha = sinf(alpha);
		cos_alpha = cosf(alpha);
	}
	else
	{
		sin_alpha = 0.0f;
		cos_alpha = 1.0f;
	}
	
	float sprite_rect_left;
	float sprite_rect_right;
	float sprite_rect_top;
	float sprite_rect_bottom;
	
	float sprite_tc_rect_left;
	float sprite_tc_rect_right;
	float sprite_tc_rect_top;
	float sprite_tc_rect_bottom;
	
	if (flags & SPRITE_ALIGN_LEFT)
	{
		sprite_rect_left = 0.0f;
		sprite_rect_right = sptr->width * scale_x;
	}
	else if (flags & SPRITE_ALIGN_RIGHT)
	{
		sprite_rect_left = - sptr->width * scale_x;
		sprite_rect_right = 0.0f;
	}
	else
	{
		sprite_rect_right = sptr->width * 0.5f * scale_x;
		sprite_rect_left = - sprite_rect_right;
	}
	
	if (flags & SPRITE_ALIGN_DOWN)
	{
		sprite_rect_bottom = 0.0f;
		sprite_rect_top = sptr->height * scale_y;
	}
	else if (flags & SPRITE_ALIGN_UP)
	{
		sprite_rect_bottom = - sptr->height * scale_y;
		sprite_rect_top = 0.0f;
	}
	else
	{
		sprite_rect_top = sptr->height * 0.5f * scale_y;
		sprite_rect_bottom = - sprite_rect_top;
	}
	
	if (flags & SPRITE_FLIP_X)
	{
		sprite_tc_rect_right = sptr->tc_x + tc_bias_x * sptr->tc_width;
		sprite_tc_rect_left = sptr->tc_x + tc_scale_x * sptr->tc_width;
	}
	else
	{
		sprite_tc_rect_left = sptr->tc_x + tc_bias_x * sptr->tc_width;
		sprite_tc_rect_right = sptr->tc_x + tc_scale_x * sptr->tc_width;
	}
	
	if (flags & SPRITE_FLIP_Y)
	{
		sprite_tc_rect_bottom = sptr->tc_y + tc_bias_y * sptr->tc_height;
		sprite_tc_rect_top = sptr->tc_y + tc_scale_y * sptr->tc_height;
	}
	else
	{
		sprite_tc_rect_top = sptr->tc_y + tc_bias_y * sptr->tc_height;
		sprite_tc_rect_bottom = sptr->tc_y + tc_scale_y * sptr->tc_height;
	}
	
	
	float *vptr = &gptr->sprites_vertices[gptr->sprites_to_draw_count * 8];
	float *tcptr = &gptr->sprites_tex_coords[gptr->sprites_to_draw_count * 8];
	U32 *cptr = &gptr->sprites_colors[gptr->sprites_to_draw_count * 4];
		
	*vptr = sprite_rect_left * cos_alpha - sprite_rect_bottom * sin_alpha + x;		vptr++;
	*vptr = sprite_rect_left * sin_alpha + sprite_rect_bottom * cos_alpha + y;		vptr++;
	*vptr = sprite_rect_right * cos_alpha - sprite_rect_bottom * sin_alpha + x;		vptr++;
	*vptr = sprite_rect_right * sin_alpha + sprite_rect_bottom * cos_alpha + y;		vptr++;
	*vptr = sprite_rect_right * cos_alpha - sprite_rect_top * sin_alpha + x;		vptr++;
	*vptr = sprite_rect_right * sin_alpha + sprite_rect_top * cos_alpha + y;		vptr++;
	*vptr = sprite_rect_left * cos_alpha - sprite_rect_top * sin_alpha + x;			vptr++;
	*vptr = sprite_rect_left * sin_alpha + sprite_rect_top * cos_alpha + y;
		
	*tcptr = sprite_tc_rect_left;	tcptr++;
	*tcptr = sprite_tc_rect_bottom;	tcptr++;
	*tcptr = sprite_tc_rect_right;	tcptr++;
	*tcptr = sprite_tc_rect_bottom;	tcptr++;
	*tcptr = sprite_tc_rect_right;	tcptr++;
	*tcptr = sprite_tc_rect_top;	tcptr++;
	*tcptr = sprite_tc_rect_left;	tcptr++;
	*tcptr = sprite_tc_rect_top;
		
	*cptr = color;					cptr++;
	*cptr = color;					cptr++;
	*cptr = color;					cptr++;
	*cptr = color;

	gptr->sprites_to_draw_count ++;
}

void Sprites_GetSpriteSize(SpriteHandler *sprite_handler, Vector2D *size)
{
	if (sprite_handler == nullptr)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	if (sprite_handler->sprite_id < 0 || sprite_handler->group_id < 0 || sprite_handler->group_id >= sprite_groups_count)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	if (sprite_handler->sprite_id >= sprite_groups[sprite_handler->group_id].sprites_count)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	size->x = sprite_groups[sprite_handler->group_id].sprites[sprite_handler->sprite_id].width;
	size->y = sprite_groups[sprite_handler->group_id].sprites[sprite_handler->sprite_id].height;
}

void Sprites_SetSpriteSize(SpriteHandler *sprite_handler, const Vector2D *size)
{
	if (sprite_handler == nullptr)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	if (sprite_handler->sprite_id < 0 || sprite_handler->group_id < 0 || sprite_handler->group_id >= sprite_groups_count)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	if (sprite_handler->sprite_id >= sprite_groups[sprite_handler->group_id].sprites_count)
	{
		LogPrint("Error: sprite not found!\n");
		return;
	}
	
	sprite_groups[sprite_handler->group_id].sprites[sprite_handler->sprite_id].width = size->x;
	sprite_groups[sprite_handler->group_id].sprites[sprite_handler->sprite_id].height = size->y;
}

void Sprites_DrawSpriteNow(SpriteHandler *sprite_handler, float x, float y, float scale, float angle, U32 color, U32 flags)
{
	Sprites_DrawSprite(sprite_handler, x, y, scale, angle, color, flags);

	Render_EnableTextures();
	Render_EnableVertexArray();
	Render_EnableColorArray();
	Render_EnableTexCoordArray();
	Render_EnableIndexArray();
	
	int group = sprite_handler->group_id;

	if (sprite_groups[group].sprites_to_draw_count > 0)
	{
		TexManager_SetTextureByName(sprite_groups[group].texture);

		Render_SetVertexArray(sprite_groups[group].sprites_vertices, 2, TR_FLOAT, 0);
		Render_SetColorArray(sprite_groups[group].sprites_colors, 4, TR_UNSIGNED_BYTE, 0);
		Render_SetTexCoordArray(sprite_groups[group].sprites_tex_coords, 2, TR_FLOAT, 0);
		Render_SetIndexArray(&sprites_index_array[(sprite_groups[group].sprites_to_draw_count - 1) * 6], TR_UNSIGNED_SHORT);
		
		Render_DrawArrays(TR_TRIANGLES, 6);

		sprite_groups[group].sprites_to_draw_count --;
	}
}

void Sprites_Render()
{
	Render_EnableTextures();
	Render_EnableVertexArray();
	Render_EnableColorArray();
	Render_EnableTexCoordArray();
	Render_EnableIndexArray();
	
	for (int i = 0; i < sprite_groups_count; i++)
	{
		if (sprite_groups[i].sprites_to_draw_count > 0)
		{
			TexManager_SetTextureByName(sprite_groups[i].texture);

			Render_SetVertexArray(sprite_groups[i].sprites_vertices, 2, TR_FLOAT, 0);
			Render_SetColorArray(sprite_groups[i].sprites_colors, 4, TR_UNSIGNED_BYTE, 0);
			Render_SetTexCoordArray(sprite_groups[i].sprites_tex_coords, 2, TR_FLOAT, 0);
			Render_SetIndexArray(sprites_index_array, TR_UNSIGNED_SHORT);
			
			Render_DrawArrays(TR_TRIANGLES, sprite_groups[i].sprites_to_draw_count * 6);
		}
		
		sprite_groups[i].sprites_to_draw_count = 0;
	}
}

void Sprites_Release()
{
	for (int i = 0; i < sprite_groups_count; i++)
		TexManager_RemoveTextureByName(sprite_groups[i].texture);
}

