#pragma once

#include <cstdio>

#define NO_LOG 0x0
#define ERROR_LEVEL 0x01
#define INFO_LEVEL 0x02
#define TRACE_LEVEL 0x03
#define DEBUG_LEVEL 0x04

#ifndef LOG_LEVEL
#define LOG_LEVEL ERROR_LEVEL
#endif

#define PRINT(level, fmt, ...) fprintf(stderr, "%-9s: %s:%d (%s) " fmt "\n", level, __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__)

#if LOG_LEVEL >= DEBUG_LEVEL
#define LOG_DEBUG(fmt, ...) PRINT("LOG_DEBUG", fmt, ## __VA_ARGS__)
#else 
#define LOG_DEBUG(fmt, ...) 
#endif

#if LOG_LEVEL >= TRACE_LEVEL
#define LOG_TRACE(fmt, ...) PRINT("LOG_TRACE", fmt, ## __VA_ARGS__)
#else 
#define LOG_TRACE(fmt, ...) 
#endif

#if LOG_LEVEL >= INFO_LEVEL
#define LOG_INFO(fmt, ...) PRINT("LOG_INFO", fmt, ## __VA_ARGS__)
#else 
#define LOG_INFO(fmt, ...) 
#endif

#if LOG_LEVEL >= ERROR_LEVEL
#define LOG_ERROR(fmt, ...) PRINT("LOG_ERROR", fmt, ## __VA_ARGS__)
#else 
#define LOG_ERROR(fmt, ...) 
#endif
