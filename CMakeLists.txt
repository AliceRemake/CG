CMAKE_MINIMUM_REQUIRED(VERSION 3.29)

PROJECT(CGHomework)

SET(CMAKE_CXX_STANDARD 23)

ADD_SUBDIRECTORY(External)

ADD_EXECUTABLE(CGHomework)

TARGET_COMPILE_DEFINITIONS(CGHomework
PUBLIC
  PROJECT_DIR=${CMAKE_SOURCE_DIR}
)

TARGET_COMPILE_OPTIONS(CGHomework
PUBLIC
  "-fno-exceptions" "-fno-rtti" "-Wall" "-Wextra" "-Werror"
)

TARGET_INCLUDE_DIRECTORIES(CGHomework
PUBLIC
  ${CMAKE_SOURCE_DIR}
)

TARGET_SOURCES(CGHomework
PUBLIC
  main.cpp
  Accelerator.h
  Controller.h
  Actor.h
  Pipeline.h
  Shader.h
  Rasterizer.h
  Transformer.h
  Initializer.h
  Loader.h
  Debugger.h
  Entity.h
  Common.h
)

TARGET_LINK_LIBRARIES(CGHomework
PUBLIC
  glm
  imgui
  SDL3-static
  fmt
)
