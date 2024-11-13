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

  NODISCARD FORCE_INLINE static glm::mat4 Scale(const glm::vec3& v) NOEXCEPT;

  NODISCARD FORCE_INLINE static glm::mat4 RotateX(float radians) NOEXCEPT;
  
  NODISCARD FORCE_INLINE static glm::mat4 RotateY(float radians) NOEXCEPT;
  
  NODISCARD FORCE_INLINE static glm::mat4 RotateZ(float radians) NOEXCEPT;
  
  NODISCARD FORCE_INLINE static glm::mat4 Translate(const glm::vec3& v) NOEXCEPT;

  NODISCARD FORCE_INLINE static glm::mat4 Model(const Model& model) NOEXCEPT;

  NODISCARD FORCE_INLINE static glm::mat4 View(const Camera& camera) NOEXCEPT;
  
  NODISCARD FORCE_INLINE static glm::mat4 Project(const Camera& camera) NOEXCEPT;

  NODISCARD FORCE_INLINE static glm::mat4 Viewport(const Canvas& canvas) NOEXCEPT;
  
  FORCE_INLINE static void TransformAABB(AABB& aabb, const glm::mat4& matrix) NOEXCEPT;
  
};

#endif //TRANSFORMER_H
