#include "renderer.h"
#include <cmath>
#include <iostream>
#include "../game/setup.h"
#include "../utils/config.h"
#include "../utils/logger.h"
#include <SDL2/SDL_image.h> 

static constexpr SDL_Color C_BG        = { 10,  18,  45, 255};  
static constexpr SDL_Color C_PANEL     = { 15,  30,  70, 210};  
static constexpr SDL_Color C_PANEL2    = { 20,  40,  90, 230};  
static constexpr SDL_Color C_BORDER    = { 90, 130, 210, 255}; 
static constexpr SDL_Color C_BORDER2   = {140, 180, 255, 255};  
static constexpr SDL_Color C_GRID      = { 50,  80, 140, 180};  
static constexpr SDL_Color C_X         = {130, 200, 255, 255}; 
static constexpr SDL_Color C_O         = {255, 200,  70, 255};  
static constexpr SDL_Color C_WIN_HL    = {250, 0, 0, 255};  
static constexpr SDL_Color C_TEXT      = {220, 235, 255, 255};  
static constexpr SDL_Color C_TEXT_HI   = {255, 255, 255, 255};  
static constexpr SDL_Color C_HINT      = {110, 145, 205, 255};  
static constexpr SDL_Color C_ERR       = {255,  80,  80, 255};  
static constexpr SDL_Color C_BTN       = { 25,  50, 115, 220}; 
static constexpr SDL_Color C_BTN_HL    = { 55,  95, 195, 255}; 
static constexpr SDL_Color C_LAST      = { 35,  55, 105, 180};  
static constexpr SDL_Color C_SPINNER   = { 18,  35,  80, 240};  
static constexpr SDL_Color C_INFO_PANEL= { 20,  35,  75, 220};  
static constexpr SDL_Color C_INFO_TEXT = {200, 0, 0, 255}; 

SDLRenderer::SDLRenderer() : I_Renderer() {}
SDLRenderer::~SDLRenderer() {}

SDL_Texture* SDLRenderer::loadTexture(const std::string& path) {
    SDL_Surface* surf = nullptr;
#ifdef SDL_IMAGE_AVAILABLE
    surf = IMG_Load(path.c_str());
#else
    surf = SDL_LoadBMP(path.c_str());
#endif
    if (!surf) {
        Logger::log("Cannot load texture: " + path + " -- " + SDL_GetError(),
                    Logger::Level::WARNING);
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
    SDL_FreeSurface(surf);
    if (tex) Logger::log("Texture loaded: " + path);
    return tex;
}

void SDLRenderer::init(const RunConfig& config) {
    screenW_ = config.screenWidth;
    screenH_ = config.screenHeight;
    padding_ = config.boardPadding;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Logger::log("SDL_Init failed: " + std::string(SDL_GetError()),
                    Logger::Level::ERROR);
        return;
    }
    if (TTF_Init() < 0) {
        Logger::log("TTF_Init failed: " + std::string(TTF_GetError()),
                    Logger::Level::ERROR);
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        Logger::log("IMG_Init failed: " + std::string(IMG_GetError()),
                    Logger::Level::ERROR);
    }

    window_ = SDL_CreateWindow(
        ("TicTacToe v" + VERSION).c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenW_, screenH_, SDL_WINDOW_SHOWN);
    if (!window_) {
        Logger::log("SDL_CreateWindow failed: " + std::string(SDL_GetError()),
                    Logger::Level::ERROR);
        return;
    }

    renderer_ = SDL_CreateRenderer(window_, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        Logger::log("SDL_CreateRenderer failed: " + std::string(SDL_GetError()),
                    Logger::Level::ERROR);
        return;
    }

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

    const char* fontPaths[] = {
        "assets/font.ttf",
        "C:\\Windows\\Fonts\\segoeui.ttf",
        "C:\\Windows\\Fonts\\calibri.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        nullptr
    };
    for (int i = 0; fontPaths[i]; ++i) {
        fontLg_ = TTF_OpenFont(fontPaths[i], 40);
        if (fontLg_) {
            fontMd_ = TTF_OpenFont(fontPaths[i], 30);
            fontSm_ = TTF_OpenFont(fontPaths[i], 20);
            Logger::log("Font loaded: " + std::string(fontPaths[i]));
            break;
        }
    }
    if (!fontLg_)
        Logger::log("No font found -- text will not render", Logger::Level::WARNING);

    bgMenu_   = loadTexture("assets/bg_menu.png");
    bgSetup_  = loadTexture("assets/bg_setup.png");
    bgGame_   = loadTexture("assets/bg_game.png");
    bgResult_ = loadTexture("assets/bg_result.png");

    Logger::log("SDLRenderer initialized.");
}

