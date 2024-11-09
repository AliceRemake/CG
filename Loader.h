/**
  ******************************************************************************
  * @file           : Loader.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-8
  ******************************************************************************
  */



#ifndef LOADER_H
#define LOADER_H

#include <Common.h>
#include <Entity.h>

// COMMENT: Loader System. For Loading Model Files.
struct Loader
{
  enum Result
  {
    SUCCESS,
    ERROR_OPEN_FILE,
    ERROR_CLOSE_FILE,
    ERROR_READ_FILE,
  };
  
  // WARN: Very Simple Code For Loading .obj File. May Have Bugs.
  static Result LoadObj(const char* filename, Model& model) NOEXCEPT
  {

    // COMMENT: Open File.
    FILE* fp = fopen(filename, "rb");
    if (fp == nullptr)
    {
      return ERROR_OPEN_FILE;
    }

    model.name = std::filesystem::path(filename).filename().string();
    // COMMENT: Pre Allocate Memories.
    model.vertices.clear();
    model.normals.clear();
    model.indices.clear();
    model.vertices.reserve(4096);
    model.normals.reserve(4096);
    model.indices.reserve(4096 * 3);
    model.scale = glm::vec3(1.0f);
    model.rotate = glm::vec3(0.0f);
    model.translate = glm::vec3(0.0f);

    // COMMENT: Auto Translate To Center And Scale To 1.
    auto center = glm::vec3(0.0f);
    auto aabb = AABB{ glm::vec3(1E9), glm::vec3(-1E9) };

    char buffer[128];
    while(char* s = fgets(buffer, 128, fp))
    {
      // COMMENT: Skip Empty Line.
      if (s[0] == '\n' || s[0] == '\r') continue;

      // COMMENT: Skip Comment.
      if (s[0] == '#') continue;

      float a, b, c;
      int vi0, vni0, vi1, vni1, vi2, vni2;

      // COMMENT: Read Vertex Data.
      if (sscanf(s, "v %f %f %f\n", &a, &b, &c) == 3)
      {
        model.vertices.emplace_back(a, b, c);
        center += model.vertices.back();
        aabb.vmin = glm::min(aabb.vmin, model.vertices.back());
        aabb.vmax = glm::max(aabb.vmax, model.vertices.back());
      }
      // COMMENT: Read Vertex Normal Data.
      else if (sscanf(s, "vn %f %f %f\n", &a, &b, &c) == 3)
      {
        model.normals.emplace_back(a, b, c);        
      }
      // COMMENT: Read Face Data
      else if (sscanf(s, "f %d//%d %d//%d %d//%d\n", &vi0, &vni0, &vi1, &vni1, &vi2, &vni2) == 6)
      {
        model.indices.emplace_back(vi0-1, vni0-1);
        model.indices.emplace_back(vi1-1, vni1-1);
        model.indices.emplace_back(vi2-1, vni2-1);
      }
      // COMMENT: Unsupported Keywords.
      else
      {
        return ERROR_READ_FILE;
      }
    }

    // COMMENT: Close File.
    if (fclose(fp) == EOF)
    {
      return ERROR_CLOSE_FILE;
    }

    // COMMENT: Auto Translate To Center And Scale To 1
    center /= model.vertices.size();
    glm::vec3 t = aabb.vmax - aabb.vmin;
    float scale = (t.x + t.y + t.z) / 3.0f;
    
    // COMMENT: Auto Translate To Center And Scale To 1
    for (auto& vertex : model.vertices)
    {
      vertex -= center;
      vertex /= scale;
    }

    // COMMENT: Free Extra Memories.
    model.vertices.shrink_to_fit();
    model.normals.shrink_to_fit();
    model.indices.shrink_to_fit();

    return SUCCESS;
  }
  
};

#endif //LOADER_H
