/**
  ******************************************************************************
  * @file           : Rasterizer.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-8
  ******************************************************************************
  */



#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <Common.h>
#include <Entity.h>

#include "Acceleration/HZBuffer.h"

struct Rasterizer
{
  // COMMENT: Map A Color To The Format That The Frame Buffer Accepts.
  static Uint32 MapColor(const FrameBuffer& frame_buffer, const Color& color) NOEXCEPT
  {
    return SDL_MapRGB(
      frame_buffer.format, nullptr,
      (Uint8)(255.999f * std::clamp(color.r, 0.0f, 1.0f)),
      (Uint8)(255.999f * std::clamp(color.g, 0.0f, 1.0f)),
      (Uint8)(255.999f * std::clamp(color.b, 0.0f, 1.0f))
    );
  }

  // COMMENT: Render A Pixel At Row i, Col j Of The Frame Buffer.
  static void RenderPixel(const FrameBuffer& frame_buffer, const int x, const int y, const Uint32 color) NOEXCEPT
  {
    ASSERT(0 <= x && x < frame_buffer.width && 0 <= y && y < frame_buffer.height);
    frame_buffer.buffer[frame_buffer.width * y + x] = color;
  }
  static void RenderPixel(const FrameBuffer& frame_buffer, const int x, const int y, const Color& color) NOEXCEPT
  {
    RenderPixel(frame_buffer, x, y, MapColor(frame_buffer, color));
  }


  // COMMENT: Render A Segment At Row i, From Col jmin To Col jmax Of The Frame Buffer.
  static void RenderSegment(const FrameBuffer& frame_buffer, const int xmin, const int xmax, const int y, const Uint32 color) NOEXCEPT
  {
    ASSERT(0 <= xmin && xmin <= xmax && xmax < frame_buffer.width && 0 <= y && y < frame_buffer.height);
    for (int x = xmin; x <= xmax; ++x)
    {
      RenderPixel(frame_buffer, x, y, color);
    }
  }
  static void RenderSegment(const FrameBuffer& frame_buffer, const int xmin, const int xmax, const int y, const Color& color) NOEXCEPT
  {
    RenderSegment(frame_buffer, xmin, xmax, y, MapColor(frame_buffer, color));
  }
  
  // COMMENT: Render A Tangent From (x0, y0) To (x1, y1). Use Algorithm DDA.
  static void RenderTangentDDA(const Canvas& canvas, int x0, int y0, int x1, int y1, const Uint32& color) NOEXCEPT
  {
    x0 += canvas.offsetx, y0 += canvas.offsety, x1 += canvas.offsetx, y1 += canvas.offsety;

    int len = std::max(std::abs(x1 - x0), std::abs(y1 - y0));

    float dx = (float)(x1 - x0) / (float)len;
    float dy = (float)(y1 - y0) / (float)len;
    
    float x = x0, y = y0;

    for (int i = 0; i < len && 0 <= x && x < canvas.frame_buffer->width && 0 <= y && y < canvas.frame_buffer->height; ++i)
    {
      RenderPixel(*canvas.frame_buffer, (int)x, (int)y, color);
      x += dx, y += dy;
    }
  }
  static void RenderTangentDDA(const Canvas& canvas, const int x0, const int y0, const int x1, const int y1, const Color& color) NOEXCEPT
  {
    RenderTangentDDA(canvas, x0, y0, x1, y1, MapColor(*canvas.frame_buffer, color));
  }

