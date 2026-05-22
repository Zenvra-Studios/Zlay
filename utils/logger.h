#pragma once

#include <cctype>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <source_location>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

#include <fmt/chrono.h>
#include <fmt/format.h>

namespace spectrax::log {

enum class level : std::uint8_t {
  trace = 0,
  debug = 1,
  info = 2,
  warn = 3,
  warning = warn,
  error = 4,
  critical = 5,
  off = 6,
};

using Level = level;
using source_location = std::source_location;

template <typename... Args>
using format_string = fmt::format_string<Args...>;

[[nodiscard]] constexpr level default_level() noexcept {
#if defined(NDEBUG)
  return level::info;
#else
  return level::debug;
#endif
}

[[nodiscard]] constexpr level build_default_level() noexcept {
  return default_level();
}

[[nodiscard]] constexpr std::string_view to_string(level value) noexcept {
  switch (value) {
    case level::trace:
      return "trace";
    case level::debug:
      return "debug";
    case level::info:
      return "info";
    case level::warn:
      return "warn";
    case level::error:
      return "error";
    case level::critical:
      return "critical";
    case level::off:
      return "off";
  }
  return "unknown";
}

[[nodiscard]] constexpr std::string_view level_name(level value) noexcept {
  return to_string(value);
}

struct options {
  std::string name = "SpectraX";
  level minimum_level = default_level();
#if defined(NDEBUG)
  bool enable_console = false;
#else
  bool enable_console = true;
#endif
  bool enable_file = true;
  bool flush_each_message = true;
  std::filesystem::path file_path{};
};

namespace detail {

struct shared_state {
  std::mutex mutex{};
  std::string name = "SpectraX";
  std::filesystem::path file_path{};
  std::ofstream file_stream{};
  level minimum_level = default_level();
  bool enable_console = true;
  bool enable_file = true;
  bool flush_each_message = true;
  bool initialized = false;
};

inline shared_state& state() {
  static shared_state instance;
  return instance;
}

[[nodiscard]] inline std::string sanitize_file_stem(std::string_view value) {
  std::string out;
  out.reserve(value.size());
  for (const unsigned char ch : value) {
    if (std::isalnum(ch) != 0) {
      out.push_back(static_cast<char>(ch));
    } else if (ch == '-' || ch == '_') {
      out.push_back(static_cast<char>(ch));
    } else {
      out.push_back('_');
    }
  }

  if (out.empty()) {
    out = "SpectraX";
  }

  return out;
}

[[nodiscard]] inline std::filesystem::path default_log_path(std::string_view name) {
  std::error_code ec;
  auto dir = std::filesystem::temp_directory_path(ec);
  if (ec || dir.empty()) {
    dir = std::filesystem::current_path(ec);
  }
  if (ec || dir.empty()) {
    dir = ".";
  }

  return dir / fmt::format("{}.log", sanitize_file_stem(name));
}

inline void apply_options_unlocked(shared_state& state_ref, options opts) {
  state_ref.name = opts.name.empty() ? std::string("SpectraX") : std::move(opts.name);
  state_ref.minimum_level = opts.minimum_level;
  state_ref.enable_console = opts.enable_console;
  state_ref.enable_file = opts.enable_file;
  state_ref.flush_each_message = opts.flush_each_message;
  state_ref.file_path = opts.file_path.empty() ? default_log_path(state_ref.name)
                                               : std::move(opts.file_path);

  state_ref.file_stream.close();
  if (state_ref.enable_file) {
    std::error_code ec;
    const auto parent = state_ref.file_path.parent_path();
    if (!parent.empty()) {
      std::filesystem::create_directories(parent, ec);
    }

    state_ref.file_stream.open(state_ref.file_path, std::ios::app);
    if (!state_ref.file_stream) {
      state_ref.enable_file = false;
      std::cerr << "SpectraX logger: failed to open log file at "
                << state_ref.file_path.string() << '\n';
    }
  }

  state_ref.initialized = true;
}

inline void ensure_initialized() {
  auto& state_ref = state();
  if (state_ref.initialized) {
    return;
  }

  std::lock_guard<std::mutex> lock(state_ref.mutex);
  if (!state_ref.initialized) {
    apply_options_unlocked(state_ref, options{});
  }
}

[[nodiscard]] inline bool enabled(level value) {
  ensure_initialized();
  const auto& state_ref = state();
  return value != level::off &&
         static_cast<std::uint8_t>(value) >= static_cast<std::uint8_t>(state_ref.minimum_level);
}

[[nodiscard]] inline std::string timestamp_now() {
  using namespace std::chrono;
  const auto now = system_clock::now();
  const auto time = system_clock::to_time_t(now);
  const auto millis = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
  std::tm local_tm{};
#if defined(_WIN32)
  localtime_s(&local_tm, &time);
#else
  localtime_r(&time, &local_tm);
#endif
  return fmt::format("{:%Y-%m-%d %H:%M:%S}.{:03}",
                     local_tm,
                     static_cast<int>(millis.count()));
}

[[nodiscard]] inline std::string source_suffix(const source_location& location) {
  const auto filename = std::filesystem::path(location.file_name()).filename().string();
  return fmt::format("[{}:{}]", filename, location.line());
}

[[nodiscard]] inline std::string compose_line(level value,
                                              std::string_view logger_name,
                                              std::string_view message,
                                              const source_location* location) {
  if (location != nullptr) {
    return fmt::format("{} [{}] [{}] {} {}",
                       timestamp_now(),
                       to_string(value),
                       logger_name,
                       source_suffix(*location),
                       message);
  }

  return fmt::format("{} [{}] [{}] {}",
                     timestamp_now(),
                     to_string(value),
                     logger_name,
                     message);
}

inline void emit(level value, std::string_view message, const source_location* location = nullptr) {
  ensure_initialized();

  auto& state_ref = state();
  std::lock_guard<std::mutex> lock(state_ref.mutex);
  if (value == level::off ||
      static_cast<std::uint8_t>(value) < static_cast<std::uint8_t>(state_ref.minimum_level)) {
    return;
  }

  const std::string line = compose_line(value, state_ref.name, message, location);
  if (state_ref.enable_console) {
    std::ostream& stream =
        static_cast<std::uint8_t>(value) >= static_cast<std::uint8_t>(level::error) ? std::cerr
                                                                                     : std::clog;
    stream << line << '\n';
    if (state_ref.flush_each_message) {
      stream.flush();
    }
  }

  if (state_ref.enable_file && state_ref.file_stream) {
    state_ref.file_stream << line << '\n';
    if (state_ref.flush_each_message) {
      state_ref.file_stream.flush();
    }
  }
}

} // namespace detail

inline void init(options opts = {}) {
  auto& state_ref = detail::state();
  std::lock_guard<std::mutex> lock(state_ref.mutex);
  detail::apply_options_unlocked(state_ref, std::move(opts));
}

inline void init(std::string_view logger_name, level minimum_level = default_level()) {
  options opts;
  opts.name = std::string(logger_name);
  opts.minimum_level = minimum_level;
  init(std::move(opts));
}

inline void set_level(level value) noexcept {
  detail::ensure_initialized();
  auto& state_ref = detail::state();
  std::lock_guard<std::mutex> lock(state_ref.mutex);
  state_ref.minimum_level = value;
}

[[nodiscard]] inline level get_level() noexcept {
  detail::ensure_initialized();
  auto& state_ref = detail::state();
  std::lock_guard<std::mutex> lock(state_ref.mutex);
  return state_ref.minimum_level;
}

[[nodiscard]] inline bool enabled(level value) {
  try {
    return detail::enabled(value);
  } catch (...) {
    return false;
  }
}

[[nodiscard]] inline std::filesystem::path log_file_path() {
  detail::ensure_initialized();
  auto& state_ref = detail::state();
  std::lock_guard<std::mutex> lock(state_ref.mutex);
  return state_ref.file_path;
}

inline void flush() {
  detail::ensure_initialized();
  auto& state_ref = detail::state();
  std::lock_guard<std::mutex> lock(state_ref.mutex);
  if (state_ref.file_stream) {
    state_ref.file_stream.flush();
  }
  std::clog.flush();
  std::cerr.flush();
}

template <level Value>
inline void log(std::string_view message) {
  try {
    if (!enabled(Value)) {
      return;
    }
    detail::emit(Value, message);
  } catch (...) {
  }
}

template <level Value, typename... Args>
  requires (sizeof...(Args) > 0)
inline void log(format_string<Args...> fmt_str, Args&&... args) {
  try {
    if (!enabled(Value)) {
      return;
    }
    detail::emit(Value, fmt::format(fmt_str, std::forward<Args>(args)...));
  } catch (...) {
  }
}

template <level Value>
inline void log_at(std::string_view message,
                   const source_location& location = source_location::current()) {
  try {
    if (!enabled(Value)) {
      return;
    }
    detail::emit(Value, message, &location);
  } catch (...) {
  }
}

template <level Value, typename... Args>
  requires (sizeof...(Args) > 0)
inline void log_at(const source_location& location,
                   format_string<Args...> fmt_str,
                   Args&&... args) {
  try {
    if (!enabled(Value)) {
      return;
    }
    detail::emit(Value, fmt::format(fmt_str, std::forward<Args>(args)...), &location);
  } catch (...) {
  }
}

inline void trace(std::string_view message) {
  log<level::trace>(message);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void trace(format_string<Args...> fmt_str, Args&&... args) {
  log<level::trace>(fmt_str, std::forward<Args>(args)...);
}

inline void debug(std::string_view message) {
  log<level::debug>(message);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void debug(format_string<Args...> fmt_str, Args&&... args) {
  log<level::debug>(fmt_str, std::forward<Args>(args)...);
}

inline void info(std::string_view message) {
  log<level::info>(message);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void info(format_string<Args...> fmt_str, Args&&... args) {
  log<level::info>(fmt_str, std::forward<Args>(args)...);
}

inline void warn(std::string_view message) {
  log<level::warn>(message);
}

inline void warning(std::string_view message) {
  warn(message);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void warn(format_string<Args...> fmt_str, Args&&... args) {
  log<level::warn>(fmt_str, std::forward<Args>(args)...);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void warning(format_string<Args...> fmt_str, Args&&... args) {
  warn(fmt_str, std::forward<Args>(args)...);
}

inline void error(std::string_view message) {
  log<level::error>(message);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void error(format_string<Args...> fmt_str, Args&&... args) {
  log<level::error>(fmt_str, std::forward<Args>(args)...);
}

inline void critical(std::string_view message) {
  log<level::critical>(message);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void critical(format_string<Args...> fmt_str, Args&&... args) {
  log<level::critical>(fmt_str, std::forward<Args>(args)...);
}

inline void trace_at(std::string_view message,
                     const source_location& location = source_location::current()) {
  log_at<level::trace>(message, location);
}

inline void debug_at(std::string_view message,
                     const source_location& location = source_location::current()) {
  log_at<level::debug>(message, location);
}

inline void info_at(std::string_view message,
                    const source_location& location = source_location::current()) {
  log_at<level::info>(message, location);
}

inline void warn_at(std::string_view message,
                    const source_location& location = source_location::current()) {
  log_at<level::warn>(message, location);
}

inline void error_at(std::string_view message,
                     const source_location& location = source_location::current()) {
  log_at<level::error>(message, location);
}

inline void critical_at(std::string_view message,
                        const source_location& location = source_location::current()) {
  log_at<level::critical>(message, location);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void trace_at(const source_location& location,
                     format_string<Args...> fmt_str,
                     Args&&... args) {
  log_at<level::trace>(location, fmt_str, std::forward<Args>(args)...);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void debug_at(const source_location& location,
                     format_string<Args...> fmt_str,
                     Args&&... args) {
  log_at<level::debug>(location, fmt_str, std::forward<Args>(args)...);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void info_at(const source_location& location,
                    format_string<Args...> fmt_str,
                    Args&&... args) {
  log_at<level::info>(location, fmt_str, std::forward<Args>(args)...);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void warn_at(const source_location& location,
                    format_string<Args...> fmt_str,
                    Args&&... args) {
  log_at<level::warn>(location, fmt_str, std::forward<Args>(args)...);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void error_at(const source_location& location,
                     format_string<Args...> fmt_str,
                     Args&&... args) {
  log_at<level::error>(location, fmt_str, std::forward<Args>(args)...);
}

template <typename... Args>
  requires (sizeof...(Args) > 0)
inline void critical_at(const source_location& location,
                        format_string<Args...> fmt_str,
                        Args&&... args) {
  log_at<level::critical>(location, fmt_str, std::forward<Args>(args)...);
}

} // namespace spectrax::log

namespace rpc {
namespace log = spectrax::log;
}