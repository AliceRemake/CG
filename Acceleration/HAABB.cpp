/**
  ******************************************************************************
  * @file           : HAABB.cpp
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-13
  ******************************************************************************
  */



#include <Acceleration/HAABB.h>

NODISCARD glm::vec2 HAABB::Center(const HAABB& haabb) NOEXCEPT
{
  return glm::vec2(haabb.vmin + haabb.vmax) / 2.0f; 
}

NODISCARD glm::vec2 HAABB::Radius(const HAABB& haabb) NOEXCEPT
{
  return glm::vec2(haabb.vmax - haabb.vmin) / 2.0f;
}

NODISCARD std::vector<HAABB> HAABB::Build(const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons) NOEXCEPT
{
  std::vector<HAABB> haabbs;
  haabbs.resize(polygons.size() * 2);
  for (size_t i = 0; i < polygons.size(); ++i)
  {
    haabbs[polygons.size() + i].vmin = Vertex(INF);
    haabbs[polygons.size() + i].vmax = Vertex(-INF);
    haabbs[polygons.size() + i].l = 0;
    haabbs[polygons.size() + i].r = 0;
    haabbs[polygons.size() + i].pid = i;
    for (const auto& vertex : polygons[i].vertices)
    {
      haabbs[polygons.size() + i].vmin = glm::min(haabbs[polygons.size() + i].vmin, vertices[vertex]);  
      haabbs[polygons.size() + i].vmax = glm::max(haabbs[polygons.size() + i].vmax, vertices[vertex]);
    }
  }
  std::sort(haabbs.begin() + polygons.size(), haabbs.end(), [](const HAABB& lhs, const HAABB& rhs) NOEXCEPT -> bool {
    return Center(lhs).x < Center(rhs).x;
  });
  for (size_t i = haabbs.size() - 1; i >= 2; --i)
  {
    if (i % 2)
    {
      haabbs[i >> 1].r = i;
    }
    else
    {
      haabbs[i >> 1].l = i;
      if (haabbs[i >> 1].r == 0)
      {
        haabbs[i >> 1].vmin = haabbs[i].vmin;
        haabbs[i >> 1].vmax = haabbs[i].vmax;
        haabbs[i >> 1].pid = polygons.size();
      }
      else
      {
        haabbs[i >> 1].vmin = glm::min(haabbs[i].vmin, haabbs[i + 1].vmin);
        haabbs[i >> 1].vmax = glm::max(haabbs[i].vmax, haabbs[i + 1].vmax);
        haabbs[i >> 1].pid = polygons.size();
      }
    }
  }

  return haabbs;
}
