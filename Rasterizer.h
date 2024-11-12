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
#include <Accelerator.h>
#include <numeric>
#include <unordered_set>

struct Rasterizer
{
public: // COMMENT: Internal Util Functions For Rasterization.

  // COMMENT: Map A Color To The Format That Canvas Accepts.
  static uint32_t MapColor(const Canvas&canvas, const Color& color) NOEXCEPT
  {
    // NOTE: Clamp Color To [0.0f, 1.0f] To Avoid Light Energy Issue.
    return SDL_MapRGB(canvas.pixel_format_details, nullptr, (Uint8)255.999f * std::clamp(color.r, 0.0f, 1.0f), (Uint8)255.999f * std::clamp(color.g, 0.0f, 1.0f), (Uint8)255.999f * std::clamp(color.b, 0.0f, 1.0f));
  }

  // COMMENT: Render A Pixel At (i, j) Of The Canvas, With The Specific Color.
  static void RenderPixel(const Canvas& canvas, const int i, const int j, const Color& color) NOEXCEPT
  {
    // COMMENT: Ignore Out Of Range
    if (i < 0 || i >= canvas.height || j < 0 || j >= canvas.width) return;
    // NOTE: Cord (i, j) Are Related To Canvas Offsets.
    canvas.pixels[canvas.surface->w * (i + canvas.offsety) + j + canvas.offsetx] = MapColor(canvas, color);
  }

  // COMMENT: Render A Segment Of Pixels In [(i, jmin), (i, jmax)], With The Specific Color.
  static void RenderSegment(const Canvas& canvas, const int i, int jmin, int jmax, const Color& color) NOEXCEPT
  {
    for (int j = jmin; j <= jmax; ++j)
    {
      RenderPixel(canvas, i, j, color);
    }
  }

  // COMMENT: Render A Rectangle Area Of Pixels In [(imin, jmin), (imax, jmax)], With The Specific Color.
  static void RenderRect(const Canvas& canvas, int imin, const int jmin, int imax, const int jmax, const Color& color) NOEXCEPT
  {
    imin = std::clamp(imin, 0, canvas.height-1);
    imax = std::clamp(imax, 0, canvas.height-1);
    // NOTE: Automatically Ignore Out Of Range
    for (int i = imin; i <= imax; ++i)
    {
      RenderSegment(canvas, i, jmin, jmax, color);
    }
  }

  // COMMENT: Render A Line From p0 To p1 Using The Algorithm DDA, With The Specific Color.
  static void RenderLineDDA(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color) NOEXCEPT
  {
    float length = std::max(std::abs(p1.x - p0.x), std::abs(p1.y - p0.y));
    float dx = (p1.x - p0.x) / length;
    float dy = (p1.y - p0.y) / length;
    // COMMENT: x, y Denote Current Position
    float i = 1.0f, x = p0.x, y = p0.y;
    while(i <= length && x > 0.0f && x < canvas.width && y > 0.0f && y < canvas.height)
    {
      // NOTE: Use Floor
      RenderPixel(canvas, (int)std::floor(y), (int)std::floor(x), color);
      x = x + dx;
      y = y + dy;
      i = i + 1.0f;
    }
  }

  // COMMENT: Render A Line From p0 To p1 Using The Algorithm Bresenham, With The Specific Color.
  static void RenderLineBresenham(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color) NOEXCEPT
  {
    const int dx = p1.x - p0.x;
    const int dy = p1.y - p0.y;

    int x = p0.x;
    int y = p0.y;
    
    // COMMENT: If The Line Is A Point.
    if (dx == 0 && dy == 0)
    {
      RenderPixel(canvas, y, x, color);
      return;
    }

    const int delta_x = dx > 0 ? 1 : -1;
    const int delta_y = dy > 0 ? 1 : -1;

    // COMMENT: Choose A Larger Direction As Main Direction.
    if (std::abs(dx) > std::abs(dy))
    {
      const float m = std::abs((float)dy / (float)dx);
      float e = m - 0.5f;
      for (int i = 0; i <= std::abs(dx); ++i)
      {
        RenderPixel(canvas, y, x, color);
        while(e > 0.0f)
        {
          y += delta_y;
          e -= 1.0f;
        }        
        x += delta_x;
        e += m;
      }
    }
    else
    {
      const float m = std::abs((float)dx / (float)dy);
      float e = m - 0.5f;
      for (int i = 0; i <= std::abs(dy); ++i)
      {
        RenderPixel(canvas, y, x, color);
        while(e > 0.0f)
        {
          x += delta_x;
          e -= 1.0f;
        }        
        y += delta_y;
        e += m;
      }
    }
  }

