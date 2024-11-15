CMAKE_MINIMUM_REQUIRED(VERSION 3.29)

PROJECT(SoftwareRenderer
  VERSION 1.0.0
  DESCRIPTION "CG Homework 2024 Fall"
  HOMEPAGE_URL "https://github.com/AliceRemake/SoftwareRenderer"
  LANGUAGES C CXX
)

SET(CMAKE_CXX_STANDARD 20 REQUIRED)

ADD_SUBDIRECTORY(External)

ADD_EXECUTABLE(SoftwareRenderer)

IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
  MESSAGE(FATAL_ERROR "Unsupported Compiler: MSVC")
ENDIF()

IF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  TARGET_COMPILE_DEFINITIONS(SoftwareRenderer PUBLIC
    PROJECT_DIR=${CMAKE_SOURCE_DIR}
  )
ELSE()
  TARGET_COMPILE_DEFINITIONS(SoftwareRenderer PUBLIC
    PROJECT_DIR=${CMAKE_SOURCE_DIR}
    NDEBUG
  )
ENDIF()

IF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  TARGET_COMPILE_OPTIONS(SoftwareRenderer PUBLIC
    "-fno-exceptions" "-fno-rtti" "-Wall" "-Wextra" "-Werror"
  )
ELSE()
  TARGET_COMPILE_OPTIONS(SoftwareRenderer PUBLIC
    "-O3" "-fno-exceptions" "-fno-rtti" "-w"
  )
ENDIF()

TARGET_INCLUDE_DIRECTORIES(SoftwareRenderer PUBLIC
  ${CMAKE_SOURCE_DIR}
)

TARGET_SOURCES(SoftwareRenderer PUBLIC
  main.cpp
  Actor.h
  Controller.h
  Acceleration/HAABB.cpp
  Acceleration/HAABB.h
  Acceleration/HZBuffer.cpp
  Acceleration/HZBuffer.h
  Pipeline.cpp
  Pipeline.h
  Rasterizer.cpp
  Rasterizer.h
  Shader.cpp
  Shader.h
  Transformer.cpp
  Transformer.h
  Loader.cpp
  Loader.h
  Entity.cpp
  Entity.h
  Common.h
)

TARGET_LINK_LIBRARIES(SoftwareRenderer PUBLIC
  imgui
  SDL3-static
  glm
  fmt
)

TARGET_LINK_OPTIONS(SoftwareRenderer PUBLIC
  "-mwindows"
)
