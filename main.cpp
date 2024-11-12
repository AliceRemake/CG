#include <Common.h>
#include <Entity.h>
#include <Initializer.h>
#include <Shader.h>
#include <Pipeline.h>
#include <Actor.h>
#include <Controller.h>
#include <Loader.h>
#include <Debugger.h>

// COMMENT: Size Of The Renderer.
CONSTEXPR int RENDERER_WIDTH = 1280;
CONSTEXPR int RENDERER_HEIGHT = 960;

// COMMENT: Size Of The Controller.
CONSTEXPR int CONTROLLER_WIDTH = 600;
CONSTEXPR int CONTROLLER_HEIGHT = 960;

// COMMENT: Entities For Controller To Modify.
Canvas canvas;
Camera camera;
Scene scene;
Shader::Config config;
Setting setting;
Model* selected_model;

int main(const int argc, char** argv)
{
  
  // COMMENT: Disable UnUsed Variables Warning.
  (void)argc; (void)argv;

  // COMMENT: Initialize SDL Library.
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    Fatal("Can Not Init SDL!\n");
  }

  // COMMENT: Create The Renderer Window.
  SDL_Window* renderer_window = SDL_CreateWindow("Renderer", RENDERER_WIDTH, RENDERER_HEIGHT,  SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
  if (renderer_window == nullptr)
  {
    Fatal("Can Not Create cWindow! %s\n", SDL_GetError());
  }

  // COMMENT: Create The Controller Window.
  SDL_Window* controller_window = SDL_CreateWindow("Controller", CONTROLLER_WIDTH, CONTROLLER_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
  if (controller_window == nullptr)
  {
    Fatal("Can Not Create Controller Window! %s\n", SDL_GetError());
  }
  
  // COMMENT: Create A Render For Controller. GPU Accelerated.
  SDL_Renderer* controller_renderer = SDL_CreateRenderer(controller_window, nullptr);
  if (controller_renderer == nullptr)
  {
    Fatal("Can Not Create Renderer! %s\n", SDL_GetError());
  }

  // COMMENT: Init Some Stuffs.
  setting.show_normal = false;
  setting.show_wireframe = false;
  setting.enable_cull = true;
  setting.enable_clip = true;
  setting.algorithm = Setting::ScanConvertHZBuffer;

  canvas.offsetx = 0;
  canvas.offsety = 0;
  canvas.width = RENDERER_WIDTH;
  canvas.height = RENDERER_HEIGHT;
  canvas.window = renderer_window;
  canvas.surface = SDL_GetWindowSurface(canvas.window);
  canvas.pixel_format_details = SDL_GetPixelFormatDetails(canvas.surface->format);
  canvas.pixels = (uint32_t*)canvas.surface->pixels;
  canvas.zbuffer = new float*[canvas.height];
  for (int i = 0; i < canvas.height; ++i)
  {
    canvas.zbuffer[i] = new float[canvas.width];
  }
  canvas.z = 1E9;
  canvas.h_zbuffer_tree = Accelerator::BuildHZBufferTree(canvas, 0, canvas.height-1, 0, canvas.width-1);
  
  camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
  camera.direction = glm::vec3(0.0f, 0.0f, -1.0f);
  camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
  camera.right = glm::vec3(1.0f, 0.0f, 0.0f);
  camera.yaw = glm::radians(180.0f);
  camera.pitch = 0.0f;
  camera.fov = glm::radians(75.0f);
  camera.aspect = 4.0f / 3.0f;
  camera.near = 0.1f;
  camera.far = 10.0f;

  // AABB aabb = { glm::vec3{0, 0, 0}, glm::vec3{1, 1, 1} };
  // Transformer::TransformAABB(aabb, Transformer::RotateZ(glm::radians(30.0f)));
  // Debugger::Dump(aabb.vmin);
  // Debugger::Dump(aabb.vmax);
  
  // COMMENT: Set Up Basic Scene.
  Model m;
  Loader::LoadObj((std::filesystem::path(STR(PROJECT_DIR)) / "Model" / "bunny.obj").string().c_str(), m);
  scene.models.emplace_back(std::move(m));
  scene.lights.emplace_back(Light {
    .position = glm::vec3(0.0f, 1000.0f, 1000.0f),
    .color = glm::vec3(1.0f),
  });
  selected_model = &scene.models.front();
  
  // COMMENT: Set Up Controller.
  Controller::SetUp(controller_window, controller_renderer);
  
  // COMMENT: Everything Is Ready. Show The Window.
  SDL_SetWindowPosition(renderer_window, 0, 100);
  SDL_ShowWindow(renderer_window);
  SDL_SetWindowPosition(controller_window, RENDERER_WIDTH, 100);
  SDL_ShowWindow(controller_window);
  
  // COMMENT: Main Loop.
  bool running = true;
  while(running)
  {
    // COMMENT: Handle Event.
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      // COMMENT: Whenever Request A Close To Any Of The Windows, Quit The Whole Application.
      if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
      {
        running = false;
        break;
      }
      // COMMENT: Pass The Event To Controller.
      if (event.window.windowID == SDL_GetWindowID(controller_window))
      {
        Controller::OnEvent(&event);
        continue;
      }
      // COMMENT: Pass The Event To Renderer.
      if (event.window.windowID == SDL_GetWindowID(renderer_window))
      {
        if (selected_model != nullptr)
        {
          Actor::OnEvent(*selected_model, &event);
        }
        Actor::OnEvent(camera, &event);
      }
    }
      
    // COMMENT: If The Window Is Minimized. Sleep For A Second. And Check Out Whether Itself Has Been Restored.
    if (SDL_GetWindowFlags(renderer_window) & SDL_WINDOW_MINIMIZED)
    {
      SDL_Delay(10);
      continue;
    }

    // COMMENT: First. Clear Data From Last Frame.
    SDL_ClearSurface(canvas.surface, canvas.color.r, canvas.color.g, canvas.color.b, 1.0f);
    if (setting.algorithm == Setting::ScanConvertZBuffer || setting.algorithm == Setting::ScanConvertHZBuffer)
    {
      for (int i = 0; i < canvas.height; ++i)
      {
        for (int j = 0; j < canvas.width; ++j)
        {
          canvas.zbuffer[i][j] = canvas.z;
        }
      }
    }
    if (setting.algorithm == Setting::ScanConvertHZBuffer)
    {
      Accelerator::ReSetHZBufferTree(canvas.h_zbuffer_tree);  
    }

    // COMMENT: Second. Update On Each Frame.
    Controller::OnUpdate(controller_renderer);
    Actor::OnUpdate(canvas);
    Actor::OnUpdate(camera);

    // COMMENT: Third. Do Rendering.
    for (const auto& model : scene.models)
    {
      Pipeline::Render(setting, scene, model, camera, canvas, config);
    }
    
    // COMMENT: Finally. Show Image.
    SDL_UpdateWindowSurface(renderer_window);
  }

  // COMMENT: Shut Down Controller.
  Controller::ShutDown();
  
  // COMMENT: Free All Resources Created.
  for (int i = 0; i < canvas.height; ++i)
  {
    delete[] canvas.zbuffer[i];  
  }
  delete[] canvas.zbuffer;
  Accelerator::DestroyHZBufferTree(canvas.h_zbuffer_tree);
  SDL_DestroySurface(canvas.surface);
  SDL_DestroyRenderer(controller_renderer);
  SDL_DestroyWindow(controller_window);
  SDL_DestroyWindow(renderer_window);

  // COMMENT: Free SDL Library.
  SDL_Quit();

  return 0;
}
