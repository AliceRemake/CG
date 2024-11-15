/**
  ******************************************************************************
  * @file           : Shader.cpp
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-13
  ******************************************************************************
  */



#include <Shader.h>

NODISCARD  Color Shader::BlinnPhong(const std::vector<ParallelLight>& parallel_lights, const std::vector<PointLight>& point_lights, const Vertex& vertex, const Normal& normal, const Config& config) NOEXCEPT
{
  const Color ambient = config.ka * glm::vec3(1.0f);
    
  auto diffuse = Color(0.0f);
  auto specular = Color(0.0f);

  for (const auto& light : parallel_lights)
  {
    glm::vec3 i = glm::normalize(-light.direction);
    glm::vec3 o = glm::normalize(-vertex);
    glm::vec3 h = glm::normalize(i + o);
    diffuse += config.kd * std::max(0.0f, glm::dot(i, o)) * light.color;
    specular += (config.ps + 8.0f) / 8.0f * glm::pi<float>() * config.ks * std::pow(std::max(0.0f, glm::dot(h, glm::normalize(normal))), config.ps) * light.color;
  }

  for (const auto& light : point_lights)
  {
    glm::vec3 i = glm::normalize(light.position - vertex);
    glm::vec3 o = glm::normalize(-vertex);
    glm::vec3 h = glm::normalize(i + o);
    diffuse += config.kd * std::max(0.0f, glm::dot(i, o)) * light.color;
    specular += (config.ps + 8.0f) / 8.0f * glm::pi<float>() * config.ks * std::pow(std::max(0.0f, glm::dot(h, glm::normalize(normal))), config.ps) * light.color;
  }
    
  return ambient + diffuse / (float)(parallel_lights.size() + point_lights.size()) + specular / (float)(parallel_lights.size() + point_lights.size());
}
