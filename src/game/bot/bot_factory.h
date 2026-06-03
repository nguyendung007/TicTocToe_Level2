#pragma once
#include "../setup.h"
#include "bot.h"

class BotFactory {
   public:
    static Bot* createBot(BotLevel level, const char& symbol);
};