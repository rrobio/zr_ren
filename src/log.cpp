#include "log.hpp"

namespace ren {
static Log *_log;

Log &Log::the() { return *_log; }

void Log::init() {
  if (_log == nullptr)
    _log = new Log();
}

void Log::destroy() {
  if (_log != nullptr)
    delete _log;
}

} // namespace ren
