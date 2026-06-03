#pragma once
#include <fstream>
#include "../game/interface/i_interaction.h"
#include "../game/setup.h"
#include "../utils/config.h"


class TerminalInteraction : public I_Interaction {
   private:
    std::ifstream global_file_in;
    std::streambuf* cin_backup;  

   public:
    TerminalInteraction();
    ~TerminalInteraction();

    void init(const RunConfig& config) override;
    void initInteraction(const RunConfig& config);
    void closeInteraction();

    bool validateInput(const std::string& input);
    bool getInput(int* val);
    void pause(int timeout = 0) override;
    bool selectSize(int* size) override;
    bool selectGoal(int* goal, const int size) override;
    bool selectGameMode(GameMode* mode) override;
    bool selectBotLevel(BotLevel* levels, const int index) override;
    bool getPlayerMove(int* row, int* col) override;
    void close() override;

    bool selectEndRule(EndRule* rule) override;
};