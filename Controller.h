/**
  ******************************************************************************
  * @file           : Controller.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-9
  ******************************************************************************
  */



#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Common.h>
#include <Entity.h>
#include <Shader.h>
#include <Loader.h>
#include <Initializer.h>

extern Canvas canvas;
extern Camera camera;
extern Scene scene;
extern Shader::Config config;
extern Setting setting;
extern Model* selected_model;

// COMMENT: Controller System. For Debugging And Playing.
struct Controller
{
  // COMMENT: Copy From ImGui Examples.
  static void SetUp(SDL_Window* window, SDL_Renderer* renderer) NOEXCEPT
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsLight();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    Style();
  }

  // COMMENT: Copy From ImGui Examples.
  static void ShutDown() NOEXCEPT
  {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
  }

  // COMMENT: Copy From ImGui Examples.
  static void OnEvent(const SDL_Event* event) NOEXCEPT
  {
    ImGui_ImplSDL3_ProcessEvent(event);
  }

  // COMMENT: Copy From ImGui Examples.
  static void OnUpdate(SDL_Renderer* renderer) NOEXCEPT
  {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    UI();
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
  }

  // Ref: https://github.com/ocornut/imgui/issues/707
  static void Style() NOEXCEPT
  {
    ImGui::GetIO().Fonts->AddFontFromFileTTF((std::filesystem::path(STR(PROJECT_DIR)) / "Font" / "Roboto-Medium.ttf").string().c_str(), 18.0f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.3f;
    style.FrameRounding = 2.3f;
    style.ScrollbarRounding = 0;

    style.Colors[ImGuiCol_Text]                 = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
    style.Colors[ImGuiCol_Border]               = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
  }

  // TODO Add Log
  static void UI() NOEXCEPT
  {
    const ImGuiID dock_id = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    #ifndef NDEBUG
    ImGui::SetNextWindowDockID(dock_id);
    ImGui::ShowDemoWindow();
    #endif
    
    ImGui::SetNextWindowDockID(dock_id);
    ImGui::Begin("Controller");

    if (ImGui::CollapsingHeader("Help", ImGuiTreeNodeFlags_DefaultOpen))
    {
      ImGui::Indent(10.0f);
    
      ImGui::BulletText("Press W Move Forward");
      ImGui::BulletText("Press A Move Left");
      ImGui::BulletText("Press S Move Backward");
      ImGui::BulletText("Press D Move Right");
      ImGui::BulletText("Press Space Move Up");
      ImGui::BulletText("Press Shift Move Down");
      ImGui::BulletText("Hold Left Button, Rotate Model");
      ImGui::BulletText("Hold Right Button, Rotate Camera");
      ImGui::BulletText("Scroll Mouse Wheel, Scale Model");

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

      // TODO Use Table
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

      if (ImGui::Button("Reset Camera", ImVec2(ImGui::GetWindowWidth() - 25.0f, 0.0f)))
      {
        Initializer::ReSet(camera);
      }

      ImGui::Unindent(10.0f);
    }

    if (ImGui::CollapsingHeader("Scene"))
    {
      ImGui::Indent(10.0f);

      if (ImGui::CollapsingHeader("Models"))
      {
        ImGui::Indent(10.0f);

        ImGui::Text("Model Number");
        ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
        ImGui::Text("%llu", scene.models.size());

        CONSTEXPR size_t size = 256;
        static char buffer[size];
        if (ImGui::Button("Load"))
        {
          Model model;
          Loader::Result result = Loader::LoadObj(buffer, model);
          if (result == Loader::SUCCESS)
          {
            scene.models.emplace_back(std::move(model));  
          }
          else if (result == Loader::ERROR_OPEN_FILE)
          {
            fmt::printf("ERROR_OPEN_FILE\n");
            fflush(stdout);
          }
          else if (result == Loader::ERROR_CLOSE_FILE)
          {
            fmt::printf("ERROR_CLOSE_FILE\n");
            fflush(stdout);
          }
          else if (result == Loader::ERROR_READ_FILE)
          {
            fmt::printf("ERROR_READ_FILE\n");
            fflush(stdout);
          }
        }
        ImGui::SameLine();
        ImGui::InputTextWithHint("##LoadModelInputText", "Path To Your Model", buffer, size);

        ImGui::SeparatorText("Models");

        static size_t selected = -1;
        size_t i = 0;
        for (auto& model : scene.models)
        {
          ImGui::PushID(i);
          if (ImGui::Selectable(fmt::sprintf("%s", model.name).c_str(), selected == i))
          {
            selected_model = &model;
            selected = i;
          }
          ImGui::PopID();
          ++i;
        }

        auto it = scene.models.begin();
        if (selected != (size_t)-1)
        {
          for (size_t _ = 0; _ < selected; ++_) ++it;
          ImGui::SeparatorText(fmt::sprintf("Selected %s", it->name).c_str());
        }
        else
        {
          ImGui::SeparatorText("No Model Selected");
        }
        
        if (selected != (size_t)-1)
        {
          if (ImGui::BeginTable("##ModelTable", 4))
          {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Property");
            ImGui::TableNextColumn();
            ImGui::Text("X");
            ImGui::TableNextColumn();
            ImGui::Text("Y");
            ImGui::TableNextColumn();
            ImGui::Text("Z");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Scale");
            ImGui::TableNextColumn();
            ImGui::DragFloat("##ModelScaleX", &it->scale.x, 0.1f, 0.1f, 10.0f);
            ImGui::TableNextColumn();
            ImGui::DragFloat("##ModelScaleY", &it->scale.y, 0.1f, 0.1f, 10.0f);
            ImGui::TableNextColumn();
            ImGui::DragFloat("##ModelScaleZ", &it->scale.z, 0.1f, 0.1f, 10.0f);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Rotate");
            ImGui::TableNextColumn();
            ImGui::SliderAngle("##ModelRotateX", &it->rotate.x);
            ImGui::TableNextColumn();
            ImGui::SliderAngle("##ModelRotateY", &it->rotate.y);
            ImGui::TableNextColumn();
            ImGui::SliderAngle("##ModelRotateZ", &it->rotate.z);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Translate");
            ImGui::TableNextColumn();
            ImGui::DragFloat("##ModelTranslateX", &it->translate.x, 0.1f, -10.0f, 10.0f);
            ImGui::TableNextColumn();
            ImGui::DragFloat("##ModelTranslateY", &it->translate.y, 0.1f, -10.0f, 10.0f);
            ImGui::TableNextColumn();
            ImGui::DragFloat("##ModelTranslateZ", &it->translate.z, 0.1f, -10.0f, 10.0f);
            
            ImGui::EndTable();
          }

          if (ImGui::Button("Reset"))
          {
            Initializer::ReSet(*it);
          }
          ImGui::SameLine();
          if (ImGui::Button("UnLoad"))
          {
            scene.models.erase(it);
            selected_model = nullptr;
            selected = (size_t)-1;
          }
        }
        
        ImGui::Unindent(10.0f);
      }

      // TODO Use List
      if (ImGui::CollapsingHeader("Lights"))
      {
        ImGui::Indent(10.0f);
        
        ImGui::Text("Light Number");
        ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
        ImGui::Text("%llu", scene.lights.size());
        if (ImGui::Button("Push Light"))
        {
          scene.lights.emplace_back(Light{
            .position = glm::vec3(0.0f, 2.0f, 2.0f),
            .color = glm::vec3(1.0f),
          });
        }
        ImGui::SameLine();
        if (ImGui::Button("Pop Light"))
        {
          if (!scene.lights.empty())
          {
            scene.lights.pop_back();
          }
        }

        for (size_t i = 0; i < scene.lights.size(); ++i)
        {
          ImGui::PushID(i);
          if (ImGui::TreeNode(&scene.lights[i], "Light %llu", i))
          {
            ImGui::Text("Position");
            ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
            ImGui::Text("(%.1f, %.1f, %.1f)", scene.lights[i].position.x, scene.lights[i].position.y, scene.lights[i].position.z);
            ImGui::ColorPicker3("Color", (float*)&scene.lights[i].color, ImGuiColorEditFlags_Float);
            ImGui::TreePop();
          }
          ImGui::PopID();
        }
        
        ImGui::Unindent(10.0f);
      }

      ImGui::Unindent(10.0f);
    }
    
    if (ImGui::CollapsingHeader("Shading"))
    {
      ImGui::Indent(10.0f);
      
      ImGui::Text("Color =");
      ImGui::Text("+ ka * ambient");
      ImGui::Text("+ kd * diffuse");
      ImGui::Text("+ ks * specular^ps");
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
  
};

#endif //CONTROLLER_H
