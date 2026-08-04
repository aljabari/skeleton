// Copyright 2026 aljabari

#include "skeledit/editorlogsink.h"

#include <spdlog/spdlog.h>

#include <string>
#include <utility>
#include <vector>

#include "libskeleton/logging.h"

namespace skeleton {

namespace {

LogLevel ToLogLevel(spdlog::level::level_enum level) {
  switch (level) {
    case spdlog::level::debug:
      return LogLevel::kDebug;
    case spdlog::level::info:
      return LogLevel::kInfo;
    case spdlog::level::warn:
      return LogLevel::kWarn;
    case spdlog::level::err:
      return LogLevel::kError;
    case spdlog::level::critical:
      return LogLevel::kCritical;
    default:
      return LogLevel::kInfo;
  }
}

}  // namespace

EditorLogSink::EditorLogSink() : formatter_(kLogPattern) {}

std::vector<LogEntry> EditorLogSink::Entries() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return entries_;
}

void EditorLogSink::Clear() {
  std::lock_guard<std::mutex> lock(mutex_);
  entries_.clear();
}

void EditorLogSink::sink_it_(const spdlog::details::log_msg& message) {
  spdlog::memory_buf_t formatted;
  formatter_.format(message, formatted);

  std::string text(formatted.data(), formatted.size());
  // The formatter appends the platform EOL; the editor renders each entry on
  // its own line, so the trailing newline is dropped.
  while (!text.empty() && (text.back() == '\n' || text.back() == '\r')) {
    text.pop_back();
  }

  std::lock_guard<std::mutex> lock(mutex_);
  entries_.push_back(LogEntry{ToLogLevel(message.level), std::move(text)});
  if (entries_.size() > kMaxEntries) {
    entries_.erase(entries_.begin());
  }
}

void EditorLogSink::flush_() {}

}  // namespace skeleton
