
#pragma once

#include "Common.h"

class Renderer
{
public:
	enum VariableType
	{
		UNSIGNED_BYTE,
		BYTE,
		UNSIGNED_SHORT,
		SHORT,
		FLOAT
	};

	enum PrimitiveType
	{
		POINTS,
		LINES,
		LINE_LOOP,
		LINE_STRIP,
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN
	};

	enum BlendType
	{
		ZERO,
		ONE,
		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		ONE_MINUS_DST_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		DST_COLOR
	};

	enum MatrixMode
	{
		PROJECTION,
		MODEL,
		TEXTURE
	};

private:
	ApplicationSettings appSettings;

	void *index_array = nullptr;
	int index_array_gl_type;

	int viewport_width, viewport_height;

private:
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

private:
	Renderer(const ApplicationSettings& settings);
	
public:
	static Renderer* Renderer::Create(const ApplicationSettings& settings);
	~Renderer();
	
	void CreateTexture(U32 *texture_id_ptr, U8 *image_data, I32 width, I32 height, I32 bpp, bool compressed, I32 compressed_size, bool clamped, bool nearest);
	void BindTexture(U32 texture_id);
	void DeleteTexture(U32 *texture_id_ptr);

	void SetActiveTextureLayer(int layer);

	void Clear(float r, float g, float b, float a);
	void SwapBuffers();

	void SetMatrixMode(enum TRMatrixMode matrix_mode);
	void ResetMatrix();
	void SetMatrix(const float *m);
	void GetMatrix(enum TRMatrixMode matrix_mode, float *m);
	void MatrixTranslate(float x, float y, float z);
	void MatrixRotate(float angle, float x, float y, float z);
	void MatrixScale(float x, float y, float z);
	void PushMatrix();
	void PopMatrix();
	void SetProjectionOrtho();
	void SetProjectionFrustum(float z_near, float z_far, float fov_x, float fov_y);

	void SetColor(float r, float g, float b, float a);
	void SetBlendFunc(BlendType sfactor, BlendType dfactor);

	void EnableFaceCulling();
	void DisableFaceCulling();
	void EnableTextures();
	void DisableTextures();
	void EnableBlend();
	void DisableBlend();
	void EnableDepthMask();
	void DisableDepthMask();
	void EnableDepthTest();
	void DisableDepthTest();
	void EnableAlphaTest();
	void DisableAlphaTest();
	void EnableVertexArray();
	void DisableVertexArray();
	void EnableColorArray();
	void DisableColorArray();
	void EnableTexCoordArray();
	void DisableTexCoordArray();
	void EnableIndexArray();
	void DisableIndexArray();

	void SetVertexArray(void *pointer, int size, VariableType type, int stride);
	void SetColorArray(void *pointer, int size, VariableType type, int stride);
	void SetTexCoordArray(void *pointer, int size, VariableType type, int stride);
	void SetIndexArray(void *pointer, VariableType type);

	void DrawArrays(PrimitiveType primitive_type, int vertex_count);
};