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

// COMMENT: Shader System. For Calculating The Color Of A Point.
struct Shader
{

  // COMMENT: Configuration Of The Blinn-Phong Shading Model.
  struct Config
  {
    // COMMENT: Ratio Of Ambient Light.
    float ka = 0.1f;
    // COMMENT: Ratio Of Diffuse Light.
    float kd = 0.5f;
    // COMMENT: Ratio Of Specular Light.
    float ks = 0.4f;
    // COMMENT: Pow Of Specular Light.
    float ps = 2.5f;
  };

  // COMMENT: Use Blinn-Phong Shading Model To Calculate The Color At A Point. 
  // NOTE: This Function Should Be Called At Camera Space. Assuming Camera Is At Origin Point.
  static Color BlinnPhong(const std::vector<Light>& lights, const Vertex& vertex, const Normal& normal, const Config& config) noexcept {
    const Color ambient = config.ka * glm::vec3(1.0f);
    auto diffuse = glm::vec3(0.0f);
    auto specular = glm::vec3(0.0f);
    for (const auto& light : lights) {
      glm::vec3 i = glm::normalize(light.position - vertex);
      glm::vec3 o = glm::normalize(-vertex);
      glm::vec3 h = glm::normalize(i + o);
      diffuse += config.kd * std::max(0.0f, glm::dot(i, o)) * light.color;
      specular += config.ks * std::pow(std::max(0.0f, glm::dot(h, glm::normalize(normal))), config.ps) * light.color;
    }
    return ambient + diffuse + specular;
  }
  
};

#endif //SHADER_H
