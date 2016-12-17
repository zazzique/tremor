//
//  GUIControls.m
//  Carnivores
//
//  Created by Dmitry Nechay on 25.08.09.
//  Copyright 2009 Action Forms. All rights reserved.
//

#include "Common.h"

#include "FastMath.h"
#include "Vector.h"
#include "TextureManager.h"
#include "Sprites.h"
#include "Font.h"
#include "GUIControls.h"

#define GUI_CONTROLLER_INNER_RADIUS 40.0f

#define GUI_QUICK_TOUCH_TIME 0.15f

typedef struct _GUIControl
{
	int group;
	U32 subgroup_flags;
	int type;
	float x;
	float y;
	float x2;
	float y2;
	float width;
	float height;
	U32 flags;
	U32 color;
	float scale;
	bool use_sprites;
	bool use_text;
	bool active;
	bool visible;
	bool touch_began;
	bool touch_done;
	bool dynamic_size;
	SpriteHandler sprite_1;
	SpriteHandler sprite_2;
	char font_name[32];
	float font_scale;
	char text[256];
	float text_pos_x;
	float text_pos_y;
	float slider_value;
	float slider_min;
	float slider_max;
    
} GUIControl;

GUIControl gui_controls[MAX_GUI_CONTROLS_COUNT];
int gui_controls_count;

bool gui_lock;

int gui_active_group;
U32 gui_active_subgroups;

int gui_touched_controls[MAX_GUI_TOUCHES_SUPPORTED];
Vector2D gui_touched_locations[MAX_GUI_TOUCHES_SUPPORTED];
Vector2D gui_touched_start_locations[MAX_GUI_TOUCHES_SUPPORTED];

int gui_event_id;
int gui_event_activated;

int quick_touch;
Vector2D quick_touch_location;
float quick_touch_time;

bool gui_enable_touches_reset;


bool GUI_IsTouchBackground(int touch_id)
{
	if ((touch_id < 0) || (touch_id >= MAX_GUI_TOUCHES_SUPPORTED))
		return false;
    
	bool is_background = false;

	if (gui_touched_controls[touch_id] == GUI_CONTROL_BACKGROUND)
		is_background = true;

	if ((gui_touched_controls[touch_id] >= 0) || (gui_touched_controls[touch_id] < MAX_GUI_CONTROLS_COUNT))
		if (gui_controls[gui_touched_controls[touch_id]].flags & GUI_CONTROL_ALLOW_BACKGROUND)
			is_background = true;

	return is_background;
}

void GUI_Init()
{
	gui_enable_touches_reset = true;

    gui_lock = false;
	
	gui_controls_count = 0;
	gui_active_group = 0;
	gui_active_subgroups = GUI_SUBGROUP_DEFAULT;
	
	for (int i = 0; i < MAX_GUI_CONTROLS_COUNT; i++)
	{
		gui_controls[i].touch_began = false;
		gui_controls[i].touch_done = false;
	}
	
	for (int i = 0; i < MAX_GUI_TOUCHES_SUPPORTED; i++)
	{
		gui_touched_controls[i] = GUI_CONTROL_NONE;
	}
	
	gui_event_id = -1;
	gui_event_activated = false;
	
	quick_touch = false;
	quick_touch_time = 0.0f;
}

void GUI_Lock()
{
    gui_lock = true;
}

void GUI_Unlock()
{
    gui_lock = false;
}

bool GUI_IsLocked()
{
    return gui_lock;
}

