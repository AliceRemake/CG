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

extern Setting setting;
extern Shader::Config config;
extern FrameBuffer frame_buffer;
extern ZBuffer z_buffer;
extern Canvas canvas;
extern Camera camera;
extern Scene scene;
extern Model* selected_model;
extern ParallelLight* selected_parallel_light;
extern PointLight* selected_point_light;
extern size_t frame_time;

struct Controller
{
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

   static void ShutDown() NOEXCEPT
  {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
  }

   static void OnEvent(const SDL_Event* event) NOEXCEPT
  {
    ImGui_ImplSDL3_ProcessEvent(event);
  }

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

   static void UI() NOEXCEPT
  {
    const ImGuiID dock_id = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    #ifndef NDEBUG
    ImGui::SetNextWindowDockID(dock_id);
    ImGui::ShowDemoWindow();
    #endif
    
    ImGui::SetNextWindowDockID(dock_id);
    ImGui::Begin("Controller");

    ImGui::Text("Frame Time(ms): %llu", frame_time);
    
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

      ImGui::Checkbox("Show AABB", &setting.show_aabb);
      ImGui::Checkbox("Show Normal", &setting.show_normal);
      ImGui::Checkbox("Show ZBuffer", &setting.show_z_buffer);
      ImGui::Checkbox("Enable Cull", &setting.enable_cull);
      ImGui::Checkbox("Enable Clip", &setting.enable_clip);
      {
        static const char* const items[] = {
          "Scan Convert ZBuffer",
          "Scan Convert Hierarchical ZBuffer",
          "Scan Convert Hierarchical AABB Hierarchical ZBuffer",
          "Interval ScanLine",
        };
        ImGui::Combo("Algorithm", (int*)&setting.algorithm, items, 4);
      }
      {
        static const char* const items[] = {
          "Normal",
          "Wireframe",
        };
        ImGui::Combo("DisplayMode", (int*)&setting.display_mode, items, 2);
      }

      ImGui::Unindent(10.0f);
    }

    if (ImGui::CollapsingHeader("Camera"))
    {
      ImGui::Indent(10.0f);

      if (ImGui::BeginTable("##CameraTable", 4))
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
        ImGui::Text("Position");
        ImGui::TableNextColumn();
        ImGui::DragFloat("##CameraPositionX", &camera.position.x, 0.1f, -10.0f, 10.0f);
        ImGui::TableNextColumn();
        ImGui::DragFloat("##CameraPositionY", &camera.position.y, 0.1f, -10.0f, 10.0f);
        ImGui::TableNextColumn();
        ImGui::DragFloat("##CameraPositionZ", &camera.position.z, 0.1f, -10.0f, 10.0f);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Direction");
        ImGui::TableNextColumn();
        ImGui::Text("%.3f", camera.direction.x);
        ImGui::TableNextColumn();
        ImGui::Text("%.3f", camera.direction.y);
        ImGui::TableNextColumn();
        ImGui::Text("%.3f", camera.direction.z);
        
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Up");
        ImGui::TableNextColumn();
        ImGui::Text("%.3f", camera.up.x);
        ImGui::TableNextColumn();
        ImGui::Text("%.3f", camera.up.y);
        ImGui::TableNextColumn();
        ImGui::Text("%.3f", camera.up.z);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Right");
        ImGui::TableNextColumn();
        ImGui::Text("%.3f", camera.right.x);
        ImGui::TableNextColumn();
        ImGui::Text("%.3f", camera.right.y);
        ImGui::TableNextColumn();
        ImGui::Text("%.3f", camera.right.z);

        ImGui::EndTable();
      }
      
      ImGui::SliderAngle("Yaw", &camera.yaw, 0.0f, 360.0f);
      ImGui::SliderAngle("Pitch", &camera.pitch, -89.0f, 89.0f);
      ImGui::SliderAngle("Fov", &camera.fov, 30.0f, 120.0f);
      ImGui::DragFloat("Aspect", &camera.aspect, 0.1f, 0.1f, 10.0f);
      ImGui::DragFloat("Near", &camera.near, 0.1f, 0.1f, camera.far);
      ImGui::DragFloat("Far", &camera.far, 0.1f, camera.near, 10.0f);

      if (ImGui::Button("Reset Camera", ImVec2(ImGui::GetWindowWidth() - 25.0f, 0.0f)))
      {
        camera.position  = Vertex(0.0f, 0.0f, 2.0f);
        camera.direction = Vector(0.0f, 0.0f, -1.0f);
        camera.up        = Vector(0.0f, 1.0f, 0.0f);
        camera.right     = Vector(1.0f, 0.0f, 0.0f);
        camera.yaw       = glm::radians(180.0f);
        camera.pitch     = 0.0f;
        camera.fov       = glm::radians(75.0f);
        camera.aspect    = 4.0f / 3.0f;
        camera.near      = 0.1f;
        camera.far       = 4.0f;
      }

