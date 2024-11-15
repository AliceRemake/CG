/**
  ******************************************************************************
  * @file           : Transformer.cpp
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-13
  ******************************************************************************
  */



#include <Transformer.h>

NODISCARD  glm::mat4 Transformer::Scale(const glm::vec3& v) NOEXCEPT
{
  return glm::scale(glm::mat4(1.0f), v);
}

NODISCARD  glm::mat4 Transformer::RotateX(const float radians) NOEXCEPT
{
  return glm::rotate(glm::mat4(1.0f), radians, glm::vec3(1.0f, 0.0f, 0.0f));
}

NODISCARD  glm::mat4 Transformer::RotateY(const float radians) NOEXCEPT
{
  return glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 1.0f, 0.0f));
}

NODISCARD  glm::mat4 Transformer::RotateZ(const float radians) NOEXCEPT
{
  return glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 0.0f, 1.0f));
}

NODISCARD  glm::mat4 Transformer::Translate(const glm::vec3& v) NOEXCEPT
{
  return glm::translate(glm::mat4(1.0f), v);
}

NODISCARD  glm::mat4 Transformer::Model(const ::Model& model) NOEXCEPT
{
  return Translate(model.translate) * RotateZ(model.rotate.z) * RotateY(model.rotate.y) * RotateX(model.rotate.x) * Scale(model.scale);
}

NODISCARD  glm::mat4 Transformer::View(const Camera& camera) NOEXCEPT
{
  return glm::lookAt(camera.position, camera.position + camera.direction, camera.up);
}

NODISCARD  glm::mat4 Transformer::Project(const Camera& camera) NOEXCEPT
{
  return glm::perspective(camera.fov, camera.aspect, camera.near, camera.far);
}

NODISCARD  glm::mat4 Transformer::Viewport(const Canvas& canvas) NOEXCEPT
{
  const int& w = canvas.width;
  const int& h = canvas.height;
  return glm::mat4 {
    glm::vec4{      w/2.0f,        0.0f, 0.0f, 0.0f },
    glm::vec4{        0.0f,     -h/2.0f, 0.0f, 0.0f },
    glm::vec4{        0.0f,        0.0f, 1.0f, 0.0f },
    glm::vec4{ w/2.0f-0.5f, h/2.0f-0.5f, 0.0f, 1.0f },
  };
}

 void Transformer::TransformAABB(AABB& aabb, const glm::mat4& matrix) NOEXCEPT
{
  glm::vec3 vmin = aabb.vmin;
  glm::vec3 vmax = aabb.vmax;

  aabb.vmin = matrix[3].xyz();
  aabb.vmax = matrix[3].xyz();

  for (int j = 0; j < 3; ++j)
  {
    glm::vec3 a = matrix[j].xyz() * vmin;
    glm::vec3 b = matrix[j].xyz() * vmax;
    aabb.vmin += glm::min(a, b);
    aabb.vmax += glm::max(a, b);
  }
}
