#pragma once
#include <fstream>
#include <string>

class Logger {
public:

    enum class Level {
        DEBUG,   
        INFO,     
        WARNING,  
        ERROR,   
        MSG,     
    };

    static const std::string RESET;
    static const std::string RED;
    static const std::string GREEN;
    static const std::string YELLOW;
    static const std::string BLUE;
    static const std::string CYAN;

    static void init(bool judge_mode, bool to_file = true,
                     const std::string& path = "log.txt",
                     bool verbose_flag = false);
    static void log(const std::string& msg, Level level = Level::INFO);
    static void close();

    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger()  = default;
    ~Logger() = default;
    static Logger& getInstance();

    std::ofstream log_file_;
    bool          write_to_file_ = false;
    bool          is_judge_mode_ = false;
    Level         min_level_     = Level::INFO;

    static std::string levelToString(Level level);
    static std::string getColor(Level level);
};