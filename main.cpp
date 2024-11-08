#include <Common.h>
#include <Entity.h>
#include <System.h>

CONSTEXPR int WIDTH = 1280;
CONSTEXPR int HEIGHT = 960;
CONSTEXPR int IMGUI_WIDTH = 360;

Scene scene;
Camera camera;
Canvas canvas;
Shader::BlinnPhongConfig config;
Setting app_state;

void SetUpImGui(SDL_Window* window, SDL_Renderer* renderer) NOEXCEPT;
void ImGuiOnUpdate(SDL_Renderer* renderer) NOEXCEPT;
void ShutDownImGui() NOEXCEPT;

int main(const int argc, char** argv) {

  (void)argc; (void)argv;

  // 初始化SDL库
  if (!SDL_Init(SDL_INIT_VIDEO)) { Fatal("Can Not Init SDL!\n"); }

  // 创建主窗口
  SDL_Window* main_window = SDL_CreateWindow("CG Homework", IMGUI_WIDTH + WIDTH, HEIGHT,  SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
  if (main_window == nullptr) { Fatal("Can Not Create Window! %s\n", SDL_GetError()); }
  SDL_Surface* surface =  SDL_GetWindowSurface(main_window);
  if (surface == nullptr) { Fatal("Can Not Get Window Surface! %s\n", SDL_GetError()); }

  // 创建UI窗口
  SDL_Window* imgui_window = SDL_CreatePopupWindow(main_window, 0, 0, 360, 960, SDL_WINDOW_POPUP_MENU | SDL_WINDOW_HIDDEN);
  if (imgui_window == nullptr) { Fatal("Can Not Create Window! %s\n", SDL_GetError()); }
  SDL_Renderer* renderer = SDL_CreateRenderer(imgui_window, nullptr);
  if (renderer == nullptr) { Fatal("Can Not Create Renderer! %s\n", SDL_GetError()); }

  tinyobj::ObjReader reader;
  reader.ParseFromFile((std::filesystem::path(STR(PROJECT_DIR)) / "Model" / "bun_zipper_res2.obj").string());

  Model m;
  m.name = "suzanne";
  m.vertices.resize(reader.GetAttrib().vertices.size() / 3);
  memcpy(m.vertices.data(), reader.GetAttrib().vertices.data(), m.vertices.size() * sizeof(Vertex));
  m.normals.resize(reader.GetAttrib().normals.size() / 3);
  memcpy(m.normals.data(), reader.GetAttrib().normals.data(), m.normals.size() * sizeof(Normal));
  const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
  m.meshes.resize(shapes.size());
  for (size_t i = 0; i < m.meshes.size(); ++i) {
    m.meshes[i].indices.resize(shapes[i].mesh.indices.size());
    for (size_t j = 0; j < m.meshes[i].indices.size(); ++j) {
      m.meshes[i].indices[j].vertex = shapes[i].mesh.indices[j].vertex_index;
      m.meshes[i].indices[j].normal = shapes[i].mesh.indices[j].normal_index;
    }
  }
  m.scale = glm::vec3(1.0f);
  m.rotate = glm::vec3(0.0f);
  m.translate = glm::vec3(0.0f);

  scene.models.emplace_back(std::move(m));
  scene.lights.emplace_back(Light {
    .position = glm::vec3(0.0f, 10.0f, 2.0f),
    .color = glm::vec3(1.0f),
  });
  
  camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
  camera.yaw      = glm::radians(180.0f);
  camera.pitch    = glm::radians(0.0f);
  camera.fov      = glm::radians(75.0f);
  camera.aspect   = (float)WIDTH / (float)HEIGHT;
  camera.near     = 1.0f;
  camera.far      = 10.0f;
  Actor::OnUpdate(camera);

  Initializer::Init(canvas, IMGUI_WIDTH, 0, WIDTH, HEIGHT, main_window);
  // canvas.window = main_window;
  // canvas.offsetx = IMGUI_WIDTH;
  // canvas.offsety = 0;
  // canvas.width = WIDTH;
  // canvas.height = HEIGHT;
  // canvas.surface = surface;
  // canvas.pixel_format_details = SDL_GetPixelFormatDetails(surface->format);
  // canvas.pixels = (Uint32*)surface->pixels;

  app_state.cull_mode = Setting::CULL_MODE_BACK;
  app_state.clip_mode = Setting::CLIP_MODE_VIEW;
  app_state.polygon_mode = Setting::POLYGON_MODE_FILL;
  app_state.shading_model = Setting::SHADING_MODEL_BLINN_PHONG;

  // 创建UI
  SetUpImGui(imgui_window, renderer);

  // 准备完毕，显示窗口
  SDL_ShowWindow(main_window);
  SDL_ShowWindow(imgui_window);
  
  // 主循环
  bool running = true;
  while(running) {
    // 事件处理
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      // UI窗口处理事件
      if (event.window.windowID == SDL_GetWindowID(imgui_window)) { ImGui_ImplSDL3_ProcessEvent(&event); continue; }
      // 退出程序
      if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(main_window))) { running = false; }
      // 主窗口处理事件
      Actor::OnEvent(scene.models[0], &event);

      Actor::OnEvent(camera, &event);
    }
      
    // 程序最小化
    if (SDL_GetWindowFlags(main_window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    SDL_SetRenderDrawColorFloat(renderer, 0.0f, 0.0f, 0.0f, 0.0f);
    SDL_RenderClear(renderer);
    SDL_ClearSurface(surface, 0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < canvas.width; ++i) {
      memset(canvas.zbuffer[i], 0xFF, sizeof(int) * canvas.height);
    }
    
    // 绘制UI
    ImGuiOnUpdate(renderer);

    Actor::OnUpdate(camera);


    
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<Line> lines;
    std::vector<Triangle> triangles;
    for (const auto& model : scene.models) {
      Pipeline::Transform(model, camera, vertices, normals);
      Pipeline::Assembly(vertices, model.meshes, app_state.cull_mode, lines, triangles);
      Pipeline::Project(camera, app_state.clip_mode, lines, triangles);
      if (app_state.polygon_mode == Setting::POLYGON_MODE_LINE) {
        Pipeline::RasterizationLine(app_state, canvas, lines, triangles);
      } else if (app_state.polygon_mode == Setting::POLYGON_MODE_FILL) {
        if (app_state.shading_model == Setting::SHADING_MODEL_BLINN_PHONG) {
          Pipeline::RasterizationBlinnPhong(app_state, camera, canvas, scene.lights, config, lines, triangles);          
        }
      }
    }
    
    SDL_UpdateWindowSurface(main_window);
  }

  // 释放UI
  ShutDownImGui();

  // 释放UI窗口
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(imgui_window);

  // 释放主窗口
  SDL_DestroySurface(surface);
  SDL_DestroyWindow(main_window);

  // 释放SDL
  SDL_Quit();

  return 0;
}

