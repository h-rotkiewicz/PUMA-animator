#pragma once
#include <chrono>
#include <concepts>
#include <fstream>
#include <iostream>

namespace ColorMod {
enum Code {
  FG_RED = 31,
  FG_GREEN = 32,
  FG_YELLOW = 33,
  FG_BLUE = 34,
  FG_MAGENDA = 35,
  FG_CYAN = 36,
  FG_GRAY = 37,
  FG_DEFAULT = 39,
};
};

inline std::ostream &operator<<(std::ostream &os, ColorMod::Code _c) {
  os << "\033[" << (int)_c << "m";
  return os;
}

namespace LOG {

template <typename T>
concept Writable = requires(T t, std::ostream &os) {
  { os << t } -> std::same_as<std::ostream &>;
};

inline class logger {
private:
  std::ofstream outFile_;

public:
  logger(std::string_view LogFile) {
    outFile_.open(LogFile.data(), std::ios_base::trunc);
  }

#ifdef STDOUT
  void log(auto &&mess, ColorMod::Code color = ColorMod::FG_CYAN) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_time);

    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y-%m-%d %H:%M");

    outFile_ << oss.str() << " || " << mess << '\n';
    std::cout << color << oss.str() << " || " << mess << '\n'
              << ColorMod::FG_DEFAULT;
  }
#else
  void log(auto &&mess) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_time);

    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y-%m-%d %H:%M");

    outFile_ << oss.str() << " || " << mess << '\n';
  }
#endif
} Logger("Log.txt");
} // namespace LOG
