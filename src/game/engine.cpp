#include "engine.h"
#include <chrono>
#include <optional>
#include <sstream>
#include <thread>
#include <vector>
#include "../utils/helper.h"
#include "../utils/logger.h"
#include "bot/bot_factory.h"
#include "logic.h"

Engine::Engine(const RunConfig* _config, I_Renderer* _iRenderer,
               I_Interaction* _iInteraction) {
    config       = _config;
    iRenderer    = _iRenderer;
    iInteraction = _iInteraction;
}

Engine::~Engine() {}

void Engine::init() {
    Logger::log("Engine initializing . . .");
    iRenderer->init(*config);
    iInteraction->init(*config);
    Logger::log("Engine initialized!");
}

bool Engine::sanity_check() {
    bool ok = true;
    if (!iRenderer)    { Logger::log("Interface Renderer not implemented!",    Logger::Level::WARNING); ok = false; }
    if (!iInteraction) { Logger::log("Interface Interaction not implemented!", Logger::Level::WARNING); ok = false; }
    return ok;
}

void Engine::startGame() {
    Logger::log("[Engine] Starting game . . .");
    if (!sanity_check()) { Logger::log("[Engine] Game stopped!", Logger::Level::ERROR); return; }

    if (config->interactive) {
        iRenderer->clearScreen();
        iRenderer->showSelectMenu(SelectType::TITLE_UI);
        iInteraction->pause();
    }

    bool isSelected;

    do {
        if (config->interactive) iRenderer->showSelectMenu(SelectType::SIZE_UI);
        isSelected = iInteraction->selectSize(&gameSetup.size);
        if (config->interactive && !isSelected) iRenderer->showInvalidSelect(SelectType::SIZE_UI, gameSetup.size);
    } while (!isSelected);
    if (config->interactive) iRenderer->showValidSelect(SelectType::SIZE_UI, gameSetup.size);
    Logger::log(std::format("user input 'size' = {}", gameSetup.size), Logger::Level::DEBUG);

    do {
        if (config->interactive) iRenderer->showSelectMenu(SelectType::GOAL_UI, gameSetup.size);
        isSelected = iInteraction->selectGoal(&gameSetup.goal, gameSetup.size);
        if (config->interactive && !isSelected) iRenderer->showInvalidSelect(SelectType::GOAL_UI, gameSetup.goal);
    } while (!isSelected);
    if (config->interactive) iRenderer->showValidSelect(SelectType::GOAL_UI, gameSetup.goal);
    Logger::log(std::format("user input 'goal' = {}", gameSetup.goal), Logger::Level::DEBUG);
    
    do {
    if (config->interactive) iRenderer->showSelectMenu(SelectType::END_RULE_UI);
    isSelected = iInteraction->selectEndRule(&gameSetup.rule);
    if (config->interactive && !isSelected) iRenderer->showInvalidSelect(SelectType::END_RULE_UI, 0);
} while (!isSelected);

    do {
        if (config->interactive) iRenderer->showSelectMenu(SelectType::GAME_MODE_UI);
        isSelected = iInteraction->selectGameMode(&gameSetup.mode);
        if (config->interactive && !isSelected) iRenderer->showInvalidSelect(SelectType::GAME_MODE_UI, (int)gameSetup.mode);
    } while (!isSelected);
    if (config->interactive) iRenderer->showValidSelect(SelectType::GAME_MODE_UI, (int)gameSetup.mode);
    Logger::log(std::format("user input 'game mode' = {}", modeToString((int)gameSetup.mode)), Logger::Level::DEBUG);

    if (gameSetup.mode == GameMode::PVE) {
        do {
            if (config->interactive) iRenderer->showSelectMenu(SelectType::BOT_LEVEL_UI);
            isSelected = iInteraction->selectBotLevel(gameSetup.levels, 1);
            if (config->interactive && !isSelected) iRenderer->showInvalidSelect(SelectType::BOT_LEVEL_UI, (int)gameSetup.levels[1]);
        } while (!isSelected);
        if (config->interactive) iRenderer->showValidSelect(SelectType::BOT_LEVEL_UI, (int)gameSetup.levels[1]);
        Logger::log(std::format("user input 'bot level' = {}", botToString((int)gameSetup.levels[1])), Logger::Level::DEBUG);
    }

    if (gameSetup.mode == GameMode::EVE) {
        for (int idx : {0, 1}) {
            do {
                if (config->interactive) iRenderer->showSelectMenu(SelectType::MUL_BOT_LEVEL_UI, idx);
                isSelected = iInteraction->selectBotLevel(gameSetup.levels, idx);
                if (config->interactive && !isSelected) iRenderer->showInvalidSelect(SelectType::MUL_BOT_LEVEL_UI, (int)gameSetup.levels[idx]);
            } while (!isSelected);
            if (config->interactive) iRenderer->showValidSelect(SelectType::MUL_BOT_LEVEL_UI, (int)gameSetup.levels[idx]);
            Logger::log(std::format("user input 'bot level[{}]' = {}", idx, botToString((int)gameSetup.levels[idx])), Logger::Level::DEBUG);
        }
    }

    Logic::initBoard(gameSetup.board, gameSetup.size);
    moveHistory_.clear();
    Logger::log("Board initialized!");
    Logger::log("[Engine] Game started!");
}

