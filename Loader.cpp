/**
  ******************************************************************************
  * @file           : Loader.cpp
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-13
  ******************************************************************************
  */



#include <Loader.h>

FORCE_INLINE Loader::Result Loader::LoadObj(const char* filename, Model& model) NOEXCEPT
{

  // COMMENT: Open File.
  FILE* fp = fopen(filename, "rb");
  if (fp == nullptr)
  {
    return ERROR_OPEN_FILE;
  }

  // COMMENT: Get Model Name From Filename.
  model.name = std::filesystem::path(filename).filename().string();

  model.vertices.clear();
  model.indices.clear();
  model.polygon_sides.clear();

  // COMMENT: Pre Allocate Memories.
  model.vertices.reserve(4096);
  model.indices.reserve(4096 * 3);
  model.polygon_sides.reserve(4096);

  // COMMENT: Init Model Transformations.
  model.scale = glm::vec3(1.0f);
  model.rotate = glm::vec3(0.0f);
  model.translate = glm::vec3(0.0f);

  // COMMENT: For Automatically Translate To Center And Scale To 2.
  auto center = glm::vec3(0.0f);
  auto aabb = AABB{ glm::vec3(INF), glm::vec3(-INF) };

  char buffer[128];
  while(char* s = fgets(buffer, 128, fp))
  {
    // COMMENT: Skip Empty Line.
    if (s[0] == '\r' || s[0] == '\n') continue;

    // COMMENT: Skip Comment.
    if (s[0] == '#') continue;

    float a, b, c;
    uint32_t i, j, k;
    (void)j;
    (void)k;

    // COMMENT: Read Vertex Data.
    if (s[0] == 'v')
    {
      // SYNTAX: v x y z w?
      if (s[1] == ' ')
      {
        char* ptr = &s[2];
        while(*ptr == ' ') ++ptr;
        a = std::strtof(ptr, &ptr);
        while(*ptr == ' ') ++ptr;
        b = std::strtof(ptr, &ptr);
        while(*ptr == ' ') ++ptr;
        c = std::strtof(ptr, &ptr);
        model.vertices.emplace_back(a, b, c);
        center += model.vertices.back();
        aabb.vmin = glm::min(aabb.vmin, model.vertices.back());
        aabb.vmax = glm::max(aabb.vmax, model.vertices.back());
        // TODO: Support Data With w
      }
      // SYNTAX: vt u v? w?
      else if (s[1] == 't' && s[2] == ' ')
      {
        // TODO: Support Data With vt
      }
      // SYNTAX: vn i j k
      else if (s[1] == 'n' && s[2] == ' ')
      {
        // char* ptr = &s[3];
        // while(*ptr == ' ') ++ptr;
        // a = std::strtof(ptr, &ptr);
        // while(*ptr == ' ') ++ptr;
        // b = std::strtof(ptr, &ptr);
        // while(*ptr == ' ') ++ptr;
        // c = std::strtof(ptr, &ptr);
        // model.normals.emplace_back(a, b, c);
      }
      else
      {
        // TODO: Support More Keywords.
      }
    }
    else if (s[0] == 'f')
    {
      // SYNTAX: f (v)|(v/vt)|(v//vn)|v/vt/vn ...
      if (s[1] == ' ')
      {
        char* ptr = &s[2];

        // COMMENT: How Many Vertices Are In This Polygon.
        int polygon = 0;

        while(true)
        {
          // COMMENT: Find Next Vertex.
          while(*ptr == ' ') ++ptr;

          // COMMENT: Keep Read Till End.
          if (*ptr == '\r' || *ptr == '\n') break;
          
          // COMMENT: First v Always Exists.
          i = std::strtoul(ptr, &ptr, 10);

          // COMMENT: vt Or vn.
          if (*ptr == '/')
          {
            ++ptr;
            // COMMENT: No vt. Has vn.
            if (*ptr == '/')
            {
              ++ptr;
              j = -1;
              k = std::strtoul(ptr, &ptr, 10);
            }
            // COMMENT: Has vt.
            else
            {
              j = std::strtoul(ptr, &ptr, 10);
              // COMMENT: Has vn.
              if (*ptr == '/')
              {
                ++ptr;
                k = std::strtoul(ptr, &ptr, 10);
              }
              // COMMENT: No vn.
              else
              {
                k = -1;  
              }
            }
          }
          // COMMENT: None Of vt And vn.
          else
          {
            j = -1, k = -1;  
          }

          ASSERT(std::isspace(*ptr));

          ++polygon;
          // model.indices.emplace_back(i-1, j-1, k-1);
          model.indices.emplace_back(i-1);
        }

        // COMMENT: How Many Vertices Are In This Polygon.
        model.polygon_sides.emplace_back(polygon);

      }
      else
      {
        // TODO: Support More Keywords.
      }
    }
    else
    {
      // TODO: Support More Keywords.
    }
  }

  // COMMENT: Close File.
  if (fclose(fp) == EOF)
  {
    return ERROR_CLOSE_FILE;
  }

  // COMMENT: For Automatically Translate To Center And Scale To 2.
  center /= model.vertices.size();
  glm::vec3 t = aabb.vmax - aabb.vmin;
  float scale = 6.0f / (t.x + t.y + t.z);
  
  // COMMENT: Translate To Center And Scale To 2.
  for (auto& vertex : model.vertices)
  {
    vertex -= center;
    vertex *= scale;
  }

  // COMMENT: Free Extra Memories.
  model.vertices.shrink_to_fit();
  model.indices.shrink_to_fit();
  model.polygon_sides.shrink_to_fit();

  return SUCCESS;
}
