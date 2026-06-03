#include "bot_factory.h"
#include "bot_lv1.h"
#include "bot_lv2.h"
#include "bot_lv3.h"

Bot* BotFactory::createBot(BotLevel level, const char& symbol) {
    switch (level) {
        case BotLevel::EASY:   return new BotLevel1(level, symbol);
        case BotLevel::MEDIUM: return new BotLevel2(level, symbol);
        case BotLevel::HARD:   return new BotLevel3(level, symbol);
        default:               return nullptr;
    }
}
