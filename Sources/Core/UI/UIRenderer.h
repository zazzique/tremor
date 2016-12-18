
#pragma once

#include "Common.h"

void UIRenderer_Init();
void UIRenderer_AddQuadsToRenderList(const float* vertices, const float* tc, const U32* color, int count, int texture_index, int layer);
void UIRenderer_Render();
void UIRenderer_Release();