void SDLRenderer::renderPresent() { SDL_RenderPresent(renderer_); }

void SDLRenderer::showGameInfo(const GameSetup& setup) {
    currentSetup_ = &setup;
}

void SDLRenderer::showGameResult(const GameResult& result) {
    currentResult_ = &result;
}

void SDLRenderer::drawRect(int x, int y, int w, int h,
                            SDL_Color c, bool filled) {
    SDL_Rect r = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
    if (filled) SDL_RenderFillRect(renderer_, &r);
    else        SDL_RenderDrawRect(renderer_, &r);
}

void SDLRenderer::drawRoundedRect(int x, int y, int w, int h,
                                   int r, SDL_Color c, bool filled) {
    if (r <= 0) { drawRect(x, y, w, h, c, filled); return; }
    SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
    if (filled) {
    
        SDL_Rect mid = {x + r, y, w - 2 * r, h};
        SDL_RenderFillRect(renderer_, &mid);
        SDL_Rect left  = {x,         y + r, r, h - 2 * r};
        SDL_Rect right = {x + w - r, y + r, r, h - 2 * r};
        SDL_RenderFillRect(renderer_, &left);
        SDL_RenderFillRect(renderer_, &right);

        auto fillCorner = [&](int cx, int cy) {
            for (int dy = 0; dy <= r; ++dy)
                for (int dx = 0; dx <= r; ++dx)
                    if (dx * dx + dy * dy <= r * r) {
                        SDL_RenderDrawPoint(renderer_, cx + dx, cy + dy);
                        SDL_RenderDrawPoint(renderer_, cx - dx, cy + dy);
                        SDL_RenderDrawPoint(renderer_, cx + dx, cy - dy);
                        SDL_RenderDrawPoint(renderer_, cx - dx, cy - dy);
                    }
        };
        fillCorner(x + r,     y + r);
        fillCorner(x + w - r, y + r);
        fillCorner(x + r,     y + h - r);
        fillCorner(x + w - r, y + h - r);
    } else {

        SDL_RenderDrawLine(renderer_, x + r, y,     x + w - r, y);
        SDL_RenderDrawLine(renderer_, x + r, y + h, x + w - r, y + h);
        SDL_RenderDrawLine(renderer_, x,     y + r, x,         y + h - r);
        SDL_RenderDrawLine(renderer_, x + w, y + r, x + w,     y + h - r);

        auto drawArc = [&](int cx, int cy, int startDx, int startDy) {
            for (int angle = 0; angle <= 90; angle += 3) {
                double rad = angle * M_PI / 180.0;
                int px = cx + (int)(startDx * std::cos(rad) - startDy * std::sin(rad));
                int py = cy + (int)(startDx * std::sin(rad) + startDy * std::cos(rad));
                SDL_RenderDrawPoint(renderer_, px, py);
            }
        };
        (void)drawArc; 
        for (int i = 0; i <= r; ++i) {
            int j = (int)std::sqrt((double)(r * r - i * i));
            SDL_RenderDrawPoint(renderer_, x + r - i, y + r - j);
            SDL_RenderDrawPoint(renderer_, x + w - r + i, y + r - j);
            SDL_RenderDrawPoint(renderer_, x + r - i, y + h - r + j);
            SDL_RenderDrawPoint(renderer_, x + w - r + i, y + h - r + j);
        }
    }
}

void SDLRenderer::drawLine(int x1, int y1, int x2, int y2,
                            SDL_Color c, int thickness) {
    SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
    for (int t = -(thickness / 2); t <= thickness / 2; ++t) {
        bool steep = std::abs(y2 - y1) > std::abs(x2 - x1);
        if (steep) SDL_RenderDrawLine(renderer_, x1 + t, y1, x2 + t, y2);
        else       SDL_RenderDrawLine(renderer_, x1, y1 + t, x2, y2 + t);
    }
}

