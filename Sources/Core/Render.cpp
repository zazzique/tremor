
#define GL_GLEXT_PROTOTYPES

#include <windows.h>
#include "glee.h"
#include "glues_project.h"

#include "Common.h"
#include "FastMath.h"
#include "Vector.h"
#include "Render.h"

bool force_mode = false;

bool textures_enabled = false;
bool blend_enabled = false;
bool depth_mask_enabled = false;
bool depth_test_enabled = false;
bool alpha_test_enabled = false;
bool vertex_array_enabled = false;
bool color_array_enabled = false;
bool tc_array_enabled = false;
bool index_array_enabled = false;

void *index_array = NULL;
int index_array_gl_type;

int r_sx, r_sy;


int Render_GetGLType(enum TRVariableType type)
{
	int result = GL_FLOAT;

	switch (type)
	{
		case TR_UNSIGNED_BYTE:
            result = GL_UNSIGNED_BYTE;
            break;

		case TR_BYTE:
            result = GL_BYTE;
            break;

		case TR_UNSIGNED_SHORT:
            result = GL_UNSIGNED_SHORT;
            break;

		case TR_SHORT:
            result = GL_SHORT;
            break;

		case TR_FLOAT:
            result = GL_FLOAT;
            break;
	}

	return result;
}

int Render_GetGLPrimitiveType(enum TRPrimitiveType type)
{
	int result = GL_POINTS;

	switch (type)
	{
		case TR_POINTS:
            result = GL_POINTS;
            break;

		case TR_LINES:
            result = GL_LINES;
            break;

		case TR_LINE_LOOP:
            result = GL_LINE_LOOP;
            break;

		case TR_LINE_STRIP:
            result = GL_LINE_STRIP;
            break;

		case TR_TRIANGLES:
            result = GL_TRIANGLES;
            break;

		case TR_TRIANGLE_STRIP:
            result = GL_TRIANGLE_STRIP;
            break;

		case TR_TRIANGLE_FAN:
            result = GL_TRIANGLE_FAN;
            break;
	}

	return result;
}

int Render_GetGLBlendType(enum TRBlendType type)
{
	int result = GL_ZERO;
    
	switch (type)
	{
		case TR_ZERO:
            result = GL_ZERO;
            break;
            
        case TR_ONE:
            result = GL_ONE;
            break;
            
        case TR_SRC_COLOR:
            result = GL_SRC_COLOR;
            break;

		case TR_ONE_MINUS_SRC_COLOR:
            result = GL_ONE_MINUS_SRC_COLOR;
            break;

		case TR_ONE_MINUS_DST_COLOR:
            result = GL_ONE_MINUS_DST_COLOR;
            break;
            
        case TR_SRC_ALPHA:
            result = GL_SRC_ALPHA;
            break;
            
        case TR_ONE_MINUS_SRC_ALPHA:
            result = GL_ONE_MINUS_SRC_ALPHA;
            break;
            
        case TR_DST_COLOR:
            result = GL_DST_COLOR;
            break;
	}
    
	return result;
}


bool Render_Init(I32 width, I32 height)
{
	r_sx = width;
	r_sy = height;
    
    force_mode = true;

	glViewport(0, 0, r_sx, r_sy);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    Render_EnableDepthMask();
	glDepthFunc(GL_LEQUAL);
	Render_EnableDepthTest();
	glEnable(GL_CULL_FACE);
	Render_SetBlendFunc(TR_SRC_ALPHA, TR_ONE_MINUS_SRC_ALPHA);
	Render_DisableBlend();
    glAlphaFunc(GL_GREATER, 0.0f);
    Render_DisableAlphaTest();

	Render_EnableVertexArray();
    Render_EnableColorArray();
    Render_EnableTexCoordArray();
    Render_DisableIndexArray();

    Render_EnableTextures();

	force_mode = false;

	return true;
}

void Render_CreateTexture(U32 *texture_id_ptr, U8 *image_data, I32 width, I32 height, I32 bpp, bool compressed, I32 compressed_size, bool clamped, bool nearest)
{
    Render_EnableTextures();
    
	GLint pixel_format;
	if (bpp == 8)
		pixel_format = GL_LUMINANCE;
	else if (bpp == 24)
		pixel_format = GL_RGB;
	else if (bpp == 32)
		pixel_format = GL_RGBA;
	else
		return;

	glEnable(GL_TEXTURE_2D);
	textures_enabled = true;

	glGenTextures(1, (GLuint *)texture_id_ptr);
	glBindTexture(GL_TEXTURE_2D, *texture_id_ptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamped ? GL_CLAMP : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamped ? GL_CLAMP : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, nearest ? GL_NEAREST : GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, nearest ? GL_NEAREST : GL_LINEAR);

    if (compressed)
	{
        //glCompressedTexImage2D(GL_TEXTURE_2D, 0, (bpp == 24) ? GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, width, height, 0, compressed_size, image_data);
	}
    else
	{
        glTexImage2D(GL_TEXTURE_2D, 0, pixel_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, image_data);
	}

	glGetError();
}