bool Engine::doUndo_(int& player, int turns) {
    (void)turns;
    bool isPvE = (gameSetup.mode == GameMode::PVE);
    int undoCount = isPvE ? 2 : 1;

    if ((int)moveHistory_.size() < undoCount) return false; 

    for (int i = 0; i < undoCount; ++i) {
        MoveRecord rec = moveHistory_.back();
        moveHistory_.pop_back();
        gameSetup.board[rec.row][rec.col] = '.';
        player = rec.player;                       
    }
    Logger::log(std::format("[Engine] Undo {} move(s), back to player {}", undoCount, player + 1), Logger::Level::DEBUG);
    return true;
}

GameResult Engine::playGame() {
    Logger::log("[Engine] Playing game . . .");
    GameResult gameResult(-1, false, 0);
    if (!sanity_check()) { Logger::log("[Engine] Game stopped!", Logger::Level::ERROR); return gameResult; }

    const char symbols[2] = {'X', 'O'};
    bool is_bot[2] = {
        gameSetup.mode == GameMode::EVE,
        gameSetup.mode != GameMode::PVP,
    };

    std::vector<std::unique_ptr<Bot>> bots;
    if (gameSetup.mode == GameMode::PVE) {
        bots.emplace_back(nullptr);
        bots.emplace_back(BotFactory::createBot(gameSetup.levels[1], symbols[1]));
    } else if (gameSetup.mode == GameMode::EVE) {
        bots.emplace_back(BotFactory::createBot(gameSetup.levels[0], symbols[0]));
        bots.emplace_back(BotFactory::createBot(gameSetup.levels[1], symbols[1]));
    }

    int player = 0;
    bool is_running = true;

    while (is_running) {
        Logger::log(std::format("[Engine] starting turn #{}", gameResult.turns), Logger::Level::DEBUG);

        if (config->interactive) {
            iRenderer->clearScreen();
            iRenderer->displayBoard(gameSetup.board, gameSetup.size);
            iRenderer->showGameInfo(gameSetup);
            iRenderer->showPlayer(player, is_bot[player]);
        }

        int row = -1, col = -1;

        if (is_bot[player]) {
            pII point = measureExecutionTime(
                std::format("bot#{}->getMove()", player),
                [&]() { return bots[player]->getMove(gameSetup.board, gameSetup.size, gameSetup.goal); },
                TIME_ENABLED);
            row = point.first;
            col = point.second;
        } else {
            bool is_valid = false;
            do {
                try {
                    if (config->interactive) iRenderer->showSelectMenu(SelectType::PLAYER_UI);
                    iInteraction->getPlayerMove(&row, &col);
                    is_valid = Logic::isValidMove(gameSetup.board, gameSetup.size, row, col);
                    if (!is_valid && config->interactive) iRenderer->showInvalidMove();
                } catch (const UndoException&) {
               
                    if (doUndo_(player, gameResult.turns)) {
                        gameResult.turns = std::max(0, gameResult.turns - (gameSetup.mode == GameMode::PVE ? 2 : 1));
                        if (config->interactive) {
                            iRenderer->clearScreen();
                            iRenderer->displayBoard(gameSetup.board, gameSetup.size);
                            iRenderer->showGameInfo(gameSetup);
                            iRenderer->showPlayer(player, is_bot[player]);
                        }
                    }
                } catch (const SurrenderException&) {
                    gameResult.winner     = (player + 1) % 2;
                    gameResult.isBot      = is_bot[gameResult.winner];
                    gameResult.isSurrender = true;
                    is_running = false;
                    is_valid   = true; 
                    row = col = -1;     
                }
            } while (!is_valid);
        }
        if (!is_running) break;  

        moveHistory_.push_back({row, col, player, symbols[player]});

        Logic::makeMove(gameSetup.board, row, col, symbols[player]);
        if (config->interactive) iRenderer->showMove(row, col);
        gameResult.turns++;

        Logger::log(std::format("player {} make move to ({}, {})", player + 1, row, col), Logger::Level::DEBUG);

        if (is_bot[player] && config->interactive)
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));

        if (Logic::checkWin(gameSetup.board, gameSetup.size, symbols[player], gameSetup.goal, gameSetup.rule)) {
            gameResult.winner = player;
            gameResult.isBot  = is_bot[player];
            is_running = false;
        } else if (Logic::checkDraw(gameSetup.board, gameSetup.size)) {
            gameResult.winner = -1;
            gameResult.isBot  = false;
            is_running = false;
        }

        player = (player + 1) % 2;
        Logger::log("[Engine] turn done!", Logger::Level::DEBUG);
    }

    Logger::log("[Engine] Game done!");
    return gameResult;
}

