#include <iostream>
#include "MainLayer.h"

using namespace std;
using namespace cocos2d;
using namespace extension;

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

    // Обработчики мыши
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

    {
        m_plusButton = ControlButton::create(Label::createWithTTF("+", "fonts/arial.ttf", 36), Scale9Sprite::create("button.png"));
        m_plusButton->setBackgroundSpriteForState(Scale9Sprite::create("buttonHighlighted.png"), Control::State::HIGH_LIGHTED);
        m_plusButton->setPosition(30, 700);
        m_plusButton->addTargetWithActionForControlEvents(this, static_cast<void (Ref::*)(Ref*, Control::EventType)>(&MainLayer::plus), Control::EventType::TOUCH_DOWN);
        addChild(m_plusButton);
    }

    {
        m_minusButton = ControlButton::create(Label::createWithTTF("-", "fonts/arial.ttf", 36), Scale9Sprite::create("button.png"));
        m_minusButton->setBackgroundSpriteForState(Scale9Sprite::create("buttonHighlighted.png"), Control::State::HIGH_LIGHTED);
        m_minusButton->setPosition(30, 600);
        m_minusButton->addTargetWithActionForControlEvents(this, static_cast<void (Ref::*)(Ref*, Control::EventType)>(&MainLayer::minus), Control::EventType::TOUCH_DOWN);
        addChild(m_minusButton);
    }

    {
        stringstream ss;
        ss << "Количество\nзаблокированных\nклеток: " << m_disabled;
        m_disabledCells = Label::createWithTTF(ss.str(), "fonts/arial.ttf", 14);
        m_disabledCells->setPosition(30, 650);
        m_disabledCells->setAnchorPoint(Vec2(.2f, .5f));
        addChild(m_disabledCells);
    }

    createWat(this, visibleSize);

    switchState<PrepareState>(true);

    return true;
}

void MainLayer::menuCloseCallback(Ref*) {
    Director::getInstance()->end();
}

void MainLayer::repeat(Cell start, Cell end, function<void()> doAfter,
        function<void()> doError) {

    // Очищаем доску с маркерами и заполняем -1 те клетки которые у нас заблокированы
    vector<int> chessBoard(rows * cols);
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            chessBoard[cellIndex(row, col)] = -1 * !m_cells[cellIndex(row, col)].m_spriteEnabled->isVisible();
        }
    }

    assert(chessBoard.size() == rows * cols);

    vector<Cell> tour;
    tour.push_back(start);
    chessBoard[cellIndex(start)] = 1;

    // Ищем путь
    bool tourExists = findKnightsTour(tour, chessBoard, 1, start, end);
    if(!tourExists && doError){
        doError();
    }

    // Копазываем маркеры на доске
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            m_cells[cellIndex(row, col)].m_label->setString(to_string(chessBoard[cellIndex(row, col)]));
        }
    }

    // Удаляем старые линии
    if(m_lines){
        removeChild(m_lines, true);
        m_lines = nullptr;
    }

    // Строим новые линии по найденному пути
    m_lines = DrawNode::create();
    for(size_t i = 0; i < tour.size() - 1; ++i){
        m_lines->drawLine(
                m_cells[cellIndex(tour[i])].m_spriteEnabled->getPosition(),
                m_cells[cellIndex(tour[i + 1])].m_spriteEnabled->getPosition(),
                Color4F(1.f, 0.f, 0.f, 1.f));
    }
    addChild(m_lines);

    // Строим анимацию коня по новым линиям по найденному пути
    Vector<FiniteTimeAction*> knightsMoves;
    for(const auto& c: tour){
        knightsMoves.pushBack(MoveTo::create(.2f, m_cells[cellIndex(c)].m_spriteEnabled->getPosition()));
    }
    knightsMoves.pushBack(DelayTime::create(.5f));

    knightsMoves.pushBack(CallFunc::create([=]{
        m_state->handleTourAnimationEnd(this);
    }));
    knightsMoves.pushBack(CallFunc::create(doAfter));

    // Запускаем анимацию
    m_knight->runAction(Sequence::create(knightsMoves));
}

void MainLayer::createCells(Node* layer) {
    for(int row = 0; row < rows; ++row)
        for(int col = 0; col < cols; ++col)
            m_cells.push_back(createCell(layer, {row, col}));
}

const char* rows_labels[] = {"a", "b", "c", "d", "e", "f", "g", "h"};
const char* cols_labels[] = {"1", "2", "3", "4", "5", "6", "7", "8"};

MainLayer::CellNode MainLayer::createCell(Node* layer, Cell cell) {
    assert(layer);

    auto spriteEnabled = Sprite::create("cell_normal/" + string(rows_labels[cell.row]) + string(cols_labels[cell.col]) + ".png");
    assert(spriteEnabled);
    spriteEnabled->retain();
    spriteEnabled->setVisible(true);
    layer->addChild(spriteEnabled);

    auto spriteDisabled = Sprite::create("cell_disabled/" + string(rows_labels[cell.row]) + string(cols_labels[cell.col]) + ".png");
    assert(spriteDisabled);
    spriteDisabled->retain();
    spriteDisabled->setVisible(false);
    layer->addChild(spriteDisabled);

    auto label = Label::createWithTTF(string(rows_labels[cell.row]) + cols_labels[cell.col], "fonts/arial.ttf", 12);
    assert(label);
    label->retain();
    label->setColor(Color3B { 0, 0, 0 });
    layer->addChild(label);
    return {spriteEnabled, spriteDisabled, label, cell};
}

