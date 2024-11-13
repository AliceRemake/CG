/**
  ******************************************************************************
  * @file           : Common.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-4
  ******************************************************************************
  */



// ReSharper disable CppUnusedIncludeDirective

#ifndef COMMON_H
#define COMMON_H

#include <fmt/format.h>
#include <fmt/printf.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <bitset>
#include <numeric>
#include <list>
#include <array>
#include <vector>
#include <string>
#include <queue>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <functional>

#define NODISCARD [[nodiscard]]
#define NOEXCEPT noexcept
#define CONSTEXPR constexpr
#ifdef NDEBUG
  #define FORCE_INLINE
#else
  #define FORCE_INLINE
#endif

#define __STR(s) #s
#define STR(s) __STR(s)

#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2

#ifndef LOG_LEVEL
  #define LOG_LEVEL LOG_LEVEL_INFO
#endif

#if LOG_LEVEL_INFO >= LOG_LEVEL
  #define Info(...)                                                                       \
    do {                                                                                  \
      fmt::fprintf(stderr, "\033[0m[%s:%d]\033[0;32m[INFO]\033[0m:", __FILE__, __LINE__); \
      fmt::fprintf(stderr, __VA_ARGS__); putchar('\n'); fflush(stderr);                   \
    } while (0)
#else
  #define Info(...)
#endif

#if LOG_LEVEL_WARN >= LOG_LEVEL
  #define Warn(...)                                                                       \
    do {                                                                                  \
      fmt::fprintf(stderr, "\033[0m[%s:%d]\033[1;33m[WARN]\033[0m:", __FILE__, __LINE__); \
      fmt::fprintf(stderr, __VA_ARGS__); putchar('\n'); fflush(stderr);                   \
    } while (0)
#else
  #define Warn(...)
#endif

#define Fatal(...)                                                                       \
  do {                                                                                   \
    fmt::fprintf(stderr, "\033[0m[%s:%d]\033[0;31m[ERROR]\033[0m:", __FILE__, __LINE__); \
    fmt::fprintf(stderr, __VA_ARGS__); putchar('\n'); fflush(stderr);                    \
    exit(EXIT_FAILURE);                                                                  \
  } while (0)

#ifdef NDEBUG
  #define DEBUGBREAK()
#else
  #define DEBUGBREAK() __debugbreak()
#endif

#ifdef NDEBUG
  #define ASSERT(exp) (void)(exp)
#else
  #define ASSERT(exp) do { if(!(exp)) DEBUGBREAK(); } while (0)
#endif

CONSTEXPR float INF = std::numeric_limits<float>::infinity();
CONSTEXPR float EPS = std::numeric_limits<float>::epsilon();

#endif //COMMON_H
