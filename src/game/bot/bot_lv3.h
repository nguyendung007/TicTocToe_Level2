#pragma once
#include "../setup.h"
#include "bot.h"

class BotLevel3 : public Bot {
   protected:
    int evaluate(char board[][BOARD_N_MAX], const int size,
                 const int goal, const char botSym, const char oppSym);
    std::vector<pII> getCandidates(char board[][BOARD_N_MAX],
                                   const int size);

    int minimax(char board[][BOARD_N_MAX], int size, int goal,
                char botSym, char oppSym,
                int depth, int alpha, int beta, bool isMax);

    pII hard_level(char board[][BOARD_N_MAX], const int size, const int goal);

   public:
    BotLevel3(const BotLevel& _level, const char& _symbol);
    ~BotLevel3() override;
    pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) override;
};