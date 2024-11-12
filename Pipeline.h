/**
  ******************************************************************************
  * @file           : Pipeline.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-8
  ******************************************************************************
  */



#ifndef PIPELINE_H
#define PIPELINE_H

#include <Common.h>
#include <Entity.h>
#include <Transformer.h>
#include <Shader.h>
#include <Rasterizer.h>

// COMMENT: Pipeline System. For Rendering A Scene.
struct Pipeline
{

  // COMMENT: Grab All Parameters, And Do The Rendering.
  // 1. Transform All Vertices And Normals Into Camera Space.
  // 2. In Camera Space. Do The Cull, Shading. Then Assembly Primitives.
  // 3. Transform All Primitives Into [-1, 1]^3.
  // 4. In [-1, 1]^3. Do The Clip.
  // 5. Transform All Primitives Into Screen Space.
  // 6. Render Primitives.
  static void Render(const Setting& setting, const Scene& scene, const Model& model, const Camera& camera, const Canvas& canvas, const Shader::Config& config) NOEXCEPT
  {
    // COMMENT: Buffers Used In Rendering.
    static std::vector<Light> lights;
    static std::vector<Vertex> vertices;
    static std::vector<Normal> normals;
    static std::vector<Line> lines;
    // static std::vector<Triangle> triangles;
    static std::vector<struct Polygon> polygons;
    
    // COMMENT: Clear All Data From Last Frame.
    lights.clear();
    vertices.clear();
    normals.clear();
    lines.clear();
    // triangles.clear();
    polygons.clear();

    // COMMENT: Pre Allocate Memories.
    lights.resize(scene.lights.size());
    vertices.resize(model.vertices.size());
    // normals.resize(model.normals.size());

    // COMMENT: Pre Calculate Transform Matrices.
    const glm::mat4 view = Transformer::View(camera);
    const glm::mat4 vertex_matrix = view * Transformer::Model(model);
    // const glm::mat4 normal_matrix = glm::transpose(glm::inverse(vertex_matrix));

    // COMMENT: Transform Lights From World Space To Camera Space.
    // TODO: Parallel.
    for (size_t i = 0; i < lights.size(); ++i) {
      glm::vec4 position = view * glm::vec4(scene.lights[i].position, 1.0f);
      lights[i].position = position.xyz() / position.w;
      lights[i].color = scene.lights[i].color;
    }

    // COMMENT: Transform Vertices From Model Space To Camera Space.
    // TODO: Parallel.
    for (size_t i = 0; i < vertices.size(); ++i) {
      glm::vec4 vertex = vertex_matrix * glm::vec4(model.vertices[i], 1.0f);
      vertices[i] = vertex.xyz() / vertex.w;
    }

    // COMMENT: Pre Allocate Memories.
    lines.reserve(model.polygons.size());
    // triangles.reserve(model.polygons.size());
    polygons.reserve(model.polygons.size());

    // COMMENT: Assembly Geometry Primitives. eg: Lines, Triangles, Polygons.
    for (size_t i = 0, j = 0; i < model.polygons.size(); j += model.polygons[i], ++i)
    {
      ASSERT(model.polygons[i] >= 3);

      struct Polygon polygon;

      // COMMENT: Assembly Polygon Vertices.
      auto c = glm::vec3(0.0f);
      polygon.vertices.reserve(model.polygons[i]);
      for (size_t k = 0; k < model.polygons[i]; ++k)
      {
        polygon.vertices.emplace_back(model.indices[j+k].vertex);
        c += vertices[polygon.vertices.back()];
      }

      c /= model.polygons[i];
      const glm::vec3 u = vertices[polygon.vertices[0]] - vertices[polygon.vertices[1]];
      const glm::vec3 v = vertices[polygon.vertices[1]] - vertices[polygon.vertices[2]];
      const glm::vec3 n = glm::normalize(glm::cross(u, v));

      // COMMENT: Camera Is At Origin Point Now. Cull Triangles Back To Us.
      if (setting.enable_cull) {
        if (glm::dot(c, n) >= 0.0f) {
          continue;
        }
      }

      // COMMENT: Use Blinn-Phong Shading Model To Calculate Color Of The Polygon.
      polygon.color = Shader::BlinnPhong(lights, c, n, config);

      // COMMENT: If We Want To Draw Face Normal, Assembly Corresponding Line.
      if (setting.show_normal)
      {
        vertices.emplace_back(c);
        vertices.emplace_back(c + 0.1f * n);
        lines.emplace_back(Line{ (uint32_t)vertices.size() - 2, (uint32_t)vertices.size() - 1, Color(0.0f, 1.0f, 0.0f)});
      }
        
      // COMMENT: The Properties Of A Polygon Are Calculated. Push In.
      polygons.emplace_back(std::move(polygon));
    }
    
    // COMMENT: Pre Calculate Transform Matrices.
    glm::mat4 project = Transformer::Project(camera);

    // TODO: Parallel.
    for (auto& vertex : vertices)
    {
      glm::vec4 t = project * glm::vec4(vertex, 1.0f);
      vertex = t.xyz() / t.w;
    }

    if (setting.enable_clip)
    {
      if (!lines.empty()) {
        // COMMENT: Util Function For Judging Whether A Line Should Be Clipped.
        const auto clip = [&](const Line& line) -> bool
        {
          // COMMENT: If The AABB Has No Intersection With [-1, 1]^3, Then Clip.
          AABB aabb = AABB::From(vertices, line);
          glm::vec3 v = glm::min(glm::abs(aabb.vmin), glm::abs(aabb.vmax));
          return !(v.x < 1.0f && v.y < 1.0f && v.z < 1.0f);
        };

        // COMMENT: Find The First Clipped And The Last UnClipped. Swap Them And Continue.
        int i = 0, j = lines.size() - 1;
        while(i < j)
        {
          while(!clip(lines[i]) && i < j) ++i;
          while(clip(lines[j]) && i < j) --j;
          if (i >= j) break;
          std::swap(lines[i], lines[j]);
          ++i, --j;
        }
        
        // COMMENT: Re Calculate The Size.
        if (clip(lines[i])) lines.resize(i);
        else lines.resize(i + 1);
      }
      if (!polygons.empty()) {
        // COMMENT: Util Function For Judging Whether A Polygon Should Be Clipped.
        const auto clip = [&](const struct Polygon& polygon) -> bool
        {
          // COMMENT: If The AABB Has No Intersection With [-1, 1]^3, Then Clip.
          AABB aabb = AABB::From(vertices, polygon);
          glm::vec3 v = glm::min(glm::abs(aabb.vmin), glm::abs(aabb.vmax));
          return !(v.x < 1.0f && v.y < 1.0f && v.z < 1.0f);
        };

        // COMMENT: Find The First Clipped And The Last UnClipped. Swap Them And Continue.
        int i = 0, j = polygons.size() - 1;
        while(i < j)
        {
          while(!clip(polygons[i]) && i < j) ++i;
          while(clip(polygons[j]) && i < j) --j;
          if (i >= j) break;
          std::swap(polygons[i], polygons[j]);
          ++i, --j;
        }
        
        // COMMENT: Re Calculate The Size.
        if (clip(polygons[i])) polygons.resize(i);
        else polygons.resize(i + 1);
      }
    }

    // COMMENT: Pre Calculate Transform Matrices.
    glm::mat4 viewport = Transformer::Viewport(canvas);

    // TODO: Parallel.
    for (auto& vertex : vertices)
    {
      glm::vec4 t = viewport * glm::vec4(vertex, 1.0f);
      vertex = t.xyz() / t.w;
    }

    if (setting.algorithm == Setting::ScanConvertZBuffer)
    {
      Rasterizer::RenderPolygonsScanConvertZBuffer(canvas, vertices, polygons);
    }
    else if (setting.algorithm == Setting::IntervalScanLine)
    {
      Rasterizer::RenderPolygonsIntervalScanLine(canvas, vertices, polygons);
    }
    else if (setting.algorithm == Setting::ScanConvertHZBuffer)
    {
      Rasterizer::RenderPolygonsScanConvertHZBuffer(canvas, vertices, polygons);
    }
    
    Rasterizer::RenderLines(canvas, vertices, lines);
  
    if (setting.show_wireframe)
    {
      Rasterizer::RenderPolygonsWireframe(canvas, vertices, polygons);
    }
  }
  
};

