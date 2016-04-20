#pragma once

#include <vector>
#include <memory>

namespace cocos2d{
class Node;
}

class Cell;

class Board {
public:
    Board(cocos2d::Node* = nullptr);
    virtual ~Board();

private:
    void createCells(cocos2d::Node*);

    using Cells = std::vector<std::unique_ptr<Cell>>;
    Cells m_cells;
};
