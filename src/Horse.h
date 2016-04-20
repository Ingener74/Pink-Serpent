#pragma once

namespace cocos2d{
class Node;
class Sprite;
}

class Horse {
public:
	Horse(cocos2d::Node*);
	virtual ~Horse();

private:
	cocos2d::Sprite* m_horse = nullptr;
};