void SDLRenderer::drawCircle(int cx, int cy, int radius,
                              SDL_Color c, int thickness) {
    SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
    for (int t = 0; t < thickness; ++t) {
        int r = radius - t;
        if (r <= 0) break;
        int x = r, y = 0, err = 0;
        while (x >= y) {
            SDL_RenderDrawPoint(renderer_, cx + x, cy + y);
            SDL_RenderDrawPoint(renderer_, cx + y, cy + x);
            SDL_RenderDrawPoint(renderer_, cx - y, cy + x);
            SDL_RenderDrawPoint(renderer_, cx - x, cy + y);
            SDL_RenderDrawPoint(renderer_, cx - x, cy - y);
            SDL_RenderDrawPoint(renderer_, cx - y, cy - x);
            SDL_RenderDrawPoint(renderer_, cx + y, cy - x);
            SDL_RenderDrawPoint(renderer_, cx + x, cy - y);
            ++y; err += 1 + 2 * y;
            if (2 * (err - x) + 1 > 0) { --x; err += 1 - 2 * x; }
        }
    }
}

void SDLRenderer::renderText(const std::string& text, int x, int y,
                              SDL_Color c, TTF_Font* font) {
    if (!renderer_) return;
    TTF_Font* f = font ? font : fontMd_;
    if (!f) return;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, text.c_str(), c);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
    if (tex) {
        SDL_Rect dst = {x, y, surf->w, surf->h};
        SDL_RenderCopy(renderer_, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

void SDLRenderer::renderTextCentered(const std::string& text, int y,
                                      SDL_Color c, TTF_Font* font) {
    if (!renderer_) return;
    TTF_Font* f = font ? font : fontMd_;
    if (!f) return;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, text.c_str(), c);
    if (!surf) return;
    int x = (screenW_ - surf->w) / 2;
    SDL_FreeSurface(surf);
    renderText(text, x, y, c, f);
}

SDL_Point SDLRenderer::getTextSize(const std::string& text, TTF_Font* font) {
    TTF_Font* f = font ? font : fontMd_;
    if (!f) return {0, 0};
    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, text.c_str(), {255,255,255,255});
    if (!surf) return {0, 0};
    SDL_Point sz = {surf->w, surf->h};
    SDL_FreeSurface(surf);
    return sz;
}

void SDLRenderer::drawOverlay(SDL_Color color, Uint8 alpha) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, alpha);
    SDL_Rect full = {0, 0, screenW_, screenH_};
    SDL_RenderFillRect(renderer_, &full);
}

void SDLRenderer::drawBackground() {
    SDL_Texture* bg = nullptr;
    switch (screenState_) {
        case ScreenState::TITLE:  bg = bgMenu_;   break;
        case ScreenState::SETUP:  bg = bgSetup_;  break;
        case ScreenState::GAME:   bg = bgGame_;   break;
        case ScreenState::RESULT: bg = bgResult_; break;
    }
    if (bg) {
        SDL_RenderCopy(renderer_, bg, nullptr, nullptr);
        drawOverlay({0, 5, 20, 255}, 100);
    } else {
        drawRect(0, 0,            screenW_, screenH_ / 2, { 10, 18, 45, 255}, true);
        drawRect(0, screenH_ / 2, screenW_, screenH_ / 2, {  6, 12, 32, 255}, true);
    }
}

void SDLRenderer::drawButton(int x, int y, int w, int h,
                              const std::string& label, bool highlight) {
    SDL_Color bg  = highlight ? C_BTN_HL : C_BTN;
    SDL_Color bdr = highlight ? C_BORDER2 : C_BORDER;
    drawRoundedRect(x, y, w, h, 6, bg, true);
    drawRoundedRect(x, y, w, h, 6, bdr, false);
    TTF_Font* f = fontMd_;
    if (f) {
        SDL_Point sz = getTextSize(label, f);
        int tx = x + (w - sz.x) / 2;
        int ty = y + (h - sz.y) / 2;
        renderText(label, tx, ty, C_TEXT, f);
    }
}

