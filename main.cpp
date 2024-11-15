#include <Common.h>
#include <Entity.h>
#include <Pipeline.h>
#include <Actor.h>
#include <Controller.h>
#include <Acceleration/HZBuffer.h>

CONSTEXPR int RENDERER_WIDTH = 800;
CONSTEXPR int RENDERER_HEIGHT = 600;

CONSTEXPR int CONTROLLER_WIDTH = 600;
CONSTEXPR int CONTROLLER_HEIGHT = 600;

Setting setting;
Shader::Config config;
FrameBuffer frame_buffer;
ZBuffer z_buffer;
Canvas canvas;
Camera camera;
Scene scene;
Model* selected_model;
ParallelLight* selected_parallel_light;
PointLight* selected_point_light;
size_t frame_time;

int main(const int argc, char** argv)
{
  (void)argc; (void)argv;
  
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    Fatal("Can Not Init SDL!\n");
  }

  SDL_Window* renderer_window = SDL_CreateWindow("Renderer", RENDERER_WIDTH, RENDERER_HEIGHT,  SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
  if (renderer_window == nullptr)
  {
    Fatal("Can Not Create cWindow! %s\n", SDL_GetError());
  }

  SDL_Window* controller_window = SDL_CreateWindow("Controller", CONTROLLER_WIDTH, CONTROLLER_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
  if (controller_window == nullptr)
  {
    Fatal("Can Not Create Controller Window! %s\n", SDL_GetError());
  }
  
  SDL_Renderer* controller_renderer = SDL_CreateRenderer(controller_window, nullptr);
  if (controller_renderer == nullptr)
  {
    Fatal("Can Not Create Renderer! %s\n", SDL_GetError());
  }

  setting.show_aabb     = true;
  setting.show_normal   = false;
  setting.show_z_buffer = false;
  setting.enable_cull   = true;
  setting.enable_clip   = true;
  setting.algorithm     = Setting::ScanConvertZBuffer;
  setting.display_mode  = Setting::NORMAL;

  config.ka = 0.1f;
  config.kd = 0.5f;
  config.ks = 0.4f;
  config.ps = 2.5f;
  
  frame_buffer     = FrameBuffer::From(renderer_window, Color(0.0f, 0.0f, 0.0f));
  frame_buffer.bgc = Color(0.70f, 0.60f, 0.80f);

  z_buffer = ZBuffer::From(frame_buffer, INF);

  canvas.offsetx      = 0;
  canvas.offsety      = 0;
  canvas.width        = RENDERER_WIDTH;
  canvas.height       = RENDERER_HEIGHT;
  canvas.frame_buffer = &frame_buffer;
  canvas.z_buffer     = &z_buffer;
  canvas.h_z_buffer   = HZBuffer::Build(canvas, 0, canvas.width-1, 0, canvas.height-1);

  camera.position  = Vertex(0.0f, 0.0f, 2.0f);
  camera.direction = Vector(0.0f, 0.0f, -1.0f);
  camera.up        = Vector(0.0f, 1.0f, 0.0f);
  camera.right     = Vector(1.0f, 0.0f, 0.0f);
  camera.yaw       = glm::radians(180.0f);
  camera.pitch     = 0.0f;
  camera.fov       = (float)RENDERER_WIDTH / (float)RENDERER_HEIGHT;
  camera.aspect    = 4.0f / 3.0f;
  camera.near      = 0.1f;
  camera.far       = 100.0f;

  Model model;
  Loader::LoadObj((std::filesystem::path(STR(PROJECT_DIR)) / "Model" / "geodesic_dual_classIII_20_7.obj").string().c_str(), model);
  
  ParallelLight parallel_light_0;
  parallel_light_0.direction = Vector(1.0f, -1.0f, 0.0f);
  parallel_light_0.color     = Color(1.0f , 0.5f, 0.5f);

  ParallelLight parallel_light_1;
  parallel_light_1.direction = Vector(1.0f, 1.0f, 0.0f);
  parallel_light_1.color     = Color(0.5f , 0.5f, 1.0f);

  PointLight point_light_0;
  point_light_0.position = Vertex(2.0f, 2.0f, 0.0f),
  point_light_0.color    = Color(0.5f , 1.0f, 0.5f);

  PointLight point_light_1;
  point_light_1.position = Vertex(2.0f, -2.0f, 0.0f),
  point_light_1.color    = Color(1.0f , 1.0f, 1.0f);

  scene.models.emplace_back(model);
  scene.parallel_lights.emplace_back(parallel_light_0);
  scene.parallel_lights.emplace_back(parallel_light_1);
  scene.point_lights.emplace_back(point_light_0);
  scene.point_lights.emplace_back(point_light_1);

  selected_model = &scene.models.front();
  selected_parallel_light = &scene.parallel_lights.front();
  selected_point_light = &scene.point_lights.front();
  
  Controller::SetUp(controller_window, controller_renderer);
  
  SDL_SetWindowPosition(renderer_window, 0, 100);
  SDL_ShowWindow(renderer_window);
  SDL_SetWindowPosition(controller_window, RENDERER_WIDTH, 100);
  SDL_ShowWindow(controller_window);
  
  bool running = true;
  while(running)
  {
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
      {
        running = false;
        break;
      }
      if (event.window.windowID == SDL_GetWindowID(controller_window))
      {
        Controller::OnEvent(&event);
        continue;
      }
      if (event.window.windowID == SDL_GetWindowID(renderer_window))
      {
        if (selected_model != nullptr)
        {
          Actor::OnEvent(*selected_model, &event);
        }
        Actor::OnEvent(camera, &event);
      }
    }
      
    if (SDL_GetWindowFlags(renderer_window) & SDL_WINDOW_MINIMIZED)
    {
      SDL_Delay(10);
      continue;
    }

    FrameBuffer::Clear(frame_buffer);

    // COMMENT: Begin Render. 

    auto start_time = std::chrono::high_resolution_clock::now();
    
    switch (setting.algorithm)
    {
      case Setting::ScanConvertZBuffer:
        ZBuffer::Clear(z_buffer);
      break;
      case Setting::ScanConvertHZBuffer: 
      case Setting::ScanConvertHAABBHZBuffer:
        ZBuffer::Clear(z_buffer);
        HZBuffer::Clear(canvas.h_z_buffer, canvas);
      break;
      case Setting::IntervalScanLine: 
      break;
      default:
        Fatal("Unsupported Algorithm");
    }

    Controller::OnUpdate(controller_renderer);
    Actor::OnUpdate(camera);

    Pipeline::Render(setting, config, canvas, camera, scene);

    auto end_time = std::chrono::high_resolution_clock::now();

    frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // COMMENT: End Render. 

    FrameBuffer::Display(frame_buffer);
  }

  Controller::ShutDown();

  SDL_Quit();

  return 0;
}
