#include "log.hpp"

#include <mutex>

namespace ren {
static Log *_log;
static std::mutex log_mutex;

Log &Log::the() { return *_log; }

void Log::init() {
  if (_log == nullptr)
    _log = new Log();
}

void Log::destroy() {
  if (_log != nullptr)
    delete _log;
}

void Log::add_log(const char *fmt, ...) {
  std::lock_guard<std::mutex> const lock(log_mutex);
  int old_size = Buf.size();
  va_list args;
  va_start(args, fmt);
  Buf.appendfv(fmt, args);
  va_end(args);
  for (int new_size = Buf.size(); old_size < new_size; old_size++)
    if (Buf[old_size] == '\n')
      LineOffsets.push_back(old_size + 1);
}
} // namespace ren
