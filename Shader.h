/**
  ******************************************************************************
  * @file           : Shader.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-8
  ******************************************************************************
  */



#ifndef SHADER_H
#define SHADER_H

#include <Common.h>
#include <Entity.h>

struct Shader
{
  struct Config
  {
    float ka = 0.1f;
    float kd = 0.5f;
    float ks = 0.4f;
    float ps = 2.5f;
  };

  NODISCARD  static Color BlinnPhong(const std::vector<ParallelLight>& parallel_lights, const std::vector<PointLight>& point_lights, const Vertex& vertex, const Normal& normal, const Config& config) NOEXCEPT;
};

#endif //SHADER_H
