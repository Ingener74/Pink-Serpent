#include <assert.h>
#include <iostream>
#include <cstdlib>
#include <cocos2d.h>
#include "Board.h"

using namespace std;
using namespace cocos2d;

Board::Board(Node* layer) {
    assert(layer);

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
        m_target->setPosition(m_cells[cellIndex(randomTargetCell)].m_spriteEnabled->getPosition());
    }

    auto randomKnightCell = randomEnabledCell();
    {
        m_knight = Sprite::create("figures/wn.png");
        assert(m_knight);
        m_knight->retain();
        layer->addChild(m_knight);

        m_knight->setPosition(m_cells[cellIndex(randomKnightCell)].m_spriteEnabled->getPosition());

//        m_knight->runAction(
//            Repeat::create(
//                Sequence::create(
//                    MoveTo::create(1.f, m_cells[cellIndex(0, 0)].m_spriteEnabled->getPosition()),
//                    MoveTo::create(1.f, m_cells[cellIndex(7, 0)].m_spriteEnabled->getPosition()),
//                    MoveTo::create(1.f, m_cells[cellIndex(7, 7)].m_spriteEnabled->getPosition()),
//                    MoveTo::create(1.f, m_cells[cellIndex(0, 7)].m_spriteEnabled->getPosition()),
//                    nullptr
//                    ),
//                5
//            )
//        );
    }

    auto knightTour = findKnightsTour(randomKnightCell, randomTargetCell);
    cout << "knightTour " << endl;
    for (const auto& c : knightTour) {
        cout << c.row << " " << c.col << endl;
    }
    cout << "knightTour end" << endl;

    Vector<FiniteTimeAction*> knightsMoves;

    for(const auto& c: knightTour){
        knightsMoves.pushBack(MoveTo::create(1.f, m_cells[cellIndex(c)].m_spriteEnabled->getPosition()));
    }

    m_knight->runAction(Sequence::create(knightsMoves));
}

Board::~Board() {
    assert(m_knight);
    m_knight->release();
    assert(m_target);
    m_target->release();
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

Cell Board::randomCell() const {
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

float Board::distance(const Cell& c1, const Cell& c2) const {
    return sqrt((c2.col - c1.col) * (c2.col - c1.col) + (c2.row - c1.row) * (c2.row - c1.row));
}

Cell Board::closestCell(const std::vector<Cell>& cells, const Cell& cell) const {
    size_t resc = 0;
    float resf = std::numeric_limits<float>::max();
    for(size_t i = 0; i < cells.size(); ++i){
        float dist = distance(cells[i], cell);
        if (dist < resf) {
            resf = dist;
            resc = i;
        }
    }
    return cells[resc];
}

std::vector<Cell> Board::getPosibleMoves(Cell cell) const {
    static Cell dirs[8]{
        {1, 2},
        {2, 1},

        {2, -1},
        {1, -2},

        {-1, -2},
        {-2, -1},

        {-2, 1},
        {-1, 2},
    };

    std::vector<Cell> res;
    for(const auto& c: dirs){
        int col = cell.col + c.col;
        int row = cell.row + c.row;

        if(col >= 0 && col <8 && row >= 0 && row < 8)
            if(m_cells[cellIndex(row, col)].m_spriteEnabled->isVisible())
                res.push_back({row, col});
    }
    return res;
}

std::vector<Cell> Board::findKnightsTour(Cell start, Cell end) const {
    Cell cell = start;
    std::vector<Cell> tour;
    do {
        auto posMoves = getPosibleMoves(cell);
        cell = closestCell(posMoves, end);
        tour.push_back(cell);
    } while (cell.row != end.row && cell.col != end.col);
    return tour;
}
