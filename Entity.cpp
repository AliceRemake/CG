/**
  ******************************************************************************
  * @file           : Entity.cpp
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-13
  ******************************************************************************
  */



#include <Entity.h>
#include <Loader.h>

NODISCARD  Vertex Polygon::Center(const std::vector<Vertex>& vertices, const Polygon& polygon) NOEXCEPT
{
  return std::accumulate(polygon.vertices.begin(), polygon.vertices.end(), Vertex(0.0f), [&](const Vertex& acc, const uint32_t vertex) -> Vertex {
    return acc + vertices[vertex];
  }) / (float)polygon.vertices.size();
}

NODISCARD  Normal Polygon::Normal(const std::vector<Vertex>& vertices, const Polygon& polygon) NOEXCEPT
{
  if (polygon.vertices.size() < 3)
  {
    return ::Normal(0.0f);  
  }
  Vector v0 = vertices[polygon.vertices[0]] - vertices[polygon.vertices[1]];
  Vector v1 = vertices[polygon.vertices[1]] - vertices[polygon.vertices[2]];
  return glm::normalize(glm::cross(v0, v1));
}

NODISCARD  Vertex AABB::Center(const AABB& aabb) NOEXCEPT
{
  return (aabb.vmin + aabb.vmax) / 2.0f; 
}

NODISCARD  Vector AABB::Radius(const AABB& aabb) NOEXCEPT
{
  return (aabb.vmax - aabb.vmin) / 2.0f;
}

NODISCARD  AABB AABB::From(const std::vector<Vertex>& vertices, const Polygon& polygon) NOEXCEPT
{
  AABB aabb = {
    .vmin = glm::vec3(INF), .vmax = glm::vec3(-INF),
  };
  for (const auto& vertex : polygon.vertices)
  {
    aabb.vmin = glm::min(aabb.vmin, vertices[vertex]);  
    aabb.vmax = glm::max(aabb.vmax, vertices[vertex]);
  }
  return aabb;
}

NODISCARD  bool AABB::OverLap(const AABB& lhs, const AABB& rhs) NOEXCEPT
{
  return (lhs.vmin.x <= rhs.vmax.x && rhs.vmin.x <= lhs.vmax.x)
      && (lhs.vmin.y <= rhs.vmax.y && rhs.vmin.y <= lhs.vmax.y)
      && (lhs.vmin.z <= rhs.vmax.z && rhs.vmin.z <= lhs.vmax.z);
}

NODISCARD  Model Model::FromObj(const char* filename) NOEXCEPT
{
  Model model;
  if (Loader::LoadObj(filename, model) != Loader::SUCCESS)
  {
    Fatal("Can Not Create Model From Obj File: %s", filename);  
  }
  return model;
}

NODISCARD  FrameBuffer FrameBuffer::From(SDL_Window* window, const Color& bgc) NOEXCEPT
{
  FrameBuffer frame_buffer;
  frame_buffer.window = window;
  frame_buffer.surface = SDL_GetWindowSurface(window);
  frame_buffer.format = SDL_GetPixelFormatDetails(frame_buffer.surface->format);
  frame_buffer.width = frame_buffer.surface->w;
  frame_buffer.height = frame_buffer.surface->h;
  frame_buffer.bgc = bgc;
  frame_buffer.buffer = (Uint32*)frame_buffer.surface->pixels;
  return frame_buffer;
}

 void FrameBuffer::Display(const FrameBuffer& frame_buffer) NOEXCEPT
{
  SDL_UpdateWindowSurface(frame_buffer.window);
}

 void FrameBuffer::Clear(const FrameBuffer& frame_buffer) NOEXCEPT
{
  SDL_ClearSurface(frame_buffer.surface, frame_buffer.bgc.r, frame_buffer.bgc.g, frame_buffer.bgc.b, 0.0f);
}

NODISCARD  ZBuffer ZBuffer::From(const FrameBuffer& frame_buffer, const float bgz) NOEXCEPT
{
  ZBuffer z_buffer;
  z_buffer.width = frame_buffer.width;
  z_buffer.height = frame_buffer.height;
  z_buffer.bgz = bgz;
  z_buffer.buffer = new float*[z_buffer.height];
  for (int i = 0; i < z_buffer.height; ++i)
  {
    z_buffer.buffer[i] = new float[z_buffer.width];
  }
  return z_buffer;
}

 void ZBuffer::Clear(const ZBuffer& z_buffer) NOEXCEPT
{
  for (int i = 0; i < z_buffer.height; ++i)
  {
    for (int j = 0; j < z_buffer.width; ++j)
    {
      z_buffer.buffer[i][j] = z_buffer.bgz;
    }
  }
}

NODISCARD  Canvas Canvas::From(FrameBuffer& frame_buffer, ZBuffer& z_buffer, const int offsetx, const int offsety, const int width, const int height) NOEXCEPT
{
  ASSERT(0 <= offsetx && 0 <= width && offsetx + width < frame_buffer.width);
  ASSERT(0 <= offsety && 0 <= height && offsety + height < frame_buffer.height);
  Canvas canvas;
  canvas.frame_buffer = &frame_buffer;
  canvas.z_buffer = &z_buffer;
  canvas.offsetx = offsetx;
  canvas.offsety = offsety;
  canvas.width = width;
  canvas.height = height;
  return canvas;
}