int SDLRenderer::cellSize() const {
    int reservedHeight = 140;  
    int availableWidth = screenW_ - 2 * padding_;
    int availableHeight = screenH_ - 2 * padding_ - reservedHeight;
    int area = std::min(availableWidth, availableHeight);
    return (boardSize_ > 0) ? area / boardSize_ : area;
}

SDL_Rect SDLRenderer::cellRect(int row, int col) const {
    int cs = cellSize();
    int boardWidth = cs * boardSize_;
    int offsetX = (screenW_ - boardWidth) / 2;
    int offsetY = padding_ + 20; 
    return {offsetX + col * cs, offsetY + row * cs, cs, cs};
}

void SDLRenderer::clearScreen() {
    SDL_SetRenderDrawColor(renderer_, C_BG.r, C_BG.g, C_BG.b, 255);
    SDL_RenderClear(renderer_);
}

void SDLRenderer::showSelectMenu(SelectType selectType, int context) {
    if (selectType == SelectType::TITLE_UI)
        screenState_ = ScreenState::TITLE;
    else if (selectType == SelectType::PLAYER_UI)
        screenState_ = ScreenState::GAME;
    else
        screenState_ = ScreenState::SETUP;

    if (selectType == SelectType::PLAYER_UI) {
        drawRect(0, screenH_ - 40, screenW_, 30, C_BG, true);
        renderText("Click o trong  |  U = Undo  |  Esc = Surrender",
                   12, screenH_ - 32, C_HINT, fontSm_);
        renderPresent();
        return;  
    }

    clearScreen();
    drawBackground();

    if (selectType == SelectType::TITLE_UI) {
        renderTextCentered("TicTacToe v" + VERSION,
                           (int)(screenH_ * 0.18), C_HINT, fontSm_);

        std::string line1 = "CHON MOT SO BAT KY";
        std::string line2 = "DE BAT DAU TRO CHOI TICTOCTOE";
        int mid = screenH_ / 2;
        renderTextCentered(line1, mid - 70, C_TEXT_HI, fontLg_);
        renderTextCentered(line2, mid - 30, C_TEXT_HI, fontLg_);

        renderPresent();
        return;
    }

    renderText("TicTacToe v" + VERSION, 12, 10, C_HINT, fontSm_);

    std::string prompt, hint;

    switch (selectType) {
        case SelectType::SIZE_UI: {
        
            int pw = 480, ph = 160;
            int px = (screenW_ - pw) / 2;
            int py = (screenH_ - ph) / 2 - 30;
            drawRoundedRect(px, py, pw, ph, 10, C_PANEL, true);
            drawRoundedRect(px, py, pw, ph, 10, C_BORDER, false);

            renderTextCentered("CHON KICH THUOC BAN CO", py + 30, C_TEXT, fontMd_);
            renderTextCentered("(RANGE: 3-12)", py + 65, C_HINT, fontSm_);

            renderTextCentered(statusMsg_, screenH_ - 36, C_HINT, fontSm_);
            renderPresent();
            return;
        }
        case SelectType::GOAL_UI: {
            int pw = 480, ph = 160;
            int px = (screenW_ - pw) / 2;
            int py = (screenH_ - ph) / 2 - 30;
            drawRoundedRect(px, py, pw, ph, 10, C_PANEL, true);
            drawRoundedRect(px, py, pw, ph, 10, C_BORDER, false);

            renderTextCentered("CHON DIEU KIEN THANG (GOAL)", py + 30, C_TEXT, fontMd_);
            renderTextCentered("(RANGE: 3->SIZE DA CHON)", py + 65, C_HINT, fontSm_);

            renderTextCentered(statusMsg_, screenH_ - 36, C_HINT, fontSm_);
            renderPresent();
            return;
        }
        case SelectType::GAME_MODE_UI:
            prompt = "CHON CHE DO CHOI";
            hint   = "1:PvP    2:PvE    3:EvE";
            break;
        case SelectType::BOT_LEVEL_UI:
            prompt = "CHON DO KHO BOT";
            hint   = "1:EASY      2:MEDIUM      3:HARD";
            break;
        case SelectType::MUL_BOT_LEVEL_UI:
            prompt = "BOT " + std::to_string(context + 1) + " DO KHO";
            hint   = "1:EASY      2:MEDIUM      3:HARD";
            break;
        case SelectType::PLAYER_UI:
            prompt = "LUOT CUA BAN";
            hint   = "Click o trong  |  U = Undo  |  Esc = Surrender";
            screenState_ = ScreenState::GAME;
            break;
        case SelectType::END_RULE_UI:
            prompt = "CHON LUAT KET THUC";
            hint   = "1.NONE  2.OPEN_ONE  3.OPEN_TWO";
            break;
        default:
            prompt = "...";
            hint   = "";
    }

    int pw = 520, ph = 180;
    int px = (screenW_ - pw) / 2;
    int py = (screenH_ - ph) / 2 - 20;
    drawRoundedRect(px, py, pw, ph, 10, C_PANEL, true);
    drawRoundedRect(px, py, pw, ph, 10, C_BORDER, false);

    drawLine(px + 20, py + 60, px + pw - 20, py + 60, C_BORDER, 1);

    renderTextCentered(prompt, py + 25, C_TEXT_HI, fontLg_);
    renderTextCentered(hint,   py + 85, C_HINT,    fontMd_);

    renderTextCentered(statusMsg_, screenH_ - 36, C_HINT, fontSm_);
    renderPresent();
}