      ImGui::Unindent(10.0f);
    }

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
        ImGui::Text("Vertex Number: %llu", selected_model->vertices.size());
        ImGui::Text("Polygon Number: %llu", selected_model->polygon_sides.size());
        
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
          it->scale = glm::vec3(1.0f);
          it->rotate = glm::vec3(0.0f);
          it->translate = glm::vec3(0.0f);
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

    if (ImGui::CollapsingHeader("Parallel Lights"))
    {
      ImGui::PushID(0);
      ImGui::Indent(10.0f);

      ImGui::Text("Parallel Light Number");
      ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
      ImGui::Text("%llu", scene.parallel_lights.size());

      if (ImGui::Button("Load##ParallelLight"))
      {
        scene.parallel_lights.emplace_back(ParallelLight{
          .direction = Vector(0.0f, -1.0f, 1.0f),
          .color     = Color(1.0f , 1.0f, 1.0f),
        });
      }

      ImGui::SeparatorText("Parallel Lights");

      static size_t selected = -1;
      size_t i = 0;
      for (auto & parallel_light : scene.parallel_lights)
      {
        ImGui::PushID(i);
        if (ImGui::Selectable(fmt::sprintf("Parallel Light %llu", i).c_str(), selected == i))
        {
          selected_parallel_light = &parallel_light;
          selected = i;
        }
        ImGui::PopID();
        ++i;
      }

      auto it = scene.parallel_lights.begin();
      if (selected != (size_t)-1)
      {
        for (size_t _ = 0; _ < selected; ++_) ++it;
        ImGui::SeparatorText(fmt::sprintf("Selected Parallel Light %llu", selected).c_str());
      }
      else
      {
        ImGui::SeparatorText("No Parallel Lights Selected");
      }
      
      if (selected != (size_t)-1)
      {
        if (ImGui::BeginTable("##ParallelLightTable", 4))
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
          ImGui::Text("Direction");
          ImGui::TableNextColumn();
          ImGui::DragFloat("##ParallelLightDirectionX", &it->direction.x, 0.1f, -1.0f, 1.0f);
          ImGui::TableNextColumn();
          ImGui::DragFloat("##ParallelLightDirectionY", &it->direction.y, 0.1f, -1.0f, 1.0f);
          ImGui::TableNextColumn();
          ImGui::DragFloat("##ParallelLightDirectionZ", &it->direction.z, 0.1f, -1.0f, 1.0f);

          ImGui::EndTable();
        }

        ImGui::ColorPicker3("##ParallelLightColorPicker", (float*)&it->color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);

        if (ImGui::Button("Reset"))
        {
          it->direction = Vector(0.0f, -1.0f, 1.0f);
          it->color = Color(1.0f, 1.0f, 1.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("UnLoad"))
        {
          scene.parallel_lights.erase(it);
          selected_model = nullptr;
          selected = (size_t)-1;
        }
      }
      
      ImGui::Unindent(10.0f);
      ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Point Lights"))
    {
      ImGui::PushID(1);
      ImGui::Indent(10.0f);

      ImGui::Text("Point Light Number");
      ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
      ImGui::Text("%llu", scene.point_lights.size());

      if (ImGui::Button("Load##PointLight"))
      {
        scene.point_lights.emplace_back(PointLight{
          .position = Vertex(0.0f, 2.0f, 2.0f),
          .color    = Color(1.0f , 1.0f, 1.0f),
        });
      }

      ImGui::SeparatorText("Point Lights");

      static size_t selected = -1;
      size_t i = 0;
      for (auto & light : scene.point_lights)
      {
        ImGui::PushID(i);
        if (ImGui::Selectable(fmt::sprintf("Parallel Light %llu", i).c_str(), selected == i))
        {
          selected_point_light = &light;
          selected = i;
        }
        ImGui::PopID();
        ++i;
      }

      auto it = scene.point_lights.begin();
      if (selected != (size_t)-1)
      {
        for (size_t _ = 0; _ < selected; ++_) ++it;
        ImGui::SeparatorText(fmt::sprintf("Selected Point Light %llu", selected).c_str());
      }
      else
      {
        ImGui::SeparatorText("No Point Light Selected");
      }
      
      if (selected != (size_t)-1)
      {
        if (ImGui::BeginTable("##PointLightTable", 4))
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
          ImGui::Text("Position");
          ImGui::TableNextColumn();
          ImGui::DragFloat("##PointLightDirectionX", &it->position.x, 0.1f, -10.0f, 10.0f);
          ImGui::TableNextColumn();
          ImGui::DragFloat("##PointLightDirectionY", &it->position.y, 0.1f, -10.0f, 10.0f);
          ImGui::TableNextColumn();
          ImGui::DragFloat("##PointLightDirectionZ", &it->position.z, 0.1f, -10.0f, 10.0f);

          ImGui::EndTable();
        }

        ImGui::ColorPicker3("##PointLightColorPicker", (float*)&it->color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);

        if (ImGui::Button("Reset"))
        {
          it->position = Vertex(0.0f, 2.0f, 2.0f),
          it->color    = Color(1.0f , 1.0f, 1.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("UnLoad"))
        {
          scene.point_lights.erase(it);
          selected_model = nullptr;
          selected = (size_t)-1;
        }
      }
      
      ImGui::Unindent(10.0f);
      ImGui::PopID();
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
