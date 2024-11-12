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

// COMMENT: Basic Data Structures. All Mathematics. No Geometry Meaning.
using Color = glm::vec3;
using Vertex = glm::vec3;
using Vector = glm::vec3;
using Normal = glm::vec3;

// COMMENT: Geometry Primitives For Rendering.
struct Line
{
  uint32_t vertices[2] = {};
  Color color = {};
};

// struct Triangle
// {
//   uint32_t vertices[3] = {};
//   Color color = {};
// };

struct Polygon
{
  std::vector<uint32_t> vertices = {};
  Color color = {};
};

// COMMENT: Axis-Aligned-Bounding-Box.
struct AABB
{
  Vertex vmin;
  Vertex vmax;

  static AABB From(const std::vector<Vertex>& vertices, const Line& line) NOEXCEPT
  {
    return AABB {
      .vmin = glm::min(vertices[line.vertices[0]], vertices[line.vertices[1]]),
      .vmax = glm::max(vertices[line.vertices[0]], vertices[line.vertices[1]]),
    };
  }

  // static AABB From(const std::vector<Vertex>& vertices, const Triangle& triangle) NOEXCEPT
  // {
  //   return AABB {
  //     .vmin = glm::min(glm::min(vertices[triangle.vertices[0]], vertices[triangle.vertices[1]]), vertices[triangle.vertices[2]]),
  //     .vmax = glm::max(glm::max(vertices[triangle.vertices[0]], vertices[triangle.vertices[1]]), vertices[triangle.vertices[2]]),
  //   };
  // }

  static AABB From(const std::vector<Vertex>& vertices, const struct Polygon& polygon) NOEXCEPT
  {
    AABB aabb = { .vmin = glm::vec3(1E9), .vmax = glm::vec3(-1E9) };
    for (const auto& vertex : polygon.vertices)
    {
      aabb.vmin = glm::min(aabb.vmin, vertices[vertex]);  
      aabb.vmax = glm::max(aabb.vmax, vertices[vertex]);  
    }
    return aabb;
  }

};

// COMMENT: 3D Model.
struct Model
{
  struct Index
  {
    uint32_t vertex  = (uint32_t)-1;
    uint32_t texture = (uint32_t)-1;
    uint32_t normal  = (uint32_t)-1;
  };

  std::string name = {};
  
  // COMMENT: Raw Data Read From .obj File.
  std::vector<Vertex> vertices = {};
  std::vector<Vertex> textures = {};
  std::vector<Normal> normals  = {};
  std::vector<Index> indices   = {};

  // COMMENT: Number Of Vertices In A Polygon.
  std::vector<uint32_t> polygons = {};
  
  // COMMENT: Transformations.
  glm::vec3 scale     = {};
  glm::vec3 rotate    = {};
  glm::vec3 translate = {};
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

// COMMENT: Classic FPS Style Camera.
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

struct HZBufferNode;

// COMMENT: Canvas For Rendering. (aka Viewport In Hardware).
struct Canvas
{
  int offsetx                                        = {};
  int offsety                                        = {};
  int width                                          = {};
  int height                                         = {};
  Color color                                        = {};
  uint32_t* pixels                                   = {};
  float z                                            = {};
  float** zbuffer                                    = {};
  HZBufferNode* h_zbuffer_tree                       = {};
  SDL_Window* window                                 = {};
  SDL_Surface* surface                               = {};
  const SDL_PixelFormatDetails* pixel_format_details = {};
};

// COMMENT: Setting Of The Application.
struct Setting
{
  enum Algorithm {
    ScanConvertZBuffer,
    IntervalScanLine,
  };
  
  bool show_normal    = {};
  bool show_wireframe = {};
  bool enable_cull    = {};
  bool enable_clip    = {};
  Algorithm algorithm = {};
};

#endif //ENTITY_H
