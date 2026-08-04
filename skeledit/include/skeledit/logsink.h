// Copyright 2026 aljabari

#ifndef SKELEDIT_LOGSINK_H_
#define SKELEDIT_LOGSINK_H_

#include <string>
#include <vector>

namespace skeleton {

// Severity of a log message, mirroring spdlog's levels.
enum class LogLevel {
  kDebug,
  kInfo,
  kWarn,
  kError,
  kCritical,
};

// A single buffered log message.
struct LogEntry {
  LogLevel level;
  std::string message;
};

// Interface for a log sink that buffers messages for display in the editor.
class LogSink {
 public:
  virtual ~LogSink() = default;

  virtual std::vector<LogEntry> Entries() const = 0;
  virtual void Clear() = 0;
};

}  // namespace skeleton

#endif  // SKELEDIT_LOGSINK_H_