  // COMMENT: Render A Tangent From (x0, y0) To (x1, y1). Use Algorithm Bresenham.
  static void RenderTangentBresenham(const Canvas& canvas, int x0, int y0, int x1, int y1, const Uint32& color) NOEXCEPT
  {
    x0 += canvas.offsetx, y0 += canvas.offsety, x1 += canvas.offsetx, y1 += canvas.offsety;

    int dx = x1 - x0;
    int dy = y1 - y0;

    if (dx == 0 && dy == 0)
    {
      if (0 <= x0 && x0 < canvas.frame_buffer->width && 0 <= y0 && y0 < canvas.frame_buffer->height)
      {
        RenderPixel(*canvas.frame_buffer, x0, y0, color);
      }
      return;
    }

    if (dy == 0)
    {
      if (0 <= y0 && y0 < canvas.frame_buffer->height)
      {
        if (x0 < x1)
        {
          for (int x = std::max(x0, 0); x <= std::min(x1, canvas.frame_buffer->width-1); ++x)
          {
            RenderPixel(*canvas.frame_buffer, x, y0, color);
          }
        }
        else
        {
          ASSERT(x0 > x1);
          for (int x = std::max(x1, 0); x <= std::min(x0, canvas.frame_buffer->width-1); ++x)
          {
            RenderPixel(*canvas.frame_buffer, x, y0, color);
          }
        }
      }
      return;
    }
    if (dx == 0)
    {
      if (0 <= x0 && x0 < canvas.frame_buffer->width)
      {
        if (y0 < y1)
        {
          for (int y = std::max(y0, 0); y < std::min(y1, canvas.frame_buffer->height-1); ++y)
          {
            RenderPixel(*canvas.frame_buffer, x0, y, color);
          }
        }
        else
        {
          ASSERT(y0 > y1);
          for (int y = std::max(y1, 0); y < std::min(y0, canvas.frame_buffer->height-1); ++y)
          {
            RenderPixel(*canvas.frame_buffer, x0, y, color);
          }
        }
      }
      return;
    }

    int x = x0;
    int y = y0;

    int sx = 0;
    int sy = 0;
    
    if (dx > 0)
    {
      sx = 1;
    }
    else
    {
      ASSERT(dx < 0);
      sx = -1;
      dx = -dx;
    }

    if (dy > 0)
    {
      sy = 1;
    }
    else
    {
      ASSERT(dy < 0);
      sy = -1;
      dy = -dy;
    }

    const int dy2 = dy << 1;
    const int dx2 = dx << 1;
    
    if (dx > dy)
    {
      int e = dy2 - dx;
      for (int i = 0; i <= dx && 0 <= x && x < canvas.frame_buffer->width && 0 <= y && y < canvas.frame_buffer->height; ++i)
      {
        RenderPixel(*canvas.frame_buffer, x, y, color);
        while(e > 0)
        {
          y += sy;
          e -= dx2;
        }        
        x += sx;
        e += dy2;
      }
    }
    else
    {
      int e = dx2 - dy;
      for (int i = 0; i <= dy && 0 <= x && x < canvas.frame_buffer->width && 0 <= y && y < canvas.frame_buffer->height; ++i)
      {
        RenderPixel(*canvas.frame_buffer, x, y, color);
        while(e > 0)
        {
          x += sx;
          e -= dy2;
        }        
        y += sy;
        e += dx2;
      }
    }
  }
  static void RenderTangentBresenham(const Canvas& canvas, const int x0, const int y0, const int x1, const int y1, const Color& color) NOEXCEPT
  {
    RenderTangentBresenham(canvas, x0, y0, x1, y1, MapColor(*canvas.frame_buffer, color));
  }

  static void RenderPolygonsWireframe(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons) NOEXCEPT
  {
    for (const auto& polygon : polygons)
    {
      if (polygon.vertices.size() == 1) { continue; }
      for (size_t i = 0; i < polygon.vertices.size(); ++i)
      {
        size_t j = (i + 1) % polygon.vertices.size();
        glm::vec3 v0 = vertices[polygon.vertices[i]];
        glm::vec3 v1 = vertices[polygon.vertices[j]];
        RenderTangentBresenham(canvas,
          (int)std::round(v0.x),
          (int)std::round(v0.y),
          (int)std::round(v1.x),
          (int)std::round(v1.y),
          polygon.color
        );
      }
    }
  }

  struct Edge
  {
    int ymin;
    int ymax;
    int x;
    int d;
    float m;
    float e;
    size_t pid;
  
    bool operator<(const Edge& oth) const NOEXCEPT
    {
      if (ymin != oth.ymin)
      {
        return ymin < oth.ymin;  
      }
      if (x != oth.x)
      {
        return x < oth.x;  
      }
      return d * m < oth.d * oth.m;
    }
  
    bool operator==(const Edge& oth) const NOEXCEPT
    {
      return ymin == oth.ymin && x == oth.x && d * m == oth.d * oth.m;
    }
  };
  
