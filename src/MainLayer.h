#pragma once

#include <memory>

#include <cocos2d.h>
#include <cocos-ext.h>

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
    void repeat(Cell start, Cell end, std::function<void()> doAfter);

    static const int side = 80, rows = 8, cols = 8;

private:
    cocos2d::Label* m_help = nullptr;

    cocos2d::extension::ControlButton* m_startButton = nullptr;
    cocos2d::extension::ControlButton* m_autoButton = nullptr;

    CellNode createCell(cocos2d::Node*, Cell);
    void createCells(cocos2d::Node*);
    size_t findClosestCellNode(const cocos2d::Vec2& mousePos) const;

    static Cell randomCell();
    Cell randomEnabledCell() const;

    static size_t cellIndex(int row, int col) {
        return row * cols + col;
    }
    static size_t cellIndex(Cell cell) {
        return cellIndex(cell.row, cell.col);
    }

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

        virtual void onEnter(MainLayer*){
        }
        virtual void onExit(MainLayer*){
        }
        virtual void handleStart(MainLayer*) {
        }
        virtual void handleAuto(MainLayer*) {
        }
        virtual void handleMouse(MainLayer*, Action, float x, float y) {
        }
        virtual void handleTourAnimationEnd(MainLayer*) {
        }
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

    // Переключение состояния
    template<typename T>
    void switchState() {
        if(m_state){
            m_state->onExit(this);
        }
        m_state.reset(new T);
        m_state->onEnter(this);
    }
    std::unique_ptr<State> m_state;
};
