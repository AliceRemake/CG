/**
  ******************************************************************************
  * @file           : Pipeline.cpp
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-13
  ******************************************************************************
  */



#include <Pipeline.h>
#include <Rasterizer.h>
#include <Transformer.h>

 void Pipeline::Render(const Setting& setting, const Shader::Config& config, Canvas& canvas, const Camera& camera, const Scene& scene) NOEXCEPT
{
  static std::vector<ParallelLight> parallel_lights;         parallel_lights.clear();
  static std::vector<PointLight>    point_lights;            point_lights.clear();

  static std::vector<Vertex>        vertices;                vertices.clear();
  static std::vector<Polygon>       polygons;                polygons.clear();

  static std::vector<Vertex>        polygon_normal_vertices; polygon_normal_vertices.clear();
  static std::vector<Polygon>       polygon_normals;         polygon_normals.clear();

  static std::vector<bool>          visited;                 visited.clear();

  parallel_lights.reserve(scene.parallel_lights.size());
  point_lights.reserve(scene.point_lights.size());

  const glm::mat4 V = Transformer::View(camera);

  for (const auto& light : scene.parallel_lights)
  {
    glm::vec4 t = V * glm::vec4(light.direction, 0.0f);
    parallel_lights.emplace_back(t.xyz(), light.color);
  }

  for (const auto& light : scene.point_lights)
  {
    glm::vec4 t = V * glm::vec4(light.position, 1.0f);
    point_lights.emplace_back(t.xyz() / t.w, light.color);
  }

  for (const auto& model : scene.models)
  {
    vertices.clear(); vertices.reserve(model.vertices.size());
    polygons.clear(); polygons.reserve(model.polygon_sides.size());

    if (setting.show_normal)
    {
      polygon_normal_vertices.clear(); polygon_normal_vertices.reserve(model.polygon_sides.size() * 2);
      polygon_normals.clear(); polygon_normals.reserve(model.polygon_sides.size());
    }

    const glm::mat4 MV = V * Transformer::Model(model);
    
    for (const auto& vertex : model.vertices)
    {
      glm::vec4 t = MV * glm::vec4(vertex, 1.0f);
      vertices.emplace_back(t.xyz() / t.w);
    }

    for (size_t i = 0, j = 0; i < model.polygon_sides.size() && j < model.indices.size(); j += model.polygon_sides[i], ++i)
    {
      Polygon polygon;
      
      polygon.vertices.reserve(model.polygon_sides[i]);
      for (uint32_t k = 0; k < model.polygon_sides[i]; ++k)
      {
        polygon.vertices.emplace_back(model.indices[j + k].vertex);
      }

      Vertex c = Polygon::Center(vertices, polygon);
      Normal n = Polygon::Normal(vertices, polygon);

      if (setting.enable_cull)
      {
        if (glm::dot(c, n) >= 0.0f) { continue; }
      }

      polygon.color = Shader::BlinnPhong(parallel_lights, point_lights, c, n, config);
      
      if (setting.show_normal)
      {
        polygon_normal_vertices.emplace_back(c);
        polygon_normal_vertices.emplace_back(c + 0.1f * n);

        Polygon line;
        line.vertices = {(uint32_t)(polygon_normal_vertices.size() - 2), (uint32_t)(polygon_normal_vertices.size() - 1)};
        line.color = Color(0.0f, 1.0f, 0.0f);
        
        polygon_normals.emplace_back(std::move(line));
      }
      
      polygons.emplace_back(std::move(polygon));
    }

    glm::mat4 P = Transformer::Project(camera);

    visited.clear(); visited.resize(vertices.size(), false);
    for (auto& polygon : polygons)
    {
      for (const auto& vertex : polygon.vertices)
      {
        if (!visited[vertex])
        {
          visited[vertex] = true;
          glm::vec4 t = P * glm::vec4(vertices[vertex], 1.0f);
          vertices[vertex] = t.xyz() / t.w;
        }
      }
    }

    visited.clear(); visited.resize(polygon_normal_vertices.size(), false);
    for (auto& polygon : polygon_normals)
    {
      for (const auto& vertex : polygon.vertices)
      {
        if (!visited[vertex])
        {
          visited[vertex] = true;
          glm::vec4 t = P * glm::vec4(polygon_normal_vertices[vertex], 1.0f);
          polygon_normal_vertices[vertex] = t.xyz() / t.w;
        }
      }
    }
    
    if (setting.enable_clip)
    {
      AABB aabb;
      aabb.vmin = Vertex(-1.0f, -1.0f, -1.0f);
      aabb.vmax = Vertex(1.0f, 1.0f, 1.0f);

      {
        int i = 0, j = polygons.size() - 1;
        while(i < j)
        {
          while(AABB::OverLap(aabb, AABB::From(vertices, polygons[i])) && i < j) { ++i;}
          while(!AABB::OverLap(aabb, AABB::From(vertices, polygons[j])) && i < j) { --j; }
          if (i >= j) { break; }
          std::swap(polygons[i], polygons[j]);
          ++i, --j;
        }

        if ((size_t)i == polygons.size() || !AABB::OverLap(aabb, AABB::From(vertices, polygons[i]))) { polygons.resize(i); }
        else { polygons.resize(i + 1); }
      }
      {
        int i = 0, j = polygon_normals.size() - 1;
        while(i < j)
        {
          while(AABB::OverLap(aabb, AABB::From(polygon_normal_vertices, polygon_normals[i])) && i < j) { ++i;}
          while(!AABB::OverLap(aabb, AABB::From(polygon_normal_vertices, polygon_normals[j])) && i < j) { --j; }
          if (i >= j) { break; }
          std::swap(polygon_normals[i], polygon_normals[j]);
          ++i, --j;
        }

        if ((size_t)i == polygon_normals.size() || !AABB::OverLap(aabb, AABB::From(polygon_normal_vertices, polygon_normals[i]))) { polygon_normals.resize(i); }
        else { polygon_normals.resize(i + 1); }
      }
    }

    glm::mat4 viewport = Transformer::Viewport(canvas);

    visited.clear(); visited.resize(vertices.size(), false);
    for (auto& polygon : polygons)
    {
      for (const auto& vertex : polygon.vertices)
      {
        if (!visited[vertex])
        {
          visited[vertex] = true;
          glm::vec4 t = viewport * glm::vec4(vertices[vertex], 1.0f);
          vertices[vertex] = t.xyz() / t.w;
        }
      }
    }
    visited.clear(); visited.resize(polygon_normal_vertices.size(), false);
    for (auto& polygon : polygon_normals)
    {
      for (const auto& vertex : polygon.vertices)
      {
        if (!visited[vertex])
        {
          visited[vertex] = true;
          glm::vec4 t = viewport * glm::vec4(polygon_normal_vertices[vertex], 1.0f);
          polygon_normal_vertices[vertex] = t.xyz() / t.w;
        }
      }
    }

    if (setting.display_mode == Setting::NORMAL)
    {
      if (setting.algorithm == Setting::ScanConvertZBuffer)
      {
        Rasterizer::RenderPolygonsScanConvertZBuffer(canvas, vertices, polygons);
      }
      else if (setting.algorithm == Setting::ScanConvertHZBuffer)
      {
        Rasterizer::RenderPolygonsScanConvertHZBuffer(canvas, vertices, polygons);
      }
      else if (setting.algorithm == Setting::ScanConvertHAABBHZBuffer)
      {
        auto haabbs = HAABB::Build(vertices, polygons);
        Rasterizer::RenderPolygonsScanConvertHAABBHZBuffer(canvas, vertices, polygons, haabbs);
        if (!setting.show_z_buffer && setting.show_aabb)
        {
          for (size_t i = 1; i < haabbs.size(); ++i)
          {
            glm::ivec2 vmin = glm::max(glm::ivec2(glm::round(haabbs[i].vmin)), glm::ivec2(0, 0));
            glm::ivec2 vmax = glm::min(glm::ivec2(glm::round(haabbs[i].vmax)), glm::ivec2(canvas.width-1, canvas.height-1));
            Rasterizer::RenderTangentBresenham(canvas, vmin.x, vmin.y, vmax.x, vmin.y, Color(1.0f, 0.0f, 0.0f));
            Rasterizer::RenderTangentBresenham(canvas, vmin.x, vmax.y, vmax.x, vmax.y, Color(1.0f, 0.0f, 0.0f));
            Rasterizer::RenderTangentBresenham(canvas, vmin.x, vmin.y, vmin.x, vmax.y, Color(1.0f, 0.0f, 0.0f));
            Rasterizer::RenderTangentBresenham(canvas, vmax.x, vmin.y, vmax.x, vmax.y, Color(1.0f, 0.0f, 0.0f));
          }
        }
      }
      else if (setting.algorithm == Setting::IntervalScanLine)
      {
        if (!setting.show_z_buffer)
        {
          Rasterizer::RenderPolygonsIntervalScanLine(canvas, vertices, polygons);
        }
      }
    }
    else
    {
      ASSERT(setting.display_mode == Setting::WIREFRAME);
      if (!setting.show_z_buffer)
      {
        Rasterizer::RenderPolygonsWireframe(canvas, vertices, polygons);
      }
    }
    
    if (!setting.show_z_buffer && setting.show_normal)
    {
      Rasterizer::RenderPolygonsWireframe(canvas, polygon_normal_vertices, polygon_normals);
    }

    if (setting.show_z_buffer)
    {
      for (int y = canvas.offsety; y < canvas.offsety + canvas.height; ++y)
      {
        for (int x = canvas.offsetx; x < canvas.offsetx + canvas.width; ++x)
        {
          if (canvas.z_buffer->buffer[y][x] == INF)
          {
            Rasterizer::RenderPixel(*canvas.frame_buffer, x, y, Rasterizer::MapColor(*canvas.frame_buffer, Color(0.0f)));
          }
          else
          {
            Rasterizer::RenderPixel(*canvas.frame_buffer, x, y, Rasterizer::MapColor(*canvas.frame_buffer, Color(canvas.z_buffer->buffer[y][x] / 2.0f)));
          }
        }
      }
    }
  }
}
