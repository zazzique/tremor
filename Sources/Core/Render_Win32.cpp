
#define GL_GLEXT_PROTOTYPES

#include <windows.h>
#include "glee.h"

#include "Common.h"
#include "FastMath.h"
#include "Vector.h"
#include "Render.h"


int RenderGL10_GetGLType(Render::VariableType type)
{
	int result = GL_FLOAT;

	switch (type)
	{
		case Render::VariableType::UNSIGNED_BYTE:
            result = GL_UNSIGNED_BYTE;
            break;

		case Render::VariableType::BYTE:
            result = GL_BYTE;
            break;

		case Render::VariableType::UNSIGNED_SHORT:
            result = GL_UNSIGNED_SHORT;
            break;

		case Render::VariableType::SHORT:
            result = GL_SHORT;
            break;

		case Render::VariableType::FLOAT:
            result = GL_FLOAT;
            break;
	}

	return result;
}

int RenderGL10_GetGLPrimitiveType(Render::PrimitiveType type)
{
	int result = GL_POINTS;

	switch (type)
	{
		case Render::PrimitiveType::POINTS:
            result = GL_POINTS;
            break;

		case Render::PrimitiveType::LINES:
            result = GL_LINES;
            break;

		case Render::PrimitiveType::LINE_LOOP:
            result = GL_LINE_LOOP;
            break;

		case Render::PrimitiveType::LINE_STRIP:
            result = GL_LINE_STRIP;
            break;

		case Render::PrimitiveType::TRIANGLES:
            result = GL_TRIANGLES;
            break;

		case Render::PrimitiveType::TRIANGLE_STRIP:
            result = GL_TRIANGLE_STRIP;
            break;

		case Render::PrimitiveType::TRIANGLE_FAN:
            result = GL_TRIANGLE_FAN;
            break;
	}

	return result;
}

int RenderGL10_GetGLBlendType(Render::BlendType type)
{
	int result = GL_ZERO;
    
	switch (type)
	{
		case Render::BlendType::ZERO:
            result = GL_ZERO;
            break;
            
        case Render::BlendType::ONE:
            result = GL_ONE;
            break;
            
        case Render::BlendType::SRC_COLOR:
            result = GL_SRC_COLOR;
            break;

		case Render::BlendType::ONE_MINUS_SRC_COLOR:
            result = GL_ONE_MINUS_SRC_COLOR;
            break;

		case Render::BlendType::ONE_MINUS_DST_COLOR:
            result = GL_ONE_MINUS_DST_COLOR;
            break;
            
        case Render::BlendType::SRC_ALPHA:
            result = GL_SRC_ALPHA;
            break;
            
        case Render::BlendType::ONE_MINUS_SRC_ALPHA:
            result = GL_ONE_MINUS_SRC_ALPHA;
            break;
            
        case Render::BlendType::DST_COLOR:
            result = GL_DST_COLOR;
            break;
	}
    
	return result;
}


bool Render::Init(I32 width, I32 height)
{
	viewport_width = width;
	viewport_height = height;
    
    force_mode = true;

	glViewport(0, 0, viewport_width, viewport_height);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    Render::EnableDepthMask();
	glDepthFunc(GL_LEQUAL);
	Render::EnableDepthTest();
	glEnable(GL_CULL_FACE);
	Render::SetBlendFunc(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	Render::DisableBlend();
    glAlphaFunc(GL_GREATER, 0.0f);
    Render::DisableAlphaTest();

	Render::EnableVertexArray();
    Render::EnableColorArray();
    Render::EnableTexCoordArray();
    Render::DisableIndexArray();

    Render::EnableTextures();

	force_mode = false;

	return true;
}

void Render::CreateTexture(U32 *texture_id_ptr, U8 *image_data, I32 width, I32 height, I32 bpp, bool compressed, I32 compressed_size, bool clamped, bool nearest)
{
    Render::EnableTextures();
    
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

void Render::BindTexture(U32 texture_id)
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Render::DeleteTexture(U32 *texture_id_ptr)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, (GLuint *)texture_id_ptr);
}

void Render::SetActiveTextureLayer(int layer)
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

void Render::Clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render::SetMatrixMode(enum TRMatrixMode matrix_mode)
{
	GLenum gl_matrix_mode = GL_MODELVIEW;

	switch (matrix_mode)
	{
		case PROJECTION:
			gl_matrix_mode = GL_PROJECTION;
			break;
		case MODEL:
			gl_matrix_mode = GL_MODELVIEW;
			break;
		case TEXTURE:
			gl_matrix_mode = GL_TEXTURE;
			break;
	}

	glMatrixMode(gl_matrix_mode);
}

void Render::ResetMatrix()
{
	glLoadIdentity();
}

void Render::SetMatrix(const float *m)
{
	glLoadMatrixf(m);
}

void Render::GetMatrix(enum TRMatrixMode matrix_mode, float *m)
{
	GLenum gl_matrix_mode = GL_MODELVIEW_MATRIX;

	switch (matrix_mode)
	{
		case PROJECTION:
			gl_matrix_mode = GL_PROJECTION_MATRIX;
			break;
		case MODEL:
			gl_matrix_mode = GL_MODELVIEW_MATRIX;
			break;
		case TEXTURE:
			gl_matrix_mode = GL_TEXTURE_MATRIX;
			break;
	}

	glGetFloatv(gl_matrix_mode, m);
}

void Render::MatrixTranslate(float x, float y, float z)
{
	glTranslatef(x, y, z);
}

void Render::MatrixRotate(float angle, float x, float y, float z)
{
	glRotatef(angle, x, y, z);
}

