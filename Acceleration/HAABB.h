/**
  ******************************************************************************
  * @file           : HAABB.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-13
  ******************************************************************************
  */



#ifndef HAABB_H
#define HAABB_H

#include <Common.h>
#include <Entity.h>

struct HAABB
{
  Vertex vmin, vmax;
  size_t l;
  size_t r;
  size_t pid;

  NODISCARD  static glm::vec2 Center(const HAABB& haabb) NOEXCEPT;

  NODISCARD  static glm::vec2 Radius(const HAABB& haabb) NOEXCEPT;

  NODISCARD  static std::vector<HAABB> Build(const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons) NOEXCEPT;
};

#endif //HAABB_H
