#pragma once
#include "../utils/config.h"
#include "interface/i_interaction.h"
#include "interface/i_renderer.h"
#include "setup.h"
#include <vector>

class Engine {
   private:
    const RunConfig* config;
    I_Renderer* iRenderer;
    I_Interaction* iInteraction;

    GameSetup gameSetup;  
    bool sanity_check();

    std::vector<MoveRecord> moveHistory_;
    bool doUndo_(int& player, int turns);

   public:
    Engine(const RunConfig* _config, I_Renderer* _iRenderer,
           I_Interaction* _iInteraction);
    ~Engine();
    void init();
    void startGame();
    GameResult playGame();
    void endGame(const GameResult& gameResult);
    void close();
};
