/**
  ******************************************************************************
  * @file           : Actor.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-8
  ******************************************************************************
  */



#ifndef ACTOR_H
#define ACTOR_H

#include <Common.h>
#include <Entity.h>

// COMMENT: Actor System. For Updating Entities And Handling Events.
struct Actor
{
  // COMMENT: Update Direction, Up, Right Of The Camera.
  static void OnUpdate(Camera& camera) NOEXCEPT
  {
    camera.direction = glm::normalize(glm::vec3{
      glm::cos(camera.pitch) * glm::sin(camera.yaw),
      glm::sin(camera.pitch),
      glm::cos(camera.pitch) * glm::cos(camera.yaw),
    });

    // NOTE: Up Is Always (0, 1, 0).
    
    camera.right = glm::normalize(glm::vec3{
      - camera.direction.z,
      0.0f,
      camera.direction.x,
    });
  }

  static void OnUpdate(const Canvas& canvas) NOEXCEPT
  {
    SDL_ClearSurface(canvas.surface, 0.0f, 0.0f, 0.0f, 0.0f);
  }
  
  static void OnEvent(Model& m, const SDL_Event* event) NOEXCEPT
  {
    // COMMENT: Use Mouse Wheel To Scale Model.
    if (event->type == SDL_EVENT_MOUSE_WHEEL)
    {
      if (event->wheel.y > 0)
      {
        m.scale *= glm::vec3{1.1f, 1.1f, 1.1f};
      }
      if (event->wheel.y < 0)
      {
        m.scale *= glm::vec3{0.9f, 0.9f, 0.9f};
      }
    }
    // COMMENT: Holding Left Button And Drag To Rotate Model.
    else if (event->type == SDL_EVENT_MOUSE_MOTION && event->motion.state & SDL_BUTTON_LMASK)
    {
      constexpr float ratio = 0.01f; 
      m.rotate.y += ratio * event->motion.xrel;
      m.rotate.x -= ratio * event->motion.yrel;
    }
  }
  
  static void OnEvent(Camera& camera, const SDL_Event* event) NOEXCEPT
  {
    // COMMENT: W/A/S/D/Space/Shift To Move Camera Forward/Left/Backward/Right/Up/Down.
    if (event->type == SDL_EVENT_KEY_DOWN)
    {
      if (event->key.key == SDLK_W) 
      {
        camera.position += 0.1f * camera.direction;
      }
      if (event->key.key == SDLK_A)
      {
          camera.position -= 0.1f * camera.right;
      }
      if (event->key.key == SDLK_S) 
      {
        camera.position -= 0.1f * camera.direction;
      }
      if (event->key.key == SDLK_D)
      {
          camera.position += 0.1f * camera.right;
      }
      // NOTE: Y Is Flipped In Screen Space.
      if (event->key.key == SDLK_SPACE)
      {
          camera.position -= 0.1f * camera.up;
      }
      if (event->key.key == SDLK_LSHIFT)
      {
          camera.position += 0.1f * camera.up;
      }
    }
    // COMMENT: Hold Right Button And Drag To Move Camera Direction.
    else if (event->type == SDL_EVENT_MOUSE_MOTION && event->motion.state & SDL_BUTTON_RMASK)
    {
      camera.yaw = camera.yaw - 0.001f * event->motion.xrel;
      // NOTE: Clamp Pitch In [-89.0f, 89.0f].
      camera.pitch = std::clamp(camera.pitch + 0.001f * event->motion.yrel, glm::radians(-89.0f), glm::radians(89.0f));
    }
  }
  
};

#endif //ACTOR_H