void Render_BindTexture(U32 texture_id)
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Render_DeleteTexture(U32 *texture_id_ptr)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, (GLuint *)texture_id_ptr);
}

void Render_ActiveTextureLayer(int layer)
{
    if (layer < 0 || layer > 7)
        return;
    
    if (layer != 0)
        force_mode = true;
    else
        force_mode = false;
    
    GLenum active_texture = GL_TEXTURE0;
    
    switch (layer)
    {
        case 0:
            active_texture = GL_TEXTURE0;
            break;
            
        case 1:
            active_texture = GL_TEXTURE1;
            break;
            
        case 2:
            active_texture = GL_TEXTURE2;
            break;
            
        case 3:
            active_texture = GL_TEXTURE3;
            break;
            
        case 4:
            active_texture = GL_TEXTURE4;
            break;
            
        case 5:
            active_texture = GL_TEXTURE5;
            break;
            
        case 6:
            active_texture = GL_TEXTURE6;
            break;
            
        case 7:
            active_texture = GL_TEXTURE7;
            break;
    }
    
    glActiveTexture(active_texture);
    glClientActiveTexture(active_texture);
    
    if (layer != 0)
    {
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
    }
}

void Render_Clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render_SetMatrixMode(enum TRMatrixMode matrix_mode)
{
	GLenum gl_matrix_mode = GL_MODELVIEW;

	switch (matrix_mode)
	{
		case TR_PROJECTION:
			gl_matrix_mode = GL_PROJECTION;
			break;
		case TR_MODEL:
			gl_matrix_mode = GL_MODELVIEW;
			break;
		case TR_TEXTURE:
			gl_matrix_mode = GL_TEXTURE;
			break;
	}

	glMatrixMode(gl_matrix_mode);
}

void Render_ResetMatrix()
{
	glLoadIdentity();
}

void Render_SetMatrix(const float *m)
{
	glLoadMatrixf(m);
}

void Render_GetMatrix(enum TRMatrixMode matrix_mode, float *m)
{
	GLenum gl_matrix_mode = GL_MODELVIEW_MATRIX;

	switch (matrix_mode)
	{
		case TR_PROJECTION:
			gl_matrix_mode = GL_PROJECTION_MATRIX;
			break;
		case TR_MODEL:
			gl_matrix_mode = GL_MODELVIEW_MATRIX;
			break;
		case TR_TEXTURE:
			gl_matrix_mode = GL_TEXTURE_MATRIX;
			break;
	}

	glGetFloatv(gl_matrix_mode, m);
}

void Render_MatrixTranslate(float x, float y, float z)
{
	glTranslatef(x, y, z);
}

void Render_MatrixRotate(float angle, float x, float y, float z)
{
	glRotatef(angle, x, y, z);
}

void Render_MatrixScale(float x, float y, float z)
{
	glScalef(x, y, z);
}

void Render_PushMatrix()
{
	glPushMatrix();
}

void Render_PopMatrix()
{
	glPopMatrix();
}

void Render_SetProjectionOrtho()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	float scale = 0.5f / coreVariables.pixel_scale;
    
	glOrtho(-r_sx * scale, r_sx * scale, -r_sy * scale, r_sy * scale, -2048.0f, 2048.0f);
    
	glTranslatef(-r_sx * scale, -r_sy * scale, 0.0f);
}

void Render_SetProjectionFrustum(float z_near, float z_far, float fov_x, float fov_y)
{
    GLfloat size_x, size_y;
    size_y = z_near * tanf(DEG2RAD(fov_y) * 0.5f);
    size_x = size_y * (fov_x / fov_y);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glFrustum(-size_x, size_x, -size_y, size_y, z_near, z_far);
}

Vector3D Render_ScreenPointToWorldVector(float x, float y, float z, const float *model_matrix, const float *projection_matrix)
{
	Vector3D result = { 0, 0, 0 };
	GLint viewport[4] = { 0, 0, coreVariables.screen_width, coreVariables.screen_height };

	gluUnProject(x, y, z, model_matrix, projection_matrix, viewport, &result.x, &result.y, &result.z);
	
	return result;
}

Vector3D Render_WorldVectorToScreenPoint(float x, float y, float z, const float *model_matrix, const float *projection_matrix)
{
	Vector3D result = { 0, 0, 0 };
	GLint viewport[4] = { 0, 0, coreVariables.screen_width, coreVariables.screen_height };

	gluProject(x, y, z, model_matrix, projection_matrix, viewport, &result.x, &result.y, &result.z);
	
	return result;
}

