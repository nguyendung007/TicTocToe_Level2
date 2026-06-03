#include "interaction.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include "../utils/logger.h"

TerminalInteraction::TerminalInteraction() : cin_backup(nullptr) {}
TerminalInteraction::~TerminalInteraction() {}

void TerminalInteraction::init(const RunConfig& config) {
    initInteraction(config);
}

void TerminalInteraction::initInteraction(const RunConfig& config) {
    cin_backup = nullptr;
    if (!config.interactive && !config.input_file.empty()) {
        global_file_in.open(config.input_file);
        if (global_file_in.is_open()) {
            cin_backup = std::cin.rdbuf();
            std::cin.rdbuf(global_file_in.rdbuf());
            Logger::log("cin redirected to: " + config.input_file);
        } else {
            Logger::log("Cannot open input file, using console.", Logger::Level::ERROR);
        }
    }
}

void TerminalInteraction::closeInteraction() {
    if (cin_backup) { std::cin.rdbuf(cin_backup); cin_backup = nullptr; }
    if (global_file_in.is_open()) global_file_in.close();
}

bool TerminalInteraction::validateInput(const std::string& input) {
    if (input.empty()) return false;
    for (char ch : input)
        if (!std::isdigit((unsigned char)ch)) return false;
    return true;
}

bool TerminalInteraction::getInput(int* val) {
    std::string s;
    if (!std::getline(std::cin, s)) return false;
    // trim
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ')) s.pop_back();
    while (!s.empty() && s.front() == ' ') s.erase(s.begin());
    if (!validateInput(s)) {
        Logger::log("Invalid input: '" + s + "'", Logger::Level::DEBUG);
        return false;
    }
    try { *val = std::stoi(s); } catch (...) { return false; }
    return true;
}

void TerminalInteraction::pause(int timeout) {
    if (timeout > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
        return;
    }

    if (!std::cin.good() || std::cin.eof()) return;
    std::cin.ignore(10000, '\n');
}

bool TerminalInteraction::selectSize(int* size) {
    int v;
    if (!getInput(&v)) return false;
    if (v < BOARD_N_MIN || v > BOARD_N_MAX) return false;
    *size = v;
    return true;
}

bool TerminalInteraction::selectGoal(int* goal, const int size) {
    int v;
    if (!getInput(&v)) return false;
    if (v < 3 || v > size) return false;
    *goal = v;
    return true;
}

bool TerminalInteraction::selectGameMode(GameMode* mode) {
    int v;
    if (!getInput(&v)) return false;
    switch (v) {
        case 1: *mode = GameMode::PVP; break;
        case 2: *mode = GameMode::PVE; break;
        case 3: *mode = GameMode::EVE; break;
        default: return false;
    }
    return true;
}

bool TerminalInteraction::selectBotLevel(BotLevel* levels, const int index) {
    int v;
    if (!getInput(&v)) return false;
    switch (v) {
        case 1: levels[index] = BotLevel::EASY;   break;
        case 2: levels[index] = BotLevel::MEDIUM; break;
        case 3: levels[index] = BotLevel::HARD;   break;
        default: return false;
    }
    return true;
}

bool TerminalInteraction::getPlayerMove(int* row, int* col) {
    std::string s;
    if (!std::getline(std::cin, s)) return false;

    // trim đầu/cuối
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ')) s.pop_back();
    while (!s.empty() && s.front() == ' ') s.erase(s.begin());

    std::istringstream iss(s);
    int r, c;
    if (!(iss >> r >> c)) return false;

    *row = r;
    *col = c;
    return true;
}

bool TerminalInteraction::selectEndRule(EndRule* rule) {
    int v;
    if (!getInput(&v)) return false;
    switch (v) {
        case 1: *rule = EndRule::NONE;     break;
        case 2: *rule = EndRule::OPEN_ONE; break;
        case 3: *rule = EndRule::OPEN_TWO; break;
        default: return false;
    }
    return true;
}

void TerminalInteraction::close() { closeInteraction(); }