#endif //PIPELINE_H


// COMMENT: Transform Normals From Model Space To Camera Space.
// for (size_t i = 0; i < normals.size(); ++i) {
//   glm::vec4 normal = normal_matrix * glm::vec4(model.normals[i], 0.0f);
//   normals[i] = normal.xyz();
// }
// {
//   // COMMENT: Util Function For Judging Whether A Triangle Should Be Clipped.
//   const auto clip = [&](const Triangle& triangle) -> bool
//   {
//     // COMMENT: If The AABB Has No Intersection With [-1, 1]^3, Then Clip.
//     AABB aabb = AABB::From(vertices, triangle);
//     glm::vec3 v = glm::min(glm::abs(aabb.vmin), glm::abs(aabb.vmax));
//     return !(v.x < 1.0f && v.y < 1.0f && v.z < 1.0f);
//   };
//
//   // COMMENT: Find The First Clipped And The Last UnClipped. Swap Them And Continue.
//   int i = 0, j = triangles.size() - 1;
//   while(i < j)
//   {
//     while(!clip(triangles[i]) && i < j) ++i;
//     while(clip(triangles[j]) && i < j) --j;
//     if (i >= j) break;
//     std::swap(triangles[i], triangles[j]);
//     ++i, --j;
//   }
//   
//   // COMMENT: Re Calculate The Size.
//   if (clip(triangles[i])) triangles.resize(i);
//   else triangles.resize(i + 1);
// }
// for (auto& triangle : triangles)
// {
//   if (setting.show_wireframe)
//   {
//     Rasterizer::RenderTriangleWireframe(canvas, vertices, triangle);
//   }
// }
      // // COMMENT: This Is A Triangle.
      // if (model.polygons[i] == 3)
      // {
      //   // COMMENT: Assembly Triangle Vertices.
      //   Triangle triangle {
      //     .vertices = {
      //       model.indices[j+0].vertex,
      //       model.indices[j+1].vertex,
      //       model.indices[j+2].vertex,
      //     },
      //   };
      //
      //   // COMMENT: Calculate Center And Normal Of The Triangle.
      //   const glm::vec3 c = (vertices[triangle.vertices[0]] + vertices[triangle.vertices[1]] + vertices[triangle.vertices[2]]) / 3.0f;
      //   const glm::vec3 u = vertices[triangle.vertices[0]] - vertices[triangle.vertices[1]];
      //   const glm::vec3 v = vertices[triangle.vertices[1]] - vertices[triangle.vertices[2]];
      //   const glm::vec3 n = glm::normalize(glm::cross(u, v));
      //
      //   // COMMENT: Camera Is At Origin Point Now. Cull Triangles Back To Us.
      //   if (setting.enable_cull) {
      //     if (glm::dot(c, n) >= 0.0f) {
      //       continue;
      //     }
      //   }
      //   
      //   // COMMENT: Departure
      //   // COMMENT: Use Blinn-Phong Shading Model To Calculate Colors Of The Triangle's Vertices.
      //   // triangle.colors[0] = Shader::BlinnPhong(lights, triangle.vertices[0], normals[model.indices[i+0].normal], config);
      //   // triangle.colors[1] = Shader::BlinnPhong(lights, triangle.vertices[1], normals[model.indices[i+1].normal], config);
      //   // triangle.colors[2] = Shader::BlinnPhong(lights, triangle.vertices[2], normals[model.indices[i+2].normal], config);
      //
      //   // COMMENT: Use Blinn-Phong Shading Model To Calculate Color Of The Triangle.
      //   triangle.color = Shader::BlinnPhong(lights, c, n, config);
      //   
      //   // COMMENT: If We Want To Draw Face Normal, Assembly Corresponding Line.
      //   if (setting.show_normal)
      //   {
      //     vertices.emplace_back(c);
      //     vertices.emplace_back(c + 0.1f * n);
      //     lines.emplace_back(Line{ (uint32_t)vertices.size() - 2, (uint32_t)vertices.size() - 1, Color(0.0f, 1.0f, 0.0f)});
      //   }
      //   
      //   // COMMENT: The Properties Of A Triangle Are Calculated. Push In.
      //   triangles.emplace_back(triangle);
      //   
      // }
      // // COMMENT: This Is A Polygon.
      // else
      // {
      // }

    // // COMMENT: Transform Lines From Camera Space To [-1, 1]^3
    // for (auto& line : lines) {
    //   for (auto& vertex : line.vertices) {
    //     glm::vec4 t = project * glm::vec4(vertex, 1.0f);
    //     vertex = t.xyz() / t.w;
    //   }
    // }
    //
    // // COMMENT: Transform Triangles From Camera Space To [-1, 1]^3
    // for (auto& triangle : triangles) {
    //   for (auto& vertex : triangle.vertices) {
    //     glm::vec4 t = project * glm::vec4(vertex, 1.0f);
    //     vertex = t.xyz() / t.w;
    //   }
    // }
    //
    // // COMMENT: Transform Polygons From Camera Space To [-1, 1]^3
    // for (auto& polygon : polygons) {
    //   for (auto& vertex : polygon.vertices) {
    //     glm::vec4 t = project * glm::vec4(vertex, 1.0f);
    //     vertex = t.xyz() / t.w;
    //   }
    // }
