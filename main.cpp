#include <Common.h>
#include <Entity.h>
#include <Debugger.h>
#include <Initializer.h>
#include <Transformer.h>
#include <Shader.h>
#include <Rasterizer.h>
#include <Pipeline.h>
#include <Actor.h>

// COMMENT: The Layout Of The Main Window Is Below:
//
//  OffsetX=360, OffsetY=0
//        |               WIDTH=1280
//   +----+-----------------------------------------+----> Y Axis
//   |    |                                         |   ^
//   |    |                                         |   |
//   | UI |                Canvas                   | HEIGHT=960
//   |    |                                         |   |
//   |    |                                         |   V
//   +----+-----------------------------------------+----
//   | ^---------------- IMGUI_WIDTH=360
//   V
// X Axis

// COMMENT: Size Of The Main Rendering Canvas.
CONSTEXPR int WIDTH = 1280;
CONSTEXPR int HEIGHT = 960;

// COMMENT: Size Of The Debug UI.
CONSTEXPR int IMGUI_WIDTH = 360;
CONSTEXPR int IMGUI_HEIGHT = 960;

Canvas canvas;
Camera camera;
Scene scene;
Shader::Config config;
Setting setting;

void SetUpImGui(SDL_Window* window, SDL_Renderer* renderer) NOEXCEPT;
void ImGuiOnUpdate(SDL_Renderer* renderer) NOEXCEPT;
void ShutDownImGui() NOEXCEPT;

int main(const int argc, char** argv)
{
  
  // COMMENT: Disable UnUsed Variables Warning.
  (void)argc; (void)argv;

  // COMMENT: Initialize SDL Library.
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    Fatal("Can Not Init SDL!\n");
  }

  // COMMENT: Create The Main Window.
  SDL_Window* main_window = SDL_CreateWindow("CG Homework", IMGUI_WIDTH + WIDTH, HEIGHT,  SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
  if (main_window == nullptr)
  {
    Fatal("Can Not Create Window! %s\n", SDL_GetError());
  }

  // COMMENT: Create The ImGui Window.
  SDL_Window* imgui_window = SDL_CreatePopupWindow(main_window, 0, 0, IMGUI_WIDTH, HEIGHT, SDL_WINDOW_POPUP_MENU | SDL_WINDOW_HIDDEN);
  if (imgui_window == nullptr)
  {
    Fatal("Can Not Create Window! %s\n", SDL_GetError());
  }
  
  // COMMENT: Create A Render For ImGui.
  SDL_Renderer* renderer = SDL_CreateRenderer(imgui_window, nullptr);
  if (renderer == nullptr)
  {
    Fatal("Can Not Create Renderer! %s\n", SDL_GetError());
  }

  // COMMENT: Init Some Stuffs.
  Initializer::Init(canvas, IMGUI_WIDTH, 0, WIDTH, HEIGHT, main_window);
  Initializer::Init(camera);
  Initializer::Init(setting);
  
  // COMMENT: Set Up Basic Scene.
  scene.models.emplace_back(Loader::LoadObj((std::filesystem::path(STR(PROJECT_DIR)) / "Model" / "cube.obj").string().c_str()));
  scene.lights.emplace_back(Light {
    .position = glm::vec3(0.0f, 10.0f, 2.0f),
    .color = glm::vec3(1.0f),
  });
  
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

    Initializer::ReSet(canvas);
    
    // 绘制UI
    ImGuiOnUpdate(renderer);

    Actor::OnUpdate(camera);

    for (const auto& model : scene.models) {
      Pipeline::Render(setting, scene, model, camera, canvas, config);
    }
    
    SDL_UpdateWindowSurface(main_window);
  }

  // 释放UI
  ShutDownImGui();

  // 释放UI窗口
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(imgui_window);

  // 释放主窗口
  SDL_DestroySurface(canvas.surface);
  SDL_DestroyWindow(main_window);

  // 释放SDL
  SDL_Quit();

  return 0;
}

