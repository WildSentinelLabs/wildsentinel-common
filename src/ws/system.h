#pragma once

#include "ws/config.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <errno.h>
#include <unistd.h>
#endif

#include <cstring>
#include <mutex>
#include <string>
#include <string_view>

namespace ws {

inline std::string GetLastErrorMessage() {
#ifdef _WIN32
  DWORD error = GetLastError();
  if (error == 0) return "No error";
  LPSTR messageBuffer = nullptr;
  size_t size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      reinterpret_cast<LPSTR>(&messageBuffer), 0, nullptr);

  std::string message(messageBuffer, size);
  LocalFree(messageBuffer);
  return message;
#else
  return std::string(strerror(errno));
#endif
}

inline int GetPid() {
#if defined(_WIN32)
  return static_cast<int>(::GetCurrentProcessId());
#else
  return static_cast<int>(::getpid());
#endif
}

class Console {
 private:
  static std::mutex console_mutex_;
  static void WriteToConsole(std::string_view message, bool is_error);

 public:
  static void FormatOutput() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
  }

  static void Write(const std::string& message) {
    WriteToConsole(message, false);
  }

  static void WriteLine(const std::string& message = "") {
    WriteToConsole(message + '\n', false);
  }

  class Output {
   public:
    static void Write(const std::string& message) {
      WriteToConsole(message, false);
    }

    static void WriteLine(const std::string& message = "") {
      WriteToConsole(message + '\n', false);
    }
  };

  class Error {
   public:
    static void Write(const std::string& message) {
      WriteToConsole(message, true);
    }

    static void WriteLine(const std::string& message = "") {
      WriteToConsole(message + '\n', true);
    }
  };
};

inline std::mutex Console::console_mutex_;

inline void Console::WriteToConsole(std::string_view message, bool is_error) {
  std::lock_guard<std::mutex> lock(console_mutex_);
  static constexpr size_t kMaxBufferSize = 4096;
#ifdef _WIN32
  HANDLE handle = is_error ? GetStdHandle(STD_ERROR_HANDLE)
                           : GetStdHandle(STD_OUTPUT_HANDLE);
  if (handle == INVALID_HANDLE_VALUE) return;
  DWORD written = 0;
  WriteConsoleA(handle, message.data(), static_cast<DWORD>(message.size()),
                &written, nullptr);
#elif defined(__APPLE__) || defined(__linux__)
  int fd = is_error ? STDERR_FILENO : STDOUT_FILENO;
  ::write(fd, message.data(), message.size());
#else
  FILE* stream = is_error ? stderr : stdout;
  fprintf(stream, "%s", message.data());
#endif
}

}  // namespace ws
