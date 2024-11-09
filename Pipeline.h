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
    static std::vector<Triangle> triangles;

    // COMMENT: Clear All Data From Last Frame.
    lights.clear();
    vertices.clear();
    normals.clear();
    lines.clear();
    triangles.clear();

    // COMMENT: Pre Allocate Memories.
    lights.resize(scene.lights.size());
    vertices.resize(model.vertices.size());
    normals.resize(model.normals.size());

    // COMMENT: Pre Calculate Transform Matrices.
    const glm::mat4 view = Transformer::View(camera);
    const glm::mat4 vertex_matrix = view * Transformer::Model(model);
    const glm::mat4 normal_matrix = glm::transpose(glm::inverse(vertex_matrix));

    // COMMENT: Transform Lights From World Space To Camera Space.
    for (size_t i = 0; i < lights.size(); ++i) {
      glm::vec4 position = view * glm::vec4(scene.lights[i].position, 1.0f);
      lights[i].position = position.xyz() / position.w;
      lights[i].color = scene.lights[i].color;
    }

    // COMMENT: Transform Vertices From Model Space To Camera Space.
    for (size_t i = 0; i < vertices.size(); ++i) {
      glm::vec4 vertex = vertex_matrix * glm::vec4(model.vertices[i], 1.0f);
      vertices[i] = vertex.xyz() / vertex.w;
    }

    // COMMENT: Transform Vertices From Model Space To Camera Space.
    for (size_t i = 0; i < normals.size(); ++i) {
      glm::vec4 normal = normal_matrix * glm::vec4(model.normals[i], 0.0f);
      normals[i] = normal.xyz();
    }

    // COMMENT: Pre Allocate Memories.
    lines.reserve(model.indices.size());
    triangles.reserve(model.indices.size());

    // COMMENT: Assembly Primitives eg: Lines, Triangles etc.
    for (size_t i = 0; i < model.indices.size(); i += 3) {

      // COMMENT: Assembly Triangle Vertices.
      Triangle triangle {
        .vertices = {
          vertices[model.indices[i+0].vertex],
          vertices[model.indices[i+1].vertex],
          vertices[model.indices[i+2].vertex],
        },
      };

      // COMMENT: Calculate Center And Normal Of The Triangle.
      const glm::vec3 c = (triangle.vertices[0] + triangle.vertices[1] + triangle.vertices[2]) / 3.0f;
      const glm::vec3 u = triangle.vertices[0] - triangle.vertices[1];
      const glm::vec3 v = triangle.vertices[1] - triangle.vertices[2];
      const glm::vec3 n = glm::normalize(glm::cross(u, v));

      // COMMENT: Camera Is At Origin Point Now. Cull Triangles Back To Us.
      if (setting.enable_cull) {
        if (glm::dot(c, n) >= 0.0f) {
          continue;
        }
      }

      // COMMENT: Use Blinn-Phong Shading Model To Calculate Colors Of The Triangle's Vertices.
      triangle.colors[0] = Shader::BlinnPhong(lights, triangle.vertices[0], normals[model.indices[i+0].normal], config);
      triangle.colors[1] = Shader::BlinnPhong(lights, triangle.vertices[1], normals[model.indices[i+1].normal], config);
      triangle.colors[2] = Shader::BlinnPhong(lights, triangle.vertices[2], normals[model.indices[i+2].normal], config);

      // COMMENT: If We Want To Draw Face Normal, Assembly Corresponding Line.
      if (setting.show_normal)
      {
        lines.emplace_back(Line{c, c + 0.1f * n, Color(0.0f, 1.0f, 0.0f)});
      }
      
      // COMMENT: The Properties Of A Triangle Are Calculated. Push In.
      triangles.emplace_back(triangle);
    }

    // COMMENT: Pre Calculate Transform Matrices.
    glm::mat4 project = Transformer::Project(camera);

    // COMMENT: Transform Lines From Camera Space To [-1, 1]^3
    for (auto& line : lines) {
      for (auto& vertex : line.vertices) {
        glm::vec4 t = project * glm::vec4(vertex, 1.0f);
        vertex = t.xyz() / t.w;
      }
    }
    
    // COMMENT: Transform Triangles From Camera Space To [-1, 1]^3
    for (auto& triangle : triangles) {
      for (auto& vertex : triangle.vertices) {
        glm::vec4 t = project * glm::vec4(vertex, 1.0f);
        vertex = t.xyz() / t.w;
      }
    }

    if (setting.enable_clip)
    {
      if (setting.show_normal)
      {
        // COMMENT: Util Function For Judging Whether A Line Should Be Clipped.
        const auto clip = [](const Line& line) -> bool
        {
          // COMMENT: If The AABB Has No Intersection With [-1, 1]^3, Then Clip.
          AABB aabb = AABB::From(line);
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
          std::swap(triangles[i], triangles[j]);
          ++i, --j;
        }
        
        // COMMENT: Re Calculate The Size.
        if (clip(lines[i])) lines.resize(i);
        else lines.resize(i + 1);
      }

      // COMMENT: Util Function For Judging Whether A Triangle Should Be Clipped.
      const auto clip = [](const Triangle& triangle) -> bool
      {
        // COMMENT: If The AABB Has No Intersection With [-1, 1]^3, Then Clip.
        AABB aabb = AABB::From(triangle);
        glm::vec3 v = glm::min(glm::abs(aabb.vmin), glm::abs(aabb.vmax));
        return !(v.x < 1.0f && v.y < 1.0f && v.z < 1.0f);
      };

      // COMMENT: Find The First Clipped And The Last UnClipped. Swap Them And Continue.
      int i = 0, j = triangles.size() - 1;
      while(i < j)
      {
        while(!clip(triangles[i]) && i < j) ++i;
        while(clip(triangles[j]) && i < j) --j;
        if (i >= j) break;
        std::swap(triangles[i], triangles[j]);
        ++i, --j;
      }
      
      // COMMENT: Re Calculate The Size.
      if (clip(triangles[i])) triangles.resize(i);
      else triangles.resize(i + 1);
    }

    // COMMENT: Pre Calculate Transform Matrices.
    glm::mat4 viewport = Transformer::Viewport(canvas);

    if (setting.show_normal)
    {
      // COMMENT: Transform Each Line From [-1, 1]^3 To Screen Space. Then Render It.
      for (auto& line : lines)
      {
        for (auto& vertex : line.vertices)
        {
          glm::vec4 t = viewport * glm::vec4(vertex, 1.0f);
          vertex = t.xyz() / t.w;
        }
        Rasterizer::RenderLine(canvas, glm::round(line.vertices[0].xy()), glm::round(line.vertices[1].xy()), line.color);
      }
    }
    
    for (auto& triangle : triangles)
    {
      // COMMENT: Transform Each Triangle From [-1, 1]^3 To Screen Space. Then Render It.
      for (auto& vertex : triangle.vertices)
      {
        glm::vec4 t = viewport * glm::vec4(vertex, 1.0f);
        vertex = t.xyz() / t.w;
      }
      Rasterizer::RenderTriangleFill(
        canvas,
        glm::round(triangle.vertices[0].xy()), glm::round(triangle.vertices[1].xy()), glm::round(triangle.vertices[2].xy()),
        triangle.vertices[0].z, triangle.vertices[1].z, triangle.vertices[2].z,
        triangle.colors[0], triangle.colors[1], triangle.colors[2]
      );
      if (setting.show_wireframe)
      {
        Rasterizer::RenderTriangleLine(canvas, glm::round(triangle.vertices[0].xy()), glm::round(triangle.vertices[1].xy()), glm::round(triangle.vertices[2].xy()), glm::vec3(0.0f));
      }
    }
  }
  
};

#endif //PIPELINE_H
