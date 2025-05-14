#include "d_Logger.h"

void d_Logger::err(std::string const& message) {
    std::time_t time_now = std::time(nullptr);
    std::cerr << "[" << std::put_time(std::localtime(&time_now), "%F %T") << "] ERR: " << message << std::endl;
}

void d_Logger::info(std::string const& message) {
    std::time_t time_now = std::time(nullptr);
    std::cout << "[" << std::put_time(std::localtime(&time_now), "%F %T") << "] INFO: " << message << std::endl;
}

void d_Logger::warn(std::string const& message) {
    std::time_t time_now = std::time(nullptr);
    std::cout << "[" << std::put_time(std::localtime(&time_now), "%F %T") << "] WARN: " << message << std::endl;
}

void d_Logger::debug(std::string const& message) {
    std::time_t time_now = std::time(nullptr);
    std::cout << "[" << std::put_time(std::localtime(&time_now), "%F %T") << "] DEBUG: " << message << std::endl;
}