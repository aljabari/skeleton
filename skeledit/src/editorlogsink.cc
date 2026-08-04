// Copyright 2026 aljabari

#include "skeledit/editorlogsink.h"

#include <spdlog/spdlog.h>

#include <string>
#include <vector>

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

std::vector<LogEntry> EditorLogSink::Entries() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return entries_;
}

void EditorLogSink::Clear() {
  std::lock_guard<std::mutex> lock(mutex_);
  entries_.clear();
}

void EditorLogSink::sink_it_(const spdlog::details::log_msg& message) {
  const std::string payload(message.payload.data(), message.payload.size());
  const spdlog::string_view_t level_view =
      spdlog::level::to_string_view(message.level);
  const std::string level(level_view.data(), level_view.size());

  std::lock_guard<std::mutex> lock(mutex_);
  entries_.push_back(
      LogEntry{ToLogLevel(message.level), "[" + level + "] " + payload});
  if (entries_.size() > kMaxEntries) {
    entries_.erase(entries_.begin());
  }
}

void EditorLogSink::flush_() {}

}  // namespace skeleton
