/**
  ******************************************************************************
  * @file           : Transformer.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-8
  ******************************************************************************
  */



#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include <Common.h>
#include <Entity.h>

// COMMENT: Transformer System. For Generating Transform Matrices.
struct Transformer
{

  static glm::mat4 Scale(const glm::vec3& v) NOEXCEPT
  {
    return glm::scale(glm::mat4(1.0f), v);
  }

  static glm::mat4 RotateX(const float radians) NOEXCEPT
  {
    return glm::rotate(glm::mat4(1.0f), radians, glm::vec3(1.0f, 0.0f, 0.0f));
  }
  
  static glm::mat4 RotateY(const float radians) NOEXCEPT
  {
    return glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 1.0f, 0.0f));
  }
  
  static glm::mat4 RotateZ(const float radians) NOEXCEPT
  {
    return glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 0.0f, 1.0f));
  }
  
  static glm::mat4 Translate(const glm::vec3& v) NOEXCEPT
  {
    return glm::translate(glm::mat4(1.0f), v);
  }

  static glm::mat4 Model(const Model& model) NOEXCEPT
  {
    return Translate(model.translate) * RotateZ(model.rotate.z) * RotateY(model.rotate.y) * RotateX(model.rotate.x) * Scale(model.scale);
  }

  static glm::mat4 View(const Camera& camera) NOEXCEPT
  {
    return glm::lookAt(camera.position, camera.position + camera.direction, camera.up);
  }
  
  static glm::mat4 Project(const Camera& camera) NOEXCEPT
  {
    return glm::perspective(camera.fov, camera.aspect, camera.near, camera.far);
  }

  static glm::mat4 Viewport(const Canvas& canvas) NOEXCEPT
  {
    // NOTE: glm stores column vectors
    const int& w = canvas.width;
    const int& h = canvas.height;
    return glm::mat4 {
      glm::vec4{      w/2.0f,        0.0f, 0.0f, 0.0f },
      glm::vec4{        0.0f,     -h/2.0f, 0.0f, 0.0f },
      glm::vec4{        0.0f,        0.0f, 1.0f, 0.0f },
      glm::vec4{ w/2.0f-0.5f, h/2.0f-0.5f, 0.0f, 1.0f },
    };
  }

};

#endif //TRANSFORMER_H
