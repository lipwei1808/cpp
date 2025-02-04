#pragma once

#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>

static inline char* currentTime() {
    std::chrono::time_point now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    char* s = std::ctime(&tt);
    s[strlen(s) - 1] = '\0';
    return s;
}

#define NO_LOG 0x0
#define ERROR_LEVEL 0x01
#define INFO_LEVEL 0x02
#define TRACE_LEVEL 0x03
#define DEBUG_LEVEL 0x04

#ifndef LOG_LEVEL
#define LOG_LEVEL ERROR_LEVEL
#endif

#define PRINT(level, del, fmt, ...) fprintf(stderr, "[%s] %-9s: %s:%d (%s) " fmt del, currentTime(), level, __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__)

#if LOG_LEVEL >= DEBUG_LEVEL
#define LOG_DEBUG(fmt, ...) PRINT("LOG_DEBUG", "\n", fmt, ## __VA_ARGS__)
#define LOG_DEBUGF(fmt, ...) PRINT("LOG_DEBUG", "", fmt, ## __VA_ARGS__)
#else 
#define LOG_DEBUG(fmt, ...) 
#endif

#if LOG_LEVEL >= TRACE_LEVEL
#define LOG_TRACE(fmt, ...) PRINT("LOG_TRACE", "\n", fmt, ## __VA_ARGS__)
#define LOG_TRACEF(fmt, ...) PRINT("LOG_TRACE", "", fmt, ## __VA_ARGS__)
#else 
#define LOG_TRACE(fmt, ...) 
#endif

#if LOG_LEVEL >= INFO_LEVEL
#define LOG_INFO(fmt, ...) PRINT("LOG_INFO", "\n", fmt, ## __VA_ARGS__)
#define LOG_INFOF(fmt, ...) PRINT("LOG_INFO", "", fmt, ## __VA_ARGS__)
#else 
#define LOG_INFO(fmt, ...) 
#endif

#if LOG_LEVEL >= ERROR_LEVEL
#define LOG_ERROR(fmt, ...) PRINT("LOG_ERROR", "\n", fmt, ## __VA_ARGS__)
#define LOG_ERRORF(fmt, ...) PRINT("LOG_ERROR", "", fmt, ## __VA_ARGS__)
#else 
#define LOG_ERROR(fmt, ...) 
#endif