void GUI_AddControl(GUIControlId *control_id, int group, U32 subgroups, int type, Vector2D *pos, Vector2D *size, U32 flags, SpriteHandler *sprite_1, SpriteHandler *sprite_2, const char *text, const char *font_name, Vector2D *text_pos)
{
	if (gui_controls_count >= MAX_GUI_CONTROLS_COUNT)
	{
		Log::Print("Error: too many GUI elements!\n");
		return;
	}
	
	*control_id = gui_controls_count;
	
	gui_controls[gui_controls_count].group = group;
	
	gui_controls[gui_controls_count].subgroup_flags = subgroups;
	
	gui_controls[gui_controls_count].type = type;
	
	if (pos != nullptr)
	{
		gui_controls[gui_controls_count].x = pos->x;
		gui_controls[gui_controls_count].y = pos->y;
	}
	else
	{
		gui_controls[gui_controls_count].x = 0.0f;
		gui_controls[gui_controls_count].y = 0.0f;
	}
	
	gui_controls[gui_controls_count].x2 = 0.0f;
	gui_controls[gui_controls_count].y2 = 0.0f;
	
	if (size != nullptr)
	{
		gui_controls[gui_controls_count].width = size->x;
		gui_controls[gui_controls_count].height = size->y;
		
		gui_controls[gui_controls_count].dynamic_size = false;
	}
	else
	{
		gui_controls[gui_controls_count].width = 0.0f;
		gui_controls[gui_controls_count].height = 0.0f;
		
		gui_controls[gui_controls_count].dynamic_size = true;
	}
	
	if (text_pos != nullptr)
	{
		gui_controls[gui_controls_count].text_pos_x = text_pos->x;
		gui_controls[gui_controls_count].text_pos_y = text_pos->y;
	}
	else
	{
		gui_controls[gui_controls_count].text_pos_x = 0.0f;
		gui_controls[gui_controls_count].text_pos_y = 0.0f;
	}
	
	gui_controls[gui_controls_count].flags = flags;
	
	gui_controls[gui_controls_count].color = 0xffffffff;
	
	gui_controls[gui_controls_count].scale = 1.0f;
	
	gui_controls[gui_controls_count].font_scale = 1.0f;
	
	if (sprite_1 == nullptr)
	{
		gui_controls[gui_controls_count].use_sprites = false;
	}
	else
	{
		gui_controls[gui_controls_count].use_sprites = true;
		
		gui_controls[gui_controls_count].sprite_1.sprite_id = sprite_1->sprite_id;
		gui_controls[gui_controls_count].sprite_1.group_id = sprite_1->group_id;
		
		if (sprite_2 == nullptr)
		{
			gui_controls[gui_controls_count].sprite_2.sprite_id = sprite_1->sprite_id;
			gui_controls[gui_controls_count].sprite_2.group_id = sprite_1->group_id;
		}
		else
		{
			gui_controls[gui_controls_count].sprite_2.sprite_id = sprite_2->sprite_id;
			gui_controls[gui_controls_count].sprite_2.group_id = sprite_2->group_id;
		}
	}
	
	if (font_name == nullptr)
	{
		gui_controls[gui_controls_count].use_text = false;
	}
	else
	{
		gui_controls[gui_controls_count].use_text = true;
		
		strcpy(gui_controls[gui_controls_count].font_name, font_name);
		strcpy(gui_controls[gui_controls_count].text, text);
	}
	
	gui_controls[gui_controls_count].touch_began = false;
	gui_controls[gui_controls_count].touch_done = false;
	
	gui_controls[gui_controls_count].active = true;
	gui_controls[gui_controls_count].visible = true;
	
	gui_controls[gui_controls_count].slider_value = 0;
	gui_controls[gui_controls_count].slider_min = -1;
	gui_controls[gui_controls_count].slider_max = 1;
	
	if (gui_controls[gui_controls_count].dynamic_size)
	{
		Vector2D dynamic_size;
		
		if (gui_controls[gui_controls_count].use_sprites)
		{
			Sprites_GetSpriteSize(&gui_controls[gui_controls_count].sprite_1, &dynamic_size);
			
			gui_controls[gui_controls_count].width = dynamic_size.x;
			gui_controls[gui_controls_count].height = dynamic_size.y;
		}
		else if (gui_controls[gui_controls_count].use_text)
		{
			Font_GetTextSize(gui_controls[gui_controls_count].text, gui_controls[gui_controls_count].font_name, &dynamic_size);
			
			gui_controls[gui_controls_count].width = dynamic_size.x;
			gui_controls[gui_controls_count].height = dynamic_size.y;
		}
	}
	
	gui_controls_count ++;
}


void GUI_SetControlPos(GUIControlId control_id, float x, float y)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	gui_controls[control_id].x = x;
	gui_controls[control_id].y = y;
}


void GUI_SetControlFlags(GUIControlId control_id, U32 flags)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	gui_controls[control_id].flags = flags;
}


void GUI_SetControlColor(GUIControlId control_id, U32 color)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	gui_controls[control_id].color = color;
}


void GUI_SetControlScale(GUIControlId control_id, float scale)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	gui_controls[control_id].scale = scale;
}