void Render_SetColor(float r, float g, float b, float a)
{
	glColor4f(r, g, b, a);
}

void Render_SetBlendFunc(enum TRBlendType sfactor, enum TRBlendType dfactor)
{
    glBlendFunc(Render_GetGLBlendType(sfactor), Render_GetGLBlendType(dfactor));
}

void Render_EnableFaceCulling()
{
	glEnable(GL_CULL_FACE);
}

void Render_DisableFaceCulling()
{
	glDisable(GL_CULL_FACE);
}

void Render_EnableTextures()
{
	if (!textures_enabled || force_mode)
	{
		glEnable(GL_TEXTURE_2D);
		textures_enabled = true;
	}
}

void Render_DisableTextures()
{
	if (textures_enabled || force_mode)
	{
		glDisable(GL_TEXTURE_2D);
		textures_enabled = false;
	}
}

void Render_EnableBlend()
{
	if (!blend_enabled || force_mode)
	{
		glEnable(GL_BLEND);
		blend_enabled = true;
	}
}

void Render_DisableBlend()
{
	if (blend_enabled || force_mode)
	{
		glDisable(GL_BLEND);
		blend_enabled = false;
	}
}

void Render_EnableDepthMask()
{
	if (!depth_mask_enabled || force_mode)
	{
		glDepthMask(GL_TRUE);
		depth_mask_enabled = true;
	}
}

void Render_DisableDepthMask()
{
	if (depth_mask_enabled || force_mode)
	{
		glDepthMask(GL_FALSE);
		depth_mask_enabled = false;
	}
}

void Render_EnableDepthTest()
{
	if (!depth_test_enabled || force_mode)
	{
		glEnable(GL_DEPTH_TEST);
		depth_test_enabled = true;
	}
}

void Render_DisableDepthTest()
{
	if (depth_test_enabled || force_mode)
	{
		glDisable(GL_DEPTH_TEST);
		depth_test_enabled = false;
	}
}

void Render_EnableAlphaTest()
{
	if (!alpha_test_enabled || force_mode)
	{
		glEnable(GL_ALPHA_TEST);
		alpha_test_enabled = true;
	}
}

void Render_DisableAlphaTest()
{
	if (alpha_test_enabled || force_mode)
	{
		glDisable(GL_ALPHA_TEST);
		alpha_test_enabled = false;
	}
}

void Render_EnableVertexArray()
{
	if (!vertex_array_enabled || force_mode)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		vertex_array_enabled = true;
	}
}

void Render_DisableVertexArray()
{
	if (vertex_array_enabled || force_mode)
	{
		glDisableClientState(GL_VERTEX_ARRAY);
		vertex_array_enabled = false;
	}
}

void Render_EnableColorArray()
{
	if (!color_array_enabled || force_mode)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		color_array_enabled = true;
	}
}

void Render_DisableColorArray()
{
	if (color_array_enabled || force_mode)
	{
		glDisableClientState(GL_COLOR_ARRAY);
		color_array_enabled = false;
	}
}

void Render_EnableTexCoordArray()
{
	if (!tc_array_enabled || force_mode)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		tc_array_enabled = true;
	}
}

void Render_DisableTexCoordArray()
{
	if (tc_array_enabled || force_mode)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		tc_array_enabled = false;
	}
}

void Render_EnableIndexArray()
{
	index_array_enabled = true;
}

void Render_DisableIndexArray()
{
	index_array_enabled = false;
}

void Render_SetVertexArray(void *pointer, int size, enum TRVariableType type, int stride)
{
	glVertexPointer(size, Render_GetGLType(type), stride, pointer);
}

void Render_SetColorArray(void *pointer, int size, enum TRVariableType type, int stride)
{
	glColorPointer(size, Render_GetGLType(type), stride, pointer);
}

void Render_SetTexCoordArray(void *pointer, int size, enum TRVariableType type, int stride)
{
	glTexCoordPointer(size, Render_GetGLType(type), stride, pointer);
}

void Render_SetIndexArray(void *pointer, enum TRVariableType type)
{
	index_array = pointer;
	index_array_gl_type = Render_GetGLType(type);
}

void Render_DrawArrays(enum TRPrimitiveType primitive_type, int vertex_count)
{
	int e = glGetError();
	if (index_array_enabled)
	{
		glDrawElements(Render_GetGLPrimitiveType(primitive_type), vertex_count, index_array_gl_type, index_array);
	}
	else
	{
		glDrawArrays(Render_GetGLPrimitiveType(primitive_type), 0, vertex_count);
	}
	e = glGetError();
}

void Render_Release()
{
    //
}

