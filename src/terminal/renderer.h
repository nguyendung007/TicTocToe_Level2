#pragma once
#include "../game/interface/i_renderer.h"

class TerminalRenderer : public I_Renderer {
   public:
    TerminalRenderer();
    ~TerminalRenderer();

    void init(const RunConfig& config) override;
    void clearScreen() override;
    void showSelectMenu(SelectType selectType, int context = NO_CONTEXT) override;
    void showInvalidSelect(SelectType selectType, int context = NO_CONTEXT) override;
    void showValidSelect(SelectType selectType, int context = NO_CONTEXT) override;
    void displayBoard(const char board[][BOARD_N_MAX], const int size) override;
    void showMove(const int row, const int col) override;
    void showInvalidMove() override;
    void showPlayer(const int player, const bool is_bot) override;
    void showResult(const int winner, const bool is_bot, const WinLine* winLine = nullptr) override;
    void printResult(const GameResult& gameResult) override;
    void close() override;

    void showGameInfo(const GameSetup& setup) override;
    void showGameResult(const GameResult& result) override;
};