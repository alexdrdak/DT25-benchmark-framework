#ifndef D_LOGGER_H
#define D_LOGGER_H

#include <string>
#include <iostream>
#include <iomanip>

class d_Logger {
public:
    static void err(std::string const& message);
    static void info(std::string const& message);
    static void warn(std::string const& message);
    static void debug(std::string const& message);
};


#endif //D_LOGGER_H