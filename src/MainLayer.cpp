#include <iostream>
#include "MainLayer.h"

using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;

MainLayer::MainLayer() {
}

MainLayer::~MainLayer() {
}

Scene* MainLayer::scene() {
    auto scene = Scene::create();
    auto layer = MainLayer::create();
    scene->addChild(layer);
    return scene;
}

bool MainLayer::init() {
    if (!Layer::init())
        return false;

    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = [=](Event* e) {
        auto event = dynamic_cast<EventMouse*>(e);
        assert(event);
        if(event->getMouseButton() == MOUSE_BUTTON_LEFT){
            m_state->handleMouse(this, Action::Down, event->getCursorX(), event->getCursorY());
        }
    };
    mouseListener->onMouseUp = [=](Event* e) {
        auto event = dynamic_cast<EventMouse*>(e);
        assert(event);
        if(event->getMouseButton() == MOUSE_BUTTON_LEFT){
            m_state->handleMouse(this, Action::Up, event->getCursorX(), event->getCursorY());
        }
    };
    mouseListener->onMouseMove = [=](Event* e) {
        auto event = dynamic_cast<EventMouse*>(e);
        assert(event);
        if(event->getMouseButton() == MOUSE_BUTTON_LEFT) {
            m_state->handleMouse(this, Action::Move, event->getCursorX(), event->getCursorY());
        }
        else if(event->getMouseButton() == -1) {
            m_state->handleMouse(this, Action::Hover, event->getCursorX(), event->getCursorY());
        }
    };
    _eventDispatcher->addEventListenerWithFixedPriority(mouseListener, 1);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    {
        auto closeItem = MenuItemImage::create("button_red_close.png", "button_grey_close.png",
            CC_CALLBACK_1(MainLayer::menuCloseCallback, this));

        closeItem->setPosition(origin + Vec2(visibleSize) - Vec2(closeItem->getContentSize() / 2));

        {
            auto menu = Menu::create(closeItem, nullptr);
            menu->setPosition(Vec2::ZERO);
            addChild(menu, 1);
        }
    }

    {
        auto label = Label::createWithTTF("Pink Serpent", "fonts/arial.ttf", 24.f);
        label->setPosition(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - label->getContentSize().height);
        addChild(label, 1);
    }

    { // Background
        auto background = Sprite::create("background.png");
        auto size = background->getContentSize();
        float scale = visibleSize.width / size.width * 1.f;
        background->setScale(scale);
        background->setPosition(scale * (size.width / 2), scale * (size.height / 2));
        addChild(background);
    }

    {
        createCells(this);

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
            addChild(leftBorder);
        }
        {
            auto bottomBorder = Sprite::create("cell_normal/border_bottom_legend.png");
            bottomBorder->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * .072f));
            addChild(bottomBorder);
        }

        {
            m_selection = Sprite::create("figures/overlay_moveable.png");
            assert(m_selection);
            m_selection->setVisible(false);
            addChild(m_selection);
        }

        {
            m_target = Sprite::create("figures/overlay_selected.png");
            assert(m_target);
            m_target->setVisible(false);
            addChild(m_target);
        }

        {
            m_knight = Sprite::create("figures/wn.png");
            assert(m_knight);
            m_target->setVisible(false);
            addChild(m_knight);
        }
    }

    auto buttonsOrigin = origin + Vec2(visibleSize.width/2, 25);
    Vec2 buttonsOffset{100, 0};

    {
        m_startButton = ControlButton::create(Label::createWithTTF("Начать", "fonts/arial.ttf", 24), Scale9Sprite::create("button.png"));
        m_startButton->setBackgroundSpriteForState(Scale9Sprite::create("buttonHighlighted.png"), Control::State::HIGH_LIGHTED);
        m_startButton->setPosition(buttonsOrigin + buttonsOffset);
        m_startButton->addTargetWithActionForControlEvents(this, static_cast<void (Ref::*)(Ref*, Control::EventType)>(&MainLayer::startButtonHandler), Control::EventType::TOUCH_DOWN);
        addChild(m_startButton);
    }

    {
        m_autoButton = ControlButton::create(Label::createWithTTF("Авто режим", "fonts/arial.ttf", 24), Scale9Sprite::create("button.png"));
        m_autoButton->setBackgroundSpriteForState(Scale9Sprite::create("buttonHighlighted.png"), Control::State::HIGH_LIGHTED);
        m_autoButton->setPosition(buttonsOrigin - buttonsOffset);
        m_autoButton->addTargetWithActionForControlEvents(this, static_cast<void (Ref::*)(Ref*, Control::EventType)>(&MainLayer::autoButtonHandler), Control::EventType::TOUCH_DOWN);
        addChild(m_autoButton);
    }

    {
        m_help = Label::createWithTTF("", "fonts/arial.ttf", 14, Size(170, 400));
        m_help->setPosition(visibleSize.width - 95, visibleSize.height - 400);
        addChild(m_help);
    }

    switchState<ManualState>();

    return true;
}

