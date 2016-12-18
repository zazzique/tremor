
#include "Common.h"
#include "UINode.h"
#include "UI.h"

UINode::UINode(float _x, float _y, UINode* const _parent, UI* _ui)
{
	x = _x;
	y = _y;
	parent = _parent;

	ui = _ui;
}

UINode::~UINode()
{
	if (ui != nullptr)
		ui->RemoveFromList(this);
}


UI::UI()
{
	//
}

UI::~UI()
{
	for (auto node : nodes)
	{
		if (node != nullptr)
			delete node;
	}

	nodes.erase(nodes.begin(), nodes.end());
}

UINode* UI::CreateNode(float _x, float _y, UINode* const _parent = nullptr)
{
	UINode* node = new UINode(_x, _y, _parent/*, this*/);

	return node;
}

void UI::RemoveFromList(UINode* node)
{
	//nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end()); // TODO: bad
}

void UI::TouchBegan(float x, float y) // TODO: write
{
}

void UI::TouchMoved(int touch_id, float x, float y)
{
}

void UI::TouchEnded(int touch_id, float x, float y)
{
}

void UI::TouchesReset()
{
}
