
#include "Common.h"
#include "UI.h"
#include "UINode.h"

UINode::UINode(float _x, float _y, UINode* const _parent/*, UI* _ui*/)
{
	x = _x;
	y = _y;
	parent = _parent;

	//ui = _ui;
}

UINode::~UINode()
{
	// TODO: check for null
	//ui->RemoveFromList(this);
}