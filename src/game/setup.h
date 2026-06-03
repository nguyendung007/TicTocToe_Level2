#pragma once
#include <exception>
#include <random>
#include <utility>
#include <vector>

inline const bool ALGORITHM_FLAG = true;
inline const bool TIME_ENABLED = true;
inline const int RANDOM_SEED = 8702;

inline const int BOARD_N_MIN = 3;
inline const int BOARD_N_MAX = 12;
inline const int GOAL_MAX = 5;
inline const int SLEEP_TIME = 1500;

inline const int SCORE_INF = 1000;
inline const int EVALUATE_SCORE = 10;

inline std::mt19937 generator(RANDOM_SEED);

constexpr int DRAW_RESULT = -1;
constexpr int NO_CONTEXT = -1;

using pII = std::pair<int, int>;

enum class SelectType {
    TITLE_UI, 
    SIZE_UI, 
    GOAL_UI, 
    GAME_MODE_UI,
    BOT_LEVEL_UI, 
    PLAYER_UI, 
    MUL_BOT_LEVEL_UI, 
    END_RULE_UI,   
    INVALID_UI
};

enum class EndRule { NONE, OPEN_ONE, OPEN_TWO };

enum class BotLevel { EASY, MEDIUM, HARD, INVALID_LV };

enum class GameMode { PVP, PVE, EVE, INVALID_MODE };

inline static std::string botToString(int v) {
    switch ((BotLevel)v) {
        case BotLevel::EASY:   return "EASY";
        case BotLevel::MEDIUM: return "MEDIUM";
        case BotLevel::HARD:   return "HARD";
        default:               return "?";
    }
}

inline static std::string modeToString(int v) {
    switch ((GameMode)v) {
        case GameMode::PVP: return "PVP";
        case GameMode::PVE: return "PVE";
        case GameMode::EVE: return "EVE";
        default:            return "?";
    }
}

inline static std::string endRuleToString(int rule) {
    switch (rule) {
        case 0: return "None";
        case 1: return "OpenOne";
        case 2: return "OpenTwo";
        default: return "Unknown";
    }
}

enum class PlayerAction { MOVE, UNDO, SURRENDER };

struct GameSetup {
    char board[BOARD_N_MAX][BOARD_N_MAX];
    int size;
    int goal;
    GameMode mode;
    BotLevel levels[2];
    EndRule rule = EndRule::OPEN_TWO;  
};

struct GameResult {
    int winner;
    bool isBot;
    int turns;
    bool isSurrender = false;
    GameResult(int _winner, bool _isBot, int _turns)
        : winner(_winner), isBot(_isBot), turns(_turns), isSurrender(false) {}
};

struct MoveRecord {
    int row, col;  
    int player;    
    char symbol;   
};


class QuitException : public std::exception {
   public:
    const char* what() const noexcept override { return "User requested quit"; }
};


class UndoException : public std::exception {
   public:
    const char* what() const noexcept override { return "Player requested undo"; }
};

class SurrenderException : public std::exception {
   public:
    const char* what() const noexcept override { return "Player surrendered"; }
};

class NotImplementedException : public std::exception {
   public:
    const char* what() const noexcept override { return "Functionality not implemented yet"; }
};
