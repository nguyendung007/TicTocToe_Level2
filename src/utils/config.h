#pragma once
#include <string>
inline const std::string VERSION = "0.4.25022808";

struct RunConfig {
    bool interactive = true; 
    bool judge_mode = false;  
    std::string input_file;  

    bool to_file = true;               
    std::string log_file = "log.txt"; 

    bool verbose_flag = false;  
    bool gui_flag = false;      

    bool is_help = false; 

    int screenWidth = 800;   
    int screenHeight = 600; 
    int boardPadding = 100;  

    friend std::ostream& operator<<(std::ostream& os, const RunConfig& config);
    std::string toString() const;
};


RunConfig parseArgs(int argc, char* argv[]);

std::string configHelpStr();