MainLayer::Cell MainLayer::randomCell() {
    return {
        static_cast<int>(rows * (rand() / double(RAND_MAX))),
        static_cast<int>(cols * (rand() / double(RAND_MAX)))
    };
}

MainLayer::Cell MainLayer::randomEnabledCell() const {

    // Проверяем что есть не заблокированые клетки чтобы не зависнуть
    assert(m_disabled != rows * cols);

    while (true) {
        auto rc = randomCell();
        if (m_cells[cellIndex(rc)].m_spriteEnabled->isVisible())
            return rc;
    }
}

size_t MainLayer::findClosestCellNode(const Vec2& mousePos) const {

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

void MainLayer::reEnableCells() {
    for (auto & c : m_cells) {
        c.m_spriteEnabled->setVisible(true);
        c.m_spriteDisabled->setVisible(false);
    }
}

void MainLayer::reDisableCells() {
    reEnableCells();

    // Проверяем что есть не заблокированые клетки чтобы не зависнуть
    assert(m_disabled != rows * cols);

    for (size_t i = 0; i < m_disabled; /*++i*/) {
        auto rc = randomCell();
        if (m_cells[cellIndex(rc)].m_spriteEnabled->isVisible()) {
            m_cells[cellIndex(rc)].m_spriteEnabled->setVisible(false);
            m_cells[cellIndex(rc)].m_spriteDisabled->setVisible(true);
            ++i;
        }
    }
}

int MainLayer::smul(const Cell& a, const Cell& b) {
    return a.row * b.row + a.col * b.col;
}

float MainLayer::len(const Cell& a) {
    return sqrt(a.row * a.row + a.col * a.col);
}

float MainLayer::icos(const Cell& c1, const Cell& c2) {
    return smul(c1, c2) / (len(c1) * len(c2));
}

void MainLayer::optimumOffset(vector<MainLayer::Cell>& offset, const Cell& start, const Cell& end) {
    Cell dir{end.row - start.row, end.col - start.col};
    std::sort(offset.begin(), offset.end(), [&dir](const Cell& a, const Cell& b){
        return icos(a, dir) > icos(b, dir);
    });
}

// Смещения "возможные ходы Коня"
vector<MainLayer::Cell> offset{
    {1, 2},
    {2, 1},

    {2, -1},
    {1, -2},

    {-1, -2},
    {-2, -1},

    {-2, 1},
    {-1, 2},
};

bool MainLayer::findKnightsTour(
        vector<Cell>& tour,
        vector<int>& chessBoard,
        int position,
        const Cell& currentCell,
        const Cell& endCell
        ) {

    optimumOffset(offset, currentCell, endCell);

    for (const auto& c : offset) {
        int col = currentCell.col + c.col;
        int row = currentCell.row + c.row;

        if (col >= 0 && col < cols && row >= 0 && row < rows && chessBoard[cellIndex(row, col)] == 0) {
            Cell r { row, col };
            tour.push_back(r);

            position++;
            chessBoard[cellIndex(row, col)] = position;

            if (r.row == endCell.row && r.col == endCell.col)
                return true;
            return findKnightsTour(tour, chessBoard, position, r, endCell);
        }
    }
    if (tour.size() > 1) {
        tour.pop_back();
        return findKnightsTour(tour, chessBoard, position, tour.back(), endCell);
    } else {
        return false;
    }
}

void MainLayer::startButtonHandler(Ref*, Control::EventType eventType) {
    m_state->handleStart(this);
}

void MainLayer::autoButtonHandler(Ref*, Control::EventType eventType) {
    m_state->handleAuto(this);
}

string MainLayer::updateDisableLabelText() {
    stringstream ss;
    ss << "Количество\nзаблокированных\nклеток: " << m_disabled;
    return ss.str();
}

void MainLayer::plus(Ref*, cocos2d::extension::Control::EventType eventType) {
    if(m_disabled < (rows * cols - 2))
        m_disabled++;
    m_disabledCells->setString(updateDisableLabelText());
}

void MainLayer::minus(Ref*, cocos2d::extension::Control::EventType eventType) {
    if(m_disabled > 0)
        m_disabled--;
    m_disabledCells->setString(updateDisableLabelText());
}

MainLayer::PrepareState::PrepareState(bool firstTime) :
        m_firstTime(firstTime) {
}

void MainLayer::PrepareState::onEnter(MainLayer* m) {
    m->m_knight->setVisible(!m_firstTime);
    m->m_target->setVisible(!m_firstTime);
    m->m_startButton->setEnabled(true);
    m->m_startButton->setEnabled(true);
    m->m_help->setString(R"(Выберите режим работы 
-- для ручного режима работы нажмите "Начать"
-- для автоматического режима работы нажмите "Авто режим")");
}

void MainLayer::PrepareState::onExit(MainLayer* m) {
    m->m_help->setString("");
}

void MainLayer::PrepareState::handleStart(MainLayer* m) {
    m->switchState<ManualState>();
}

void MainLayer::PrepareState::handleAuto(MainLayer* m) {
    m->switchState<AutoState>();
}

void MainLayer::ManualState::onEnter(MainLayer* m) {
    if(m->m_lines)
        m->m_lines->setVisible(false);

    m->m_target->setVisible(false);

    m->m_startButton->setEnabled(true);
    m->m_startButton->setEnabled(true);
    m->m_help->setString(R"(-- Для начала демонстрации перетащите Коня и цель на желаемые клетки и нажмите "Начать"
-- Для показа демонстрации в Автоматическом режиме нажмите "Авто режим")");

    m->reDisableCells();

    m_currentFigure = m_figures.end();
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
        m->repeat(knightCell, targetCell, {}, [m]{
            m->switchState<WatState>([m]{
                m->switchState<PrepareState>();
            }, "Нету решения для хода :(");
        });
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
            auto sprite = m->m_cells[closestCell].m_spriteEnabled;
            m_onEnable = sprite->isVisible();
            m_currentFigure->m_figure->setPosition(sprite->getPosition());
            m_currentFigure->m_cell = m->m_cells[closestCell].m_cell;
        } else if (action == Action::Down) {
            if(m_onEnable)
                m_currentFigure++;
        }
    } else {
        if (action == Action::Up && !m_check) {
            m_figures = { {m->m_knight, {}}, {m->m_target, {}}};
            m_currentFigure = m_figures.begin();
            m_check = true;
        }
    }
}

