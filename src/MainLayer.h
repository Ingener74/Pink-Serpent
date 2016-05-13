#pragma once

#include <memory>

#include <cocos2d.h>
#include <cocos-ext.h>

class JsonFileListener;

class MainLayer: public cocos2d::Layer {
public:
    // Клетка
    struct Cell {
        int row;
        int col;
    };

    // Клетка шахматной доски
    struct CellNode {
        cocos2d::Sprite* m_spriteEnabled, *m_spriteDisabled;
        cocos2d::Label* m_label;
        Cell m_cell;
    };

    MainLayer();
    virtual ~MainLayer();

    virtual bool init() override;

    static cocos2d::Scene* scene();

    void menuCloseCallback(Ref* sender);

    CREATE_FUNC(MainLayer);

    void startButtonHandler(Ref*, cocos2d::extension::Control::EventType);
    void autoButtonHandler(Ref*, cocos2d::extension::Control::EventType);

    void plus(Ref*, cocos2d::extension::Control::EventType);
    void minus(Ref*, cocos2d::extension::Control::EventType);

    void repeat(Cell start, Cell end, std::function<void()> doAfter,
            std::function<void()> doError = {});

    static const int side = 80, rows = 8, cols = 8;

private:
    cocos2d::Label* m_help = nullptr;

    cocos2d::extension::ControlButton* m_startButton = nullptr;
    cocos2d::extension::ControlButton* m_autoButton = nullptr;

    cocos2d::Label* m_disabledCells = nullptr;
    cocos2d::extension::ControlButton* m_plusButton = nullptr;
    cocos2d::extension::ControlButton* m_minusButton = nullptr;

    CellNode createCell(cocos2d::Node*, Cell);
    void createCells(cocos2d::Node*);

    void reEnableCells();
    void reDisableCells();

    std::string updateDisableLabelText();

    size_t findClosestCellNode(const cocos2d::Vec2& mousePos) const;

    static Cell randomCell();
    Cell randomEnabledCell() const;

    static size_t cellIndex(int row, int col) {
        return row * cols + col;
    }
    static size_t cellIndex(Cell cell) {
        return cellIndex(cell.row, cell.col);
    }

    static int smul(const Cell& a, const Cell& b);
    static float len(const Cell& a);
    static float icos(const Cell& c1, const Cell& c2);
    static void optimumOffset(std::vector<Cell>& offset, const Cell& start, const Cell& end);

    // Поиск пути для Коня
    static bool findKnightsTour(
        std::vector<Cell> &tour,      // путь
        std::vector<int> &chessBoard, // шахматная доска с метками
        int position,                 // текущая метка
        const Cell& currentCell,      // текущая клетка
        const Cell& endCell           // конечная клетка
    );

    size_t m_disabled = 10;

    using Cells = std::vector<CellNode>;
    Cells m_cells;
    cocos2d::Sprite* m_knight = nullptr;     // Конь
    cocos2d::Sprite* m_target = nullptr;     // Цель
    cocos2d::Sprite* m_selection = nullptr;  // Выделение
    cocos2d::DrawNode* m_lines = nullptr;    // Линия показывающая просчитанный путь

    cocos2d::Sprite* m_wat = nullptr;
    cocos2d::Label* m_watMessage = nullptr;
    void createWat(cocos2d::Node*, const cocos2d::Size&);

    std::unique_ptr<JsonFileListener> m_jfl;

    enum class Action {
        Down,    // Мышь нажата
        Up,      // Мышь отжата
        Move,    // Мышь двигается нажатая
        Hover,   // Мышь двигается отжатая
        Unknown, // хз
    };

    // Паттерн "Состояние" для описания логики чтоли :)
    class State {
    public:
        virtual ~State() = default;

        virtual void onEnter(MainLayer*){}
        virtual void onExit(MainLayer*){}
        virtual void handleStart(MainLayer*){}
        virtual void handleAuto(MainLayer*){}
        virtual void handleMouse(MainLayer*, Action, float x, float y){}
        virtual void handleTourAnimationEnd(MainLayer*){}
    };

    class PrepareState: public State {
        bool m_firstTime;
    public:
        PrepareState(bool firstTime = false);
        virtual ~PrepareState() = default;

        virtual void onEnter(MainLayer*);
        virtual void onExit(MainLayer*);
        virtual void handleStart(MainLayer*);
        virtual void handleAuto(MainLayer*);
    };

    // Состояния работы в ручном режиме
    class ManualState: public State {
    public:
        virtual ~ManualState() = default;

        virtual void onEnter(MainLayer*);
        virtual void onExit(MainLayer*);
        virtual void handleStart(MainLayer*);
        virtual void handleAuto(MainLayer*);
        virtual void handleMouse(MainLayer*, Action, float x, float y);

        struct FiguresCellNode{
            cocos2d::Sprite* m_figure;
            Cell m_cell;
        };

        using FiguresCellNodes = std::vector<FiguresCellNode>;

        FiguresCellNodes m_figures;
        FiguresCellNodes ::iterator m_currentFigure;
        bool m_check = false;
        bool m_onEnable = false;
    };

    // Состояние ожидания работы анимации движения Коня по пути
    class TourAnimationState: public State {
    public:
        virtual ~TourAnimationState() = default;

        virtual void onEnter(MainLayer*);
        virtual void onExit(MainLayer*);
        virtual void handleTourAnimationEnd(MainLayer*);
    };

    // Состояние автоматической работы
    class AutoState: public State {
    public:
        virtual ~AutoState() = default;

        virtual void onEnter(MainLayer*);
        virtual void onExit(MainLayer*);
        virtual void handleAuto(MainLayer*);
        virtual void handleTourAnimationEnd(MainLayer*);

        std::function<void()> m_func;
        bool m_exit = false;
    };

    class WatState: public State {
        std::function<void()> m_afterWat;
        std::string m_message;
    public:
        WatState(std::function<void()> afterWat, const std::string& message =
                "Ошибка") :
                m_afterWat(afterWat), m_message(message) {
        }
        virtual ~WatState() = default;

        virtual void onEnter(MainLayer*);
        virtual void onExit(MainLayer*);
    };

    // Переключение состояния
    template<typename T, typename ... Args>
    void switchState(Args&& ...args) {
        if(m_state){
            m_state->onExit(this);
        }
        m_state.reset(new T(std::forward<Args>(args)...));
        m_state->onEnter(this);
    }
    std::unique_ptr<State> m_state;
};
