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
  
  NODISCARD  static Vertex Center(const AABB& aabb) NOEXCEPT;

  NODISCARD  static Vector Radius(const AABB& aabb) NOEXCEPT;

  NODISCARD  static AABB From(const std::vector<Vertex>& vertices, const Polygon& polygon) NOEXCEPT;

  NODISCARD  static bool OverLap(const AABB& lhs, const AABB& rhs) NOEXCEPT;
};

struct Polygon
{
  std::vector<uint32_t> vertices = {};
  Color color = {};

  NODISCARD  static Vertex Center(const std::vector<Vertex>& vertices, const Polygon& polygon) NOEXCEPT;

  NODISCARD  static Normal Normal(const std::vector<Vertex>& vertices, const Polygon& polygon) NOEXCEPT;
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

  NODISCARD  static Model FromObj(const char* filename) NOEXCEPT;
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

  NODISCARD  static FrameBuffer From(SDL_Window* window, const Color& bgc) NOEXCEPT;

   static void Display(const FrameBuffer& frame_buffer) NOEXCEPT;

   static void Clear(const FrameBuffer& frame_buffer) NOEXCEPT;
};

struct ZBuffer
{
  int width      = {};
  int height     = {};
  float bgz      = {};
  float** buffer = {};

  NODISCARD  static ZBuffer From(const FrameBuffer& frame_buffer, float bgz) NOEXCEPT;

   static void Clear(const ZBuffer& z_buffer) NOEXCEPT;
};

struct Canvas;

struct ZBH
{
    bool valid = false;
    float zmax = -INF;
    int xmin   = -1;
    int xmax   = -1;
    int ymin   = -1;
    int ymax   = -1;

    FORCE_INLINE NODISCARD static int Log2(const int x) NOEXCEPT
    {
        return ((int)sizeof(uint32_t) << 3) - std::countl_zero((uint32_t)x - 1);
    }

    FORCE_INLINE NODISCARD static int Pow4(const int x) NOEXCEPT
    {
        return (1 << (x << 1));
    }
    
    FORCE_INLINE NODISCARD static int Father(const int cur) NOEXCEPT
    {
        ASSERT(cur != 0);
        return (cur - 1) >> 2;
    }
    
    FORCE_INLINE NODISCARD static int Child0(const int cur) NOEXCEPT
    {
        return (cur << 2) | 1;
    }
    
    FORCE_INLINE NODISCARD static int Child1(const int cur) NOEXCEPT
    {
        return (cur << 2) | 2;
    }
    
    FORCE_INLINE NODISCARD static int Child2(const int cur) NOEXCEPT
    {
        return (cur << 2) | 3;
    }
    
    FORCE_INLINE NODISCARD static int Child3(const int cur) NOEXCEPT
    {
        return (cur << 2) + 4;
    }

    FORCE_INLINE NODISCARD static int Child(const int cur, const int i) NOEXCEPT
    {
        return (cur << 2) + 1 + i;
    }
    
    NODISCARD static std::vector<ZBH> From(const Canvas& canvas) NOEXCEPT;
    
    // ReSharper disable once CppParameterMayBeConstPtrOrRef
    static void Clear(std::vector<ZBH>& zbh_tree, Canvas& canvas) NOEXCEPT;
    
    NODISCARD static float Query(const std::vector<ZBH>& zbh_tree, int xmin, int xmax, int ymin, int ymax) NOEXCEPT;
    
    static void Update(std::vector<ZBH>& zbh_tree, Canvas& canvas, int xmin, int xmax, int ymin, int ymax) NOEXCEPT;
};

struct Canvas
{
  int offsetx               = {};
  int offsety               = {};
  int width                 = {};
  int height                = {};
  FrameBuffer* frame_buffer = {};
  ZBuffer* z_buffer         = {};
  // HZBuffer* h_z_buffer      = {};
  std::vector<ZBH> zbh_tree = {}; 
    
  NODISCARD  static Canvas From(FrameBuffer& frame_buffer, ZBuffer& z_buffer, int offsetx, int offsety, int width, int height) NOEXCEPT;
};

struct Setting
{
  enum Algorithm
  {
    ScanConvertZBuffer,
    ScanConvertHZBuffer,
    ScanConvertHAABBHZBuffer,
    IntervalScanLine,
  };
  enum DisplayMode
  {
    NORMAL,
    WIREFRAME,
  };
  
  bool show_aabb           = {};
  bool show_normal         = {};
  bool show_z_buffer       = {};
  bool enable_cull         = {};
  bool enable_clip         = {};
  Algorithm algorithm      = {};
  DisplayMode display_mode = {};
};

#endif //ENTITY_H
