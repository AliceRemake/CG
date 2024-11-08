/**
  ******************************************************************************
  * @file           : System.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-6
  ******************************************************************************
  */



#ifndef SYSTEM_H
#define SYSTEM_H

#include <Common.h>
#include <Entity.h>

struct Initializer {

  inline static void Init(Canvas& canvas, int offsetx, int offsety, int width, int height, SDL_Window* window) NOEXCEPT FORCE_INLINE {
    canvas.offsetx = offsetx;
    canvas.offsety = offsety;
    canvas.width = width;
    canvas.height = height;
    canvas.window = window;
    canvas.surface = SDL_GetWindowSurface(window);
    canvas.pixel_format_details = SDL_GetPixelFormatDetails(canvas.surface->format);
    canvas.pixels = (uint32_t*)canvas.surface->pixels;
    canvas.zbuffer = new float*[width];
    for (int i = 0; i < width; ++i) {
      canvas.zbuffer[i] = new float[height];
    }
  }
  
};

struct Transform {

  // 缩放变换矩阵
   static glm::mat4 Scale(const glm::vec3& v) NOEXCEPT  ;

  // 旋转变换矩阵
   static glm::mat4 RotateX(float radians) NOEXCEPT  ;
   static glm::mat4 RotateY(float radians) NOEXCEPT  ;
   static glm::mat4 RotateZ(float radians) NOEXCEPT  ;

  // 平移变换矩阵
   static glm::mat4 Translate(const glm::vec3& v) NOEXCEPT  ;

  // 模型局部坐标变换矩阵
   static glm::mat4 Model(const Model& model) NOEXCEPT  ;

  // 相机视图变换矩阵
   static glm::mat4 View(const Camera& camera) NOEXCEPT;
  // 相机投影变换矩阵
   static glm::mat4 Project(const Camera& camera) NOEXCEPT  ;

  // 屏幕视点变换矩阵
   static glm::mat4 Viewport(const Canvas& canvas) NOEXCEPT  ;

};

struct Rasterizer {

private:
   static uint32_t MapColor(const Canvas&canvas, const Color& color) ;

   static void RenderPixel(const Canvas& canvas, int i, int j, const Color& color) ;
  
   static void RenderSegment(const Canvas& canvas, int i, int jmin, int jmax, const Color& color) ;

  static void RenderRect(const Canvas& canvas, int imin, int jmin, int imax, int jmax, const Color& color) noexcept;

  static void RenderLineDDA(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color);

  static void RenderLineBresenham(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color);

public:
  static void RenderLine(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color) noexcept;

  static void RenderTriangleLine(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const glm::ivec2& p2, const Color& color) noexcept;
  
  static void RenderTriangleFill(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const glm::ivec2& p2, const Color& color) noexcept;

};

struct Shader {
  struct BlinnPhongConfig {
    float ambient_ratio = 0.3f;
    Color ambient_color = {0.3f, 0.3f, 0.3f};
    float diffuse_ratio = 0.3f;
    float specular_ratio = 0.3f;
    float specular_pow = 2.5f;
  };

  static Color BlinnPhongShading(const Camera& camera, const std::vector<Light>& lights, const Triangle& triangle, const BlinnPhongConfig& config) noexcept {
    const Color ambient = config.ambient_ratio * config.ambient_color;
    auto diffuse = glm::vec3(0.0f);
    auto specular = glm::vec3(0.0f);
    for (const auto& light : lights) {
      const glm::vec3 i = glm::normalize(light.position - triangle.center);
      const glm::vec3 o = glm::normalize(camera.position - triangle.center);
      const glm::vec3 h = glm::normalize(i + o);
      diffuse += config.diffuse_ratio * std::max(0.0f, glm::dot(i, o)) * light.color;
      specular += config.specular_ratio * std::pow(std::max(0.0f, glm::dot(h, triangle.normal)), config.specular_pow) * light.color;
    }
    return ambient + diffuse + specular;
  }
  
};

struct Pipeline {

  static void Transform(
    const Model& model,
    const Camera& camera,
    std::vector<Vertex>& vertices,
    std::vector<Normal>& normals
  ) NOEXCEPT {
    vertices.resize(model.vertices.size());
    normals.resize(model.normals.size());
    glm::mat4 vertex_transform = Transform::View(camera) * Transform::Model(model);
    glm::mat4 normal_transform = glm::transpose(glm::inverse(vertex_transform));
    for (size_t i = 0; i < vertices.size(); ++i) {
      glm::vec4 vertex = vertex_transform * glm::vec4(model.vertices[i], 1.0f);
      vertices[i] = vertex.xyz() / vertex.w;
    }
    for (size_t i = 0; i < normals.size(); ++i) {
      glm::vec4 normal = normal_transform * glm::vec4(model.normals[i], 0.0f);
      normals[i] = normal.xyz();
    }
  }

