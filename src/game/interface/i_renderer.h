#pragma once
#include "../../utils/config.h"
#include "../logic.h"
#include "../setup.h"

class I_Renderer {
   public:

    I_Renderer();
    virtual ~I_Renderer();
    virtual void init(const RunConfig& config) = 0;
    virtual void clearScreen() = 0;
    virtual void showSelectMenu(SelectType selectType, int context = NO_CONTEXT) = 0;
    virtual void showInvalidSelect(SelectType selectType, int context = NO_CONTEXT) = 0;
    virtual void showValidSelect(SelectType selectType, int context = NO_CONTEXT) = 0;
    virtual void displayBoard(const char board[][BOARD_N_MAX], const int size) = 0;
    virtual void showMove(const int row, const int col) = 0;
    virtual void showInvalidMove() = 0;
    virtual void showPlayer(const int player, const bool is_bot) = 0;
    virtual void showResult(const int winner, const bool is_bot, const WinLine* winLine = nullptr) = 0;
    virtual void printResult(const GameResult& gameResult) = 0;
    virtual void close() = 0;

    virtual void showGameInfo(const GameSetup& setup) = 0;
    virtual void showGameResult(const GameResult& result) = 0;
};