//
// Created by frank on 20.10.22.
//

#ifndef PICOC64USB_DEBUG_H
#define PICOC64USB_DEBUG_H
#include <stdio.h>

// 1: error
// 2: warn
// 3: info
// 4: trace


#undef LOG_LOCATION
#define LOG_LEVEL 2

#ifdef LOG_LOCATION
#define LOG_PRINTF  printf("%s:%u: ", __PRETTY_FUNCTION__, __LINE__); printf
#else
#define LOG_PRINTF  printf
#endif

#if LOG_LEVEL >= 1
#define LOG_ERROR(...)   LOG_PRINTF(__VA_ARGS__); printf("\r\n")
#else
#define LOG_ERROR(...)
#endif
#if LOG_LEVEL >= 2
#define LOG_WARN(...)  LOG_PRINTF(__VA_ARGS__); printf("\r\n")
#else
#define LOG_WARN(...)
#endif
#if LOG_LEVEL >= 3
#define LOG_INFO(...)  LOG_PRINTF(__VA_ARGS__); printf("\r\n")
#else
#define LOG_INFO(...)
#endif
#if LOG_LEVEL >= 4
#define LOG_TRACE(...)  LOG_PRINTF(__VA_ARGS__); printf("\r\n")
#else
#define LOG_TRACE(...)
#endif


#endif //PICOC64USB_DEBUG_H
