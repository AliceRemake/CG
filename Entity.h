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
using Vertex = glm::vec3;
using Vector = glm::vec3;
using Normal = glm::vec3;

struct Polygon;

struct AABB
{
  glm::vec3 vmin   = {};
  glm::vec3 vmax   = {};
  AABB* l          = {};
  AABB* r          = {};
  Polygon* polygon = {};
  
  NODISCARD FORCE_INLINE static Vertex Center(const AABB& aabb) NOEXCEPT;

  NODISCARD FORCE_INLINE static Vector Radius(const AABB& aabb) NOEXCEPT;

  NODISCARD FORCE_INLINE static AABB From(const std::vector<Vertex>& vertices, Polygon& polygon) NOEXCEPT;

  NODISCARD FORCE_INLINE static bool OverLap(const AABB& lhs, const AABB& rhs) NOEXCEPT;
};

struct Polygon
{
  std::vector<uint32_t> vertices = {};
  Color color = {};
  AABB aabb   = {};

  NODISCARD FORCE_INLINE static Vertex Center(const std::vector<Vertex>& vertices, const Polygon& polygon) NOEXCEPT;

  NODISCARD FORCE_INLINE static Normal Normal(const std::vector<Vertex>& vertices, const Polygon& polygon) NOEXCEPT;
};

struct Model
{
  struct Index
  {
    uint32_t vertex   = (uint32_t)-1;
  };

  std::string name = {};
  
  std::vector<Vertex> vertices  = {};
  std::vector<Index> indices    = {};

  std::vector<uint32_t> polygon_sides = {};
  
  glm::vec3 scale     = {};
  glm::vec3 rotate    = {};
  glm::vec3 translate = {};

  NODISCARD FORCE_INLINE static Model FromObj(const char* filename) NOEXCEPT;
};

struct ParallelLight
{
  Vector direction = {};
  Color color      = {};  
};

struct PointLight
{
  Vertex position = {};
  Color color     = {};
};

struct Scene
{
  std::list<Model> models                  = {};
  std::list<ParallelLight> parallel_lights = {};
  std::list<PointLight> point_lights       = {};
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

struct FrameBuffer
{
  SDL_Window* window                   = {};
  SDL_Surface* surface                 = {};
  const SDL_PixelFormatDetails* format = {};

  int width      = {};
  int height     = {};
  Color bgc      = {};
  Uint32* buffer = {};

  NODISCARD FORCE_INLINE static FrameBuffer From(SDL_Window* window, const Color& bgc) NOEXCEPT;

  FORCE_INLINE static void Display(const FrameBuffer& frame_buffer) NOEXCEPT;

  FORCE_INLINE static void Clear(const FrameBuffer& frame_buffer) NOEXCEPT;
};

struct ZBuffer
{
  int width      = {};
  int height     = {};
  float bgz      = {};
  float** buffer = {};

  NODISCARD FORCE_INLINE static ZBuffer From(const FrameBuffer& frame_buffer, float bgz) NOEXCEPT;

  FORCE_INLINE static void Clear(const ZBuffer& z_buffer) NOEXCEPT;
};

struct HZBuffer;

struct Canvas
{
  int offsetx               = {};
  int offsety               = {};
  int width                 = {};
  int height                = {};
  FrameBuffer* frame_buffer = {};
  ZBuffer* z_buffer         = {};
  HZBuffer* h_z_buffer      = {};

  NODISCARD FORCE_INLINE static Canvas From(FrameBuffer& frame_buffer, ZBuffer& z_buffer, int offsetx, int offsety, int width, int height) NOEXCEPT;
};

struct Setting
{
  enum Algorithm {
    ScanConvertZBuffer,
    ScanConvertHZBuffer,
    IntervalScanLine,
  };
  
  bool show_normal    = {};
  bool show_wireframe = {};
  bool enable_cull    = {};
  bool enable_clip    = {};
  Algorithm algorithm = {};
};

#endif //ENTITY_H
