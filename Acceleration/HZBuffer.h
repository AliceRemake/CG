/**
  ******************************************************************************
  * @file           : HZBuffer.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-13
  ******************************************************************************
  */



#ifndef HZBUFFER_H
#define HZBUFFER_H

#include <Common.h>
#include <Entity.h>

struct HZBuffer
{
  float zmax;
  int xmin, xmax, ymin, ymax;
  HZBuffer *ll, *lr, *ul, *ur;

  NODISCARD static HZBuffer* Build(const Canvas& canvas, int xmin, int xmax, int ymin, int ymax) NOEXCEPT;
  
  static void Destroy(const HZBuffer* tree) NOEXCEPT;
  
  static void Clear(HZBuffer* tree, const Canvas& canvas) NOEXCEPT;
  
  NODISCARD static float Query(const HZBuffer* tree, int xmin, int xmax, int ymin, int ymax) NOEXCEPT;
  
  static void Update(HZBuffer* tree, const Canvas& canvas, int xmin, int xmax, int ymin, int ymax) NOEXCEPT;
  
};

#endif //HZBUFFER_H
