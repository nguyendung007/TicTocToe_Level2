#include <iostream>
#include "game/engine.h"
#include "utils/config.h"
#include "utils/logger.h"
#include "game/interface/i_interaction.h"
#include "game/interface/i_renderer.h"
#include "terminal/interaction.h"
#include "terminal/renderer.h"
#include "sdl/interaction.h"
#include "sdl/renderer.h"

int main(int argc, char* argv[]) {
    RunConfig config = parseArgs(argc, argv); 

    if (config.is_help) {
        std::cout << configHelpStr();
        return 0;
    }
    Logger::init(config.judge_mode, 
                 config.to_file, 
                 config.log_file, 
                 config.verbose_flag);
    Logger::log("Logger Khoi Tao Hoan Tat");
    Logger::log(config.toString(), Logger::Level::DEBUG);

    I_Renderer* iRenderer = nullptr; 
    I_Interaction* iInteraction = nullptr;

    if (!config.gui_flag || config.judge_mode) {
        iRenderer = new TerminalRenderer();
        Logger::log("Terminal Renderer initialized!");

        iInteraction = new TerminalInteraction();
        Logger::log("Terminal Interaction initialized!");
    } else {
        iRenderer = new SDLRenderer();
        Logger::log("SDL Renderer initialized!");

        iInteraction = new SDLInteraction();
        Logger::log("SDL Interaction initialized!");
    }

    Logger::log("Infra initialized!");
    Engine* engine = new Engine(
        &config,
        iRenderer,
        iInteraction);

    try {
        if (engine) {
            engine->init();

            engine->startGame();                         
            GameResult gameResult = engine->playGame(); 
            engine->endGame(gameResult);                 
        }
    } catch (const QuitException& e) {
        Logger::log("Quit signal received. Cleaning up...", Logger::Level::WARNING);
    } catch (const NotImplementedException& e) {
        Logger::log("Some functions is not implemented. Cleaning up...", Logger::Level::WARNING);
    }

    engine->close();
    delete engine;

    Logger::log("Engine destroyed!");

    if (iRenderer) {
        delete iRenderer;
    }

    if (iInteraction) {
        delete iInteraction;
    }
    Logger::log("Infra destroyed!");
    Logger::log("Logger closing . . .");
    Logger::close();

    return 0;
}
