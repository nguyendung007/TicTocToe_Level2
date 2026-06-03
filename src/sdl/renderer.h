#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "../game/interface/i_renderer.h"
#include "../game/logic.h"
#include "../game/setup.h"

enum class ScreenState { TITLE, SETUP, GAME, RESULT };

class SDLRenderer : public I_Renderer {
   private:
    SDL_Window*   window_   = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    TTF_Font*     fontLg_   = nullptr; 
    TTF_Font*     fontMd_   = nullptr;  
    TTF_Font*     fontSm_   = nullptr;  

    // Layout
    int screenW_   = 800;
    int screenH_   = 800;
    int padding_   = 60;
    int boardSize_ = 3;

    std::string statusMsg_;
    ScreenState screenState_ = ScreenState::TITLE;
    int lastRow_ = -1, lastCol_ = -1;

    SDL_Texture* bgMenu_   = nullptr;
    SDL_Texture* bgSetup_  = nullptr;
    SDL_Texture* bgGame_   = nullptr;
    SDL_Texture* bgResult_ = nullptr;

    void renderPresent();
    void drawRect(int x, int y, int w, int h, SDL_Color color, bool filled);
    void drawRoundedRect(int x, int y, int w, int h, int radius,
                         SDL_Color color, bool filled);
    void drawCircle(int cx, int cy, int radius, SDL_Color color, int thickness = 3);
    void drawLine(int x1, int y1, int x2, int y2, SDL_Color color, int thickness = 2);
    void renderText(const std::string& text, int x, int y,
                    SDL_Color color, TTF_Font* font = nullptr);
    void renderTextCentered(const std::string& text, int y,
                             SDL_Color color, TTF_Font* font = nullptr);
    SDL_Point getTextSize(const std::string& text, TTF_Font* font = nullptr);
    void drawOverlay(SDL_Color color, Uint8 alpha);
    void drawBackground();
    void drawButton(int x, int y, int w, int h,
                    const std::string& label, bool highlight = false);
    SDL_Texture* loadTexture(const std::string& path);

    int cellSize() const;
    SDL_Rect cellRect(int row, int col) const;

    const GameSetup* currentSetup_ = nullptr;   // thêm vào private
    const GameResult* currentResult_ = nullptr; // thêm vào private

    void showGameInfo(const GameSetup& setup) override;

    void showGameResult(const GameResult& result) override;

   public:
    SDLRenderer();
    ~SDLRenderer() override;

    void init(const RunConfig& config) override;
    void clearScreen() override;
    void showSelectMenu(SelectType selectType, int context = NO_CONTEXT) override;
    void showInvalidSelect(SelectType selectType, int context = NO_CONTEXT) override;
    void showValidSelect(SelectType selectType, int context = NO_CONTEXT) override;
    void displayBoard(const char board[][BOARD_N_MAX], const int size) override;
    void showMove(const int row, const int col) override;
    void showInvalidMove() override;
    void showPlayer(const int player, const bool is_bot) override;
    void showResult(const int winner, const bool is_bot,
                    const WinLine* winLine = nullptr) override;
    void printResult(const GameResult& gameResult) override;
    void close() override;
};