void GUI_SetControlText(GUIControlId control_id, char *text)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	if (text != nullptr)
		strcpy(gui_controls[control_id].text, text);
	
	if (!gui_controls[control_id].use_sprites && gui_controls[control_id].use_text)
	{
		Vector2D dynamic_size;
		
		Font_GetTextSize(gui_controls[control_id].text, gui_controls[control_id].font_name, &dynamic_size);
		
		gui_controls[control_id].width = dynamic_size.x;
		gui_controls[control_id].height = dynamic_size.y;
	}
}


void GUI_SetControlFontScale(GUIControlId control_id, float scale)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	gui_controls[control_id].font_scale = scale;

	if (!gui_controls[control_id].use_sprites && gui_controls[control_id].use_text)
	{
		Vector2D dynamic_size;
		
		Font_GetTextSize(gui_controls[control_id].text, gui_controls[control_id].font_name, &dynamic_size);
        
        dynamic_size.x *= scale;
        dynamic_size.y *= scale;
		
		gui_controls[control_id].width = dynamic_size.x;
		gui_controls[control_id].height = dynamic_size.y;
	}
}


void GUI_SetControlSprites(GUIControlId control_id, SpriteHandler *sprite_1, SpriteHandler *sprite_2)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	if (sprite_1 == nullptr)
		return;
	
	gui_controls[control_id].sprite_1.sprite_id = sprite_1->sprite_id;
	gui_controls[control_id].sprite_1.group_id = sprite_1->group_id;
	
	if (sprite_2 == nullptr)
	{
		gui_controls[control_id].sprite_2.sprite_id = sprite_1->sprite_id;
		gui_controls[control_id].sprite_2.group_id = sprite_1->group_id;
	}
	else
	{
		gui_controls[control_id].sprite_2.sprite_id = sprite_2->sprite_id;
		gui_controls[control_id].sprite_2.group_id = sprite_2->group_id;
	}
	
	if (gui_controls[control_id].use_sprites)
	{
		Vector2D dynamic_size;
		
		Sprites_GetSpriteSize(&gui_controls[control_id].sprite_1, &dynamic_size);
		
		gui_controls[control_id].width = dynamic_size.x;
		gui_controls[control_id].height = dynamic_size.y;
	}
}


void GUI_SetControlActive(GUIControlId control_id, bool active)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	gui_controls[control_id].active = active;
}


void GUI_SetControlVisible(GUIControlId control_id, bool visible)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	gui_controls[control_id].visible = visible;
}


void GUI_SetSliderValue(GUIControlId control_id, float value)
{
	Vector2D sprite_1_size, sprite_2_size;
	float slider_k;
	
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	if (!gui_controls[control_id].use_sprites)
		return;
	
	gui_controls[control_id].slider_value = value;
		
	Sprites_GetSpriteSize(&gui_controls[control_id].sprite_1, &sprite_1_size);
	Sprites_GetSpriteSize(&gui_controls[control_id].sprite_2, &sprite_2_size);
		
	float div_tmp = sprite_1_size.x - 2.0f - sprite_2_size.x;
	if (div_tmp == 0.0f)
		div_tmp = FLT_EPSILON;
	slider_k = (gui_controls[control_id].slider_max - gui_controls[control_id].slider_min) / div_tmp;
	gui_controls[control_id].x2 = (gui_controls[control_id].slider_value - gui_controls[control_id].slider_min) / slider_k + 1.0f;
	gui_controls[control_id].y2 = (sprite_1_size.y - sprite_2_size.y) * 0.5f;
}


void GUI_SetSliderParams(GUIControlId control_id, float min, float max)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	gui_controls[control_id].slider_min = min;
	gui_controls[control_id].slider_max = max;
}


void GUI_GetControlPos(GUIControlId control_id, float *x, float *y)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	*x = gui_controls[control_id].x;
	*y = gui_controls[control_id].y;
}


