#pragma once

#include <vector>
#include <memory>

#include <cocos2d.h>

struct Cell {
    int row;
    int col;
};

struct CellNode {
    cocos2d::Sprite* m_spriteEnabled, *m_spriteDisabled;
    cocos2d::Label* m_label;
};

class Board: public cocos2d::Ref {
public:
    Board(cocos2d::Node* = nullptr);
    virtual ~Board();

    static const int side = 80, rows = 8, cols = 8;

    void repeat();

private:
    CellNode createCell(cocos2d::Node*, const std::string& row, const std::string& column);
    void createCells(cocos2d::Node*);

    static Cell randomCell();
    Cell randomEnabledCell() const;

    size_t cellIndex(int row, int col) const {
        return row * cols + col;
    }
    size_t cellIndex(Cell cell) const {
        return cell.row * cols + cell.col;
    }

    bool findKnightsTour(std::vector<Cell> &tour, std::vector<int> &chessBoard, int position, const Cell& currentCell, const Cell& endCell) const;

    size_t m_disabled = 10;

    cocos2d::Node* m_layer = nullptr;

    using Cells = std::vector<CellNode>;
    Cells m_cells;
    cocos2d::Sprite* m_knight = nullptr;
    cocos2d::Sprite* m_target = nullptr;
    cocos2d::DrawNode* m_lines = nullptr;
};
