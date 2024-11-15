/**
  ******************************************************************************
  * @file           : HZBuffer.cpp
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-13
  ******************************************************************************
  */



#ifndef HZBUFFER_HPP
#define HZBUFFER_HPP

#include <Acceleration/HZBuffer.h>

NODISCARD HZBuffer* HZBuffer::Build(const Canvas& canvas, const int xmin, const int xmax, const int ymin, const int ymax) NOEXCEPT // NOLINT(*-no-recursion)
{
  if (ymin == ymax && xmin == xmax)
  {
    return new HZBuffer {
      .zmax = canvas.z_buffer->buffer[ymin][xmin],
      .xmin = xmin, .xmax = xmax, .ymin = ymin, .ymax = ymax, 
      .ll = nullptr, .lr = nullptr, .ul = nullptr, .ur = nullptr,
    };
  }
  
  if (ymin == ymax)
  {
    auto tree = new HZBuffer {
      .zmax = {},
      .xmin = xmin, .xmax = xmax, .ymin = ymin, .ymax = ymax,
      .ll = Build(canvas,                     xmin, (xmin + xmax) >> 1, ymin, ymax),
      .lr = Build(canvas, ((xmin + xmax) >> 1) + 1,               xmax, ymin, ymax),
      .ul = nullptr, .ur = nullptr,
    };
    tree->zmax = std::max(tree->ll->zmax, tree->lr->zmax);
    return tree;
  }
  
  if (xmin == xmax)
  {
    auto tree = new HZBuffer {
      .zmax = {},
      .xmin = xmin, .xmax = xmax, .ymin = ymin, .ymax = ymax,
      .ll = Build(canvas, xmin, xmax,                     ymin, (ymin + ymax) >> 1),
      .lr = nullptr,
      .ul = Build(canvas, xmin, xmax, ((ymin + ymax) >> 1) + 1,               ymax),
      .ur = nullptr,
    };
    tree->zmax = std::max(tree->ll->zmax, tree->ul->zmax);
    return tree;
  }
  
  auto tree = new HZBuffer {
    .zmax = {},
    .xmin = xmin, .xmax = xmax, .ymin = ymin, .ymax = ymax,
    .ll = Build(canvas,                     xmin, (xmin + xmax) >> 1,                     ymin, (ymin + ymax) >> 1),
    .lr = Build(canvas, ((xmin + xmax) >> 1) + 1,               xmax,                     ymin, (ymin + ymax) >> 1),
    .ul = Build(canvas,                     xmin, (xmin + xmax) >> 1, ((ymin + ymax) >> 1) + 1,               ymax),
    .ur = Build(canvas, ((xmin + xmax) >> 1) + 1,               xmax, ((ymin + ymax) >> 1) + 1,               ymax),
  };
  
  tree->zmax = std::max(std::max(tree->ll->zmax, tree->lr->zmax), std::max(tree->ul->zmax, tree->ur->zmax));

  return tree;
}

void HZBuffer::Destroy(const HZBuffer* tree) NOEXCEPT // NOLINT(*-no-recursion)
{
  if (tree == nullptr) { return; }
  if (tree->ll != nullptr) { Destroy(tree->ll); }
  if (tree->lr != nullptr) { Destroy(tree->lr); }
  if (tree->ul != nullptr) { Destroy(tree->ul); }
  if (tree->ur != nullptr) { Destroy(tree->ur); }
  delete tree;
}

void HZBuffer::Clear(HZBuffer* tree, const Canvas& canvas) NOEXCEPT // NOLINT(*-no-recursion)
{
  if (tree == nullptr) { return; }
  if (tree->ll != nullptr) { Clear(tree->ll, canvas); }
  if (tree->lr != nullptr) { Clear(tree->lr, canvas); }
  if (tree->ul != nullptr) { Clear(tree->ul, canvas); }
  if (tree->ur != nullptr) { Clear(tree->ur, canvas); }
  tree->zmax = canvas.z_buffer->bgz;
}

NODISCARD float HZBuffer::Query(const HZBuffer* tree, const int xmin, const int xmax, const int ymin, const int ymax) NOEXCEPT // NOLINT(*-no-recursion)
{
  if (ymin <= tree->ymin && tree->ymax <= ymax && xmin <= tree->xmin && tree->xmax <= xmax)
  {
    return tree->zmax;
  }

  float zmax = -INF;
  
  if (tree->ll != nullptr && (tree->ll->xmin <= xmax && xmin <= tree->ll->xmax) && (tree->ll->ymin <= ymax && ymin <= tree->ll->ymax))
  {
    zmax = std::max(zmax, Query(tree->ll, xmin, xmax, ymin, ymax));
  }
  if (tree->lr != nullptr && (tree->lr->xmin <= xmax && xmin <= tree->lr->xmax) && (tree->lr->ymin <= ymax && ymin <= tree->lr->ymax))
  {
    zmax = std::max(zmax, Query(tree->lr, xmin, xmax, ymin, ymax));
  }
  if (tree->ul != nullptr && (tree->ul->xmin <= xmax && xmin <= tree->ul->xmax) && (tree->ul->ymin <= ymax && ymin <= tree->ul->ymax))
  {
    zmax = std::max(zmax, Query(tree->ul, xmin, xmax, ymin, ymax));
  }
  if (tree->ur != nullptr && (tree->ur->xmin <= xmax && xmin <= tree->ur->xmax) && (tree->ur->ymin <= ymax && ymin <= tree->ur->ymax))
  {
    zmax = std::max(zmax, Query(tree->ur, xmin, xmax, ymin, ymax));
  }
  
  ASSERT(zmax != -INF);

  return zmax;
}

void HZBuffer::Update(HZBuffer* tree, const Canvas& canvas, const int xmin, const int xmax, const int ymin, const int ymax) NOEXCEPT // NOLINT(*-no-recursion)
{
  if (tree == nullptr) { return; }

  if (tree->ymin == tree->ymax && tree->xmin == tree->xmax)
  {
    tree->zmax = canvas.z_buffer->buffer[tree->ymin][tree->xmin];
    return;
  }

  tree->zmax = -INF;
  
  if (tree->ll != nullptr && (tree->ll->xmin <= xmax && xmin <= tree->ll->xmax) && (tree->ll->ymin <= ymax && ymin <= tree->ll->ymax))
  {
    Update(tree->ll, canvas, xmin, xmax, ymin, ymax);
    tree->zmax = std::max(tree->zmax, tree->ll->zmax);
  }
  if (tree->lr != nullptr && (tree->lr->xmin <= xmax && xmin <= tree->lr->xmax) && (tree->lr->ymin <= ymax && ymin <= tree->lr->ymax))
  {
    Update(tree->lr, canvas, xmin, xmax, ymin, ymax);
    tree->zmax = std::max(tree->zmax, tree->lr->zmax);
  }
  if (tree->ul != nullptr && (tree->ul->xmin <= xmax && xmin <= tree->ul->xmax) && (tree->ul->ymin <= ymax && ymin <= tree->ul->ymax))
  {
    Update(tree->ul, canvas, xmin, xmax, ymin, ymax);
    tree->zmax = std::max(tree->zmax, tree->ul->zmax);
  }
  if (tree->ur != nullptr && (tree->ur->xmin <= xmax && xmin <= tree->ur->xmax) && (tree->ur->ymin <= ymax && ymin <= tree->ur->ymax))
  {
    Update(tree->ur, canvas, xmin, xmax, ymin, ymax);
    tree->zmax = std::max(tree->zmax, tree->ur->zmax);
  }

  ASSERT(tree->zmax != -INF);
}

#endif //HZBUFFER_HPP