void GUI_DrawControls()
{
	U32 flags;
    float dx, dy;
	
	for (int i = 0; i < gui_controls_count; i ++)
	{
		if (gui_controls[i].group != gui_active_group)
			continue;
		
		if (!(gui_controls[i].subgroup_flags & gui_active_subgroups))
			continue;
		
		if (!gui_controls[i].visible)
			continue;
		
		if (gui_controls[i].use_sprites)
		{
            flags = SPRITE_ALIGN_LEFT | SPRITE_ALIGN_DOWN;
            
            dx = 0;
            dy = 0;
            
			if (gui_controls[i].flags & GUI_CONTROL_ALIGN_LEFT)
                dx = 0;
			if (gui_controls[i].flags & GUI_CONTROL_ALIGN_RIGHT)
				dx = -gui_controls[i].width * gui_controls[i].scale;
			if (gui_controls[i].flags & GUI_CONTROL_ALIGN_CENTER)
				dx = -gui_controls[i].width * 0.5f * gui_controls[i].scale;
			if (gui_controls[i].flags & GUI_CONTROL_VERTICAL_CENTERED)
				dy = -gui_controls[i].height * 0.5f * gui_controls[i].scale;
            
			if (gui_controls[i].type == GUI_LABEL)
			{
				Sprites_DrawSprite(&gui_controls[i].sprite_1, gui_controls[i].x, gui_controls[i].y, 1.0f, 0.0f, gui_controls[i].color, flags);
			}
			else if (gui_controls[i].type == GUI_BUTTON)
			{
				Sprites_DrawSprite(gui_controls[i].touch_began ? &gui_controls[i].sprite_2 : &gui_controls[i].sprite_1, gui_controls[i].x + dx, gui_controls[i].y + dy, gui_controls[i].scale, 0.0f, gui_controls[i].color, flags);
			}
			else if (gui_controls[i].type == GUI_CONTROLLER)
			{
				Sprites_DrawSprite(&gui_controls[i].sprite_1, gui_controls[i].x + dx, gui_controls[i].y + dy, gui_controls[i].scale, 0.0f, gui_controls[i].color, flags);
				Sprites_DrawSprite(&gui_controls[i].sprite_2, gui_controls[i].x2 + gui_controls[i].x, gui_controls[i].y2 + gui_controls[i].y, gui_controls[i].scale, 0.0f, gui_controls[i].color, SPRITE_CENTERED);
			}
			else if (gui_controls[i].type == GUI_SLIDER)
			{
				Sprites_DrawSprite(&gui_controls[i].sprite_1, gui_controls[i].x + dx, gui_controls[i].y + dy, gui_controls[i].scale, 0.0f, gui_controls[i].color, flags);
				Sprites_DrawSprite(&gui_controls[i].sprite_2, gui_controls[i].x2 + gui_controls[i].x + dx, gui_controls[i].y2 + gui_controls[i].y + dy, gui_controls[i].scale, 0.0f, gui_controls[i].color, flags);
			}
		}
		if (gui_controls[i].use_text)
		{
			flags = 0x00000000;
			
			if (gui_controls[i].flags & GUI_CONTROL_ALIGN_LEFT)
				flags |= TEXT_ALIGN_LEFT;
			if (gui_controls[i].flags & GUI_CONTROL_ALIGN_RIGHT)
				flags |= TEXT_ALIGN_RIGHT;
			if (gui_controls[i].flags & GUI_CONTROL_ALIGN_CENTER)
				flags |= TEXT_ALIGN_CENTER;
			if (gui_controls[i].flags & GUI_CONTROL_VERTICAL_CENTERED)
				flags |= TEXT_VERTICAL_CENTERED;
			
			if (gui_controls[i].type == GUI_LABEL)
			{
				Font_PrintText(gui_controls[i].x + gui_controls[i].text_pos_x, gui_controls[i].y + gui_controls[i].text_pos_y, gui_controls[i].font_scale, gui_controls[i].color, gui_controls[i].text, flags, gui_controls[i].font_name);
			}
			else if (gui_controls[i].type == GUI_BUTTON)
			{
				Font_PrintText(gui_controls[i].x + gui_controls[i].text_pos_x, gui_controls[i].y + gui_controls[i].text_pos_y, gui_controls[i].font_scale, gui_controls[i].touch_began ? 0xff0040ff : gui_controls[i].color, gui_controls[i].text, flags, gui_controls[i].font_name);
			}
		}
	}
}


void GUI_SetActiveGroup(int group, U32 subgroups)
{
	gui_active_group = group;
	gui_active_subgroups = subgroups;
}


