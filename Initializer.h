/**
  ******************************************************************************
  * @file           : Initializer.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-8
  ******************************************************************************
  */



#ifndef INITIALIZER_H
#define INITIALIZER_H

#include <Common.h>
#include <Entity.h>
#include <Loader.h>

// COMMENT: Initializer System. For Initialize And Reset Entities.
struct Initializer
{

  static void ReSet(Model& model) NOEXCEPT
  {
    model.scale = glm::vec3(1.0f);
    model.rotate = glm::vec3(0.0f);
    model.translate = glm::vec3(0.0f);
  }  

  static void Init(Light& light) NOEXCEPT
  {
    light.position = glm::vec3(0.0f, 2.0f, 2.0f);
    light.color = glm::vec3(1.0f, 1.0f, 1.0f);
  }

  static void ReSet(Light& light) NOEXCEPT
  {
    Init(light);
  }

  static void Init(Camera& camera) NOEXCEPT
  {
    camera.position = glm::vec3(0.0f, 0.0f, 2.0f);
    camera.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.right = glm::vec3(1.0f, 0.0f, 0.0f);
    camera.yaw = glm::radians(180.0f);
    camera.pitch = 0.0f;
    camera.fov = glm::radians(75.0f);
    camera.aspect = 4.0f / 3.0f;
    camera.near = 1.0f;
    camera.far = 10.0f;
  }

  static void ReSet(Camera& camera) NOEXCEPT
  {
    camera.position = glm::vec3(0.0f, 0.0f, 2.0f);
    camera.direction = glm::vec3(0.0f, 0.0f, 1.0f);
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.right = glm::vec3(-1.0f, 0.0f, 0.0f);
    camera.yaw = glm::radians(180.0f);
    camera.pitch = 0.0f;
    camera.fov = glm::radians(75.0f);
    camera.aspect = 4.0f / 3.0f;
    camera.near = 1.0f;
    camera.far = 10.0f;
  }
  
  static void Init(Canvas& canvas, const int offsetx, const int offsety, const int width, const int height, SDL_Window* window) NOEXCEPT
  {
    canvas.offsetx = offsetx;
    canvas.offsety = offsety;
    canvas.width = width;
    canvas.height = height;
    canvas.window = window;
    canvas.surface = SDL_GetWindowSurface(window);
    if (canvas.surface == nullptr)
    {
      Fatal("Can Not Init Canvas.\n");  
    }
    canvas.pixel_format_details = SDL_GetPixelFormatDetails(canvas.surface->format);
    if (canvas.pixel_format_details == nullptr)
    {
      Fatal("Can Not Init Canvas.\n");  
    }
    canvas.pixels = (uint32_t*)canvas.surface->pixels;
    canvas.zbuffer = new float*[height];
    for (int i = 0; i < height; ++i)
    {
      canvas.zbuffer[i] = new float[width];
    }
  }

  static void ReSet(const Canvas& canvas) NOEXCEPT
  {
    // COMMENT: Clear Surface.
    SDL_ClearSurface(canvas.surface, 0.0f, 0.0f, 0.0f, 0.0f);
    
    // COMMENT: Clear ZBuffer.
    for (int i = 0; i < canvas.height; ++i)
    {
      for (int j = 0; j < canvas.width; ++j)
      {
        canvas.zbuffer[i][j] = 1E9;
      }
    }
  }

  static void Init(Setting& setting) NOEXCEPT
  {
    setting.show_normal = true;
    setting.show_wireframe = true;
    setting.enable_cull = true;
    setting.enable_clip = true;
  }

  static void ReSet(Setting& setting) NOEXCEPT
  {
    setting.show_normal = true;
    setting.show_wireframe = true;
    setting.enable_cull = true;
    setting.enable_clip = true;
  }
  
};

#endif //INITIALIZER_H
