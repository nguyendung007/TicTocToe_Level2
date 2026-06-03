#include "renderer.h"
#include <iomanip>
#include <iostream>
#include "../game/setup.h"
#include "../utils/config.h"

static const std::string RESET  = "\033[0m";
static const std::string RED    = "\033[31m";
static const std::string GREEN  = "\033[32m";
static const std::string YELLOW = "\033[33m";
static const std::string CYAN   = "\033[36m";

TerminalRenderer::TerminalRenderer() : I_Renderer() {}
TerminalRenderer::~TerminalRenderer() {}

//Vì terminal không cần khởi tạo gì 
void TerminalRenderer::init(const RunConfig& /*config*/) { /* no-op */ }

void TerminalRenderer::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\x1B[2J\x1B[H" << std::flush;
#endif
}

void TerminalRenderer::showSelectMenu(SelectType selectType, int context) {
    switch (selectType) {
        case SelectType::TITLE_UI:
            std::cout << ">----- Tic-tac-toe [Level 2 OOP+SDL v" << VERSION << "] -----<\n\n";
            break;
        case SelectType::SIZE_UI:
            std::cout << "Board size (" << BOARD_N_MIN << "-" << BOARD_N_MAX << "): ";
            break;
        case SelectType::GOAL_UI:
            std::cout << "Goal (3-" << context << "): ";
            break;
        case SelectType::GAME_MODE_UI:
            std::cout << "Mode: (1) PvP  (2) PvE  (3) EvE\nChoice: ";
            break;
        case SelectType::BOT_LEVEL_UI:
            std::cout << "Bot level: (1) EASY  (2) MEDIUM  (3) HARD\nChoice: ";
            break;
        case SelectType::MUL_BOT_LEVEL_UI:
            std::cout << "Bot " << context + 1 << " level: (1) EASY  (2) MEDIUM  (3) HARD\nChoice: ";
            break;
        case SelectType::PLAYER_UI:
            std::cout << "Your move (row col): ";
            break;
        case SelectType::END_RULE_UI:
            std::cout << "End rule: (1) NONE  (2) OPEN_ONE  (3) OPEN_TWO\nChoice: ";
            break;
        default: break;
    }
}

void TerminalRenderer::showInvalidSelect(SelectType /*selectType*/, int /*context*/) {
    std::cout << RED << "Invalid input! Try again.\n" << RESET;
}

void TerminalRenderer::showValidSelect(SelectType /*selectType*/, int /*context*/) {
    // no-op; không cần thông báo thêm
}

void TerminalRenderer::displayBoard(const char board[][BOARD_N_MAX], const int size) {
    // Header
    std::cout << "\n    ";
    for (int c = 0; c < size; ++c)
        std::cout << std::setw(3) << c;
    std::cout << "\n    ";
    for (int c = 0; c < size; ++c) std::cout << "---";
    std::cout << "\n";

    for (int r = 0; r < size; ++r) {
        std::cout << std::setw(2) << r << " |";
        for (int c = 0; c < size; ++c) {
            char ch = board[r][c];
            std::cout << "  ";
            if (ch == 'X')      std::cout << CYAN   << ch << RESET;
            else if (ch == 'O') std::cout << YELLOW << ch << RESET;
            else                 std::cout << ch;
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void TerminalRenderer::showMove(const int row, const int col) {
    std::cout << "  -> Move: (" << row << ", " << col << ")\n";
}

void TerminalRenderer::showInvalidMove() {
    std::cout << RED << "Invalid move! Try again.\n" << RESET;
}

void TerminalRenderer::showPlayer(const int player, const bool is_bot) {
    std::cout << "\n--- ";
    if (is_bot) std::cout << "Bot (Player " << player + 1 << ") thinking...";
    else        std::cout << "Player " << player + 1 << "'s turn";
    std::cout << " ---\n";
}

void TerminalRenderer::showResult(const int winner, const bool is_bot,
                                  const WinLine* winLine) {
    if (winner == DRAW_RESULT || winner < 0) {
        std::cout << YELLOW << "\nDraw!\n" << RESET;
    } else {
        if (is_bot) std::cout << GREEN << "\nBot (Player " << winner + 1 << ") wins!\n" << RESET;
        else        std::cout << GREEN << "\nPlayer " << winner + 1 << " wins!\n" << RESET;
    }
    if (winLine) {
        std::cout << "Win line: ";
        for (auto [r, c] : winLine->cells)
            std::cout << "(" << r << "," << c << ") ";
        std::cout << "\n";
    }
    std::cout << "\nPress Enter to exit...\n";
}

void TerminalRenderer::printResult(const GameResult& gameResult) {
    std::cout << gameResult.winner << " " << gameResult.turns << "\n";
}

//Terminal không có resource nào cần giải phóng
void TerminalRenderer::close() { /* no-op */ }

void TerminalRenderer::showGameInfo(const GameSetup& setup) {
    std::cout << "--- Game Info ---\n";
    std::cout << "Size: " << setup.size << "x" << setup.size
              << "  Goal: " << setup.goal
              << "  Mode: " << modeToString((int)setup.mode);
    if (setup.mode == GameMode::PVE)
        std::cout << "  Bot: " << botToString((int)setup.levels[1]);
    else if (setup.mode == GameMode::EVE)
        std::cout << "  Bot1: " << botToString((int)setup.levels[0])
                  << "  Bot2: " << botToString((int)setup.levels[1]);
    std::cout << "  EndRule: " << endRuleToString((int)setup.rule) << "\n";
}

void TerminalRenderer::showGameResult(const GameResult& result) {
    std::cout << "--- Result ---\n";
    std::cout << "Turns: " << result.turns;
    if (result.winner == -1)
        std::cout << "  Outcome: Draw";
    else
        std::cout << "  Winner: Player " << result.winner + 1
                  << (result.isBot ? " (Bot)" : " (Human)")
                  << (result.isSurrender ? " [Surrender]" : "");
    std::cout << "\n";
}
