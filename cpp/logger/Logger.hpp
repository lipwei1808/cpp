#pragma once

#define NO_LOG 0x0
#define ERROR_LEVEL 0x01
#define INFO_LEVEL 0x02
#define TRACE_LEVEL 0x03

#ifndef LOG_LEVEL
#define LOG_LEVEL ERROR_LEVEL
#endif

#if LOG_LEVEL >= TRACE_LEVEL
#define LOG_TRACE(fmt, ...) fprintf(stderr, "LOG_TRACE: " fmt "\n", ## __VA_ARGS__)
#else 
#define LOG_TRACE(fmt, ...) 
#endif

#if LOG_LEVEL >= ERROR_LEVEL
#define LOG_ERROR(fmt, ...) fprintf(stderr, "LOG_ERROR: " fmt "\n", ## __VA_ARGS__)
#else 
#define LOG_ERROR(fmt, ...) 
#endif

#if LOG_LEVEL >= INFO_LEVEL
#define LOG_INFO(fmt, ...) fprintf(stderr, "LOG_INFO: " fmt "\n", ## __VA_ARGS__)
#else 
#define LOG_INFO(fmt, ...) 
#endif

#if LOG_LEVEL >= TRACE_LEVEL
#define LOG_TRACE(fmt, ...) fprintf(stderr, "LOG_TRACE: " fmt "\n", ## __VA_ARGS__)
#else 
#define LOG_TRACE(fmt, ...) 
#endif
