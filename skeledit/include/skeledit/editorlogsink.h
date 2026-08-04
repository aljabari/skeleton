// Copyright 2026 aljabari

#ifndef SKELEDIT_EDITORLOGSINK_H_
#define SKELEDIT_EDITORLOGSINK_H_

#include <spdlog/details/null_mutex.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/base_sink.h>

#include <cstddef>
#include <mutex>
#include <string>
#include <vector>

#include "skeledit/logsink.h"

namespace skeleton {

// spdlog sink that buffers log messages for display in the editor. Thread-safe
// for use between logging and UI threads. The sink keeps only the newest
// kMaxEntries messages. Each entry is formatted with kLogPattern, so the editor
// log window shows exactly the same timestamp/level/message layout as the
// consoles.
class EditorLogSink : public LogSink,
                      public spdlog::sinks::base_sink<spdlog::details::null_mutex> {
 public:
  static constexpr size_t kMaxEntries = 500;

  EditorLogSink();

  std::vector<LogEntry> Entries() const override;
  void Clear() override;

 protected:
  void sink_it_(const spdlog::details::log_msg& message) override;
  void flush_() override;

 private:
  mutable std::mutex mutex_;
  std::vector<LogEntry> entries_;
  spdlog::pattern_formatter formatter_;
};

}  // namespace skeleton

#endif  // SKELEDIT_EDITORLOGSINK_H_
