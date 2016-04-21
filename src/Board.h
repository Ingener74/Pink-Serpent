#pragma once

#include <vector>
#include <memory>

namespace cocos2d {
class Node;
}

struct Cell {
    int row;
    int col;
};

struct CellNode {
    cocos2d::Sprite* m_spriteEnabled, *m_spriteDisabled;
    cocos2d::Label* m_label;
};

class Board {
public:
    Board(cocos2d::Node* = nullptr);
    virtual ~Board();

    const int side = 80, rows = 8, cols = 8;

private:
    CellNode createCell(cocos2d::Node*, const std::string& row, const std::string& column);
    void createCells(cocos2d::Node*);

    Cell randomCell() const;
    Cell randomEnabledCell() const;
//    cocos2d::Vec2 randomEnabledCellPosition() const;

    size_t cellIndex(int row, int col) const {
        return row * cols + col;
    }
    size_t cellIndex(Cell cell) const {
        return cell.row * cols + cell.col;
    }

    float distance(const Cell& c1, const Cell& c2) const;
    Cell closestCell(const std::vector<Cell>& cells, const Cell& cell) const;
    std::vector<Cell> getPosibleMoves(Cell) const;
    std::vector<Cell> findKnightsTour(Cell start, Cell end) const;

    size_t m_disabled = 10;

    using Cells = std::vector<CellNode>;
    Cells m_cells;
    cocos2d::Sprite* m_knight = nullptr;
    cocos2d::Sprite* m_target = nullptr;
};
