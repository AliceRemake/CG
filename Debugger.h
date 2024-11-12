/**
  ******************************************************************************
  * @file           : Debugger.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-8
  ******************************************************************************
  */



#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <Common.h>
#include <Entity.h>

// COMMENT: Output Entities For Debugging
struct Debugger
{
  
  static void Dump(const glm::vec3& v) NOEXCEPT
  {
    #ifdef NDEBUG
    (void)v;
    #else
    fmt::printf("%.3f %.3f %.3f\n", v.x ,v.y, v.z);
    fflush(stdout);
    #endif
  }

  static void Dump(const Line& line) NOEXCEPT
  {
    #ifdef NDEBUG
    (void)line;
    #else
    fmt::printf("Line:\n");
    // fmt::printf("  Vertex 0: "); Dump(line.vertices[0]);
    // fmt::printf("  Vertex 1: "); Dump(line.vertices[1]);
    fmt::printf("  Color  0: "); Dump(line.color);
    fflush(stdout);
    #endif
  }

  // static void Dump(const Triangle& triangle) NOEXCEPT
  // {
  //   #ifdef NDEBUG
  //   (void)triangle;
  //   #else
  //   fmt::printf("Triangle:\n");
  //   (void)triangle;
  //   // fmt::printf("  Vertex 0: "); Dump(triangle.vertices[0]);
  //   // fmt::printf("  Vertex 1: "); Dump(triangle.vertices[1]);
  //   // fmt::printf("  Vertex 2: "); Dump(triangle.vertices[2]);
  //   // fmt::printf("  Color  0: "); Dump(triangle.colors[0]);
  //   // fmt::printf("  Color  1: "); Dump(triangle.colors[1]);
  //   // fmt::printf("  Color  2: "); Dump(triangle.colors[2]);
  //   fflush(stdout);
  //   #endif
  // }

  static void Dump(const AABB& aabb) NOEXCEPT
  {
    #ifdef NDEBUG
    (void)aabb;
    #else
    fmt::printf("AABB:\n");
    fmt::printf("  Vertex Min: "); Dump(aabb.vmin);
    fmt::printf("  Vertex Max: "); Dump(aabb.vmax);
    fflush(stdout);
    #endif
  }

  static void Dump(const Model& model) NOEXCEPT
  {
    #ifdef NDEBUG
    (void)model;
    #else
    fmt::printf("Model:\n");
    fmt::printf("  Name         : %s\n", model.name);
    fmt::printf("  Vertex Number: %llu\n", model.vertices.size());
    fmt::printf("  Normal Number: %llu\n", model.normals.size());
    fmt::printf("  Index  Number: %llu\n", model.indices.size());
    fmt::printf("  Scale        : "); Dump(model.scale);
    fmt::printf("  Rotate       : "); Dump(model.rotate);
    fmt::printf("  Translate    : "); Dump(model.translate);
    fflush(stdout);
    #endif
  }

  static void Dump(const Light& light) NOEXCEPT
  {
    #ifdef NDEBUG
    (void)light;
    #else
    fmt::printf("Light:\n");
    fmt::printf("  Position: "); Dump(light.position);
    fmt::printf("  Color   : "); Dump(light.color);
    fflush(stdout);
    #endif
  }

  static void Dump(const Scene& scene) NOEXCEPT
  {
    #ifdef NDEBUG
    (void)scene;
    #else
    fmt::printf("Scene:\n");
    fmt::printf("  Model Number: %llu\n", scene.models.size());
    fmt::printf("  Light Number: %llu\n", scene.lights.size());
    fflush(stdout);
    #endif
  }

  static void Dump(const Camera& camera) NOEXCEPT
  {
    #ifdef NDEBUG
    (void)camera;
    #else
    fmt::printf("Camera:\n");
    fmt::printf("  Position : "); Dump(camera.position);
    fmt::printf("  Direction: "); Dump(camera.direction);
    fmt::printf("  Right    : "); Dump(camera.right);
    fmt::printf("  Up       : "); Dump(camera.up);
    fmt::printf("  Yaw      : %.3f\n", camera.yaw);
    fmt::printf("  Pitch    : %.3f\n", camera.pitch);
    fmt::printf("  Fov      : %.3f\n", camera.far);
    fmt::printf("  Aspect   : %.3f\n", camera.aspect);
    fmt::printf("  Near     : %.3f\n", camera.near);
    fmt::printf("  Far      : %.3f\n", camera.far);
    fflush(stdout);
    #endif
  }

  static void Dump(const Canvas& canvas) NOEXCEPT
  {
    #ifdef NDEBUG
    (void)canvas;
    #else
    fmt::printf("Canvas:\n");
    fmt::printf("  OffsetX            : %d\n", canvas.offsetx);
    fmt::printf("  OffsetY            : %d\n", canvas.offsety);
    fmt::printf("  Width              : %d\n", canvas.width);
    fmt::printf("  Height             : %d\n", canvas.height);
    fmt::printf("  Pixels             : 0x%P\n", (void*)canvas.pixels);
    fmt::printf("  ZBuffer            : 0x%P\n", (void*)canvas.zbuffer);
    fmt::printf("  Window             : 0x%P\n", (void*)canvas.window);
    fmt::printf("  Surface            : 0x%P\n", (void*)canvas.surface);
    fmt::printf("  PixelFormatDetails : 0x%P\n", (void*)canvas.pixel_format_details);
    fflush(stdout);
    #endif
  }

  static void Dump(const Setting& setting) NOEXCEPT
  {
    #ifdef NDEBUG
    (void)setting;
    #else
    fmt::printf("Setting:\n");
    fmt::printf("  Show Normal    : %d\n", setting.show_normal);
    fmt::printf("  Show Wireframe : %d\n", setting.show_wireframe);
    fmt::printf("  Enable Cull    : %d\n", setting.enable_cull);
    fmt::printf("  Enable Clip    : %d\n", setting.enable_clip);
    fflush(stdout);
    #endif
  }

  // static void Dump(const HZBufferNode* node) NOEXCEPT
  // {
  //   #ifdef NDEBUG
  //   (void)node;
  //   #else
  //   fmt::printf("HZBufferNode:\n");
  //   fmt::printf("  ll : 0x%P\n", node->ll);
  //   fmt::printf("  lr : 0x%P\n", node->lr);
  //   fmt::printf("  ul : 0x%P\n", node->ul);
  //   fmt::printf("  ur : 0x%P\n", node->ur);
  //   fflush(stdout);
  //   #endif
  // }
  
};

#endif //DEBUGGER_H