int GUI_GetUnusedTouch()
{
	for (int i = 0; i < MAX_GUI_TOUCHES_SUPPORTED; i++)
		if (gui_touched_controls[i] == GUI_CONTROL_NONE)
			return i;
	
	return 0;
}

int GUI_GetTouchByLocation(float x, float y)
{
	float min_distance = 1024;
	float distance = 0;
	int nearest_touch = -1;
	
	for (int i = 0; i < MAX_GUI_TOUCHES_SUPPORTED; i++)
	{
		if (gui_touched_controls[i] == GUI_CONTROL_NONE)
			continue;
		
		distance = sqrtf(SQR(x - gui_touched_locations[i].x) + SQR(y - gui_touched_locations[i].y));
		
		if (min_distance > distance)
		{
			min_distance = distance;
			nearest_touch = i;
		}
	}
	
	return nearest_touch;
}

void GUI_TouchBegan(float x, float y)
{
    int touch_id = GUI_GetUnusedTouch();
    
	if ((touch_id < 0) || (touch_id >= MAX_GUI_TOUCHES_SUPPORTED))
		return;
    
	bool is_background = true;
	
	if (gui_lock)
		return;
    
	for (int i = 0; i < gui_controls_count; i ++)
	{
		if (gui_controls[i].group != gui_active_group)
			continue;
		
		if (!(gui_controls[i].subgroup_flags & gui_active_subgroups))
			continue;
		
		if (!gui_controls[i].visible)
			continue;
		
		if (!gui_controls[i].active)
			continue;
        
		if (gui_controls[i].type == GUI_LABEL)
			continue;

		if (GUI_PointInControl(i, x, y))
		{
			gui_controls[i].touch_began = true;
			
			gui_touched_controls[touch_id] = i;

			if (gui_controls[i].flags & GUI_CONTROL_ALLOW_BACKGROUND)
				quick_touch_time = 0.0f;

			is_background = false;

			break;
		}
	}
    
	if (is_background)
	{
		gui_touched_controls[touch_id] = GUI_CONTROL_BACKGROUND;
		
		quick_touch_time = 0.0f;
	}
    
	gui_touched_locations[touch_id].x = x;
	gui_touched_locations[touch_id].y = y;
    
	gui_touched_start_locations[touch_id].x = x;
	gui_touched_start_locations[touch_id].y = y;
}

void GUI_TouchMoved(int touch_id, float x, float y)
{
	if ((touch_id < 0) || (touch_id >= MAX_GUI_TOUCHES_SUPPORTED))
		return;
    
	gui_touched_locations[touch_id].x = x;
	gui_touched_locations[touch_id].y = y;
    
    if (gui_touched_controls[touch_id] == GUI_CONTROL_NONE)
	{
		return;
	}
    
	if (gui_touched_controls[touch_id] == GUI_CONTROL_BACKGROUND)
	{
		return;
	}
    
	if (gui_controls[gui_touched_controls[touch_id]].group != gui_active_group)
	{
		gui_touched_controls[touch_id] = GUI_CONTROL_NONE;
		return;
	}
	if (!(gui_controls[gui_touched_controls[touch_id]].subgroup_flags & gui_active_subgroups))
	{
		gui_touched_controls[touch_id] = GUI_CONTROL_NONE;
		return;
	}
	if (!gui_controls[gui_touched_controls[touch_id]].visible)
	{
		gui_touched_controls[touch_id] = GUI_CONTROL_NONE;
		return;
	}
	if (!gui_controls[gui_touched_controls[touch_id]].active)
	{
		gui_touched_controls[touch_id] = GUI_CONTROL_NONE;
		return;
	}
	if (gui_touched_controls[touch_id] != GUI_CONTROL_NONE)
	{
		if (GUI_PointInControl(gui_touched_controls[touch_id], x, y))
		{
			gui_controls[gui_touched_controls[touch_id]].touch_began = true;
		}
		else
		{
			gui_controls[gui_touched_controls[touch_id]].touch_began = false;
		}
	}
}

