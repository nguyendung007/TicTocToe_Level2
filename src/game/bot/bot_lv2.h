#pragma once
#include "../setup.h"
#include "bot.h"

class BotLevel2 : public Bot {
   protected:
    pII center_pick(char board[][BOARD_N_MAX], const int size);
    pII simple_heuristic(char board[][BOARD_N_MAX], const int size,
                         const int goal, const char botSym,
                         const char playerSym);

   public:
    BotLevel2(const BotLevel& _level, const char& _symbol);
    ~BotLevel2() override;
    pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) override;
};