void UI() NOEXCEPT {

  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

  ImGui::ShowDemoWindow();
  
  ImGui::Begin("Controller");

  if (ImGui::CollapsingHeader("Help"))
  {
    ImGui::Indent(10.0f);
  
    ImGui::Text("Press W Move Forward");
    ImGui::Text("Press A Move Left");
    ImGui::Text("Press S Move Backward");
    ImGui::Text("Press D Move Right");
    ImGui::Text("Press Space Move Up");
    ImGui::Text("Press Shift Move Down");
    ImGui::Text("Hold Left Button, Rotate Model");
    ImGui::Text("Hold Right Button, Rotate Camera");
    ImGui::Text("Scroll Mouse Wheel, Scale Model");

    ImGui::Unindent(10.0f);
  }
  
  if (ImGui::CollapsingHeader("Settings"))
  {
    ImGui::Indent(10.0f);

    ImGui::Checkbox("Show Normal", &setting.show_normal);
    ImGui::Checkbox("Show Wireframe", &setting.show_wireframe);
    ImGui::Checkbox("Enable Cull", &setting.enable_cull);
    ImGui::Checkbox("Enable Clip", &setting.enable_clip);

    ImGui::Unindent(10.0f);
  }

  if (ImGui::CollapsingHeader("Camera"))
  {
    ImGui::Indent(10.0f);

    ImGui::Text("Position");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::Text("(%.1f, %.1f, %.1f)", camera.position.x, camera.position.y, camera.position.z);
    ImGui::Text("Direction");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::Text("(%.1f, %.1f, %.1f)", camera.direction.x, camera.direction.y, camera.direction.z);
    ImGui::Text("Up");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::Text("(%.1f, %.1f, %.1f)", camera.up.x, camera.up.y, camera.up.z);
    ImGui::Text("Right");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::Text("(%.1f, %.1f, %.1f)", camera.right.x, camera.right.y, camera.right.z);
    ImGui::Text("Yaw");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::Text("%.1f", glm::degrees(camera.yaw));
    ImGui::Text("Pitch");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::Text("%.1f", glm::degrees(camera.pitch));
    ImGui::SliderAngle("Fov", &camera.fov, 30.0f, 120.0f);
    ImGui::DragFloat("Aspect", &camera.aspect, 0.1f, 0.1f, 10.0f);
    ImGui::DragFloat("Near", &camera.near, 0.1f, 0.1f, camera.far);
    ImGui::DragFloat("Far", &camera.far, 0.1f, camera.near, 10.0f);

    if (ImGui::Button("Reset Camera", ImVec2(ImGui::GetWindowWidth() - 30.0f, 0.0f)))
    {
      Initializer::ReSet(camera);
    }

    ImGui::Unindent(10.0f);
  }

  if (ImGui::CollapsingHeader("Scene"))
  {
    ImGui::Indent(10.0f);

    // TODO
    // // open Dialog Simple
    // if (ImGui::Button("Open File Dialog")) {
    //   IGFD::FileDialogConfig config;
    //   config.path = ".";
    //   ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".cpp,.h,.hpp", config);
    // }
    // // display
    // if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
    //   if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
    //     std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
    //     std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
    //     // action
    //   }
    //   
    //   // close
    //   ImGuiFileDialog::Instance()->Close();
    // }
    // if (ImGui::CollapsingHeader("Model"))
    // {
    //   for (size_t i = 0; i < scene.models.size(); ++i)
    //   {
    //     if (ImGui::TreeNode(&scene.models[i], "%s", scene.models[i].name.c_str()))
    //     {
    //       ImGui::PushID(i);
    //
    //       ImGui::Text("Vertex Number: %lld\n", scene.models[i].vertices.size());
    //       
    //       ImGui::Text("Normal Number: %lld\n", scene.models[i].normals.size());
    //
    //       ImGui::DragFloat("Scale X", &scene.models[i].scale.x, 0.1f, 0.1f, 10.0f);
    //       ImGui::DragFloat("Scale Y", &scene.models[i].scale.y, 0.1f, 0.1f, 10.0f);
    //       ImGui::DragFloat("Scale Z", &scene.models[i].scale.z, 0.1f, 0.1f, 10.0f);
    //
    //       ImGui::DragFloat("Rotate X", &scene.models[i].rotate.x, 0.1f, -10.0f, 10.0f);
    //       ImGui::DragFloat("Rotate Y", &scene.models[i].rotate.y, 0.1f, -10.0f, 10.0f);
    //       ImGui::DragFloat("Rotate Z", &scene.models[i].rotate.z, 0.1f, -10.0f, 10.0f);
    //       
    //       ImGui::DragFloat("Translate X", &scene.models[i].translate.x, 0.1f, -10.0f, 10.0f);
    //       ImGui::DragFloat("Translate Y", &scene.models[i].translate.y, 0.1f, -10.0f, 10.0f);
    //       ImGui::DragFloat("Translate Z", &scene.models[i].translate.z, 0.1f, -10.0f, 10.0f);
    //
    //       if (ImGui::Button("Reset", ImVec2(250.0f, 0.0f)))
    //       {
    //         scene.models[i].scale = glm::vec3(1.0f);
    //         scene.models[i].rotate = glm::vec3(0.0f);
    //         scene.models[i].translate = glm::vec3(0.0f);
    //       }
    //       
    //       ImGui::PopID();
    //
    //       ImGui::TreePop();
    //     }
    //   }
    //
    // }
  
    // if (ImGui::CollapsingHeader("Lights"))
    // {
    //   for (size_t i = 0; i < scene.lights.size(); ++i)
    //   {
    //     if (ImGui::TreeNode(&scene.lights[i], "Light %lld", i))
    //     {
    //       ImGui::PushID(i);
    //
    //       ImGui::DragFloat("Light X", &scene.lights[i].position.x, 1.0f, -100.0f, 100.0f);
    //
    //       ImGui::DragFloat("Light Y", &scene.lights[i].position.y, 1.0f, -100.0f, 100.0f);
    //
    //       ImGui::DragFloat("Light Z", &scene.lights[i].position.z, 1.0f, -100.0f, 100.0f);
    //
    //       ImGui::ColorPicker3("Light Color", (float*)&scene.lights[i].color, ImGuiColorEditFlags_Float);
    //
    //       if (ImGui::Button("Reset", ImVec2(250.0f, 0.0f)))
    //       {
    //         scene.lights[i].position = glm::vec3(0.0f, 2.0f, 2.0f);
    //         scene.lights[i].color = glm::vec3(1.0f);
    //       }
    //       
    //       ImGui::PopID();
    //
    //       ImGui::TreePop();
    //     }
    //   }
    // }

    ImGui::Unindent(10.0f);
  }
  
  if (ImGui::CollapsingHeader("Shading"))
  {
    ImGui::Indent(10.0f);
    
    ImGui::Text("Color = ka * ambient + kd * diffuse + ks * specular^ps");
    ImGui::DragFloat("ka", &config.ka, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("kd", &config.kd, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("ks", &config.ks, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("ps", &config.ps, 0.01f, 0.0f, 10.0f);
    if (ImGui::Button("Reset Shading", ImVec2(ImGui::GetWindowWidth() - 30.0f, 0.0f)))
    {
      config.ka = 0.3f;
      config.kd = 0.3f;
      config.ks = 0.3f;
      config.ps = 2.5f;
    }

    ImGui::Unindent(10.0f);
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
