
#pragma once

#include "Common.h"
//#include "UI.h"

class UINode
{
public:
	UINode* parent = nullptr;
	float x = 0.0f;
	float y = 0.0f;
	bool visible = true;

	//UI* ui = nullptr;
	
	UINode(float _x, float _y, UINode* const _parent);
	~UINode();
};