// Copyright 2026 aljabari

#ifndef LIBSKELETON_LOGGING_H_
#define LIBSKELETON_LOGGING_H_

namespace skeleton {

// Canonical spdlog pattern shared by every console and the editor log window,
// so all log output uses the same format: timestamp, level, message. The
// console variant wraps the level in %^...%$ colour markers, which terminal
// sinks render as ANSI colour codes; the text itself is identical.
constexpr char kLogPattern[] = "[%Y-%m-%d %H:%M:%S.%e] [%l] %v";
constexpr char kLogPatternConsole[] = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v";

}  // namespace skeleton

#endif  // LIBSKELETON_LOGGING_H_