  template<typename T>
  static T Lerp(const glm::ivec2& p0, const T& t0, const glm::ivec2& p1, const T& t1, const glm::ivec2& p) NOEXCEPT
  {
    float k = (float)std::abs(p.x - p0.x) / (float)std::abs(p1.x - p0.x);
    return (1 - k) * t0 + k * t1;
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
  


public: // COMMENT: Public Functions For Other Systems To Use.

  static void RenderLines(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<Line>& lines) NOEXCEPT
  {
    for (const auto& line : lines)
    {
      RenderLineBresenham(canvas, glm::ivec2(glm::round(vertices[line.vertices[0]])), glm::ivec2(glm::round(vertices[line.vertices[1]])), line.color);
    }
  }

  static void RenderPolygonsWireframe(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<struct Polygon>& polygons) NOEXCEPT
  {
    for (const auto& polygon : polygons)
    {
      for (size_t i = 0; i < polygon.vertices.size(); ++i)
      {
        size_t j = (i + 1) % polygon.vertices.size();
        RenderLineBresenham(canvas, glm::ivec2(glm::round(vertices[polygon.vertices[i]])), glm::ivec2(glm::round(vertices[polygon.vertices[j]])), polygon.color);
      }
    }
  }

  static void RenderPolygonsScanConvertZBuffer(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<struct Polygon>& polygons) NOEXCEPT
  {
    static std::vector<float> A; A.clear();
    static std::vector<float> B; B.clear();
    static std::vector<float> C; C.clear();
    
     A.resize(polygons.size());
     B.resize(polygons.size());
     C.resize(polygons.size());
    
    for (size_t i = 0; i < polygons.size(); ++i)
    {
      glm::vec3 p0 = vertices[polygons[i].vertices[0]]; 
      glm::vec3 p1 = vertices[polygons[i].vertices[1]]; 
      glm::vec3 p2 = vertices[polygons[i].vertices[2]]; 
      glm::vec3 n = glm::normalize(glm::cross(p0 - p1, p1 - p2));
      A[i] = -n.x / n.z;
      B[i] = -n.y / n.z;
      C[i] = glm::dot(n, p0) / n.z;
    }

    static auto TestZ = [&](const uint32_t pid, const int i, const int j) NOEXCEPT
    {
      if (pid == polygons.size()) return canvas.z;
      return A[pid] * j + B[pid] * i + C[pid];
    };
    
    static std::vector<Edge> ET; ET.clear();
    ET.reserve(std::accumulate(polygons.begin(), polygons.end(), 0, [](const size_t acc, const struct Polygon& polygon) { return std::max(acc, polygon.vertices.size()); }));

    for (size_t pid = 0; pid < polygons.size(); ++pid)
    {
      ET.clear();
      
      glm::ivec2 vmin = glm::ivec2(canvas.height-1, canvas.width-1);      
      glm::ivec2 vmax = glm::ivec2(0, 0); 

      for (size_t j = 0; j < polygons[pid].vertices.size(); ++j)
      {
        vmin = glm::min(vmin, glm::ivec2(glm::round(vertices[polygons[pid].vertices[j]])));
        vmax = glm::max(vmax, glm::ivec2(glm::round(vertices[polygons[pid].vertices[j]])));
        
        size_t k = (j + 1) % polygons[pid].vertices.size();

        glm::vec3 v0 = vertices[polygons[pid].vertices[j]];
        glm::vec3 v1 = vertices[polygons[pid].vertices[k]];

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
      
      for (int i = vmin.y, k = 0; i <= vmax.y; ++i)
      {
        while((size_t)k < ET.size() && ET[k].ymin < i)
        {
          ++k;
        }
        while ((size_t)k < ET.size() && ET[k].ymin == i)
        {
          AET.push_back(&ET[k]);
          k++;
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

        AET.sort([](const Edge* lhs, const Edge* rhs) NOEXCEPT { return *lhs < *rhs; });

        for (auto it = AET.begin(); it != std::prev(AET.end()); ++it)
        {
          auto nxt = std::next(it);

          if ((*it)->x == (*nxt)->x)
          {
            continue;
          }

          for (int j = std::max(vmin.x, (*it)->x); j <= std::min(vmax.x, (*nxt)->x); ++j)
          {
            float curz = TestZ(pid, i, j);
            if (canvas.zbuffer[i][j] > curz)
            {
              canvas.zbuffer[i][j] = curz;
              RenderPixel(canvas, i, j, polygons[pid].color);
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

  // COMMENT: Input Canvas Vertices And All Polygons. Do The Rendering. This Algorithm Do Not Use ZBuffer. 
  static void RenderPolygonsIntervalScanLine(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<struct Polygon>& polygons) NOEXCEPT
  {
    static std::vector<float> A; A.clear();
    static std::vector<float> B; B.clear();
    static std::vector<float> C; C.clear();
    
     A.resize(polygons.size());
     B.resize(polygons.size());
     C.resize(polygons.size());
    
    for (size_t i = 0; i < polygons.size(); ++i)
    {
      glm::vec3 p0 = vertices[polygons[i].vertices[0]]; 
      glm::vec3 p1 = vertices[polygons[i].vertices[1]]; 
      glm::vec3 p2 = vertices[polygons[i].vertices[2]]; 
      glm::vec3 n = glm::normalize(glm::cross(p0 - p1, p1 - p2));
      A[i] = -n.x / n.z;
      B[i] = -n.y / n.z;
      C[i] = glm::dot(n, p0) / n.z;
    }

    static auto TestZ = [&](const uint32_t pid, const int i, const int j) NOEXCEPT
    {
      if (pid == polygons.size()) return canvas.z;
      return A[pid] * j + B[pid] * i + C[pid];
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
    ET.reserve(2 + std::accumulate(polygons.begin(), polygons.end(), 0, [](const size_t acc, const struct Polygon& polygon) { return acc + polygon.vertices.size(); }));

    ET.emplace_back(vmin.y, vmax.y, vmin.x, 0, 0.0f, -0.5f, polygons.size());
    ET.emplace_back(vmin.y, vmax.y, vmax.x, 0, 0.0f, -0.5f, polygons.size());
    
    for (size_t i = 0; i < polygons.size(); ++i)
    {
      for (size_t j = 0; j < polygons[i].vertices.size(); ++j)
      {
        size_t k = (j + 1) % polygons[i].vertices.size();

        glm::vec3 v0 = vertices[polygons[i].vertices[j]];
        glm::vec3 v1 = vertices[polygons[i].vertices[k]];

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
          .pid = i,
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

    for (int i = vmin.y, j = 0; i < vmax.y; ++i)
    {
      while((size_t)j < ET.size() && ET[j].ymin < i)
      {
        ++j;
      }
      while ((size_t)j < ET.size() && ET[j].ymin == i)
      {
        AET.push_back(&ET[j]);
        j++;
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

        if ((*it)->x == (*nxt)->x)
        {
          continue;
        }

        size_t target = polygons.size();
        float z = canvas.z;
        
        for (const auto pid : APT)
        {
          float curz = TestZ(pid, i, (int)std::round(((*it)->x + (*nxt)->x) / 2.0f));
          if (z > curz)
          {
            z = curz;
            target = pid;
          }
        }

        if (target != polygons.size())
        {
          RenderSegment(canvas, i, (*it)->x, (*nxt)->x, polygons[target].color);
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

  static void RenderPolygonsScanConvertHZBuffer(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<struct Polygon>& polygons) NOEXCEPT
  {
    static std::vector<float> A; A.clear();
    static std::vector<float> B; B.clear();
    static std::vector<float> C; C.clear();
    
     A.resize(polygons.size());
     B.resize(polygons.size());
     C.resize(polygons.size());
    
    for (size_t i = 0; i < polygons.size(); ++i)
    {
      glm::vec3 p0 = vertices[polygons[i].vertices[0]]; 
      glm::vec3 p1 = vertices[polygons[i].vertices[1]]; 
      glm::vec3 p2 = vertices[polygons[i].vertices[2]]; 
      glm::vec3 n = glm::normalize(glm::cross(p0 - p1, p1 - p2));
      A[i] = -n.x / n.z;
      B[i] = -n.y / n.z;
      C[i] = glm::dot(n, p0) / n.z;
    }

    static auto TestZ = [&](const uint32_t pid, const int i, const int j) NOEXCEPT
    {
      if (pid == polygons.size()) return canvas.z;
      return A[pid] * j + B[pid] * i + C[pid];
    };
    
    static std::vector<Edge> ET; ET.clear();
    ET.reserve(std::accumulate(polygons.begin(), polygons.end(), 0, [](const size_t acc, const struct Polygon& polygon) { return std::max(acc, polygon.vertices.size()); }));

    for (size_t pid = 0; pid < polygons.size(); ++pid)
    {
      AABB aabb = AABB::From(vertices, polygons[pid]);
        
      if (Accelerator::QueryHZBufferTree(canvas.h_zbuffer_tree, std::round(aabb.vmin.y), std::round(aabb.vmax.y), std::round(aabb.vmin.x), std::round(aabb.vmax.x)) <= aabb.vmin.z)
      {
        continue;
      }
      
      ET.clear();
      
      glm::ivec2 vmin = glm::ivec2(canvas.height-1, canvas.width-1);      
      glm::ivec2 vmax = glm::ivec2(0, 0); 

      for (size_t j = 0; j < polygons[pid].vertices.size(); ++j)
      {
        vmin = glm::min(vmin, glm::ivec2(glm::round(vertices[polygons[pid].vertices[j]])));
        vmax = glm::max(vmax, glm::ivec2(glm::round(vertices[polygons[pid].vertices[j]])));
        
        size_t k = (j + 1) % polygons[pid].vertices.size();

        glm::vec3 v0 = vertices[polygons[pid].vertices[j]];
        glm::vec3 v1 = vertices[polygons[pid].vertices[k]];

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
      
      for (int i = vmin.y, k = 0; i <= vmax.y; ++i)
      {
        while((size_t)k < ET.size() && ET[k].ymin < i)
        {
          ++k;
        }
        while ((size_t)k < ET.size() && ET[k].ymin == i)
        {
          AET.push_back(&ET[k]);
          k++;
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

        AET.sort([](const Edge* lhs, const Edge* rhs) NOEXCEPT { return *lhs < *rhs; });

        for (auto it = AET.begin(); it != std::prev(AET.end()); ++it)
        {
          auto nxt = std::next(it);

          if ((*it)->x == (*nxt)->x)
          {
            continue;
          }

          for (int j = std::max(vmin.x, (*it)->x); j <= std::min(vmax.x, (*nxt)->x); ++j)
          {
            float curz = TestZ(pid, i, j);
            if (canvas.zbuffer[i][j] > curz)
            {
              canvas.zbuffer[i][j] = curz;
              RenderPixel(canvas, i, j, polygons[pid].color);
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

      Accelerator::UpdateHZBufferTree(canvas, canvas.h_zbuffer_tree, vmin.y, vmax.y, vmin.x, vmax.x);
    }
  }

  
};

// static void RenderTriangleWireframe(const Canvas& canvas, const std::vector<Vertex>& vertices, const Triangle& triangle) NOEXCEPT {
//   for (size_t i = 0; i < 3; ++i)
//   {
//     RenderLine(canvas, vertices, Line{
//       .vertices = { triangle.vertices[i], triangle.vertices[(i+1)%3] },
//       .color = triangle.color,
//     });
//   }
// }
// static void RenderTriangleZBuffer(const Canvas& canvas, const std::vector<Vertex>& vertices, const Triangle& triangle) NOEXCEPT
// {
//   struct Edge
//   {
//     int ymin;
//     int ymax;
//     int x;
//     float dx;
//     Edge* next;
//   };
//
//   std::vector<Edge> ET;
//   ET.reserve(3);
//   for (size_t i = 0; i < 3; ++i)
//   {
//     Vertex v0 = vertices[triangle.vertices[0]];
//     Vertex v1 = vertices[triangle.vertices[1]];
//     if (std::round(v0.y) == std::round(v1.y))
//     {
//       return;
//     }
//     if (std::round(v0.y) > std::round(v1.y))
//     {
//       std::swap(v0, v1);  
//     }
//     ET.emplace_back(
//       std::round(v0.y),
//       std::round(v1.y),
//       std::round(v0.x),
//       (v1.x - v0.x) / (v1.y - v0.y) 
//     );
//   }
//   std::ranges::sort(ET, [](const Edge& lhs, const Edge& rhs) -> bool {
//     if (lhs.ymin != rhs.ymin)
//     {
//       return lhs.ymin < rhs.ymin;  
//     }
//     if (lhs.x != rhs.x)
//     {
//       return lhs.x < rhs.x;  
//     }
//     return lhs.dx < rhs.dx;
//   });
//
//   std::list<Edge> AET;
//   int i = ET.front().ymin;
//   while(i <= ET.back().ymax)
//   {
//     
//   }

    // struct ZData
    // {
    //   glm::ivec2 p;
    //   float z;
    // };
    //
    // ZData zd0 = { glm::ivec2(glm::round(vertices[triangle.vertices[0]])), vertices[triangle.vertices[0]].z };
    // ZData zd1 = { glm::ivec2(glm::round(vertices[triangle.vertices[1]])), vertices[triangle.vertices[1]].z };
    // ZData zd2 = { glm::ivec2(glm::round(vertices[triangle.vertices[2]])), vertices[triangle.vertices[2]].z };
    //
    // std::vector zds = { zd0, zd1, zd2 };
    // std::ranges::sort(zds, [](const ZData& lhs, const ZData& rhs) -> bool {
    //   return lhs.p.y == rhs.p.y ? lhs.p.x < rhs.p.x : lhs.p.y < rhs.p.y;
    // });
    //
    // if (zds[0].p.y == zds[2].p.y)
    // {
    //   if (zds[0].p.y < 0 || zds[0].p.y >= canvas.height)
    //   {
    //     return;
    //   }
    //   float dz = zds[2].z - zds[0].z / zds[2].p.x - zds[0].p.x;
    //   float z = zds[0].z;
    //   for (int j = zds[0].p.x; j <= zds[2].p.x && j < canvas.width; ++j)
    //   {
    //     if (canvas.zbuffer[zds[0].p.y][j] > z)
    //     {
    //       canvas.zbuffer[zds[0].p.y][j] = z;
    //       RenderPixel(canvas, zds[0].p.y, j, triangle.color);
    //     }
    //     z += dz;
    //   }
    //   return;
    // }
    //
    // int jmin, jmax, dxl, dxr, delta_xl, delta_xr;
    // float ml, mr, el, er;
    //
    // if (points[1].y != points[0].y)
    // {
    //   jmin = jmax = points[0].x;
    //   if (points[0].x < points[1].x) {
    //     dxl = points[2].x - points[0].x;
    //     dxr = points[1].x - points[0].x;
    //     ml = std::abs((float)dxl / (float)(points[2].y - points[0].y));
    //     mr = std::abs((float)dxr / (float)(points[1].y - points[0].y));
    //   } else {
    //     dxl = points[1].x - points[0].x;
    //     dxr = points[2].x - points[0].x;
    //     ml = std::abs((float)dxl / (float)(points[1].y - points[0].y));
    //     mr = std::abs((float)dxr / (float)(points[2].y - points[0].y));
    //   }
    //   delta_xl = dxl > 0 ? 1 : -1;
    //   delta_xr = dxr > 0 ? 1 : -1;
    //   el = ml - 0.5f;
    //   er = mr - 0.5f;
    //   for (int i = points[0].y; i < canvas.height && i < points[1].y; ++i)
    //   {
    //     
    //     // RenderSegment(canvas, i, std::min(jmin, jmax), std::max(jmin, jmax), color);
    //     while(el > 0.0f)
    //     {
    //       jmin += delta_xl;
    //       el -= 1.0f;
    //     }
    //     while(er > 0.0f)
    //     {
    //       jmax += delta_xr;
    //       er -= 1.0f;
    //     }
    //     el += ml;
    //     er += mr;
    //   }
    // }
    //
    // if (points[1].y != points[2].y)
    // {
    //   jmin = jmax = points[2].x;
    //   if (points[0].x < points[1].x)
    //   {
    //     dxl = points[0].x - points[2].x;
    //     dxr = points[1].x - points[2].x;
    //     ml = std::abs((float)dxl / (float)(points[0].y - points[2].y));
    //     mr = std::abs((float)dxr / (float)(points[1].y - points[2].y));
    //   }
    //   else
    //   {
    //     dxl = points[1].x - points[2].x;
    //     dxr = points[0].x - points[2].x;
    //     ml = std::abs((float)dxl / (float)(points[1].y - points[2].y));
    //     mr = std::abs((float)dxr / (float)(points[0].y - points[2].y));
    //   }
    //   delta_xl = dxl > 0 ? 1 : -1;
    //   delta_xr = dxr > 0 ? 1 : -1;
    //   el = ml - 0.5f;
    //   er = mr - 0.5f;
    //   for (int i = points[2].y; i >= 0 && i >= points[1].y; --i)
    //   {
    //     // RenderSegment(canvas, i, std::min(jmin, jmax), std::max(jmin, jmax), color);
    //     while(el > 0.0f) 
    //     {
    //       jmin += delta_xl;
    //       el -= 1.0f;
    //     }
    //     while(er > 0.0f) 
    //     {
    //       jmax += delta_xr;
    //       er -= 1.0f;
    //     }
    //     el += ml;
    //     er += mr;
    //   }
    // }
  // }

  // static void RenderPolygonZBuffer(const Canvas& canvas, const std::vector<Vertex>& vertices, const struct Polygon& polygon) NOEXCEPT
  // {
  //   struct Edge {
  //     int ymax;
  //     int x;
  //     int dx;
  //     Edge* next;
  //   };
  // }
  
  //
  // static void RenderTriangleHZBuffer(const Canvas& canvas, const Triangle& triangle) NOEXCEPT
  // {
  // }
  //
  // static void RenderTriangleHZBufferHBB(const Canvas& canvas, const Triangle& triangle) NOEXCEPT
  // {
  // }
  // static void RenderTriangle(
  //   const Canvas& canvas,
  //   const Triangle& triangle,
  //   const bool use_h_zbuffer_acc
  // ) NOEXCEPT {
  //   glm::ivec2 vmin = glm::max(glm::min(glm::min(glm::ivec2(glm::round(triangle.vertices[0])),glm::ivec2(glm::round(triangle.vertices[1]))),glm::ivec2(glm::round(triangle.vertices[2]))), glm::ivec2(0));
  //   glm::ivec2 vmax = glm::min(glm::max(glm::max(glm::ivec2(glm::round(triangle.vertices[0])), glm::ivec2(glm::round(triangle.vertices[1]))), glm::ivec2(glm::round(triangle.vertices[2]))), glm::ivec2(canvas.width - 1, canvas.height - 1));
  //
  //   if (use_h_zbuffer_acc && Accelerator::QueryHZBufferTree(canvas.h_zbuffer_tree, vmin.y, vmax.y, vmin.x, vmax.x) <= std::min(std::min(triangle.vertices[0].z, triangle.vertices[1].z), triangle.vertices[2].z))
  //   {
  //     return;  
  //   }
  //   for (int i = vmin.y; i <= vmax.y; ++i)
  //   {
  //     for (int j = vmin.x; j <= vmax.x; ++j)
  //     {
  //       glm::ivec2 p(j, i);
  //       glm::vec3 cord(
  //         glm::cross(glm::vec3(p - glm::ivec2(glm::round(triangle.vertices[1])), 0.0f), glm::vec3(glm::ivec2(glm::round(triangle.vertices[2])) - glm::ivec2(glm::round(triangle.vertices[1])), 0.0f)).z,
  //         glm::cross(glm::vec3(p - glm::ivec2(glm::round(triangle.vertices[2])), 0.0f), glm::vec3(glm::ivec2(glm::round(triangle.vertices[0])) - glm::ivec2(glm::round(triangle.vertices[2])), 0.0f)).z,
  //         glm::cross(glm::vec3(p - glm::ivec2(glm::round(triangle.vertices[0])), 0.0f), glm::vec3(glm::ivec2(glm::round(triangle.vertices[1])) - glm::ivec2(glm::round(triangle.vertices[0])), 0.0f)).z
  //       );
  //       if (cord.x < 0 || cord.y < 0 || cord.z < 0) continue;
  //       cord = cord / (cord.x + cord.y + cord.z);
  //       // FIXME: Is This Correct?
  //       float z = cord.x * triangle.vertices[0].z + cord.y * triangle.vertices[1].z + cord.z * triangle.vertices[2].z;
  //       if (canvas.zbuffer[i][j] > z)
  //       {
  //         canvas.zbuffer[i][j] = z;
  //         if (use_h_zbuffer_acc)
  //         {
  //           Accelerator::UpdateHZBufferTree(canvas.h_zbuffer_tree, i, j, z);
  //         }
  //         if (use_primitive_color)
  //         {
  //           RenderPixel(canvas, i, j, triangle.color);
  //         }
  //         else
  //         {
  //           RenderPixel(canvas, i, j, cord.x * triangle.colors[0] + cord.y * triangle.colors[1] + cord.z * triangle.colors[2]);
  //         }
  //       }
  //     }
  //   }
  // }
  //
  // static void RenderTriangleFillHZBufferAcceleration(
  //   const Canvas& canvas,
  //   const glm::ivec2& p0, const glm::ivec2& p1, const glm::ivec2& p2,
  //   const float z0, const float z1, const float z2,
  //   const Color& color0, const Color& color1, const Color& color2
  // ) NOEXCEPT {
  //   glm::ivec2 vmin = glm::max(glm::min(glm::min(p0, p1), p2), glm::ivec2(0.0));
  //   glm::ivec2 vmax = glm::min(glm::max(glm::max(p0, p1), p2), glm::ivec2(canvas.width - 1, canvas.height - 1));
  //
  //   if (Accelerator::QueryHZBufferTree(canvas.h_zbuffer_tree, vmin.y, vmax.y, vmin.x, vmax.x) <= std::min(std::min(z0, z1), z2))
  //   {
  //     return;  
  //   }
  //   for (int i = vmin.y; i <= vmax.y; ++i)
  //   {
  //     for (int j = vmin.x; j <= vmax.x; ++j)
  //     {
  //       glm::ivec2 p(j, i);
  //       glm::vec3 cord(
  //         glm::cross(glm::vec3(p - p1, 0.0f), glm::vec3(p2 - p1, 0.0f)).z,
  //         glm::cross(glm::vec3(p - p2, 0.0f), glm::vec3(p0 - p2, 0.0f)).z,
  //         glm::cross(glm::vec3(p - p0, 0.0f), glm::vec3(p1 - p0, 0.0f)).z
  //       );
  //       if (cord.x < 0 || cord.y < 0 || cord.z < 0) continue;
  //       cord = cord / (cord.x + cord.y + cord.z);
  //       // FIXME: Is This Correct?
  //       float z = cord.x * z0 + cord.y * z1 + cord.z * z2;
  //       if (canvas.zbuffer[i][j] > z)
  //       {
  //         canvas.zbuffer[i][j] = z;
  //         Accelerator::UpdateHZBufferTree(canvas.h_zbuffer_tree, i, j, z);
  //         RenderPixel(canvas, i, j, cord.x * color0 + cord.y * color1 + cord.z * color2);
  //       }
  //     }
  //   }
  // }
    // COMMENT: Blinn-Phong Shading Needs To Calc `cord`. Departure The Scanline Algorithm Below.
    // TODO: We May Use BiLinear Interpolation?
    // TODO: Can Z Use BiLinear Interpolation?
    // std::vector points = {p0,p1,p2};
    // std::ranges::sort(points, [](const glm::ivec2& lhs, const glm::ivec2& rhs) -> bool {
    //   return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
    // });
    // if (points[0].y == points[2].y) {
    //   // RenderSegment(canvas, points[0].y, points[0].x, points[2].x, color);
    //   return;
    // }
    // int jmin, jmax, dxl, dxr, delta_xl, delta_xr;
    // float ml, mr, el, er;
    // if (points[1].y != points[0].y) {
    //   jmin = jmax = points[0].x;
    //   if (points[0].x < points[1].x) {
    //     dxl = points[2].x - points[0].x;
    //     dxr = points[1].x - points[0].x;
    //     ml = std::abs((float)dxl / (float)(points[2].y - points[0].y));
    //     mr = std::abs((float)dxr / (float)(points[1].y - points[0].y));
    //   } else {
    //     dxl = points[1].x - points[0].x;
    //     dxr = points[2].x - points[0].x;
    //     ml = std::abs((float)dxl / (float)(points[1].y - points[0].y));
    //     mr = std::abs((float)dxr / (float)(points[2].y - points[0].y));
    //   }
    //   delta_xl = dxl > 0 ? 1 : -1;
    //   delta_xr = dxr > 0 ? 1 : -1;
    //   el = ml - 0.5f;
    //   er = mr - 0.5f;
    //   for (int i = points[0].y; i < canvas.height && i < points[1].y; ++i) {
    //     // RenderSegment(canvas, i, std::min(jmin, jmax), std::max(jmin, jmax), color);
    //     while(el > 0.0f) {
    //       jmin += delta_xl;
    //       el -= 1.0f;
    //     }
    //     while(er > 0.0f) {
    //       jmax += delta_xr;
    //       er -= 1.0f;
    //     }
    //     el += ml;
    //     er += mr;
    //   }
    // }
    // if (points[1].y != points[2].y) {
    //   jmin = jmax = points[2].x;
    //   if (points[0].x < points[1].x) {
    //     dxl = points[0].x - points[2].x;
    //     dxr = points[1].x - points[2].x;
    //     ml = std::abs((float)dxl / (float)(points[0].y - points[2].y));
    //     mr = std::abs((float)dxr / (float)(points[1].y - points[2].y));
    //   } else {
    //     dxl = points[1].x - points[2].x;
    //     dxr = points[0].x - points[2].x;
    //     ml = std::abs((float)dxl / (float)(points[1].y - points[2].y));
    //     mr = std::abs((float)dxr / (float)(points[0].y - points[2].y));
    //   }
    //   delta_xl = dxl > 0 ? 1 : -1;
    //   delta_xr = dxr > 0 ? 1 : -1;
    //   el = ml - 0.5f;
    //   er = mr - 0.5f;
    //   for (int i = points[2].y; i >= 0 && i >= points[1].y; --i) {
    //     // RenderSegment(canvas, i, std::min(jmin, jmax), std::max(jmin, jmax), color);
    //     while(el > 0.0f) {
    //       jmin += delta_xl;
    //       el -= 1.0f;
    //     }
    //     while(er > 0.0f) {
    //       jmax += delta_xr;
    //       er -= 1.0f;
    //     }
    //     el += ml;
    //     er += mr;
    //   }
    // }

#endif //RASTERIZER_H
