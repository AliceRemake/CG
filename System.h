///**
//  ******************************************************************************
//  * @file           : System.h
//  * @author         : AliceRemake
//  * @brief          : None
//  * @attention      : None
//  * @date           : 24-11-6
//  ******************************************************************************
//  */
//
//
//
//#ifndef SYSTEM_H
//#define SYSTEM_H
//
//#include <Common.h>
//#include <Entity.h>
//
//
//
//// struct Rasterizer {
////
//// private:
////    static uint32_t MapColor(const Canvas&canvas, const Color& color) ;
////
////    static void RenderPixel(const Canvas& canvas, int i, int j, const Color& color) ;
////   
////    static void RenderSegment(const Canvas& canvas, int i, int jmin, int jmax, const Color& color) ;
////
////   static void RenderRect(const Canvas& canvas, int imin, int jmin, int imax, int jmax, const Color& color) noexcept;
////
////   static void RenderLineDDA(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color);
////
////   static void RenderLineBresenham(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color);
////
//// public:
////   static void RenderLine(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color) noexcept;
////
////   static void RenderTriangleLine(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const glm::ivec2& p2, const Color& color) noexcept;
////   
////   static void RenderTriangleFill(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const glm::ivec2& p2, const Color& color) noexcept;
////
//// };
//
//
//
//
//
//struct Actor {
//
//  static void OnEvent(Model& m, const SDL_Event* event) noexcept {
//    if (event->type == SDL_EVENT_MOUSE_WHEEL) {
//      if (event->wheel.y > 0) {
//        m.scale *= glm::vec3{1.1f, 1.1f, 1.1f};
//      }
//      if (event->wheel.y < 0) {
//        m.scale *= glm::vec3{0.9f, 0.9f, 0.9f};
//      }
//    } else if (event->type == SDL_EVENT_MOUSE_MOTION && event->motion.state & SDL_BUTTON_LMASK) {
//      constexpr float ratio = 0.01f; 
//      m.rotate.y += ratio * event->motion.xrel;
//      m.rotate.x -= ratio * event->motion.yrel;
//    }
//  }
//
//  static void OnUpdate(Camera& camera) NOEXCEPT {
//    camera.direction = glm::normalize(glm::vec3{
//      glm::cos(camera.pitch) * glm::sin(camera.yaw),
//      glm::sin(camera.pitch),
//      glm::cos(camera.pitch) * glm::cos(camera.yaw),
//    });
//  }
//  
//  static void OnEvent(Camera& camera, const SDL_Event* event) noexcept {
//    if (event->type == SDL_EVENT_KEY_DOWN) {
//      if (event->key.key == SDLK_W) {
//        camera.position += 0.1f * camera.direction;
//      }
//      if (event->key.key == SDLK_S) {
//        camera.position -= 0.1f * camera.direction;
//      }
//    } else if (event->type == SDL_EVENT_MOUSE_MOTION && event->motion.state & SDL_BUTTON_RMASK) {
//      camera.yaw = camera.yaw - 0.001f * event->motion.xrel;
//      camera.pitch = std::clamp(camera.pitch + 0.001f * event->motion.yrel, -89.0f, 89.0f);
//    }
//  }
//  
//};
//
//#endif //SYSTEM_H
