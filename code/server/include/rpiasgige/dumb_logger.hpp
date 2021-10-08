#ifndef RPIASGIGE_DUMB_LOGGER_HPP
#define RPIASGIGE_DUMB_LOGGER_HPP

#include <string>

// Replace it with a proper logging library

class Logger
{

public:
    Logger(const std::string &identifier) : identifier(identifier) {}
    virtual ~Logger() {}

    void log_msg(const std::string &level, const std::string &msg) const
    {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        printf("%d-%02d-%02d %02d:%02d:%02d - ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        printf("%s - %s - %s\n", level.c_str(), this->identifier.c_str(), msg.c_str());
        fflush(stdout);
    }

    void error_msg(const std::string &msg) const
    {
        log_msg("ERROR", msg);
    }

    void warn_msg(const std::string &msg) const
    {
        log_msg("WARNING", msg);
    }

    void debug_msg(const std::string &msg) const
    {
        log_msg("DEBUG", msg);
    }

private:
    const std::string identifier;
};

#endif