void GUI_TouchEnded(int touch_id, float x, float y)
{
	if ((touch_id < 0) || (touch_id >= MAX_GUI_TOUCHES_SUPPORTED))
		return;
    
	if (GUI_IsTouchBackground(touch_id))
	{
		gui_touched_locations[touch_id] = gui_touched_start_locations[touch_id];
		
		if (quick_touch_time <= GUI_QUICK_TOUCH_TIME)
		{
			quick_touch = true;
			quick_touch_location.x = x;
			quick_touch_location.y = y;
		}
        
		quick_touch_time = 0.0f;
	}
	
	if ((gui_touched_controls[touch_id] != GUI_CONTROL_NONE) && (gui_touched_controls[touch_id] != GUI_CONTROL_BACKGROUND) && (gui_touched_controls[touch_id] >= 0))
	{
		if (GUI_PointInControl(gui_touched_controls[touch_id], x, y))
		{
			if (gui_controls[gui_touched_controls[touch_id]].group != gui_active_group)
			{
				gui_controls[gui_touched_controls[touch_id]].touch_began = false;
				gui_controls[gui_touched_controls[touch_id]].touch_done = false;
			}
			else if (!(gui_controls[gui_touched_controls[touch_id]].subgroup_flags & gui_active_subgroups))
			{
				gui_controls[gui_touched_controls[touch_id]].touch_began = false;
				gui_controls[gui_touched_controls[touch_id]].touch_done = false;
			}
			else
			{
				gui_controls[gui_touched_controls[touch_id]].touch_began = false;
				gui_controls[gui_touched_controls[touch_id]].touch_done = true;
			}
		}
		else
		{
			gui_controls[gui_touched_controls[touch_id]].touch_began = false;
			gui_controls[gui_touched_controls[touch_id]].touch_done = false;
		}
	}
    
	gui_touched_controls[touch_id] = GUI_CONTROL_NONE;
}

void GUI_TouchesReset()
{
	if (!gui_enable_touches_reset)
		return;

	for (int i = 0; i < MAX_GUI_TOUCHES_SUPPORTED; i++)
		gui_touched_controls[i] = GUI_CONTROL_NONE;
}

void GUI_GetBackgroundMovements(float *x, float *y)
{
	float x_r = 0.0f;
	float y_r = 0.0f;
	
	for (int i = 0; i < MAX_GUI_TOUCHES_SUPPORTED; i++)
    {
		if (GUI_IsTouchBackground(i))
		{
			x_r += gui_touched_locations[i].x - gui_touched_start_locations[i].x;
			y_r += gui_touched_locations[i].y - gui_touched_start_locations[i].y;
			
			gui_touched_start_locations[i] = gui_touched_locations[i];
		}
    }

	*x = x_r;
	*y = y_r;
}

bool GUI_GetQuickTouch(Vector2D *location)
{
	if (quick_touch && location != nullptr)
	{
		*location = quick_touch_location;
	}

	int result = quick_touch;
	quick_touch = false;
	
	return result;
}

bool GUI_PointInControl(GUIControlId control_id, float x, float y)
{
	bool result = false;
	
	if (control_id < 0 || control_id >= gui_controls_count)
		return false;
	
	float rect_left;
	float rect_right;
	float rect_top;
	float rect_bottom;
	
	if (gui_controls[control_id].type == GUI_BUTTON || gui_controls[control_id].type == GUI_CONTROLLER || gui_controls[control_id].type == GUI_SLIDER)
	{
		rect_left = gui_controls[control_id].x;
		rect_bottom = gui_controls[control_id].y;
		rect_right = gui_controls[control_id].x + gui_controls[control_id].width * gui_controls[control_id].scale;
		rect_top = gui_controls[control_id].y + gui_controls[control_id].height * gui_controls[control_id].scale;
		
		if (gui_controls[control_id].type == GUI_SLIDER)
		{
			rect_left -= 32;
			rect_bottom -= 8;
			rect_right += 32;
			rect_top += 8;
		}
		
		if (!gui_controls[control_id].use_sprites && gui_controls[control_id].use_text)
		{
			rect_bottom -= 2;
			rect_top += 6;
		}
		
		if (gui_controls[control_id].flags & GUI_CONTROL_ALIGN_RIGHT)
		{
			rect_left -= gui_controls[control_id].width * gui_controls[control_id].scale;
			rect_right -= gui_controls[control_id].width * gui_controls[control_id].scale;
		}
		if (gui_controls[control_id].flags & GUI_CONTROL_ALIGN_CENTER)
		{
			rect_left -= gui_controls[control_id].width * 0.5f * gui_controls[control_id].scale;
			rect_right -= gui_controls[control_id].width * 0.5f * gui_controls[control_id].scale;
		}
		if (gui_controls[control_id].flags & GUI_CONTROL_VERTICAL_CENTERED)
		{
			rect_top -= gui_controls[control_id].height * 0.5f * gui_controls[control_id].scale;
			rect_bottom -= gui_controls[control_id].height * 0.5f * gui_controls[control_id].scale;
		}
		
		if (x > rect_left && x < rect_right)
			if (y > rect_bottom && y < rect_top)
				result = true;
	}
	
	return result;
}