void Engine::endGame(const GameResult& gameResult) {
    Logger::log("[Engine] Ending game . . .");
    if (!sanity_check()) { Logger::log("[Engine] Game stopped!", Logger::Level::ERROR); return; }

    if (config->interactive) {
        iRenderer->clearScreen();
        iRenderer->displayBoard(gameSetup.board, gameSetup.size);
        iRenderer->showGameInfo(gameSetup);
        iRenderer->showGameResult(gameResult);  // <-- thêm

        char symbol = (gameResult.winner == 0) ? 'X' : 'O';
        auto winLine = (!gameResult.isSurrender)
            ? Logic::getWinLine(gameSetup.board, gameSetup.size, symbol, gameSetup.goal, gameSetup.rule)
            : std::nullopt;

        if (winLine) {
            std::string s = "[WinLine] cells: ";
            for (auto [r, c] : winLine->cells) s += std::format("({}, {}) ", r, c);
            Logger::log(s, Logger::Level::DEBUG);
        }

        iRenderer->showResult(gameResult.winner, gameResult.isBot, winLine ? &(*winLine) : nullptr);
        iInteraction->pause();
    } else if (config->judge_mode) {
        iRenderer->printResult(gameResult);
    }

    std::stringstream ss;
    ss << "after " << gameResult.turns << " turns";
    Logger::log(ss.str(), Logger::Level::DEBUG);

    ss.str(""); ss.clear();
    ss << "game end with result: ";
    if (gameResult.winner == -1) ss << "draw";
    else ss << "player " << gameResult.winner + 1
            << " (" << (gameResult.isBot ? "bot" : "human") << ") win!"
            << (gameResult.isSurrender ? " [surrender]" : "");
    Logger::log(ss.str(), Logger::Level::DEBUG);
    Logger::log("[Engine] Game ended!");
}

void Engine::close() {
    Logger::log("Engine closing . . .");
    iRenderer->close();
    iInteraction->close();
    Logger::log("Engine closed!");
}
