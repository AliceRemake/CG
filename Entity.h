/**
  ******************************************************************************
  * @file           : Entity.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-6
  ******************************************************************************
  */



#ifndef ENTITY_H
#define ENTITY_H

#include <Common.h>

using Color = glm::vec3;
using Vector = glm::vec3;
using Vertex = glm::vec3;
using Normal = glm::vec3;

struct Line
{
  Vertex vertices[2] = {};
  Color color        = {};
};

struct Triangle
{
  Vertex vertices[3] = {};
  Normal colors[3]   = {};
};

struct AABB
{
  Vertex vmin;
  Vertex vmax;

  static AABB From(const Line& line) NOEXCEPT
  {
    return AABB {
      .vmin = glm::min(line.vertices[0], line.vertices[1]),
      .vmax = glm::max(line.vertices[0], line.vertices[1]),
    };
  }

  static AABB From(const Triangle& triangle) NOEXCEPT
  {
    return AABB {
      .vmin = glm::min(glm::min(triangle.vertices[0], triangle.vertices[1]), triangle.vertices[2]),
      .vmax = glm::max(glm::max(triangle.vertices[0], triangle.vertices[1]), triangle.vertices[2]),
    };
  }

};

struct Model
{
  struct Index
  {
    uint32_t vertex;
    uint32_t normal;
  };  
  std::string name             = {};
  std::vector<Vertex> vertices = {};
  std::vector<Normal> normals  = {};
  std::vector<Index> indices   = {};
  glm::vec3 scale              = {};
  glm::vec3 rotate             = {};
  glm::vec3 translate          = {};
};

struct Light
{
  Vertex position = {};
  Color color     = {};
};

struct Scene
{
  std::list<Model> models = {};
  std::vector<Light> lights = {};
};

struct Camera
{
  Vertex position  = {};
  Vector direction = {};
  Vector up        = {};
  Vector right     = {};
  float yaw        = {};
  float pitch      = {};
  float fov        = {};
  float aspect     = {};
  float near       = {};
  float far        = {};
};

struct Canvas
{
  int offsetx                                        = {};
  int offsety                                        = {};
  int width                                          = {};
  int height                                         = {};
  uint32_t* pixels                                   = {};
  float** zbuffer                                    = {};
  SDL_Window* window                                 = {};
  SDL_Surface* surface                               = {};
  const SDL_PixelFormatDetails* pixel_format_details = {};
};

struct Setting
{
  bool show_normal    = {};
  bool show_wireframe = {};
  bool enable_cull    = {};
  bool enable_clip    = {};
};

#endif //ENTITY_H
