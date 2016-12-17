
#define GL_GLEXT_PROTOTYPES

#include <windows.h>
#include "glee.h"

#include "Common.h"
#include "FastMath.h"
#include "Vector.h"
#include "Renderer.h"

HGLRC hRC = nullptr;
HDC hDC = nullptr;

bool RendererGL10_Init(HWND hWnd);
void RendererGL10_Release(HWND hWnd);
void RendererGL10_SwapBuffers();

bool RendererGL10_Init(HWND hWnd)
{
	GLuint		PixelFormat;

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,								// Version Number
		PFD_DRAW_TO_WINDOW |			// Format Must Support Window
		PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,				// Must Support Double Buffering
		PFD_TYPE_RGBA,					// Request An RGBA Format
		32,								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,				// Color Bits Ignored
		0,								// No Alpha Buffer
		0,								// Shift Bit Ignored
		0,								// No Accumulation Buffer
		0, 0, 0, 0,						// Accumulation Bits Ignored
		16,								// 16Bit Z-Buffer (Depth Buffer)
		0,								// No Stencil Buffer
		0,								// No Auxiliary Buffer
		PFD_MAIN_PLANE,					// Main Drawing Layer
		0,								// Reserved
		0, 0, 0							// Layer Masks Ignored
	};		

	if (!(hDC = GetDC(hWnd)))
	{
		RendererGL10_Release(hWnd);
		MessageBox(nullptr, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return nullptr;
	}		

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
	{
		RendererGL10_Release(hWnd);
		MessageBox(nullptr, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}		

	if(!SetPixelFormat(hDC, PixelFormat, &pfd))
	{
		RendererGL10_Release(hWnd);
		MessageBox(nullptr, "Can't Set The PixelFormat.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}		
	
	if (!(hRC = wglCreateContext(hDC)))
	{
		RendererGL10_Release(hWnd);
		MessageBox(nullptr, "Can't Create A GL Rendering Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}		
	
	if(!wglMakeCurrent(hDC, hRC))
	{
		RendererGL10_Release(hWnd);
		MessageBox(nullptr, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

void RendererGL10_Release(HWND hWnd)
{
	if (hRC)
	{		
		if (!wglMakeCurrent(nullptr, nullptr))
		{		
			MessageBox(nullptr, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}		

		if (!wglDeleteContext(hRC))
		{		
			MessageBox(nullptr, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		hRC = nullptr;
	}		

	ReleaseDC(hWnd, hDC);
}

void RendererGL10_SwapBuffers()
{
	if (hDC)
		SwapBuffers(hDC);
}

int RendererGL10_GetGLType(Renderer::VariableType type)
{
	int result = GL_FLOAT;

	switch (type)
	{
		case Renderer::VariableType::UNSIGNED_BYTE:
            result = GL_UNSIGNED_BYTE;
            break;

		case Renderer::VariableType::BYTE:
            result = GL_BYTE;
            break;

		case Renderer::VariableType::UNSIGNED_SHORT:
            result = GL_UNSIGNED_SHORT;
            break;

		case Renderer::VariableType::SHORT:
            result = GL_SHORT;
            break;

		case Renderer::VariableType::FLOAT:
            result = GL_FLOAT;
            break;
	}

	return result;
}

int RendererGL10_GetGLPrimitiveType(Renderer::PrimitiveType type)
{
	int result = GL_POINTS;

	switch (type)
	{
		case Renderer::PrimitiveType::POINTS:
            result = GL_POINTS;
            break;

		case Renderer::PrimitiveType::LINES:
            result = GL_LINES;
            break;

		case Renderer::PrimitiveType::LINE_LOOP:
            result = GL_LINE_LOOP;
            break;

		case Renderer::PrimitiveType::LINE_STRIP:
            result = GL_LINE_STRIP;
            break;

		case Renderer::PrimitiveType::TRIANGLES:
            result = GL_TRIANGLES;
            break;

		case Renderer::PrimitiveType::TRIANGLE_STRIP:
            result = GL_TRIANGLE_STRIP;
            break;

		case Renderer::PrimitiveType::TRIANGLE_FAN:
            result = GL_TRIANGLE_FAN;
            break;
	}

	return result;
}

int RendererGL10_GetGLBlendType(Renderer::BlendType type)
{
	int result = GL_ZERO;
    
	switch (type)
	{
		case Renderer::BlendType::ZERO:
            result = GL_ZERO;
            break;
            
        case Renderer::BlendType::ONE:
            result = GL_ONE;
            break;
            
        case Renderer::BlendType::SRC_COLOR:
            result = GL_SRC_COLOR;
            break;

		case Renderer::BlendType::ONE_MINUS_SRC_COLOR:
            result = GL_ONE_MINUS_SRC_COLOR;
            break;

		case Renderer::BlendType::ONE_MINUS_DST_COLOR:
            result = GL_ONE_MINUS_DST_COLOR;
            break;
            
        case Renderer::BlendType::SRC_ALPHA:
            result = GL_SRC_ALPHA;
            break;
            
        case Renderer::BlendType::ONE_MINUS_SRC_ALPHA:
            result = GL_ONE_MINUS_SRC_ALPHA;
            break;
            
        case Renderer::BlendType::DST_COLOR:
            result = GL_DST_COLOR;
            break;
	}
    
	return result;
}


Renderer* Renderer::Create(const ApplicationSettings& settings)
{
	if (!RendererGL10_Init(settings.hWnd))
		return nullptr;

	Renderer* result = new Renderer(settings);

	return result;
}

Renderer::Renderer(const ApplicationSettings& settings)
{
	appSettings = settings;

	viewport_width = settings.screen_width;
	viewport_height = settings.screen_height;
    
    force_mode = true;

	glViewport(0, 0, viewport_width, viewport_height);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    this->EnableDepthMask();
	glDepthFunc(GL_LEQUAL);
	this->EnableDepthTest();
	glEnable(GL_CULL_FACE);
	this->SetBlendFunc(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	this->DisableBlend();
    glAlphaFunc(GL_GREATER, 0.0f);
    this->DisableAlphaTest();

	Renderer::EnableVertexArray();
    Renderer::EnableColorArray();
    Renderer::EnableTexCoordArray();
    Renderer::DisableIndexArray();

    Renderer::EnableTextures();

	force_mode = false;
}

Renderer::~Renderer()
{
	RendererGL10_Release(appSettings.hWnd);
}

void Renderer::CreateTexture(U32 *texture_id_ptr, U8 *image_data, I32 width, I32 height, I32 bpp, bool compressed, I32 compressed_size, bool clamped, bool nearest)
{
    Renderer::EnableTextures();
    
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

void Renderer::BindTexture(U32 texture_id)
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Renderer::DeleteTexture(U32 *texture_id_ptr)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, (GLuint *)texture_id_ptr);
}

void Renderer::SetActiveTextureLayer(int layer)
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

void Renderer::Clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetMatrixMode(enum TRMatrixMode matrix_mode)
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

void Renderer::ResetMatrix()
{
	glLoadIdentity();
}

void Renderer::SetMatrix(const float *m)
{
	glLoadMatrixf(m);
}

void Renderer::GetMatrix(enum TRMatrixMode matrix_mode, float *m)
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

void Renderer::MatrixTranslate(float x, float y, float z)
{
	glTranslatef(x, y, z);
}

void Renderer::MatrixRotate(float angle, float x, float y, float z)
{
	glRotatef(angle, x, y, z);
}

void Renderer::MatrixScale(float x, float y, float z)
{
	glScalef(x, y, z);
}

void Renderer::PushMatrix()
{
	glPushMatrix();
}

void Renderer::PopMatrix()
{
	glPopMatrix();
}

void Renderer::SetProjectionOrtho()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	float scale = 0.5f / appSettings.pixel_scale;
    
	glOrtho(-viewport_width * scale, viewport_width * scale, -viewport_height * scale, viewport_height * scale, -2048.0f, 2048.0f);
    
	glTranslatef(-viewport_width * scale, -viewport_height * scale, 0.0f);
}

void Renderer::SetProjectionFrustum(float z_near, float z_far, float fov_x, float fov_y) // TODO: is it works
{
    GLfloat size_x, size_y;
    size_y = z_near * tanf(DEG2RAD(fov_y) * 0.5f);
    size_x = size_y * (fov_x / fov_y);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glFrustum(-size_x, size_x, -size_y, size_y, z_near, z_far);
}

void Renderer::SetColor(float r, float g, float b, float a)
{
	glColor4f(r, g, b, a);
}

void Renderer::SetBlendFunc(BlendType sfactor, BlendType dfactor)
{
    glBlendFunc(RendererGL10_GetGLBlendType(sfactor), RendererGL10_GetGLBlendType(dfactor));
}

void Renderer::EnableFaceCulling()
{
	glEnable(GL_CULL_FACE);
}

void Renderer::DisableFaceCulling()
{
	glDisable(GL_CULL_FACE);
}

void Renderer::EnableTextures()
{
	if (!textures_enabled || force_mode)
	{
		glEnable(GL_TEXTURE_2D);
		textures_enabled = true;
	}
}

void Renderer::DisableTextures()
{
	if (textures_enabled || force_mode)
	{
		glDisable(GL_TEXTURE_2D);
		textures_enabled = false;
	}
}

void Renderer::EnableBlend()
{
	if (!blend_enabled || force_mode)
	{
		glEnable(GL_BLEND);
		blend_enabled = true;
	}
}

void Renderer::DisableBlend()
{
	if (blend_enabled || force_mode)
	{
		glDisable(GL_BLEND);
		blend_enabled = false;
	}
}

void Renderer::EnableDepthMask()
{
	if (!depth_mask_enabled || force_mode)
	{
		glDepthMask(GL_TRUE);
		depth_mask_enabled = true;
	}
}

void Renderer::DisableDepthMask()
{
	if (depth_mask_enabled || force_mode)
	{
		glDepthMask(GL_FALSE);
		depth_mask_enabled = false;
	}
}

void Renderer::EnableDepthTest()
{
	if (!depth_test_enabled || force_mode)
	{
		glEnable(GL_DEPTH_TEST);
		depth_test_enabled = true;
	}
}

void Renderer::DisableDepthTest()
{
	if (depth_test_enabled || force_mode)
	{
		glDisable(GL_DEPTH_TEST);
		depth_test_enabled = false;
	}
}

void Renderer::EnableAlphaTest()
{
	if (!alpha_test_enabled || force_mode)
	{
		glEnable(GL_ALPHA_TEST);
		alpha_test_enabled = true;
	}
}

void Renderer::DisableAlphaTest()
{
	if (alpha_test_enabled || force_mode)
	{
		glDisable(GL_ALPHA_TEST);
		alpha_test_enabled = false;
	}
}

void Renderer::EnableVertexArray()
{
	if (!vertex_array_enabled || force_mode)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		vertex_array_enabled = true;
	}
}

void Renderer::DisableVertexArray()
{
	if (vertex_array_enabled || force_mode)
	{
		glDisableClientState(GL_VERTEX_ARRAY);
		vertex_array_enabled = false;
	}
}

void Renderer::EnableColorArray()
{
	if (!color_array_enabled || force_mode)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		color_array_enabled = true;
	}
}

void Renderer::DisableColorArray()
{
	if (color_array_enabled || force_mode)
	{
		glDisableClientState(GL_COLOR_ARRAY);
		color_array_enabled = false;
	}
}

void Renderer::EnableTexCoordArray()
{
	if (!tc_array_enabled || force_mode)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		tc_array_enabled = true;
	}
}

void Renderer::DisableTexCoordArray()
{
	if (tc_array_enabled || force_mode)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		tc_array_enabled = false;
	}
}

void Renderer::EnableIndexArray()
{
	index_array_enabled = true;
}

void Renderer::DisableIndexArray()
{
	index_array_enabled = false;
}

void Renderer::SetVertexArray(void *pointer, int size, VariableType type, int stride)
{
	glVertexPointer(size, RendererGL10_GetGLType(type), stride, pointer);
}

void Renderer::SetColorArray(void *pointer, int size, VariableType type, int stride)
{
	glColorPointer(size, RendererGL10_GetGLType(type), stride, pointer);
}

void Renderer::SetTexCoordArray(void *pointer, int size, VariableType type, int stride)
{
	glTexCoordPointer(size, RendererGL10_GetGLType(type), stride, pointer);
}

void Renderer::SetIndexArray(void *pointer, VariableType type)
{
	index_array = pointer;
	index_array_gl_type = RendererGL10_GetGLType(type);
}

void Renderer::DrawArrays(PrimitiveType primitive_type, int vertex_count)
{
	int e = glGetError();
	if (index_array_enabled)
	{
		glDrawElements(RendererGL10_GetGLPrimitiveType(primitive_type), vertex_count, index_array_gl_type, index_array);
	}
	else
	{
		glDrawArrays(RendererGL10_GetGLPrimitiveType(primitive_type), 0, vertex_count);
	}
	e = glGetError();
}

void Renderer::SwapBuffers()
{
	RendererGL10_SwapBuffers();
}

