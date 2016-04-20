#include <assert.h>
#include <cocos2d.h>
#include "Board.h"
#include "Cell.h"

using namespace std;
using namespace cocos2d;

Board::Board(Node* layer) {
    assert(layer);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    createCells(layer);

    const int side = 80, rows = 8, cols = 8;

    assert(m_cells.size() == rows * cols);

    auto insertOrigin =
        Vec2(visibleSize / 2) +
        Vec2(-side * (rows / 2), -side * (cols / 2)) +
        Vec2(side/2, side/2);

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            const auto pos = insertOrigin + Vec2(side * row, side * col);
            m_cells[row * cols + col]->setPosition(pos);
        }
    }

    {
        auto leftBorder = Sprite::create("cell_normal/border_left_legend.png");
        leftBorder->setPosition(Vec2(visibleSize.width * .178f, visibleSize.height / 2));
        leftBorder->setScale(.945f);
        layer->addChild(leftBorder);
    }
    {
        auto bottomBorder = Sprite::create("cell_normal/border_bottom_legend.png");
        bottomBorder->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * .072f));
        layer->addChild(bottomBorder);
    }
}

Board::~Board() {
}

void Board::createCells(cocos2d::Node* layer) {
    m_cells.emplace_back(new Cell { layer, "a", "1" });
    m_cells.emplace_back(new Cell { layer, "a", "2" });
    m_cells.emplace_back(new Cell { layer, "a", "3" });
    m_cells.emplace_back(new Cell { layer, "a", "4" });
    m_cells.emplace_back(new Cell { layer, "a", "5" });
    m_cells.emplace_back(new Cell { layer, "a", "6" });
    m_cells.emplace_back(new Cell { layer, "a", "7" });
    m_cells.emplace_back(new Cell { layer, "a", "8" });

    m_cells.emplace_back(new Cell { layer, "b", "1" });
    m_cells.emplace_back(new Cell { layer, "b", "2" });
    m_cells.emplace_back(new Cell { layer, "b", "3" });
    m_cells.emplace_back(new Cell { layer, "b", "4" });
    m_cells.emplace_back(new Cell { layer, "b", "5" });
    m_cells.emplace_back(new Cell { layer, "b", "6" });
    m_cells.emplace_back(new Cell { layer, "b", "7" });
    m_cells.emplace_back(new Cell { layer, "b", "8" });

    m_cells.emplace_back(new Cell { layer, "c", "1" });
    m_cells.emplace_back(new Cell { layer, "c", "2" });
    m_cells.emplace_back(new Cell { layer, "c", "3" });
    m_cells.emplace_back(new Cell { layer, "c", "4" });
    m_cells.emplace_back(new Cell { layer, "c", "5" });
    m_cells.emplace_back(new Cell { layer, "c", "6" });
    m_cells.emplace_back(new Cell { layer, "c", "7" });
    m_cells.emplace_back(new Cell { layer, "c", "8" });

    m_cells.emplace_back(new Cell { layer, "d", "1" });
    m_cells.emplace_back(new Cell { layer, "d", "2" });
    m_cells.emplace_back(new Cell { layer, "d", "3" });
    m_cells.emplace_back(new Cell { layer, "d", "4" });
    m_cells.emplace_back(new Cell { layer, "d", "5" });
    m_cells.emplace_back(new Cell { layer, "d", "6" });
    m_cells.emplace_back(new Cell { layer, "d", "7" });
    m_cells.emplace_back(new Cell { layer, "d", "8" });

    m_cells.emplace_back(new Cell { layer, "e", "1" });
    m_cells.emplace_back(new Cell { layer, "e", "2" });
    m_cells.emplace_back(new Cell { layer, "e", "3" });
    m_cells.emplace_back(new Cell { layer, "e", "4" });
    m_cells.emplace_back(new Cell { layer, "e", "5" });
    m_cells.emplace_back(new Cell { layer, "e", "6" });
    m_cells.emplace_back(new Cell { layer, "e", "7" });
    m_cells.emplace_back(new Cell { layer, "e", "8" });

    m_cells.emplace_back(new Cell { layer, "f", "1" });
    m_cells.emplace_back(new Cell { layer, "f", "2" });
    m_cells.emplace_back(new Cell { layer, "f", "3" });
    m_cells.emplace_back(new Cell { layer, "f", "4" });
    m_cells.emplace_back(new Cell { layer, "f", "5" });
    m_cells.emplace_back(new Cell { layer, "f", "6" });
    m_cells.emplace_back(new Cell { layer, "f", "7" });
    m_cells.emplace_back(new Cell { layer, "f", "8" });

    m_cells.emplace_back(new Cell { layer, "g", "1" });
    m_cells.emplace_back(new Cell { layer, "g", "2" });
    m_cells.emplace_back(new Cell { layer, "g", "3" });
    m_cells.emplace_back(new Cell { layer, "g", "4" });
    m_cells.emplace_back(new Cell { layer, "g", "5" });
    m_cells.emplace_back(new Cell { layer, "g", "6" });
    m_cells.emplace_back(new Cell { layer, "g", "7" });
    m_cells.emplace_back(new Cell { layer, "g", "8" });

    m_cells.emplace_back(new Cell { layer, "h", "1" });
    m_cells.emplace_back(new Cell { layer, "h", "2" });
    m_cells.emplace_back(new Cell { layer, "h", "3" });
    m_cells.emplace_back(new Cell { layer, "h", "4" });
    m_cells.emplace_back(new Cell { layer, "h", "5" });
    m_cells.emplace_back(new Cell { layer, "h", "6" });
    m_cells.emplace_back(new Cell { layer, "h", "7" });
    m_cells.emplace_back(new Cell { layer, "h", "8" });
}
