
#pragma once

#include <vector>
#include "Common.h"


class UINode
{
public:
	UINode* parent = nullptr;
	float x = 0.0f;
	float y = 0.0f;
	bool visible = true;

	UI* ui = nullptr;
	
	UINode(float _x, float _y, UINode* const _parent, UI* _ui);
	~UINode();
};

class UI
{
private:
	std::vector<UINode*> nodes;

public:
	void TouchBegan(float x, float y);
	void TouchMoved(int touch_id, float x, float y);
	void TouchEnded(int touch_id, float x, float y);
	void TouchesReset();

	UI();
	~UI();

	UINode* CreateNode(float _x, float _y, UINode* const _parent);
	//UINode* CreateButton(float _x, float _y, UINode* const _parent);

	void RemoveFromList(UINode* node);
};