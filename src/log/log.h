#ifndef _LOG_H_
#define _LOG_H_

#include <memory>
#ifndef SPDLOG_COMPILED_LIB
    #define SPDLOG_COMPILED_LIB
#endif
#include "spdlog.h"
#include "sinks/stdout_color_sinks.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>

class Log {
    public:
    static void Init();
    inline static std::shared_ptr<spdlog::logger>& GetLogger() {return logger;}

    private:
    static std::shared_ptr<spdlog::logger> logger;
};

#define LOG_ERROR(...) Log::GetLogger()->error(__VA_ARGS__)
#define LOG_INFO(...)  Log::GetLogger()->info(__VA_ARGS__)
#define LOG_GLM(...)   Log::GetLogger()->info(glm::to_string(__VA_ARGS__))

#endif