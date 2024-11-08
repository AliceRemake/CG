/**
  ******************************************************************************
  * @file           : Primitive.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-6
  ******************************************************************************
  */



#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <Common.h>

using Color = glm::vec3;

using Vector = glm::vec3;
using Vertex = glm::vec3;
using Normal = glm::vec3;

struct Point {
  Vertex vertex = {};
};
struct Line {
  Vertex vertices[2] = {};
};
struct Triangle {
  Vertex vertices[3] = {};
  Vertex center      = {};
  Normal normal      = {};
};

struct AABB {
  Vertex vmin;
  Vertex vmax;

  // FORCE_INLINE 
  inline static AABB From(const Line& line) NOEXCEPT FORCE_INLINE {
    return AABB {
      .vmin = glm::min(line.vertices[0], line.vertices[1]),
      .vmax = glm::max(line.vertices[0], line.vertices[1]),
    };
  }

  // FORCE_INLINE 
  inline static AABB From(const Triangle& triangle) NOEXCEPT FORCE_INLINE {
    return AABB {
      .vmin = glm::min(glm::min(triangle.vertices[0], triangle.vertices[1]), triangle.vertices[2]),
      .vmax = glm::max(glm::max(triangle.vertices[0], triangle.vertices[1]), triangle.vertices[2]),
    };
  }
};

struct Mesh {
  struct Index {
    uint32_t vertex;
    uint32_t normal;
  };  
  std::vector<Index> indices = {};
};

struct Model {
  std::string name             = {};
  std::vector<Vertex> vertices = {};
  std::vector<Normal> normals  = {};
  std::vector<Mesh> meshes     = {};
  glm::vec3 scale              = {};
  glm::vec3 rotate             = {};
  glm::vec3 translate          = {};
};

struct Light {
  Vertex position = {};
  Color color     = {};
};

struct Scene {
  std::vector<Model> models = {};
  std::vector<Light> lights = {};
};

struct Camera {
  Vertex position  = {};
  Vector direction = {};
  float yaw        = {};
  float pitch      = {};
  float fov        = {};
  float aspect     = {};
  float near       = {};
  float far        = {};
};

struct Canvas {
  int offsetx                                        = {};
  int offsety                                        = {};
  int width                                          = {};
  int height                                         = {};
  uint32_t* pixels                                   = {};
  float** zbuffer                                    = {};
  SDL_Window* window                                 = {};
  const SDL_Surface* surface                         = {};
  const SDL_PixelFormatDetails* pixel_format_details = {};
};

struct Setting {
  enum PolygonMode {
    POLYGON_MODE_LINE,
    POLYGON_MODE_FILL,
  };
  enum ClipMode {
    CLIP_MODE_NONE,
    CLIP_MODE_VIEW,
  };
  enum CullMode {
    CULL_MODE_NONE,
    CULL_MODE_BACK,
  };
  enum ShadingModel {
    SHADING_MODEL_BLINN_PHONG,
  };

  bool show_normal           = {};
  CullMode cull_mode         = {};
  ClipMode clip_mode         = {};
  PolygonMode polygon_mode   = {};
  ShadingModel shading_model = {};
};

#endif //PRIMITIVE_H
