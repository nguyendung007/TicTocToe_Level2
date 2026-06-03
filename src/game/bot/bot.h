#pragma once
#include "../setup.h"

class Bot {
   protected:
    const BotLevel level;
    const char symbol;
    const char op_symbol;
    pII random_pick(char board[][BOARD_N_MAX], const int size);

   public:
    Bot(const BotLevel& _level, const char& _symbol);
    virtual ~Bot();
    virtual pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) = 0;
};