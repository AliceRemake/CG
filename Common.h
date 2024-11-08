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
#include <SDL3/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <bitset>

#undef near
#undef far

#define NODISCARD [[nodiscard]]
#define NOEXCEPT noexcept
#define CONSTEXPR constexpr

#ifdef NDEBUG
#define FORCE_INLINE __attribute__((always_inlnie))
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
      fmt::fprintf(stderr, __VA_ARGS__);                                                  \
    } while (0)
#else
  #define Info(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL_WARN >= LOG_LEVEL
  #define Warn(...)                                                                       \
    do {                                                                                  \
      fmt::fprintf(stderr, "\033[0m[%s:%d]\033[1;33m[WARN]\033[0m:", __FILE__, __LINE__); \
      fmt::fprintf(stderr, __VA_ARGS__);                                                  \
    } while (0)
#else
  #define Warn(fmt, ...) ((void)0)
#endif

#define Fatal(...)                                                                       \
  do {                                                                                   \
    fmt::fprintf(stderr, "\033[0m[%s:%d]\033[0;31m[ERROR]\033[0m:", __FILE__, __LINE__); \
    fmt::fprintf(stderr, __VA_ARGS__);                                                   \
    exit(1);                                                                             \
  } while (0)

#ifdef NDEBUG
  #define ASSERT(exp) do { ((void)exp); } while (0)
#else
  #define ASSERT(exp) do { assert(exp); } while (0)
#endif

#endif //COMMON_H
