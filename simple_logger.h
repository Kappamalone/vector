#pragma once

// Simple thread-safe header-only logger
//
// Design: What goes into writing a good logger?
// - thread safety, ie some global singleton resource that can be written to
// - customisable output destination (stdout, file, socket)
// - standard log levels (debug, info, warning, error, critical)
// - compile time filtering of logs (ie logging wrapped into macros)
// - metainformation like file:line number

#include <iostream>
#include <mutex>
#include <ostream>

#if __has_include(<source_location>)
#include <source_location>
namespace sl = std;
#elif __has_include(<experimental/source_location>)
#include <experimental/source_location>
namespace sl = std::experimental;
#else
#error "source_location not available"
#endif

namespace SimpleLogger {

enum class LogLevel : std::uint8_t { Error = 1, Warn = 2, Info = 3, Debug = 4 };

inline std::ostream& operator<<(std::ostream& os, LogLevel level) {
  switch (level) {
    case LogLevel::Error:
      return os << "ERROR";
    case LogLevel::Warn:
      return os << "WARN";
    case LogLevel::Info:
      return os << "INFO";
    case LogLevel::Debug:
      return os << "DEBUG";
  }
  return os << "UNKNOWN";
}

class Logger {
public:
  Logger(const Logger& other) = delete;
  Logger& operator=(const Logger& other) = delete;
  Logger(Logger&& other) = delete;
  Logger& operator=(Logger&& other) = delete;

  static inline Logger& Get() {
    static Logger logger;
    return logger;
  }

  void SetStream(std::ostream& stream) {
    std::unique_lock<std::mutex> lck{mtx};
    out = &stream;
  }

  template <typename... Args>
  void Log(LogLevel level, const sl::source_location source_location,
           Args&&... args) {
    std::unique_lock<std::mutex> lck{mtx};

    (*out) << "[" << level << "] " << source_location.file_name() << ':'
           << source_location.line() << " `" << source_location.function_name()
           << "`: ";

    ((*out) << ... << std::forward<Args>(args)) << "\n";
  }

private:
  Logger() : out(&std::cout) {}

  std::mutex mtx;
  std::ostream* out;
};

}; // namespace SimpleLogger

#ifndef LOG_LEVEL
#define LOG_LEVEL 4
#endif

#if LOG_LEVEL >= 4
#define LOG_DEBUG(...)                                                         \
  do {                                                                         \
    SimpleLogger::Logger::Get().Log(SimpleLogger::LogLevel::Debug,             \
                                    sl::source_location::current()             \
                                        __VA_OPT__(, ) __VA_ARGS__);           \
  } while (0)
#else
#define LOG_DEBUG(...) ((void)0)
#endif

#if LOG_LEVEL >= 3
#define LOG_INFO(...)                                                          \
  do {                                                                         \
    SimpleLogger::Logger::Get().Log(SimpleLogger::LogLevel::Info,              \
                                    sl::source_location::current()             \
                                        __VA_OPT__(, ) __VA_ARGS__);           \
  } while (0)
#else
#define LOG_INFO(...) ((void)0)
#endif

#if LOG_LEVEL >= 2
#define LOG_WARN(...)                                                          \
  do {                                                                         \
    SimpleLogger::Logger::Get().Log(SimpleLogger::LogLevel::Warn,              \
                                    sl::source_location::current()             \
                                        __VA_OPT__(, ) __VA_ARGS__);           \
  } while (0)
#else
#define LOG_WARN(...) ((void)0)
#endif

#if LOG_LEVEL >= 1
#define LOG_ERROR(...)                                                         \
  do {                                                                         \
    SimpleLogger::Logger::Get().Log(SimpleLogger::LogLevel::Error,             \
                                    sl::source_location::current()             \
                                        __VA_OPT__(, ) __VA_ARGS__);           \
  } while (0)
#else
#define LOG_ERROR(...) ((void)0)
#endif
