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

struct Rasterizer
{
private: // COMMENT: Internal Util Functions For Rasterization.

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
    // COMMENT: Ignore Out Of Range
    if (i < 0 || i >= canvas.height) return;
    if (jmin > jmax || jmax < 0 || jmin >= canvas.width) return;
    jmin = std::clamp(jmin, 0, canvas.width-1);
    jmax = std::clamp(jmax, 0, canvas.width-1);
    // NOTE: Close Interval To Avoid Gaps In Rendering
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
      RenderPixel(canvas, x, y, color);
      return;
    }

    const int delta_x = dx > 0 ? 1 : -1;
    const int delta_y = dy > 0 ? 1 : -1;

    // COMMENT: Choose A Larger Direction As Main Direction.
    if (std::abs(dx) > std::abs(dy))
    {
      const float m = std::abs((float)dy / (float)dx);
      float e = m - 0.5f; // COMMENT: Error.
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
      float e = m - 0.5f; // COMMENT: Error.
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

public: // COMMENT: Public Functions For Other Systems To Use.

  // COMMENT: Render A Line From p0 To p1, With The Specific Color. Simply Call RenderLineBresenham.
  static void RenderLine(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color) NOEXCEPT
  {
    RenderLineBresenham(canvas, p0, p1, color);
  }

  // COMMENT: Render A Triangle Wireframe, With The Specific Color. Simply Draw Three Edges.
  static void RenderTriangleLine(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const glm::ivec2& p2, const Color& color) NOEXCEPT {
    RenderLineBresenham(canvas, p0, p1, color);
    RenderLineBresenham(canvas, p1, p2, color);
    RenderLineBresenham(canvas, p2, p0, color);
  }

  // COMMENT: Render A Triangle Wireframe, With The Specific Color.
  static void RenderTriangleFill(
    const Canvas& canvas,
    const glm::ivec2& p0, const glm::ivec2& p1, const glm::ivec2& p2,
    float z0, float z1, float z2,
    const Color& color0, const Color& color1, const Color& color2
  ) NOEXCEPT {
    (void)z0;
    (void)z1;
    (void)z2;
    (void)color0;
    (void)color1;
    (void)color2;
    
    glm::ivec2 vmin = glm::max(glm::min(glm::min(p0, p1), p2), glm::ivec2(0.0));
    glm::ivec2 vmax = glm::min(glm::max(glm::max(p0, p1), p2), glm::ivec2(canvas.width, canvas.height));

    for (int i = vmin.y; i <= vmax.y; ++i)
    {
      for (int j = vmin.x; j <= vmax.x; ++j)
      {
        glm::ivec2 p(j, i);
        glm::vec3 cord(
          glm::cross(glm::vec3(p2 - p1, 0.0f), glm::vec3(p - p1, 0.0f)).z,
          glm::cross(glm::vec3(p0 - p2, 0.0f), glm::vec3(p - p2, 0.0f)).z,
          glm::cross(glm::vec3(p1 - p0, 0.0f), glm::vec3(p - p0, 0.0f)).z
        );
        if (cord.x < 0 || cord.y < 0 || cord.z < 0) continue;
        cord = cord / (cord.x + cord.y + cord.z);
        float z = cord.x * z0 + cord.y * z1 + cord.z * z2;
        if (canvas.zbuffer[i][j] > z)
        {
          canvas.zbuffer[i][j] = z;
          RenderPixel(canvas, i, j, cord.x * color0 + cord.y * color1 + cord.z * color2);
        }
      }
    }
    
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
  }
};

#endif //RASTERIZER_H