void MainLayer::menuCloseCallback(Ref*) {
    Director::getInstance()->end();
}

void MainLayer::repeat(Cell start, Cell end, std::function<void()> doAfter) {
    std::vector<int> chessBoard(rows * cols);
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            chessBoard[cellIndex(row, col)] = -1 * !m_cells[cellIndex(row, col)].m_spriteEnabled->isVisible();
        }
    }

    vector<Cell> tour;
    bool tourExists = findKnightsTour(tour, chessBoard, 1, start, end);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            m_cells[cellIndex(row, col)].m_label->setString(to_string(chessBoard[cellIndex(row, col)]));
        }
    }

    if(m_lines){
        removeChild(m_lines, true);
        m_lines = nullptr;
    }

    m_lines = DrawNode::create();
    for(size_t i = 0; i < tour.size() - 1; ++i){
        m_lines->drawLine(
                m_cells[cellIndex(tour[i])].m_spriteEnabled->getPosition(),
                m_cells[cellIndex(tour[i + 1])].m_spriteEnabled->getPosition(),
                Color4F(1.f, 0.f, 0.f, 1.f));
    }
    addChild(m_lines);

    Vector<FiniteTimeAction*> knightsMoves;
    for(const auto& c: tour){
        knightsMoves.pushBack(MoveTo::create(.2f, m_cells[cellIndex(c)].m_spriteEnabled->getPosition()));
    }
    knightsMoves.pushBack(DelayTime::create(.5f));

    knightsMoves.pushBack(CallFunc::create([=]{
        m_state->handleTourAnimationEnd(this);
    }));
    knightsMoves.pushBack(CallFunc::create(doAfter));

    m_knight->runAction(Sequence::create(knightsMoves));
}

void MainLayer::createCells(cocos2d::Node* layer) {
    for(int row = 0; row < rows; ++row)
        for(int col = 0; col < cols; ++col)
            m_cells.push_back(createCell(layer, {row, col}));
}

const char* rows_[] = {"a", "b", "c", "d", "e", "f", "g", "h"};
const char* cols_[] = {"1", "2", "3", "4", "5", "6", "7", "8"};

CellNode MainLayer::createCell(Node* layer, Cell cell) {
    assert(layer);

    auto spriteEnabled = Sprite::create("cell_normal/" + string(rows_[cell.row]) + string(cols_[cell.col]) + ".png");
    assert(spriteEnabled);
    spriteEnabled->retain();
    spriteEnabled->setVisible(true);
    layer->addChild(spriteEnabled);

    auto spriteDisabled = Sprite::create("cell_disabled/" + string(rows_[cell.row]) + string(cols_[cell.col]) + ".png");
    assert(spriteDisabled);
    spriteDisabled->retain();
    spriteDisabled->setVisible(false);
    layer->addChild(spriteDisabled);

    auto label = Label::createWithTTF(string(rows_[cell.row]) + cols_[cell.col], "fonts/arial.ttf", 12);
    assert(label);
    label->retain();
    label->setColor(Color3B { 0, 0, 0 });
    layer->addChild(label);
    return {spriteEnabled, spriteDisabled, label, cell};
}

