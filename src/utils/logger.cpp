#include "logger.h"
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "config.h"

const std::string Logger::RESET  = "\033[0m";
const std::string Logger::RED    = "\033[31m";
const std::string Logger::GREEN  = "\033[32m";
const std::string Logger::YELLOW = "\033[33m";
const std::string Logger::BLUE   = "\033[34m";
const std::string Logger::CYAN   = "\033[36m";

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

std::string Logger::levelToString(Level level) {
    switch (level) {
        case Level::DEBUG:   return "DEBUG";
        case Level::INFO:    return "INFO";
        case Level::WARNING: return "WARN";
        case Level::ERROR:   return "ERROR";
        case Level::MSG:     return "";  
        default:             return "UNKNOWN";
    }
}

std::string Logger::getColor(Level level) {
    switch (level) {
        case Level::DEBUG:   return BLUE;
        case Level::INFO:    return GREEN;
        case Level::WARNING: return YELLOW;
        case Level::ERROR:   return RED;
        default:             return RESET;
    }
}

void Logger::init(bool judge_mode, bool to_file,
                  const std::string& path, bool verbose_flag) {
    Logger& inst = getInstance();  

    inst.write_to_file_ = to_file;
    inst.is_judge_mode_ = judge_mode;

    if (inst.write_to_file_) {
        inst.log_file_.open(path, std::ios::out | std::ios::trunc);

        if (!inst.log_file_.is_open()) {
            std::cerr << "[Logger] Cannot open log file: " << path
                      << ". Falling back to console only." << std::endl;
            inst.write_to_file_ = false;
        }
    }

    std::string header = "Tic-tac-toe Game (Version: " + std::string(VERSION) + " by baluong.87)\n";
    header += std::string(48, '-');

    if (inst.write_to_file_) {
        inst.log_file_ << header << std::endl;
    } else if (!inst.is_judge_mode_) {
        std::cout << header << std::endl;
    }

    inst.min_level_ = Level::INFO;

    if (verbose_flag) {
        inst.min_level_ = Level::DEBUG;
    }
}

void Logger::log(const std::string& msg, Level level) {
    Logger& inst = getInstance();  // [THAY ĐỔI] lấy singleton instance

    if (static_cast<int>(level) < static_cast<int>(inst.min_level_)) {
        return;
    }

    auto now = std::time(nullptr);
    auto tm  = *std::localtime(&now);

    std::stringstream ss_lv;
    std::string formatted_lv;
    if (level != Level::MSG) {
        ss_lv << "[" << levelToString(level) << "]";
        formatted_lv = ss_lv.str();
    }

    std::stringstream ss_msg;
    ss_msg << (formatted_lv.empty() ? "" : " - ")
           << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] ";
    ss_msg << msg;

    std::string formatted_msg = ss_msg.str();

    if (inst.write_to_file_) {
        inst.log_file_ << formatted_lv << formatted_msg << std::endl;
        inst.log_file_.flush();
    } else if (!inst.is_judge_mode_) {
        std::cout << getColor(level) << formatted_lv;
        std::cout << getColor(Level::MSG) << formatted_msg << RESET << std::endl;
    }
}

void Logger::close() {
    Logger& inst = getInstance();  

    if (inst.log_file_.is_open()) {
        inst.log_file_.close();
    }
}