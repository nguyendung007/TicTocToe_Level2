#pragma once
#include "../../utils/config.h"
#include "../setup.h"

class I_Interaction {
   public:
    I_Interaction();
    virtual ~I_Interaction();
    virtual void init(const RunConfig& config) = 0;
    virtual void pause(int timeout = 0) = 0;
    virtual bool selectSize(int* size) = 0;
    virtual bool selectGoal(int* goal, const int size) = 0;
    virtual bool selectGameMode(GameMode* mode) = 0;
    virtual bool selectBotLevel(BotLevel* levels, const int index) = 0;
    virtual bool getPlayerMove(int* row, int* col) = 0;
    virtual void close() = 0;

    virtual bool selectEndRule(EndRule* rule) = 0;
};