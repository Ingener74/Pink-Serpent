#pragma once

#include <memory>

#include <cocos2d.h>
#include <cocos-ext.h>

struct Cell {
    int row;
    int col;
};

struct CellNode {
    cocos2d::Sprite* m_spriteEnabled, *m_spriteDisabled;
    cocos2d::Label* m_label;
    Cell m_cell;
};

class MainLayer: public cocos2d::Layer {
public:
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

//    std::unique_ptr<Board> m_board;
//    CellNode createCell(cocos2d::Node*, const std::string& row, const std::string& column);
    CellNode createCell(cocos2d::Node*, Cell);
    void createCells(cocos2d::Node*);
    size_t findClosestCellNode(const cocos2d::Vec2& mousePos) const;

    static Cell randomCell();
    Cell randomEnabledCell() const;

    size_t cellIndex(int row, int col) const {
        return row * cols + col;
    }
    size_t cellIndex(Cell cell) const {
        return cellIndex(cell.row, cell.col);
    }

    bool findKnightsTour(std::vector<Cell> &tour, std::vector<int> &chessBoard, int position, const Cell& currentCell, const Cell& endCell) const;

    size_t m_disabled = 10;

    using Cells = std::vector<CellNode>;
    Cells m_cells;
    cocos2d::Sprite* m_knight = nullptr;
    cocos2d::Sprite* m_target = nullptr;
    cocos2d::Sprite* m_selection = nullptr;
    cocos2d::DrawNode* m_lines = nullptr;

    enum class Action {
        Down, Up, Move, Hover, Unknown,
    };

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

    class TourAnimationState: public State {
    public:
        virtual ~TourAnimationState() = default;

        virtual void onEnter(MainLayer*);
        virtual void onExit(MainLayer*);
        virtual void handleTourAnimationEnd(MainLayer*);
    };

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
