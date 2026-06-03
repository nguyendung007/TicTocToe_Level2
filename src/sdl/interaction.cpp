#include "interaction.h"
#include "../utils/logger.h"

SDLInteraction::SDLInteraction() {}
SDLInteraction::~SDLInteraction() {}

void SDLInteraction::init(const RunConfig& config) {
    screenW_  = config.screenWidth;
    screenH_  = config.screenHeight;
    padding_  = config.boardPadding;
}

int SDLInteraction::readIntFromKeyboard() {
    std::string digits;
    SDL_Event e;
    while (true) {
        if (!SDL_WaitEvent(&e)) continue;
        if (e.type == SDL_QUIT) throw QuitException();
        if (e.type != SDL_KEYDOWN) continue;

        SDL_Keycode key = e.key.keysym.sym;
        if (key >= SDLK_0 && key <= SDLK_9)
            digits += (char)('0' + (key - SDLK_0));
        else if (key >= SDLK_KP_0 && key <= SDLK_KP_9)
            digits += (char)('0' + (key - SDLK_KP_0));
        else if (key == SDLK_BACKSPACE && !digits.empty())
            digits.pop_back();
        else if ((key == SDLK_RETURN || key == SDLK_KP_ENTER) && !digits.empty()) {
            try { return std::stoi(digits); } catch (...) { digits.clear(); }
        }
    }
}

void SDLInteraction::pause(int timeout) {
    if (timeout > 0) { SDL_Delay(timeout); return; }
    SDL_Event e;
    while (true) {
        if (!SDL_WaitEvent(&e)) continue;
        if (e.type == SDL_QUIT) throw QuitException();
        if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) return;
    }
}

bool SDLInteraction::selectSize(int* size) {
    int v = readIntFromKeyboard();
    if (v < BOARD_N_MIN || v > BOARD_N_MAX) return false;
    *size = v;
    boardSize_ = v;
    return true;
}

bool SDLInteraction::selectGoal(int* goal, const int size) {
    int v = readIntFromKeyboard();
    if (v < 3 || v > size) return false;
    *goal = v;
    return true;
}

bool SDLInteraction::selectGameMode(GameMode* mode) {
    int v = readIntFromKeyboard();
    switch (v) {
        case 1: *mode = GameMode::PVP; break;
        case 2: *mode = GameMode::PVE; break;
        case 3: *mode = GameMode::EVE; break;
        default: return false;
    }
    return true;
}

bool SDLInteraction::selectEndRule(EndRule* rule) {
    int v = readIntFromKeyboard();
    switch (v) {
        case 1: *rule = EndRule::NONE;     break;
        case 2: *rule = EndRule::OPEN_ONE; break;
        case 3: *rule = EndRule::OPEN_TWO; break;
        default: return false;
    }
    return true;
}

bool SDLInteraction::selectBotLevel(BotLevel* levels, const int index) {
    int v = readIntFromKeyboard();
    switch (v) {
        case 1: levels[index] = BotLevel::EASY;   break;
        case 2: levels[index] = BotLevel::MEDIUM; break;
        case 3: levels[index] = BotLevel::HARD;   break;
        default: return false;
    }
    return true;
}

bool SDLInteraction::getPlayerMove(int* row, int* col) {
    int reservedHeight = 140;
    int availableWidth = screenW_ - 2 * padding_;
    int availableHeight = screenH_ - 2 * padding_ - reservedHeight;
    int area = std::min(availableWidth, availableHeight);
    int cs = (boardSize_ > 0) ? area / boardSize_ : area;
    
    int boardWidth = cs * boardSize_;
    int offsetX = (screenW_ - boardWidth) / 2;
    int offsetY = padding_ + 20; 
    
    SDL_Event e;
    while (true) {
        if (!SDL_WaitEvent(&e)) continue;

        if (e.type == SDL_QUIT) throw QuitException();

        if (e.type == SDL_KEYDOWN) {
            SDL_Keycode key = e.key.keysym.sym;
            if (key == SDLK_u || key == SDLK_z)     throw UndoException();
            if (key == SDLK_ESCAPE)                  throw SurrenderException();
        }

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mx = e.button.x - offsetX; 
            int my = e.button.y - offsetY; 
            if (mx < 0 || my < 0) continue;
            int c_ = mx / cs;
            int r_ = my / cs;
            if (r_ < 0 || r_ >= boardSize_ || c_ < 0 || c_ >= boardSize_) continue;
            *row = r_;
            *col = c_;
            return true;
        }
    }
}

void SDLInteraction::close() {
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
}
