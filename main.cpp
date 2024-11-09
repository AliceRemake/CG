#include <Common.h>
#include <Entity.h>
#include <Initializer.h>
#include <Shader.h>
#include <Pipeline.h>
#include <Actor.h>
#include <Controller.h>

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
  SDL_Window* controller_window = SDL_CreateWindow("Controller", CONTROLLER_WIDTH, CONTROLLER_HEIGHT, SDL_WINDOW_UTILITY | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
  if (controller_window == nullptr)
  {
    Fatal("Can Not Create Controller Window! %s\n", SDL_GetError());
  }
  
  // COMMENT: Create A Render For Controller. GPU Accelerated.
  SDL_Renderer* renderer = SDL_CreateRenderer(controller_window, nullptr);
  if (renderer == nullptr)
  {
    Fatal("Can Not Create Renderer! %s\n", SDL_GetError());
  }

  // COMMENT: Init Some Stuffs.
  Initializer::Init(canvas, 0, 0, RENDERER_WIDTH, RENDERER_HEIGHT, renderer_window);
  Initializer::Init(camera);
  Initializer::Init(setting);
  
  // COMMENT: Set Up Basic Scene.
  Model m;
  Loader::LoadObj((std::filesystem::path(STR(PROJECT_DIR)) / "Model" / "cube.obj").string().c_str(), m);
  scene.models.emplace_back(std::move(m));
  scene.lights.emplace_back(Light {
    .position = glm::vec3(0.0f, 2.0f, 2.0f),
    .color = glm::vec3(1.0f),
  });
  
  // COMMENT: Set Up Controller.
  Controller::SetUp(controller_window, renderer);
  
  // COMMENT: Everything Is Ready. Show The Window.
  SDL_ShowWindow(renderer_window);
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
    SDL_SetRenderDrawColorFloat(renderer, 0.40f, 0.45f, 0.50f, 1.0f);
    SDL_RenderClear(renderer);
    SDL_ClearSurface(canvas.surface, 0.40f, 0.45f, 0.50f, 1.0f);
    for (int i = 0; i < canvas.height; ++i)
    {
      for (int j = 0; j < canvas.width; ++j)
      {
        canvas.zbuffer[i][j] = 1E9;
      }
    }

    // COMMENT: Second. Update On Each Frame.
    Controller::OnUpdate(renderer);
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
  for (int i = 0; i < canvas.width; ++i)
  {
    delete[] canvas.zbuffer[i];  
  }
  delete[] canvas.zbuffer;
  SDL_DestroySurface(canvas.surface);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(controller_window);
  SDL_DestroyWindow(renderer_window);

  // COMMENT: Free SDL Library.
  SDL_Quit();

  return 0;
}
