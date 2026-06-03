#pragma once
#include "../setup.h"
#include "bot.h"

class BotLevel1 : public Bot {
   public:

    BotLevel1(const BotLevel& _level, const char& _symbol);
    ~BotLevel1() override;

    pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) override;
};