void Render::MatrixScale(float x, float y, float z)
{
	glScalef(x, y, z);
}

void Render::PushMatrix()
{
	glPushMatrix();
}

void Render::PopMatrix()
{
	glPopMatrix();
}

void Render::SetProjectionOrtho()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	float scale = 0.5f / coreVariables.pixel_scale;
    
	glOrtho(-viewport_width * scale, viewport_width * scale, -viewport_height * scale, viewport_height * scale, -2048.0f, 2048.0f);
    
	glTranslatef(-viewport_width * scale, -viewport_height * scale, 0.0f);
}

void Render::SetProjectionFrustum(float z_near, float z_far, float fov_x, float fov_y) // TODO: is it works
{
    GLfloat size_x, size_y;
    size_y = z_near * tanf(DEG2RAD(fov_y) * 0.5f);
    size_x = size_y * (fov_x / fov_y);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glFrustum(-size_x, size_x, -size_y, size_y, z_near, z_far);
}

void Render::SetColor(float r, float g, float b, float a)
{
	glColor4f(r, g, b, a);
}

void Render::SetBlendFunc(BlendType sfactor, BlendType dfactor)
{
    glBlendFunc(RenderGL10_GetGLBlendType(sfactor), RenderGL10_GetGLBlendType(dfactor));
}

void Render::EnableFaceCulling()
{
	glEnable(GL_CULL_FACE);
}

void Render::DisableFaceCulling()
{
	glDisable(GL_CULL_FACE);
}

void Render::EnableTextures()
{
	if (!textures_enabled || force_mode)
	{
		glEnable(GL_TEXTURE_2D);
		textures_enabled = true;
	}
}

void Render::DisableTextures()
{
	if (textures_enabled || force_mode)
	{
		glDisable(GL_TEXTURE_2D);
		textures_enabled = false;
	}
}

void Render::EnableBlend()
{
	if (!blend_enabled || force_mode)
	{
		glEnable(GL_BLEND);
		blend_enabled = true;
	}
}

void Render::DisableBlend()
{
	if (blend_enabled || force_mode)
	{
		glDisable(GL_BLEND);
		blend_enabled = false;
	}
}

void Render::EnableDepthMask()
{
	if (!depth_mask_enabled || force_mode)
	{
		glDepthMask(GL_TRUE);
		depth_mask_enabled = true;
	}
}

void Render::DisableDepthMask()
{
	if (depth_mask_enabled || force_mode)
	{
		glDepthMask(GL_FALSE);
		depth_mask_enabled = false;
	}
}

void Render::EnableDepthTest()
{
	if (!depth_test_enabled || force_mode)
	{
		glEnable(GL_DEPTH_TEST);
		depth_test_enabled = true;
	}
}

void Render::DisableDepthTest()
{
	if (depth_test_enabled || force_mode)
	{
		glDisable(GL_DEPTH_TEST);
		depth_test_enabled = false;
	}
}

void Render::EnableAlphaTest()
{
	if (!alpha_test_enabled || force_mode)
	{
		glEnable(GL_ALPHA_TEST);
		alpha_test_enabled = true;
	}
}

void Render::DisableAlphaTest()
{
	if (alpha_test_enabled || force_mode)
	{
		glDisable(GL_ALPHA_TEST);
		alpha_test_enabled = false;
	}
}

void Render::EnableVertexArray()
{
	if (!vertex_array_enabled || force_mode)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		vertex_array_enabled = true;
	}
}

void Render::DisableVertexArray()
{
	if (vertex_array_enabled || force_mode)
	{
		glDisableClientState(GL_VERTEX_ARRAY);
		vertex_array_enabled = false;
	}
}

void Render::EnableColorArray()
{
	if (!color_array_enabled || force_mode)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		color_array_enabled = true;
	}
}

void Render::DisableColorArray()
{
	if (color_array_enabled || force_mode)
	{
		glDisableClientState(GL_COLOR_ARRAY);
		color_array_enabled = false;
	}
}

void Render::EnableTexCoordArray()
{
	if (!tc_array_enabled || force_mode)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		tc_array_enabled = true;
	}
}

void Render::DisableTexCoordArray()
{
	if (tc_array_enabled || force_mode)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		tc_array_enabled = false;
	}
}

void Render::EnableIndexArray()
{
	index_array_enabled = true;
}

void Render::DisableIndexArray()
{
	index_array_enabled = false;
}

void Render::SetVertexArray(void *pointer, int size, VariableType type, int stride)
{
	glVertexPointer(size, RenderGL10_GetGLType(type), stride, pointer);
}

void Render::SetColorArray(void *pointer, int size, VariableType type, int stride)
{
	glColorPointer(size, RenderGL10_GetGLType(type), stride, pointer);
}

void Render::SetTexCoordArray(void *pointer, int size, VariableType type, int stride)
{
	glTexCoordPointer(size, RenderGL10_GetGLType(type), stride, pointer);
}

void Render::SetIndexArray(void *pointer, VariableType type)
{
	index_array = pointer;
	index_array_gl_type = RenderGL10_GetGLType(type);
}

void Render::DrawArrays(PrimitiveType primitive_type, int vertex_count)
{
	int e = glGetError();
	if (index_array_enabled)
	{
		glDrawElements(RenderGL10_GetGLPrimitiveType(primitive_type), vertex_count, index_array_gl_type, index_array);
	}
	else
	{
		glDrawArrays(RenderGL10_GetGLPrimitiveType(primitive_type), 0, vertex_count);
	}
	e = glGetError();
}

void Render::Release()
{
    //
}