  static void Assembly(
    const std::vector<Vertex>& vertices,
    const std::vector<Mesh>& meshes,
    const Setting::CullMode cull_mode,
    std::vector<Line>& lines,
    std::vector<Triangle>& triangles
  ) NOEXCEPT {
    size_t size = 0;
    for (const auto& mesh : meshes) {
      size += mesh.indices.size() / 3;
    }
    lines.reserve(size);
    triangles.reserve(size);
    for (const auto& mesh : meshes) {
      for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        Triangle triangle {
          vertices[mesh.indices[i+0].vertex],
          vertices[mesh.indices[i+1].vertex],
          vertices[mesh.indices[i+2].vertex],
        };
        triangle.center = (triangle.vertices[0] + triangle.vertices[1] + triangle.vertices[2]) / 3.0f;
        const glm::vec3 u = triangle.vertices[0] - triangle.vertices[1];
        const glm::vec3 v = triangle.vertices[1] - triangle.vertices[2];
        triangle.normal = glm::normalize(glm::cross(u, v));
        if (cull_mode == Setting::CULL_MODE_BACK && glm::dot(triangle.center, triangle.normal) >= 0.0f) { continue; }
        lines.emplace_back(Line{triangle.center, triangle.center + 0.1f * triangle.normal});
        triangles.emplace_back(triangle);
      }
    }
  }

  static void Project(
    const Camera& camera,
    const Setting::ClipMode clip_mode,
    std::vector<Line>& lines,
    std::vector<Triangle>& triangles
  ) NOEXCEPT {
    glm::mat4 project = Transform::Project(camera);
    for (auto& line : lines) {
      for (auto& vertex : line.vertices) {
        glm::vec4 t = project * glm::vec4(vertex, 1.0f);
        vertex = t.xyz() / t.w;
      }
    }
    for (auto& triangle : triangles) {
      for (auto& vertex : triangle.vertices) {
        glm::vec4 t = project * glm::vec4(vertex, 1.0f);
        vertex = t.xyz() / t.w;
      }
    }
    if (clip_mode == Setting::CLIP_MODE_VIEW) {
      std::vector<bool> clipped;
      { // COMMENT: Clip Triangles
        clipped.resize(triangles.size());
        for (size_t i = 0; i < triangles.size(); ++i) {
          AABB aabb = {
            glm::min(glm::min(triangles[i].vertices[0], triangles[i].vertices[1]), triangles[i].vertices[2]),
            glm::max(glm::max(triangles[i].vertices[0], triangles[i].vertices[1]), triangles[i].vertices[2]),
          };
          if (std::min(std::abs(aabb.vmin.x), std::abs(aabb.vmax.x)) <= 1.0f && std::min(std::abs(aabb.vmin.y), std::abs(aabb.vmax.y)) <= 1.0f && std::min(std::abs(aabb.vmin.z), std::abs(aabb.vmax.z)) <= 1.0f) {
            clipped[i] = false;         
          } else {
            clipped[i] = true;
          }
        }
        int i = 0, j = triangles.size() - 1;
        while(i < j) {
          while(!clipped[i] && i <= j) ++i;
          while(clipped[j] && i <= j) --j;
          if (i >= j) break; 
          std::swap(triangles[i], triangles[j]);
          ++i, --j;
        }
        triangles.resize(std::max(j + 1, i - 1));
      }
      { // COMMENT: Clip Lines
        clipped.resize(lines.size());
        for (size_t i = 0; i < lines.size(); ++i) {
          AABB aabb = {
            glm::min(lines[i].vertices[0], lines[i].vertices[1]),
            glm::max(lines[i].vertices[0], lines[i].vertices[1]),
          };
          if (std::min(std::abs(aabb.vmin.x), std::abs(aabb.vmax.x)) <= 1.0f && std::min(std::abs(aabb.vmin.y), std::abs(aabb.vmax.y)) <= 1.0f && std::min(std::abs(aabb.vmin.z), std::abs(aabb.vmax.z)) <= 1.0f) {
            clipped[i] = false;         
          } else {
            clipped[i] = true;
          }
        }
        int i = 0, j = lines.size() - 1;
        while(i < j) {
          while(!clipped[i] && i <= j) ++i;
          while(clipped[j] && i <= j) --j;
          if (i >= j) break; 
          std::swap(lines[i], lines[j]);
          ++i, --j;
        }
        lines.resize(std::max(j + 1, i - 1));
      }
    }
  }
  
  static void RasterizationLine(
    const Setting& app_state,
    const Canvas& canvas,
    std::vector<Line>& lines,
    std::vector<Triangle>& triangles
  ) NOEXCEPT {
    glm::mat4 viewport = Transform::Viewport(canvas);
    for (auto& line : lines) {
      for (auto& vertex : line.vertices) {
        glm::vec4 t = viewport * glm::vec4(vertex, 1.0f);
        vertex = t.xyz() / t.w;
      }
    }
    for (auto& triangle : triangles) {
      for (auto& vertex : triangle.vertices) {
        glm::vec4 t = viewport * glm::vec4(vertex, 1.0f);
        vertex = t.xyz() / t.w;
      }
    }
    for (const auto& triangle : triangles) {
      Rasterizer::RenderTriangleLine(canvas, 
      glm::ivec2(glm::round(triangle.vertices[0].xy())),  
      glm::ivec2(glm::round(triangle.vertices[1].xy())),  
      glm::ivec2(glm::round(triangle.vertices[2].xy())), 
      glm::vec3{1.0f, 1.0f, 1.0f}
    );
    }
    if (app_state.show_normal) {
      for (const auto& line : lines) {
        Rasterizer::RenderLine(canvas,
          glm::ivec2(glm::round(line.vertices[0])),
          glm::ivec2(glm::round(line.vertices[1])),
          glm::vec3{0.0f, 1.0f, 0.0f}
        );
      }
    }
  }

  static void RasterizationBlinnPhong(
    const Setting& app_state,
    const Camera& camera,
    const Canvas& canvas,
    const std::vector<Light>& lights,
    const Shader::BlinnPhongConfig& config,
    std::vector<Line>& lines,
    std::vector<Triangle>& triangles
  ) NOEXCEPT {
    glm::mat4 viewport = Transform::Viewport(canvas);
    for (auto& line : lines) {
      for (auto& vertex : line.vertices) {
        glm::vec4 t = viewport * glm::vec4(vertex, 1.0f);
        vertex = t.xyz() / t.w;
      }
    }
    for (auto& triangle : triangles) {
      for (auto& vertex : triangle.vertices) {
        glm::vec4 t = viewport * glm::vec4(vertex, 1.0f);
        vertex = t.xyz() / t.w;
      }
    }
    for (const auto& triangle : triangles) {
      Rasterizer::RenderTriangleFill(canvas,
        glm::ivec2(glm::round(triangle.vertices[0].xy())),
        glm::ivec2(glm::round(triangle.vertices[1].xy())),
        glm::ivec2(glm::round(triangle.vertices[2].xy())),
        Shader::BlinnPhongShading(camera, lights, triangle, config)
      );
    }
    if (app_state.show_normal) {
      for (const auto& line : lines) {
        Rasterizer::RenderLine(canvas,
          glm::ivec2(glm::round(line.vertices[0])),
          glm::ivec2(glm::round(line.vertices[1])),
          glm::vec3{0.0f, 1.0f, 0.0f}
        );
      }
    }
  }
};