void SDLRenderer::showInvalidSelect(SelectType /*st*/, int /*ctx*/) {
    statusMsg_ = "  Nhap sai -- vui long thu lai  ";
    drawRoundedRect(0, screenH_ - 54, screenW_, 38, 0,
                    {55, 5, 5, 210}, true);
    renderTextCentered(statusMsg_, screenH_ - 44, C_ERR, fontSm_);
    renderPresent();
    SDL_Delay(700);
}

void SDLRenderer::showValidSelect(SelectType /*st*/, int /*ctx*/) {
    statusMsg_ = "";
}

void SDLRenderer::displayBoard(const char board[][BOARD_N_MAX], const int size) {
    boardSize_   = size;
    screenState_ = ScreenState::GAME;

    clearScreen();
    drawBackground();

    int cs     = cellSize();
    int margin = std::max(4, cs / 8);

    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            SDL_Rect cell = cellRect(r, c);

            if (r == lastRow_ && c == lastCol_)
                drawRect(cell.x + 1, cell.y + 1, cs - 2, cs - 2, C_LAST, true);

            drawRect(cell.x, cell.y, cs, cs, C_GRID, false);

            char ch = board[r][c];
            int  cx = cell.x + cs / 2;
            int  cy = cell.y + cs / 2;
            int  inn = cs / 2 - margin - 2;
            int  thick = std::max(2, cs / 10);

            if (ch == 'X') {
                drawLine(cell.x + margin,      cell.y + margin,
                         cell.x + cs - margin, cell.y + cs - margin,
                         C_X, thick);
                drawLine(cell.x + cs - margin, cell.y + margin,
                         cell.x + margin,      cell.y + cs - margin,
                         C_X, thick);
            } else if (ch == 'O') {
                drawCircle(cx, cy, inn, C_O, thick);
            }
        }
    }

    std::string undoText = "U: Undo";
    std::string surrenderText = "Esc: Surrender";

    SDL_Point undoSize = getTextSize(undoText, fontMd_);  
    SDL_Point surrenderSize = getTextSize(surrenderText, fontMd_);

    int btnPadding = 20; 
    int btnW_undo = undoSize.x + btnPadding;
    int btnW_surrender = surrenderSize.x + btnPadding;
    int btnH = undoSize.y + 12;  

    int btnSpacing = 10;
    int btnX = screenW_ - std::max(btnW_undo, btnW_surrender) - 10;
    int btnY = screenH_ - btnH - 90;

    drawButton(btnX, btnY,
           btnW_undo, btnH, undoText);
    drawButton(btnX, btnY + btnH + btnSpacing,
           btnW_surrender, btnH, surrenderText);

    if (!statusMsg_.empty())
        renderText(statusMsg_, 12, screenH_ - 28, C_INFO_TEXT, fontSm_);

    if (currentSetup_) {
    int startY = screenH_ - 160;  
    int lineHeight = 28;          
    
    std::string line1 = "Size:" + std::to_string(currentSetup_->size)
                      + "  Goal:" + std::to_string(currentSetup_->goal);
    renderText(line1, 40, startY, C_INFO_TEXT, fontSm_); 
    
    std::string line2 = "Mode:" + modeToString((int)currentSetup_->mode);
    renderText(line2, 40, startY + lineHeight, C_INFO_TEXT, fontSm_);
  
    std::string line3 = "EndRule:" + endRuleToString((int)currentSetup_->rule);
    renderText(line3, 40, startY + lineHeight * 2, C_INFO_TEXT, fontSm_);
}

    renderPresent();
}