  static void RenderPolygonsScanConvertZBuffer(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons) NOEXCEPT
  {
    static std::vector<float> A; A.clear();
    static std::vector<float> B; B.clear();
    static std::vector<float> C; C.clear();

    A.resize(polygons.size());
    B.resize(polygons.size());
    C.resize(polygons.size());

    for (size_t pid = 0; pid < polygons.size(); ++pid)
    {
      if (polygons[pid].vertices.size() < 3) { continue; }
      glm::vec3 p0 = vertices[polygons[pid].vertices[0]];
      glm::vec3 p1 = vertices[polygons[pid].vertices[1]];
      glm::vec3 p2 = vertices[polygons[pid].vertices[2]];
      glm::vec3 n = glm::normalize(glm::cross(p0 - p1, p1 - p2));
      A[pid] = -n.x / n.z;
      B[pid] = -n.y / n.z;
      C[pid] = glm::dot(n, p0) / n.z;
    }

    static auto TestZ = [&](const uint32_t pid, const int x, const int y) NOEXCEPT -> float
    {
      if (pid == polygons.size()) { return canvas.z_buffer->bgz; }
      return A[pid] * x + B[pid] * y + C[pid];
    };

    static std::vector<Edge> ET; ET.clear();
    ET.reserve(std::accumulate(polygons.begin(), polygons.end(), 0, [](const size_t acc, const Polygon& polygon) { return std::max(acc, polygon.vertices.size()); }));

    for (size_t pid = 0; pid < polygons.size(); ++pid)
    {
      if (polygons[pid].vertices.size() < 3) { continue; }
     
      ET.clear();
     
      glm::ivec2 vmin = glm::ivec2(canvas.height-1, canvas.width-1);      
      glm::ivec2 vmax = glm::ivec2(0, 0); 

      for (size_t i = 0; i < polygons[pid].vertices.size(); ++i)
      {
        vmin = glm::min(vmin, glm::ivec2(glm::round(vertices[polygons[pid].vertices[i]])));
        vmax = glm::max(vmax, glm::ivec2(glm::round(vertices[polygons[pid].vertices[i]])));
       
        size_t j = (i + 1) % polygons[pid].vertices.size();

        glm::vec3 v0 = vertices[polygons[pid].vertices[i]];
        glm::vec3 v1 = vertices[polygons[pid].vertices[j]];

        if (std::round(v0.y) == std::round(v1.y))
        {
          continue;
        }
       
        if (v0.y > v1.y)
        {
          std::swap(v0, v1);
        }

        float dxdy = (std::round(v0.x) - std::round(v1.x)) / (std::round(v0.y) - std::round(v1.y));
       
        Edge edge {
          .ymin = (int)std::round(v0.y),
          .ymax = (int)std::round(v1.y),
          .x = (int)std::round(v0.x),
          .d = dxdy > 0.0f ? 1 : -1,
          .m = std::abs(dxdy),
          .e = std::abs(dxdy) - 0.5f,
          .pid = pid,
        };

        if (edge.ymax >= 0 && edge.ymin < canvas.height)
        {
          if (edge.ymin < 0)
          {
            edge.e += edge.m * (0 - edge.ymin);
            while(edge.e > 0.0f)
            {
              edge.x += edge.d;
              edge.e -= 1.0f;
            }
            edge.ymin = 0;
          }
 
          if(edge.ymax >= canvas.height)
          {
            edge.ymax = canvas.height-1;
          }
 
          ET.emplace_back(edge);
        }
      }

      vmin = glm::max(vmin, glm::ivec2(0, 0));
      vmax = glm::min(vmax, glm::ivec2(canvas.width-1, canvas.height-1));

      std::sort(ET.begin(), ET.end());

      std::list<Edge*> AET;
     
      for (int y = vmin.y, j = 0; y <= vmax.y; ++y)
      {
        while((size_t)j < ET.size() && ET[j].ymin < y)
        {
          ++j;
        }
        {
          auto it = AET.begin();
          while ((size_t)j < ET.size() && ET[j].ymin == y)
          {
            while(it != AET.end() && (*it)->x <= ET[j].x)
            {
              ++it;
            }
            while(it != AET.end() && (*it)->x == ET[j].x && (*it)->d * (*it)->m <= ET[j].d * ET[j].m)
            {
              ++it;
            }
            it = AET.emplace(it, &ET[j++]);
          }
        }

        for (auto it = AET.begin(); it != AET.end();)
        {
          if ((*it)->ymin == (*it)->ymax)
          {
            it = AET.erase(it);
          }
          else
          {
            ++it;
          }
        }

        for (auto it = AET.begin(); it != std::prev(AET.end()); ++it)
        {
          auto nxt = std::next(it);

          if ((*it)->x == (*nxt)->x)
          {
            continue;
          }

          for (int x = std::max(vmin.x, (*it)->x); x <= std::min(vmax.x, (*nxt)->x); ++x)
          {
            float curz = TestZ(pid, x, y);
            if (canvas.z_buffer->buffer[y][x] > curz)
            {
              canvas.z_buffer->buffer[y][x] = curz;
              RenderPixel(*canvas.frame_buffer, x, y, polygons[pid].color);
            }
          }
        }
     
        for (auto& edge : AET)
        {
          edge->ymin += 1;
          edge->e += edge->m;
          while(edge->e > 0.0f)
          {
            edge->x += edge->d;
            edge->e -= 1.0f;
          }
        }
      }
    }
  }

