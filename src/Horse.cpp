#include <cocos2d.h>
#include "Horse.h"

using namespace std;
using namespace cocos2d;

Horse::Horse(Node* layer) {
	assert(layer);
	m_horse = Sprite::create("figures/wn.png");
	assert(m_horse);
	m_horse->retain();
	layer->addChild(m_horse);

//	MoveBy::create(5.f, Vec2(300, 300));
}

Horse::~Horse() {
	assert(m_horse);
	m_horse->release();
}

