#include <assert.h>
#include <iostream>
#include <cstdlib>
#include <cocos2d.h>
#include "Board.h"

using namespace std;
using namespace cocos2d;

Board::Board(Node* layer) {
    assert(layer);

    m_layer = layer;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    createCells(layer);

    assert(m_cells.size() == rows * cols);

    auto insertOrigin =
        Vec2(visibleSize / 2) +
        Vec2(-side * (rows / 2), -side * (cols / 2)) +
        Vec2(side/2, side/2);

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            const auto pos = insertOrigin + Vec2(side * row, side * col);
            m_cells[cellIndex(row, col)].m_spriteEnabled->setPosition(pos);
            m_cells[cellIndex(row, col)].m_spriteDisabled->setPosition(pos);
            m_cells[cellIndex(row, col)].m_label->setPosition(pos);
        }
    }
    for (size_t i = 0; i < m_disabled; ++i) {
        auto rc = randomCell();
        m_cells[cellIndex(rc)].m_spriteEnabled->setVisible(false);
        m_cells[cellIndex(rc)].m_spriteDisabled->setVisible(true);
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

    auto randomTargetCell = randomEnabledCell();
    {
        m_target = Sprite::create("figures/overlay_selected.png");
        assert(m_target);
        m_target->retain();
        layer->addChild(m_target);
    }

    auto randomKnightCell = randomEnabledCell();
    {
        m_knight = Sprite::create("figures/wn.png");
        assert(m_knight);
        m_knight->retain();
        layer->addChild(m_knight);
    }

    repeat();
}

Board::~Board() {
    assert(m_knight);
    m_knight->release();
    assert(m_target);
    m_target->release();

    for (auto& c : m_cells) {
        c.m_spriteEnabled->release();
        c.m_spriteDisabled->release();
        c.m_label->release();
    }
    m_lines->release();
}

void Board::repeat() {
    assert(m_layer);

    auto randomTargetCell = randomEnabledCell();
    m_target->setPosition(m_cells[cellIndex(randomTargetCell)].m_spriteEnabled->getPosition());

    auto randomKnightCell = randomEnabledCell();
    m_knight->setPosition(m_cells[cellIndex(randomKnightCell)].m_spriteEnabled->getPosition());

    std::vector<int> chessBoard(rows * cols);
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            chessBoard[cellIndex(row, col)] = -1 * !m_cells[cellIndex(row, col)].m_spriteEnabled->isVisible();
        }
    }

    Cell start = randomKnightCell, end = randomTargetCell;

    vector<Cell> tour;
    bool tourExists = findKnightsTour(tour, chessBoard, 1, start, end);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            m_cells[cellIndex(row, col)].m_label->setString(to_string(chessBoard[cellIndex(row, col)]));
        }
    }

    if(m_lines){
        m_layer->removeChild(m_lines, true);
        m_lines = nullptr;
    }

    m_lines = DrawNode::create(2);
    for(size_t i = 0; i < tour.size() - 1; ++i){
        m_lines->drawLine(
                m_cells[cellIndex(tour[i])].m_spriteEnabled->getPosition(),
                m_cells[cellIndex(tour[i + 1])].m_spriteEnabled->getPosition(),
                Color4F(1.f, 0.f, 0.f, 1.f));
    }
    m_layer->addChild(m_lines);

    Vector<FiniteTimeAction*> knightsMoves;
    for(const auto& c: tour){
        knightsMoves.pushBack(MoveTo::create(.2f, m_cells[cellIndex(c)].m_spriteEnabled->getPosition()));
    }
    knightsMoves.pushBack(DelayTime::create(.5f));

    knightsMoves.pushBack(CallFunc::create([this]{
        repeat();
    }));

    m_knight->runAction(Sequence::create(knightsMoves));
}