  static void RenderPolygonsScanConvertHZBuffer(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons) NOEXCEPT
  {
    static std::vector<float> A; A.clear();
    static std::vector<float> B; B.clear();
    static std::vector<float> C; C.clear();
     
    A.resize(polygons.size());
    B.resize(polygons.size());
    C.resize(polygons.size());
     
    for (size_t pid = 0; pid < polygons.size(); ++pid)
    {
      if (polygons[pid].vertices.size() < 3) { continue; }
      glm::vec3 p0 = vertices[polygons[pid].vertices[0]];
      glm::vec3 p1 = vertices[polygons[pid].vertices[1]];
      glm::vec3 p2 = vertices[polygons[pid].vertices[2]];
      glm::vec3 n = glm::normalize(glm::cross(p0 - p1, p1 - p2));
      A[pid] = -n.x / n.z;
      B[pid] = -n.y / n.z;
      C[pid] = glm::dot(n, p0) / n.z;
    }

    static auto TestZ = [&](const uint32_t pid, const int x, const int y) NOEXCEPT -> float
    {
      if (pid == polygons.size()) { return canvas.z_buffer->bgz; }
      return A[pid] * x + B[pid] * y + C[pid];
    };
     
    static std::vector<Edge> ET; ET.clear();
    ET.reserve(std::accumulate(polygons.begin(), polygons.end(), 0, [](const size_t acc, const Polygon& polygon) { return std::max(acc, polygon.vertices.size()); }));

    for (size_t pid = 0; pid < polygons.size(); ++pid)
    {
      if (polygons[pid].vertices.size() < 3) { continue; }
       
      ET.clear();
       
      glm::ivec2 vmin = glm::ivec2(canvas.height-1, canvas.width-1);      
      glm::ivec2 vmax = glm::ivec2(0, 0); 

      for (size_t i = 0; i < polygons[pid].vertices.size(); ++i)
      {
        vmin = glm::min(vmin, glm::ivec2(glm::round(vertices[polygons[pid].vertices[i]])));
        vmax = glm::max(vmax, glm::ivec2(glm::round(vertices[polygons[pid].vertices[i]])));
         
        size_t j = (i + 1) % polygons[pid].vertices.size();

        glm::vec3 v0 = vertices[polygons[pid].vertices[i]];
        glm::vec3 v1 = vertices[polygons[pid].vertices[j]];

        if (std::round(v0.y) == std::round(v1.y))
        {
          continue;
        }
         
        if (v0.y > v1.y)
        {
          std::swap(v0, v1);
        }

        float dxdy = (std::round(v0.x) - std::round(v1.x)) / (std::round(v0.y) - std::round(v1.y));
         
        Edge edge {
          .ymin = (int)std::round(v0.y),
          .ymax = (int)std::round(v1.y),
          .x = (int)std::round(v0.x),
          .d = dxdy > 0.0f ? 1 : -1,
          .m = std::abs(dxdy),
          .e = std::abs(dxdy) - 0.5f,
          .pid = pid,
        };

        if (edge.ymax >= 0 && edge.ymin < canvas.height)
        {
          if (edge.ymin < 0)
          {
            edge.e += edge.m * (0 - edge.ymin);
            while(edge.e > 0.0f)
            {
              edge.x += edge.d;
              edge.e -= 1.0f;
            }
            edge.ymin = 0;
          }

          if(edge.ymax >= canvas.height)
          {
            edge.ymax = canvas.height-1;
          }

          ET.emplace_back(edge);
        }
      }

      vmin = glm::max(vmin, glm::ivec2(0, 0));
      vmax = glm::min(vmax, glm::ivec2(canvas.width-1, canvas.height-1));
       
      if (HZBuffer::Query(canvas.h_z_buffer, vmin.x, vmax.x, vmin.y, vmax.y) <= polygons[pid].aabb.vmin.z)
      {
        return;
      }
       
      std::sort(ET.begin(), ET.end());

      std::list<Edge*> AET;
       
      for (int y = vmin.y, j = 0; y <= vmax.y; ++y)
      {
        while((size_t)j < ET.size() && ET[j].ymin < y)
        {
          ++j;
        }
        {
          auto it = AET.begin();
          while ((size_t)j < ET.size() && ET[j].ymin == y)
          {
            while(it != AET.end() && (*it)->x <= ET[j].x)
            {
              ++it;
            }
            while(it != AET.end() && (*it)->x == ET[j].x && (*it)->d * (*it)->m <= ET[j].d * ET[j].m)
            {
              ++it;
            }
            it = AET.emplace(it, &ET[j++]);
          }
        }

        for (auto it = AET.begin(); it != AET.end();)
        {
          if ((*it)->ymin == (*it)->ymax)
          {
            it = AET.erase(it);
          }
          else
          {
            ++it;
          }
        }

        for (auto it = AET.begin(); it != std::prev(AET.end()); ++it)
        {
          auto nxt = std::next(it);

          if ((*it)->x == (*nxt)->x)
          {
            continue;
          }

          for (int x = std::max(vmin.x, (*it)->x); x <= std::min(vmax.x, (*nxt)->x); ++x)
          {
            float curz = TestZ(pid, x, y);
            if (canvas.z_buffer->buffer[y][x] > curz)
            {
              canvas.z_buffer->buffer[y][x] = curz;
              RenderPixel(*canvas.frame_buffer, canvas.offsetx + x, canvas.offsety + y, polygons[pid].color);
            }
          }
        }
       
        for (auto& edge : AET)
        {
          edge->ymin += 1;
          edge->e += edge->m;
          while(edge->e > 0.0f)
          {
            edge->x += edge->d;
            edge->e -= 1.0f;
          }
        }
      }

      HZBuffer::Update(canvas.h_z_buffer, canvas, vmin.x, vmax.x, vmin.y, vmax.y);
    }
  }