struct Actor {

  static void OnEvent(Model& m, const SDL_Event* event) noexcept {
    if (event->type == SDL_EVENT_MOUSE_WHEEL) {
      if (event->wheel.y > 0) {
        m.scale *= glm::vec3{1.1f, 1.1f, 1.1f};
      }
      if (event->wheel.y < 0) {
        m.scale *= glm::vec3{0.9f, 0.9f, 0.9f};
      }
    } else if (event->type == SDL_EVENT_MOUSE_MOTION && event->motion.state & SDL_BUTTON_LMASK) {
      constexpr float ratio = 0.01f; 
      m.rotate.y += ratio * event->motion.xrel;
      m.rotate.x -= ratio * event->motion.yrel;
    }
  }

  static void OnUpdate(Camera& camera) NOEXCEPT {
    camera.direction = glm::normalize(glm::vec3{
      glm::cos(camera.pitch) * glm::sin(camera.yaw),
      glm::sin(camera.pitch),
      glm::cos(camera.pitch) * glm::cos(camera.yaw),
    });
  }
  
  static void OnEvent(Camera& camera, const SDL_Event* event) noexcept {
    if (event->type == SDL_EVENT_KEY_DOWN) {
      if (event->key.key == SDLK_W) {
        camera.position += 0.1f * camera.direction;
      }
      if (event->key.key == SDLK_S) {
        camera.position -= 0.1f * camera.direction;
      }
    } else if (event->type == SDL_EVENT_MOUSE_MOTION && event->motion.state & SDL_BUTTON_RMASK) {
      camera.yaw = camera.yaw - 0.001f * event->motion.xrel;
      camera.pitch = std::clamp(camera.pitch + 0.001f * event->motion.yrel, -89.0f, 89.0f);
    }
  }
  
};

#endif //SYSTEM_H