Cell MainLayer::randomCell() {
    return {
        static_cast<int>(rows * (rand() / double(RAND_MAX))),
        static_cast<int>(cols * (rand() / double(RAND_MAX)))
    };
}

Cell MainLayer::randomEnabledCell() const {
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

size_t MainLayer::findClosestCellNode(const cocos2d::Vec2& mousePos) const {

    float minDist = numeric_limits<float>::max();
    size_t minIndx = 0;

    for(size_t i = 0; i < m_cells.size(); ++i){
        float dist = mousePos.distance(m_cells[i].m_spriteEnabled->getPosition());
        if(dist < minDist){
            minDist = dist;
            minIndx = i;
        }
    }
    return minIndx;
}

bool MainLayer::findKnightsTour(std::vector<Cell>& tour, std::vector<int>& chessBoard,
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

void MainLayer::startButtonHandler(Ref*, cocos2d::extension::Control::EventType eventType) {
    m_state->handleStart(this);
}

void MainLayer::autoButtonHandler(Ref*, cocos2d::extension::Control::EventType eventType) {
    m_state->handleAuto(this);
}

void MainLayer::ManualState::onEnter(MainLayer* m) {
    m->m_help->setString(R"(-- Для начала демонстрации перетащите Коня и цель на желаемые клетки и нажмите "Начать"
-- Для показа демонстрации в Автоматическом режиме нажмите "Авто режим")");
    m_figures = {{m->m_knight, {}}, {m->m_target, {}}};
    m_currentFigure = m_figures.begin();
}

void MainLayer::ManualState::onExit(MainLayer* m) {
    m->m_help->setString("");
}

void MainLayer::ManualState::handleStart(MainLayer* m) {
    if(m_currentFigure == m_figures.end()){
        assert(m_figures.size() == 2);

        Cell knightCell = m_figures.at(0).m_cell,
            targetCell = m_figures.at(1).m_cell;

        m->switchState<TourAnimationState>();
        m->repeat(knightCell, targetCell, {});
    }
}

void MainLayer::ManualState::handleAuto(MainLayer* m) {
    m->switchState<AutoState>();
}

void MainLayer::ManualState::handleMouse(MainLayer* m, Action action, float x, float y) {
    if (m_currentFigure != m_figures.end()) {
        if (action == Action::Hover) {
            m_currentFigure->m_figure->setVisible(true);
            size_t closestCell = m->findClosestCellNode(Vec2(x, y));
            m_currentFigure->m_figure->setPosition(m->m_cells[closestCell].m_spriteEnabled->getPosition());
            m_currentFigure->m_cell = m->m_cells[closestCell].m_cell;
        } else if (action == Action::Down) {
            m_currentFigure++;
        }
    }
}

void MainLayer::TourAnimationState::onEnter(MainLayer* m) {
}

void MainLayer::TourAnimationState::onExit(MainLayer* m) {
}

void MainLayer::TourAnimationState::handleTourAnimationEnd(MainLayer* m) {
    m->switchState<ManualState>();
}

void MainLayer::AutoState::onEnter(MainLayer* m) {
    m->m_help->setString(R"(-- Чтобы закончить демонстрацию в Автоматическом режиме нажмите "Авто режим")");
    m_func = [=] {
        if(m_exit)
            return;
        auto randomTargetCell = m->randomEnabledCell();
        m->m_target->setPosition(m->m_cells[m->cellIndex(randomTargetCell)].m_spriteEnabled->getPosition());

        auto randomKnightCell = m->randomEnabledCell();
        m->m_knight->setPosition(m->m_cells[m->cellIndex(randomKnightCell)].m_spriteEnabled->getPosition());

        m->repeat(randomKnightCell, randomTargetCell, m_func);
    };
    m_exit = false;
    m_func();
}

void MainLayer::AutoState::onExit(MainLayer* m) {
    m->m_help->setString("");
}

void MainLayer::AutoState::handleAuto(MainLayer*) {
    m_exit = true;
}

void MainLayer::AutoState::handleTourAnimationEnd(MainLayer* m) {
    if (m_exit)
        m->switchState<ManualState>();
}