void Board::createCells(cocos2d::Node* layer) {

    m_cells = {
        createCell(layer, "a", "1"),
        createCell(layer, "a", "2"),
        createCell(layer, "a", "3"),
        createCell(layer, "a", "4"),
        createCell(layer, "a", "5"),
        createCell(layer, "a", "6"),
        createCell(layer, "a", "7"),
        createCell(layer, "a", "8"),

        createCell(layer, "b", "1"),
        createCell(layer, "b", "2"),
        createCell(layer, "b", "3"),
        createCell(layer, "b", "4"),
        createCell(layer, "b", "5"),
        createCell(layer, "b", "6"),
        createCell(layer, "b", "7"),
        createCell(layer, "b", "8"),

        createCell(layer, "c", "1"),
        createCell(layer, "c", "2"),
        createCell(layer, "c", "3"),
        createCell(layer, "c", "4"),
        createCell(layer, "c", "5"),
        createCell(layer, "c", "6"),
        createCell(layer, "c", "7"),
        createCell(layer, "c", "8"),

        createCell(layer, "d", "1"),
        createCell(layer, "d", "2"),
        createCell(layer, "d", "3"),
        createCell(layer, "d", "4"),
        createCell(layer, "d", "5"),
        createCell(layer, "d", "6"),
        createCell(layer, "d", "7"),
        createCell(layer, "d", "8"),

        createCell(layer, "e", "1"),
        createCell(layer, "e", "2"),
        createCell(layer, "e", "3"),
        createCell(layer, "e", "4"),
        createCell(layer, "e", "5"),
        createCell(layer, "e", "6"),
        createCell(layer, "e", "7"),
        createCell(layer, "e", "8"),

        createCell(layer, "f", "1"),
        createCell(layer, "f", "2"),
        createCell(layer, "f", "3"),
        createCell(layer, "f", "4"),
        createCell(layer, "f", "5"),
        createCell(layer, "f", "6"),
        createCell(layer, "f", "7"),
        createCell(layer, "f", "8"),

        createCell(layer, "g", "1"),
        createCell(layer, "g", "2"),
        createCell(layer, "g", "3"),
        createCell(layer, "g", "4"),
        createCell(layer, "g", "5"),
        createCell(layer, "g", "6"),
        createCell(layer, "g", "7"),
        createCell(layer, "g", "8"),

        createCell(layer, "h", "1"),
        createCell(layer, "h", "2"),
        createCell(layer, "h", "3"),
        createCell(layer, "h", "4"),
        createCell(layer, "h", "5"),
        createCell(layer, "h", "6"),
        createCell(layer, "h", "7"),
        createCell(layer, "h", "8"),
    };
}

CellNode Board::createCell(Node* layer, const std::string& row, const std::string& column) {
    assert(layer);
    assert(!row.empty());
    assert(!column.empty());

    auto spriteEnabled = Sprite::create("cell_normal/" + row + column + ".png");
    assert(spriteEnabled);
    spriteEnabled->retain();
    spriteEnabled->setVisible(true);
    layer->addChild(spriteEnabled);

    auto spriteDisabled = Sprite::create("cell_disabled/" + row + column + ".png");
    assert(spriteDisabled);
    spriteDisabled->retain();
    spriteDisabled->setVisible(false);
    layer->addChild(spriteDisabled);

    auto label = Label::createWithTTF(row + column, "fonts/arial.ttf", 12);
    assert(label);
    label->retain();
    label->setColor(Color3B { 0, 0, 0 });
    layer->addChild(label);
    return {spriteEnabled, spriteDisabled, label};
}

Cell Board::randomCell() {
    return {
        static_cast<int>(rows * (rand() / double(RAND_MAX))),
        static_cast<int>(cols * (rand() / double(RAND_MAX)))
    };
}

Cell Board::randomEnabledCell() const {
    { // check has enabled cells
        bool check = false;
        for (size_t i = 0; i < rows * cols; ++i) {
            check = m_cells[i].m_spriteEnabled->isVisible();
        }
        assert(check);
    }

    while (true) {
        auto rc = randomCell();
        if (m_cells[cellIndex(rc)].m_spriteEnabled->isVisible())
            return rc;
    }
}

Cell possibleMoves[8]{
    {1, 2},
    {2, 1},

    {2, -1},
    {1, -2},

    {-1, -2},
    {-2, -1},

    {-2, 1},
    {-1, 2},
};

bool Board::findKnightsTour(std::vector<Cell>& tour, std::vector<int>& chessBoard,
        int position, const Cell& currentCell, const Cell& endCell) const {

    for (const auto& c : possibleMoves) {
        int col = currentCell.col + c.col;
        int row = currentCell.row + c.row;

        if (col >= 0 && col < cols && row >= 0 && row < rows && chessBoard[cellIndex(row, col)] == 0) {
            Cell r { row, col };
            tour.push_back(r);

            chessBoard[cellIndex(row, col)] = position;
            position++;

            if (r.row == endCell.row && r.col == endCell.col)
                return true;
            return findKnightsTour(tour, chessBoard, position, r, endCell);
        }
    }
    return false;
}