void SDLRenderer::showMove(const int row, const int col) {
    lastRow_ = row;
    lastCol_ = col;
    statusMsg_ = "Nuoc vua di: ("
               + std::to_string(row) + ", "
               + std::to_string(col) + ")";
}

void SDLRenderer::showInvalidMove() {
    statusMsg_ = "  Nuoc di sai! Chon o trong.  ";
    drawRoundedRect(0, screenH_ - 54, screenW_, 38, 0,
                    {55, 5, 5, 210}, true);
    renderTextCentered(statusMsg_, screenH_ - 44, C_ERR, fontSm_);
    renderPresent();
    SDL_Delay(700);
}

void SDLRenderer::showPlayer(const int player, const bool is_bot) {
    statusMsg_ = is_bot
        ? "Bot (Player " + std::to_string(player + 1) + ") dang suy nghi..."
        : "Luot cua Player " + std::to_string(player + 1);
}

void SDLRenderer::showResult(const int winner, const bool is_bot,
                              const WinLine* winLine) {
    if (winLine && !winLine->cells.empty()) {
        int cs = cellSize();
        for (auto [r, c] : winLine->cells) {
            SDL_Rect cell = cellRect(r, c);
            drawRect(cell.x + 2, cell.y + 2, cs - 4, cs - 4, C_WIN_HL, true);
        }
        renderPresent();
        SDL_Delay(700);
    }

    screenState_ = ScreenState::RESULT;
    clearScreen();
    drawBackground();

    int pw = 480, ph = 190;
    int px = (screenW_ - pw) / 2;
    int py = (screenH_ - ph) / 2;
    drawRoundedRect(px, py, pw, ph, 12, C_PANEL, true);
    drawRoundedRect(px, py, pw, ph, 12, C_BORDER, false);

    drawLine(px + 20, py + 65, px + pw - 20, py + 65, C_BORDER, 1);

    std::string msg;
    SDL_Color   col;
    if (currentResult_) {
    std::string turns = "Turns: " + std::to_string(currentResult_->turns);
    renderTextCentered(turns, py + 90, C_HINT, fontSm_);
}

    if (winner == DRAW_RESULT || winner < 0) {
        msg = "HOA!";
        col = {255, 215, 70, 255};
    } else if (is_bot) {
        msg = "Bot (P" + std::to_string(winner + 1) + ") THANG!";
        col = C_ERR;
    } else {
        msg = "Player " + std::to_string(winner + 1) + " THANG!";
        col = {70, 245, 145, 255};
    }
    

    renderTextCentered(msg,                    py + 22, col,    fontLg_);
    renderTextCentered("Click hoac nhan phim de thoat",
                                               py + 130, C_HINT, fontSm_);
    renderPresent();
}

void SDLRenderer::printResult(const GameResult& result) {
    std::cout << result.winner << " " << result.turns << "\n";
}

void SDLRenderer::close() {
    if (bgMenu_)   { SDL_DestroyTexture(bgMenu_);   bgMenu_   = nullptr; }
    if (bgSetup_)  { SDL_DestroyTexture(bgSetup_);  bgSetup_  = nullptr; }
    if (bgGame_)   { SDL_DestroyTexture(bgGame_);   bgGame_   = nullptr; }
    if (bgResult_) { SDL_DestroyTexture(bgResult_); bgResult_ = nullptr; }
    if (fontLg_)   { TTF_CloseFont(fontLg_);   fontLg_   = nullptr; }
    if (fontMd_)   { TTF_CloseFont(fontMd_);   fontMd_   = nullptr; }
    if (fontSm_)   { TTF_CloseFont(fontSm_);   fontSm_   = nullptr; }
    if (renderer_) { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_)   { SDL_DestroyWindow(window_);   window_   = nullptr; }

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    Logger::log("SDLRenderer closed.");
}