  static void RenderPolygonsIntervalScanLine(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons) NOEXCEPT
  {
    static std::vector<float> A; A.clear();
    static std::vector<float> B; B.clear();
    static std::vector<float> C; C.clear();
       
    A.resize(polygons.size());
    B.resize(polygons.size());
    C.resize(polygons.size());
       
    for (size_t pid = 0; pid < polygons.size(); ++pid)
    {
      if (polygons[pid].vertices.size() < 3) { continue; }
      glm::vec3 p0 = vertices[polygons[pid].vertices[0]]; 
      glm::vec3 p1 = vertices[polygons[pid].vertices[1]]; 
      glm::vec3 p2 = vertices[polygons[pid].vertices[2]]; 
      glm::vec3 n = glm::normalize(glm::cross(p0 - p1, p1 - p2));
      A[pid] = -n.x / n.z;
      B[pid] = -n.y / n.z;
      C[pid] = glm::dot(n, p0) / n.z;
    }

    static auto TestZ = [&](const uint32_t pid, const int x, const int y) NOEXCEPT -> float
    {
      if (pid == polygons.size()) { return canvas.z_buffer->bgz; }
      return A[pid] * x + B[pid] * y + C[pid];
    };

    glm::ivec2 vmin = glm::ivec2(canvas.height-1, canvas.width-1);      
    glm::ivec2 vmax = glm::ivec2(0, 0);
       
    for (const auto& vertex : vertices)
    {
      vmin = glm::min(vmin, glm::ivec2(glm::round(vertex)));
      vmax = glm::max(vmax, glm::ivec2(glm::round(vertex)));
    }

    vmin = glm::max(vmin, glm::ivec2(0, 0));
    vmax = glm::min(vmax, glm::ivec2(canvas.width-1, canvas.height-1));
       
    static std::vector<Edge> ET; ET.clear();
    ET.reserve(2 + std::accumulate(polygons.begin(), polygons.end(), 0, [](const size_t acc, const Polygon& polygon) { return acc + polygon.vertices.size(); }));

    ET.emplace_back(vmin.y, vmax.y, vmin.x, 0, 0.0f, -0.5f, polygons.size());
    ET.emplace_back(vmin.y, vmax.y, vmax.x, 0, 0.0f, -0.5f, polygons.size());
       
    for (size_t pid = 0; pid < polygons.size(); ++pid)
    {
      if (polygons[pid].vertices.size() < 3) { continue; }
      for (size_t i = 0; i < polygons[pid].vertices.size(); ++i)
      {
        size_t j = (i + 1) % polygons[pid].vertices.size();

        glm::vec3 v0 = vertices[polygons[pid].vertices[i]];
        glm::vec3 v1 = vertices[polygons[pid].vertices[j]];

        if (std::round(v0.y) == std::round(v1.y))
        {
          continue;
        }
           
        if (v0.y > v1.y)
        {
          std::swap(v0, v1);
        }

        float dxdy = (std::round(v0.x) - std::round(v1.x)) / (std::round(v0.y) - std::round(v1.y));
           
        Edge edge {
          .ymin = (int)std::round(v0.y),
          .ymax = (int)std::round(v1.y),
          .x = (int)std::round(v0.x),
          .d = dxdy > 0.0f ? 1 : -1,
          .m = std::abs(dxdy),
          .e = std::abs(dxdy) - 0.5f,
          .pid = pid,
        };

        if (edge.ymax >= vmin.y && edge.ymin <= vmax.y)
        {
          if (edge.ymin < vmin.y)
          {
            edge.e += edge.m * (vmin.y - edge.ymin);
            while(edge.e > 0.0f)
            {
              edge.x += edge.d;
              edge.e -= 1.0f;
            }
            edge.ymin = vmin.y;
          }

          if(edge.ymax > vmax.y)
          {
            edge.ymax = vmax.y;
          }

          ET.emplace_back(edge);
        }
      }
    }

    std::sort(ET.begin(), ET.end());

    static std::list<Edge*> AET; AET.clear();
    static std::unordered_set<uint32_t> APT; APT.clear();

    for (int y = std::max(vmin.y, 0), j = 0; y < std::min(vmax.y, canvas.height-1); ++y)
    {
      while((size_t)j < ET.size() && ET[j].ymin < y)
      {
        ++j;
      }
      {
        auto it = AET.begin();
        while ((size_t)j < ET.size() && ET[j].ymin == y)
        {
          while(it != AET.end() && (*it)->x <= ET[j].x)
          {
            ++it;
          }
          while(it != AET.end() && (*it)->x == ET[j].x && (*it)->d * (*it)->m <= ET[j].d * ET[j].m)
          {
            ++it;
          }
          it = AET.emplace(it, &ET[j++]);
        }
      }

      for (auto it = AET.begin(); it != AET.end();)
      {
        if ((*it)->ymin == (*it)->ymax)
        {
          it = AET.erase(it);
        }
        else
        {
          ++it;
        }
      }

      // TODO: Very Slow.
      AET.sort([](const Edge* lhs, const Edge* rhs) NOEXCEPT { return *lhs < *rhs; });
      APT.clear();

      for (auto it = AET.begin(); it != std::prev(AET.end()); ++it)
      {
        if (auto found = APT.find((*it)->pid); found != APT.end())
        {
          APT.erase(found);
        }
        else
        {
          APT.insert((*it)->pid);
        }

        auto nxt = std::next(it);

        if ((*it)->x >= (*nxt)->x)
        {
          continue;
        }

        if ((*it)->x >= canvas.width || (*nxt)->x < 0)
        {
          continue;
        }

        size_t target = polygons.size();
        float z = INF;
           
        for (const auto pid : APT)
        {
          float curz = TestZ(pid, (int)std::round(((*it)->x + (*nxt)->x) / 2.0f), y);
          if (z > curz)
          {
            z = curz;
            target = pid;
          }
        }

        if (target != polygons.size())
        {
          RenderSegment(*canvas.frame_buffer, std::max(canvas.offsetx + (*it)->x, 0), std::min(canvas.offsetx + (*nxt)->x, canvas.width-1), canvas.offsety + y, polygons[target].color);
        }
      }
         
      for (auto& edge : AET)
      {
        edge->ymin += 1;
        edge->e += edge->m;
        while(edge->e > 0.0f)
        {
          edge->x += edge->d;
          edge->e -= 1.0f;
        }
      }
    }
  }
  
};
  
#endif //RASTERIZER_H
