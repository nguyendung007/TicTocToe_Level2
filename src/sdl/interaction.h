#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "../game/interface/i_interaction.h"
#include "../game/setup.h"
#include "../utils/config.h"

class SDLInteraction : public I_Interaction {
   private:
    int screenW_   = 800;
    int screenH_   = 800;
    int padding_   = 60;
    int boardSize_ = 3;  
    int readIntFromKeyboard();

   public:
    SDLInteraction();
    ~SDLInteraction() override;

    void init(const RunConfig& config) override;
    void pause(int timeout = 0) override;
    bool selectSize(int* size) override;
    bool selectGoal(int* goal, const int size) override;
    bool selectGameMode(GameMode* mode) override;
    bool selectBotLevel(BotLevel* levels, const int index) override;
    bool getPlayerMove(int* row, int* col) override;

    void close() override;
    bool selectEndRule(EndRule* rule) override;
};