bool GUI_ControlIsPressed(GUIControlId control_id)
{
	bool result = false;
	
	if (gui_lock)
		return false;
	
	if (control_id < 0 || control_id >= gui_controls_count)
		return false;
	
	if (!(gui_controls[control_id].subgroup_flags & gui_active_subgroups))
		return false;
	
	if (gui_controls[control_id].group == gui_active_group)
	{
		if (gui_controls[control_id].visible && gui_controls[control_id].active)
		{
			if (gui_controls[control_id].touch_done)
			{
				gui_controls[control_id].touch_done = false;
				result = true;
			}
		}
	}
	
	return result;
}


bool GUI_ControlIsTouched(GUIControlId control_id) // TODO: background support
{
	bool result = false;
	
	if (gui_lock)
		return false;
	
	if (control_id < 0 || control_id >= gui_controls_count)
		return false;
	
	if (!(gui_controls[control_id].subgroup_flags & gui_active_subgroups))
	{
		return false;
	}
	
	if (gui_controls[control_id].group == gui_active_group)
	{
		if (gui_controls[control_id].visible && gui_controls[control_id].active)
		{
			for (int i = 0; i < MAX_GUI_TOUCHES_SUPPORTED; i ++)
			{
				if (gui_touched_controls[i] == control_id)
				{
					result = true;
				}
			}
		}
	}
	
	return result;
}


void GUI_ControlUntouch(GUIControlId control_id)
{
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	for (int i = 0; i < MAX_GUI_TOUCHES_SUPPORTED; i ++)
	{
		if (gui_touched_controls[i] == control_id)
		{
			gui_touched_controls[i] = GUI_CONTROL_BACKGROUND;
			break;
		}
	}
}


void GUI_GetControllerVector(GUIControlId control_id, Vector2D *vector)
{
	float len;
	
	vector->x = 0.0f;
	vector->y = 0.0f;
		
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	if (gui_controls[control_id].subgroup_flags & gui_active_subgroups)
	{
		if (gui_controls[control_id].group == gui_active_group)
		{
			if (gui_controls[control_id].visible && gui_controls[control_id].active)
			{
				for (int i = 0; i < MAX_GUI_TOUCHES_SUPPORTED; i ++)
				{
					if (gui_touched_controls[i] == control_id)
					{
						vector->x = gui_touched_locations[i].x - gui_touched_start_locations[i].x;
						vector->y = gui_touched_locations[i].y - gui_touched_start_locations[i].y;
						len = sqrtf(vector->x * vector->x + vector->y * vector->y);
						
						if (len <= 0)
						{
							vector->x = 0.0f;
							vector->y = 0.0f;
							break;
						}
						
						if (len > GUI_CONTROLLER_INNER_RADIUS)
						{
							vector->x *= GUI_CONTROLLER_INNER_RADIUS / len;
							vector->y *= GUI_CONTROLLER_INNER_RADIUS / len;
						}
						
						vector->x /= GUI_CONTROLLER_INNER_RADIUS;
						vector->y /= GUI_CONTROLLER_INNER_RADIUS;
					}
				}
			}
		}
	}
	
	gui_controls[control_id].x2 = vector->x * GUI_CONTROLLER_INNER_RADIUS;
	gui_controls[control_id].y2 = vector->y * GUI_CONTROLLER_INNER_RADIUS;
}