void UI() NOEXCEPT {

  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
  
  ImGui::Begin("Controller");

  if (ImGui::CollapsingHeader("Help"))
  {
    ImGui::Text("Press `W`/`S` Move Close/Away");
    ImGui::Text("Hold Left Button, Rotate Model");
    ImGui::Text("Hold Right Button, Rotate Camera");
  }
  
  if (ImGui::CollapsingHeader("Settings"))
  {
    ImGui::Checkbox("Show Normal", &app_state.show_normal);
    
    const char* cull_mode_names[] = { "none", "back" };
    ImGui::Combo("Cull Mode", (int*)&app_state.cull_mode, cull_mode_names, 2);
    
    const char* clip_mode_names[] = { "none", "view" };
    ImGui::Combo("Clip Mode", (int*)&app_state.clip_mode, clip_mode_names, 2);
    
    const char* polygon_mode_names[] = { "line", "fill" };
    ImGui::Combo("Polygon Mode", (int*)&app_state.polygon_mode, polygon_mode_names, 2);

    const char* shading_model_name[] = { "Blinn-Phong" };
    ImGui::Combo("Shading Mode", (int*)&app_state.shading_model, shading_model_name, 1);
  }

  if (ImGui::CollapsingHeader("Camera"))
  {
    ImGui::SliderAngle("fov", &camera.fov, 30.0f, 120.0f);

    ImGui::DragFloat("aspect", &camera.aspect, 0.1f, 10.0f);
    
    ImGui::DragFloat("near", &camera.near, 0.0f, camera.far);

    ImGui::DragFloat("far", &camera.far, camera.near, 10.0f);

    if (ImGui::Button("Reset", ImVec2(250.0f, 0.0f)))
    {
      camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
      camera.yaw      = glm::radians(180.0f);
      camera.pitch    = glm::radians(0.0f);
      camera.fov      = glm::radians(75.0f);
      camera.aspect   = (float)WIDTH / (float)HEIGHT;
      camera.near     = 1.0f;
      camera.far      = 10.0f;
    }
  }

  if (ImGui::CollapsingHeader("Scene"))
  {
    ImGui::Indent(10.0f);
    
    if (ImGui::CollapsingHeader("Model"))
    {
      for (size_t i = 0; i < scene.models.size(); ++i)
      {
        if (ImGui::TreeNode(&scene.models[i], "%s", scene.models[i].name.c_str()))
        {
          ImGui::PushID(i);

          ImGui::Text("Vertex Number: %lld\n", scene.models[i].vertices.size());
          
          ImGui::Text("Normal Number: %lld\n", scene.models[i].normals.size());

          ImGui::DragFloat("Scale X", &scene.models[i].scale.x, 0.1f, 0.1f, 10.0f);
          ImGui::DragFloat("Scale Y", &scene.models[i].scale.y, 0.1f, 0.1f, 10.0f);
          ImGui::DragFloat("Scale Z", &scene.models[i].scale.z, 0.1f, 0.1f, 10.0f);

          ImGui::DragFloat("Rotate X", &scene.models[i].rotate.x, 0.1f, -10.0f, 10.0f);
          ImGui::DragFloat("Rotate Y", &scene.models[i].rotate.y, 0.1f, -10.0f, 10.0f);
          ImGui::DragFloat("Rotate Z", &scene.models[i].rotate.z, 0.1f, -10.0f, 10.0f);
          
          ImGui::DragFloat("Translate X", &scene.models[i].translate.x, 0.1f, -10.0f, 10.0f);
          ImGui::DragFloat("Translate Y", &scene.models[i].translate.y, 0.1f, -10.0f, 10.0f);
          ImGui::DragFloat("Translate Z", &scene.models[i].translate.z, 0.1f, -10.0f, 10.0f);

          if (ImGui::Button("Reset", ImVec2(250.0f, 0.0f)))
          {
            scene.models[i].scale = glm::vec3(1.0f);
            scene.models[i].rotate = glm::vec3(0.0f);
            scene.models[i].translate = glm::vec3(0.0f);
          }
          
          ImGui::PopID();

          ImGui::TreePop();
        }
      }

    }
  
    if (ImGui::CollapsingHeader("Lights"))
    {
      for (size_t i = 0; i < scene.lights.size(); ++i)
      {
        if (ImGui::TreeNode(&scene.lights[i], "Light %lld", i))
        {
          ImGui::PushID(i);

          ImGui::DragFloat("Light X", &scene.lights[i].position.x, 1.0f, -100.0f, 100.0f);

          ImGui::DragFloat("Light Y", &scene.lights[i].position.y, 1.0f, -100.0f, 100.0f);

          ImGui::DragFloat("Light Z", &scene.lights[i].position.z, 1.0f, -100.0f, 100.0f);

          ImGui::ColorPicker3("Light Color", (float*)&scene.lights[i].color, ImGuiColorEditFlags_Float);

          if (ImGui::Button("Reset", ImVec2(250.0f, 0.0f)))
          {
            scene.lights[i].position = glm::vec3(0.0f, 2.0f, 2.0f);
            scene.lights[i].color = glm::vec3(1.0f);
          }
          
          ImGui::PopID();

          ImGui::TreePop();
        }
      }
    }

    ImGui::Unindent(10.0f);
  }
  
  if (ImGui::CollapsingHeader("Shading"))
  {
    if (ImGui::CollapsingHeader("Blinn-Phong"))
    {
      if (ImGui::TreeNode("Ambient"))
      {

        ImGui::SliderFloat("ambient ratio", &config.ambient_ratio, 0.0f, 1.0f);

        ImGui::ColorPicker3("ambient color", (float*)&config.ambient_color, ImGuiColorEditFlags_Float);

        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Diffuse"))
      {
        ImGui::SliderFloat("diffuse ratio", &config.diffuse_ratio, 0.0f, 1.0f);

        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Specular"))
      {
        ImGui::SliderFloat("specular ratio", &config.specular_ratio, 0.0f, 1.0f);

        ImGui::SliderFloat("specular pow", &config.specular_pow, 1.0f, 10.0f);

        ImGui::TreePop();
      }

      if (ImGui::Button("Reset##Blinn-Phong", ImVec2(250.0f, 0.0f)))
      {
        config.ambient_ratio = 0.3f;
        config.ambient_color = {0.3f, 0.3f, 0.3f};
        config.diffuse_ratio = 0.3f;
        config.specular_ratio = 0.3f;
        config.specular_pow = 2.5f;
      }
    }
  }

  ImGui::End();
}

void SetUpImGui(SDL_Window* window, SDL_Renderer* renderer) NOEXCEPT {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui::StyleColorsLight();

  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  io.Fonts->Clear();
  io.Fonts->AddFontFromFileTTF((std::filesystem::path(STR(PROJECT_DIR)) / "Font" / "Roboto-Medium.ttf").string().c_str(), 20.0f);

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 5.3f;
  style.FrameRounding = 2.3f;
  style.ScrollbarRounding = 0;
}

void ImGuiOnUpdate(SDL_Renderer* renderer) NOEXCEPT {
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  UI();
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
  SDL_RenderPresent(renderer);
}

void ShutDownImGui() NOEXCEPT {
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}