void MainLayer::TourAnimationState::onEnter(MainLayer* m) {
    m->m_startButton->setEnabled(false);
    m->m_startButton->setEnabled(false);
    m->m_help->setString(R"(-- После завершения анимации программа перейдёт в ручной режим работы)");
}

void MainLayer::TourAnimationState::onExit(MainLayer* m) {
    m->m_help->setString("");
}

void MainLayer::TourAnimationState::handleTourAnimationEnd(MainLayer* m) {
    m->switchState<PrepareState>();
}

void MainLayer::AutoState::onEnter(MainLayer* m) {
    if (!m->m_target->isVisible()) {
        m->m_target->setVisible(true);
    }
    if (!m->m_knight->isVisible()) {
        m->m_knight->setVisible(true);
    }
    m->m_startButton->setEnabled(false);
    m->m_help->setString(R"(-- Чтобы закончить демонстрацию в Автоматическом режиме нажмите "Авто режим" и после завершения анимации программа перейдёт в режим режим выбора работы)");
    m_func = [=] {
        if(m_exit)
            return;

        m->reDisableCells();

        auto randomTargetCell = m->randomEnabledCell();
        m->m_target->setPosition(m->m_cells[m->cellIndex(randomTargetCell)].m_spriteEnabled->getPosition());

        auto randomKnightCell = m->randomEnabledCell();
        m->m_knight->setPosition(m->m_cells[m->cellIndex(randomKnightCell)].m_spriteEnabled->getPosition());

        m->repeat(randomKnightCell, randomTargetCell, m_func, [m]{
            m->switchState<WatState>([m]{
                m->switchState<AutoState>();
            }, "Нету решения для хода :(");
        });
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
        m->switchState<PrepareState>();
}

const int wd1 = 150, wd2 = 130;

void MainLayer::createWat(cocos2d::Node* l, const cocos2d::Size& size) {
    m_wat = Sprite::create("wat.png");
    assert(m_wat);
    m_wat->setPosition(size.width - size.width / 8 + 30, size.height / 8 - wd1);
    m_wat->setScale(.3f);
    m_wat->setVisible(false);
    l->addChild(m_wat);

    m_watMessage = Label::createWithTTF("", "fonts/arial.ttf", 12);
    assert(m_watMessage);
    m_watMessage->setPosition(size.width - size.width / 8 + 30, size.height / 8 + 70 - wd1);
    m_watMessage->setVisible(false);
    l->addChild(m_watMessage);
}

void MainLayer::WatState::onEnter(MainLayer* m) {
    m->m_wat->setVisible(true);
    m->m_watMessage->setString(m_message);
    m->m_watMessage->setVisible(true);
    m->m_wat->runAction(Sequence::create(
            MoveBy::create(2.f, Vec2(0, wd2)),
            MoveBy::create(2.f, Vec2(0, -wd2)),
            CallFunc::create([=]{
                assert(m_afterWat);
                m_afterWat();
            }),
            nullptr));
    m->m_watMessage->runAction(Sequence::create(
            MoveBy::create(2.f, Vec2(0, wd2)),
            MoveBy::create(2.f, Vec2(0, -wd2)),
            nullptr));
}

void MainLayer::WatState::onExit(MainLayer* m) {
    m->m_wat->setVisible(false);
    m->m_watMessage->setVisible(false);
}
