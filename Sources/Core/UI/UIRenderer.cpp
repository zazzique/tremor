
#include "Common.h"
#include "opengl.h"
#include "Render.h" // TODO: remove later
#include "TextureManager.h"
#include "UIRenderer.h"


#define UI_RENDERER_MAX_ELEMENTS 256
#define UI_RENDERER_MAX_QUADS 2048

U16   ui_quads_index_array[UI_RENDERER_MAX_QUADS * 6];
float ui_quads_vertices[UI_RENDERER_MAX_QUADS * 8];
U32   ui_quads_colors[UI_RENDERER_MAX_QUADS * 4];
float ui_quads_tex_coords[UI_RENDERER_MAX_QUADS * 8];

int   ui_quads_count;


struct UIRendererElement
{
	int quad_index;
	int quads_count;
	
	int layer;
	int texture_index;
};

UIRendererElement ui_elements[UI_RENDERER_MAX_ELEMENTS];
int ui_elements_count;

int ui_elements_index_list[UI_RENDERER_MAX_ELEMENTS];


void UIRenderer_Init()
{
	for (int i = 0; i < UI_RENDERER_MAX_QUADS; i++)
	{
		ui_quads_index_array[i * 6 + 0] = (i * 4) + 0;
		ui_quads_index_array[i * 6 + 1] = (i * 4) + 1;
		ui_quads_index_array[i * 6 + 2] = (i * 4) + 2;
		ui_quads_index_array[i * 6 + 3] = (i * 4) + 2;
		ui_quads_index_array[i * 6 + 4] = (i * 4) + 3;
		ui_quads_index_array[i * 6 + 5] = (i * 4) + 0;
	}

	ui_elements_count = 0;
	ui_quads_count = 0;
}

void UIRenderer_AddQuadsToRenderList(const float* vertices, const float* tc, const U32* color, int count, int texture_index, int layer)
{
	if (count <= 0)
		return;

	if (ui_elements_count >= UI_RENDERER_MAX_ELEMENTS)
	{
		LOGE("Error! Too many elements in UI");
		return;
	}

	if (ui_quads_count + count >= UI_RENDERER_MAX_QUADS)
	{
		LOGE("Error! Too many quads in UI");
		return;
	}

	if (vertices == NULL)
		return;

	if (tc == NULL)
		return;
	
	ui_elements[ui_elements_count].quad_index = ui_quads_count;
	ui_elements[ui_elements_count].quads_count = count;
	ui_elements[ui_elements_count].texture_index = texture_index;
	ui_elements[ui_elements_count].layer = layer;

	memcpy(&ui_quads_vertices[ui_quads_count * 8], vertices, sizeof(float) * count * 8);
	memcpy(&ui_quads_tex_coords[ui_quads_count * 8], tc, sizeof(float) * count * 8);
	memcpy(&ui_quads_colors[ui_quads_count * 4], color, sizeof(U32) * count * 4);

	ui_quads_count += count;

	ui_elements_count ++;
}

void UIRenderer_Render()
{
	// Create list
	
	for (int i = 0; i <ui_elements_count; i ++)
	{
		ui_elements_index_list[i] = i;
	}
	
	// Sort by texture
	
	for (int i = 1; i < ui_elements_count; i ++)
	{
		for (int j = i; (j > 0) && (ui_elements[ui_elements_index_list[j]].texture_index < ui_elements[ui_elements_index_list[j - 1]].texture_index); j --)
		{
			int index = ui_elements_index_list[j];
			ui_elements_index_list[j] = ui_elements_index_list[j - 1];
			ui_elements_index_list[j - 1] = index;
		}
	}

	// Sort by layer
	
	for (int i = 1; i < ui_elements_count; i ++)
	{
		for (int j = i; (j > 0) && (ui_elements[ui_elements_index_list[j]].layer < ui_elements[ui_elements_index_list[j - 1]].layer); j --)
		{
			int index = ui_elements_index_list[j];
			ui_elements_index_list[j] = ui_elements_index_list[j - 1];
			ui_elements_index_list[j - 1] = index;
		}
	}
	
	// Render
	
	renderPipeline->effect_gui_sprite->applyGraphicsProgram();
        
    GLuint position   = renderPipeline->effect_gui_sprite->getAttributeByName("input_position");
    GLuint texCoord   = renderPipeline->effect_gui_sprite->getAttributeByName("input_texCoord");
	GLuint color      = renderPipeline->effect_gui_sprite->getAttributeByName("input_color");
	GLuint texture    = renderPipeline->effect_gui_sprite->getUniformByName("input_texture");
    GLuint mvp        = renderPipeline->effect_gui_sprite->getUniformByName("input_mvp");
    
 	glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(texCoord);
	glEnableVertexAttribArray(color);
    
	glUniformMatrix4fv(mvp, 1, GL_FALSE, &renderPipeline->global_gui_matrix_wvp.m[0][0]);

	glUniform1i(texture, 0);    
    glActiveTexture(GL_TEXTURE0);

	int prev_texture_index = -1;
	
	for (int i = 0; i < ui_elements_count; i ++)
	{
		UIRendererElement *element = &ui_elements[ui_elements_index_list[i]];

		int texture_index = element->texture_index;

		if (texture_index != prev_texture_index)
		{
			textureManager->setTextureByIndex(texture_index);
			prev_texture_index = texture_index;
		}
		
		glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 0, &ui_quads_vertices[element->quad_index * 8]);
		glVertexAttribPointer(color, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, &ui_quads_colors[element->quad_index * 4]);
		glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, &ui_quads_tex_coords[element->quad_index * 8]);
    
		glDrawElements(GL_TRIANGLES, element->quads_count * 6, GL_UNSIGNED_SHORT, &ui_quads_index_array);

		//LOGI("%d %d", element->texture_index, element->layer);
	}
	
	ui_quads_count = 0;

	ui_elements_count = 0;
}

void UIRenderer_Release()
{
	//
}