void GUI_GetControlVector(GUIControlId control_id, Vector2D *vector)
{
	float len;
	
	vector->x = 0.0f;
	vector->y = 0.0f;
    
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	if (gui_controls[control_id].subgroup_flags & gui_active_subgroups)
	{
		if (gui_controls[control_id].group == gui_active_group)
		{
			if (gui_controls[control_id].visible && gui_controls[control_id].active)
			{
				for (int i = 0; i < MAX_GUI_TOUCHES_SUPPORTED; i ++)
				{
					if (gui_touched_controls[i] == control_id)
					{
						vector->x = gui_touched_locations[i].x - gui_touched_start_locations[i].x;
						vector->y = gui_touched_locations[i].y - gui_touched_start_locations[i].y;
					}
				}
			}
		}
	}
}


void GUI_GetPointInControl(GUIControlId control_id, Vector2D *vector)
{
	vector->x = 0.0f;
	vector->y = 0.0f;
	
	if (control_id < 0 || control_id >= gui_controls_count)
		return;
	
	if (gui_controls[control_id].subgroup_flags & gui_active_subgroups)
	{
		if (gui_controls[control_id].group == gui_active_group)
		{
			if (gui_controls[control_id].visible && gui_controls[control_id].active)
			{
				for (int i = 0; i < MAX_GUI_TOUCHES_SUPPORTED; i ++)
				{
					if (gui_touched_controls[i] == control_id)
					{
						if (gui_controls[control_id].flags & GUI_CONTROL_ALIGN_RIGHT)
						{
							vector->x = gui_touched_locations[i].x - (gui_controls[control_id].x - gui_controls[control_id].scale * gui_controls[control_id].width);
						}
						else if (gui_controls[control_id].flags & GUI_CONTROL_ALIGN_CENTER)
						{
							vector->x = gui_touched_locations[i].x - (gui_controls[control_id].x - gui_controls[control_id].scale * gui_controls[control_id].width * 0.5f);
						}
						else
						{
							vector->x = gui_touched_locations[i].x - gui_controls[control_id].x;
						}
						
						if (gui_controls[control_id].flags & GUI_CONTROL_VERTICAL_CENTERED)
						{
							vector->y = gui_touched_locations[i].y - (gui_controls[control_id].y - gui_controls[control_id].scale * gui_controls[control_id].height * 0.5f);
						}
						else
						{
							vector->y = gui_touched_locations[i].y - gui_controls[control_id].y;
						}
					}
				}
			}
		}
	}
}


float GUI_GetSliderValue(GUIControlId control_id)
{
	Vector2D sprite_1_size, sprite_2_size;
	float slider_k;
		
	if (control_id < 0 || control_id >= gui_controls_count)
		return 0;
	
	if (gui_controls[control_id].subgroup_flags & gui_active_subgroups)
	{
		if (gui_controls[control_id].group == gui_active_group)
		{
			if (gui_controls[control_id].visible && gui_controls[control_id].active)
			{
				for (int i = 0; i < MAX_GUI_TOUCHES_SUPPORTED; i ++)
				{
					if (gui_touched_controls[i] == control_id)
					{
						if (!gui_controls[control_id].use_sprites)
							continue;
						
						Sprites_GetSpriteSize(&gui_controls[control_id].sprite_1, &sprite_1_size);
						Sprites_GetSpriteSize(&gui_controls[control_id].sprite_2, &sprite_2_size);
						
						float div_tmp = sprite_1_size.x - 2.0f - sprite_2_size.x;
						if (div_tmp == 0.0f)
							div_tmp = FLT_EPSILON;
						slider_k = (gui_controls[control_id].slider_max - gui_controls[control_id].slider_min) / div_tmp;
						
						gui_controls[control_id].slider_value += (gui_touched_locations[i].x - gui_touched_start_locations[i].x) * slider_k;
						gui_touched_start_locations[i].x = gui_touched_locations[i].x;
						
						if (gui_controls[control_id].slider_value < gui_controls[control_id].slider_min)
							gui_controls[control_id].slider_value = gui_controls[control_id].slider_min;
						if (gui_controls[control_id].slider_value > gui_controls[control_id].slider_max)
							gui_controls[control_id].slider_value = gui_controls[control_id].slider_max;
						
						gui_controls[control_id].x2 = (gui_controls[control_id].slider_value - gui_controls[control_id].slider_min) / slider_k + 1.0f;
						gui_controls[control_id].y2 = (sprite_1_size.y - sprite_2_size.y) * 0.5f;
					}
				}
			}
		}
	}
	
	return gui_controls[control_id].slider_value;
}

void GUI_Release()
{
	//
}

