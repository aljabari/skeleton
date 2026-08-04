// Copyright 2026 aljabari

#include "skeledit/editorlogsink.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>

#include "skeledit/logsink.h"

namespace skeleton {
namespace {

TEST(EditorLogSinkTest, BuffersLogMessagesWithLevel) {
  auto sink = std::make_shared<EditorLogSink>();
  auto logger = std::make_shared<spdlog::logger>("test", sink);
  logger->set_level(spdlog::level::debug);

  logger->debug("debug message");
  logger->info("hello {}", 42);
  logger->warn("warning message");
  logger->error("error message");

  const std::vector<LogEntry> entries = sink->Entries();
  ASSERT_EQ(entries.size(), 4u);
  EXPECT_EQ(entries[0].level, LogLevel::kDebug);
  EXPECT_THAT(entries[0].message,
              ::testing::MatchesRegex("\\[\\d\\d\\d\\d-\\d\\d-\\d\\d "
                                      "\\d\\d:\\d\\d:\\d\\d"
                                      "\\.\\d\\d\\d\\] "
                                      "\\[debug\\] debug message"));
  EXPECT_EQ(entries[1].level, LogLevel::kInfo);
  EXPECT_THAT(entries[1].message, ::testing::HasSubstr("] [info] hello 42"));
  EXPECT_EQ(entries[2].level, LogLevel::kWarn);
  EXPECT_THAT(entries[2].message,
              ::testing::MatchesRegex("\\[\\d\\d\\d\\d-\\d\\d-\\d\\d "
                                      "\\d\\d:\\d\\d:\\d\\d"
                                      "\\.\\d\\d\\d\\] "
                                      "\\[warning\\] warning message"));
  EXPECT_EQ(entries[3].level, LogLevel::kError);
  EXPECT_THAT(entries[3].message,
              ::testing::MatchesRegex("\\[\\d\\d\\d\\d-\\d\\d-\\d\\d "
                                      "\\d\\d:\\d\\d:\\d\\d"
                                      "\\.\\d\\d\\d\\] "
                                      "\\[error\\] error message"));
}

TEST(EditorLogSinkTest, ClearEmptiesEntries) {
  auto sink = std::make_shared<EditorLogSink>();
  auto logger = std::make_shared<spdlog::logger>("test", sink);

  logger->info("hello");

  sink->Clear();
  EXPECT_TRUE(sink->Entries().empty());
}

TEST(EditorLogSinkTest, BoundsNumberOfBufferedEntries) {
  auto sink = std::make_shared<EditorLogSink>();
  auto logger = std::make_shared<spdlog::logger>("test", sink);

  for (int i = 0; i < static_cast<int>(EditorLogSink::kMaxEntries) + 10; ++i) {
    logger->info("message {}", i);
  }

  const std::vector<LogEntry> entries = sink->Entries();
  ASSERT_EQ(entries.size(), EditorLogSink::kMaxEntries);
  EXPECT_THAT(entries.front().message,
              ::testing::HasSubstr("] [info] message 10"));
  EXPECT_THAT(entries.back().message,
              ::testing::HasSubstr("] [info] message " +
                                   std::to_string(EditorLogSink::kMaxEntries +
                                                  9)));
}

}  // namespace
}  // namespace skeleton
