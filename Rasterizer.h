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
#include <Acceleration/HAABB.h>

struct Rasterizer
{
  NODISCARD  static Uint32 MapColor(const FrameBuffer& frame_buffer, const Color& color) NOEXCEPT;

  static void RenderPixel(const FrameBuffer& frame_buffer, int x, int y, Uint32 color) NOEXCEPT;

  static void RenderPixel(const FrameBuffer& frame_buffer, int x, int y, const Color& color) NOEXCEPT;

  static void RenderSegment(const FrameBuffer& frame_buffer, int xmin, int xmax, int y, Uint32 color) NOEXCEPT;
  
  static void RenderSegment(const FrameBuffer& frame_buffer, int xmin, int xmax, int y, const Color& color) NOEXCEPT;
  
  static void RenderTangentDDA(const Canvas& canvas, int x0, int y0, int x1, int y1, const Uint32& color) NOEXCEPT;
  
  static void RenderTangentDDA(const Canvas& canvas, int x0, int y0, int x1, int y1, const Color& color) NOEXCEPT;


  static void RenderTangentBresenham(const Canvas& canvas, int x0, int y0, int x1, int y1, const Uint32& color) NOEXCEPT;
  
  static void RenderTangentBresenham(const Canvas& canvas, int x0, int y0, int x1, int y1, const Color& color) NOEXCEPT;

  static void RenderPolygonsWireframe(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons) NOEXCEPT;
  
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
  
  static void RenderPolygonsScanConvertZBuffer(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons) NOEXCEPT;
  
  static void RenderPolygonsScanConvertHZBuffer(Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons) NOEXCEPT;
  
  static void RenderPolygonsScanConvertHAABBHZBuffer(Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons, const std::vector<HAABB>& haabbs) NOEXCEPT;
  
  static void RenderPolygonsIntervalScanLine(const Canvas& canvas, const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons) NOEXCEPT;
};
  
#endif //RASTERIZER_H
