/**
  ******************************************************************************
  * @file           : Accelerator.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-9
  ******************************************************************************
  */



#ifndef ACCELERATOR_H
#define ACCELERATOR_H

#include <Common.h>
#include <Entity.h>

// COMMENT: Z Pyramid Node In Paper `Hierarchical Z-Buffer Visibility`.
// Ref: https://history.siggraph.org/learning/hierarchical-z-buffer-visibility-by-greene-kass-and-miller/
struct HZBufferNode
{
  // COMMENT: Lower Left(ll), Lower Right(lr), Upper Left(ul), Upper Right(ur).
  HZBufferNode *ll, *lr, *ul, *ur;
  // COMMENT: Region Of ZBuffer [imin, imax] X [jmin, jmax].
  int imin, imax, jmin, jmax;
  // COMMENT: Furthest Z In Region.
  float zmax;
};

struct Accelerator
{
  // COMMENT: Recursively Build Hierarchical Z Buffer Tree.
  // NOTE: This Function Do Not Check Boundary. Notice To Pass Valid Parameters.
  NODISCARD static HZBufferNode* BuildHZBufferTree(const Canvas& canvas, const int imin, const int imax, const int jmin, const int jmax) NOEXCEPT
  {
    // COMMENT: Single Point. This Is A Leaf.
    if (imin == imax && jmin == jmax)
    {
      return new HZBufferNode {
        .ll = nullptr, .lr = nullptr, .ul = nullptr, .ur = nullptr,
        .imin = imin, .imax = imax, .jmin = jmin, .jmax = jmax,
        .zmax = canvas.zbuffer[imin][jmin]
      };
    }
    // COMMENT: Single Row.
    if (imin == imax)
    {
      auto tree = new HZBufferNode {
        .ll = BuildHZBufferTree(canvas, imin, imax, jmin, (jmin + jmax) >> 1),
        .lr = BuildHZBufferTree(canvas, imin, imax, ((jmin + jmax) >> 1) + 1, jmax),
        .ul = nullptr, .ur = nullptr,
        .imin = imin, .imax = imax, .jmin = jmin, .jmax = jmax,
        .zmax = {},
      };
      tree->zmax = std::max(tree->ll->zmax, tree->lr->zmax);
      return tree;
    }
    // COMMENT: Single Column.
    if (jmin == jmax)
    {
      auto tree = new HZBufferNode {
        .ll = BuildHZBufferTree(canvas, imin, (imin + imax) >> 1, jmin, jmax),
        .lr = nullptr,
        .ul = BuildHZBufferTree(canvas, ((imin + imax) >> 1) + 1, imax, jmin, jmax),
        .ur = nullptr,
        .imin = imin, .imax = imax, .jmin = jmin, .jmax = jmax,
        .zmax = {},
      };
      tree->zmax = std::max(tree->ll->zmax, tree->ul->zmax);
      return tree;
    }
    // COMMENT: Divide To Four Parts.
    auto tree = new HZBufferNode {
      .ll = BuildHZBufferTree(canvas, imin, (imin + imax) >> 1, jmin, (jmin + jmax) >> 1),
      .lr = BuildHZBufferTree(canvas, imin, (imin + imax) >> 1, ((jmin + jmax) >> 1) + 1, jmax),
      .ul = BuildHZBufferTree(canvas, ((imin + imax) >> 1) + 1, imax, jmin, (jmin + jmax) >> 1),
      .ur = BuildHZBufferTree(canvas, ((imin + imax) >> 1) + 1, imax, ((jmin + jmax) >> 1) + 1, jmax),
      .imin = imin, .imax = imax, .jmin = jmin, .jmax = jmax,
      .zmax = {},
    };
    tree->zmax = std::max(std::max(tree->ll->zmax, tree->lr->zmax), std::max(tree->ul->zmax, tree->ur->zmax));
    return tree;
  }

  // COMMENT: Recursively Destroy Hierarchical Z Buffer Tree.
  static void DestroyHZBufferTree(const HZBufferNode* tree) NOEXCEPT
  {
    ASSERT(tree != nullptr);
    if (tree->ll != nullptr) DestroyHZBufferTree(tree->ll);
    if (tree->lr != nullptr) DestroyHZBufferTree(tree->lr);
    if (tree->ul != nullptr) DestroyHZBufferTree(tree->ul);
    if (tree->ur != nullptr) DestroyHZBufferTree(tree->ur);
    delete tree;
  }

  // COMMENT: Recursively ReSet Hierarchical Z Buffer Tree Each Frame.
  static void ReSetHZBufferTree(HZBufferNode* tree) NOEXCEPT
  {
    ASSERT(tree != nullptr);
    if (tree->ll != nullptr) ReSetHZBufferTree(tree->ll);
    if (tree->lr != nullptr) ReSetHZBufferTree(tree->lr);
    if (tree->ul != nullptr) ReSetHZBufferTree(tree->ul);
    if (tree->ur != nullptr) ReSetHZBufferTree(tree->ur);
    tree->zmax = 1E9;
  }
  
  // COMMENT: Recursively Query Z Max In Region [imin, imax] X [jmin, jmax].
  // NOTE: This Function Do Not Check Boundary. Notice To Pass Valid Parameters.
  NODISCARD static float QueryHZBufferTree(const HZBufferNode* tree, const int imin, const int imax, const int jmin, const int jmax) NOEXCEPT
  {
    // COMMENT: Currently The Tree Is Completely Inside The Query Region.
    if (imin <= tree->imin && tree->imax <= imax && jmin <= tree->jmin && tree->jmax <= jmax)
    {
      return tree->zmax;
    }
    // COMMENT: Divide The Tree.
    float zmax = -1E9;
    if (tree->ll != nullptr
      && ((tree->ll->imin <= imin && imin <= tree->ll->imax) || (tree->ll->imin <= imax && imax <= tree->ll->imax))
      && ((tree->ll->jmin <= jmin && jmin <= tree->ll->jmax) || (tree->ll->jmin <= jmax && jmax <= tree->ll->jmax))
    )
    {
      zmax = std::max(zmax, QueryHZBufferTree(tree->ll, imin, imax, jmin, jmax));
    }
    if (tree->lr != nullptr
      && ((tree->lr->imin <= imin && imin <= tree->lr->imax) || (tree->lr->imin <= imax && imax <= tree->lr->imax))
      && ((tree->lr->jmin <= jmin && jmin <= tree->lr->jmax) || (tree->lr->jmin <= jmax && jmax <= tree->lr->jmax))
    )
    {
      zmax = std::max(zmax, QueryHZBufferTree(tree->lr, imin, imax, jmin, jmax));
    }
    if (tree->ul != nullptr
      && ((tree->ul->imin <= imin && imin <= tree->ul->imax) || (tree->ul->imin <= imax && imax <= tree->ul->imax))
      && ((tree->ul->jmin <= jmin && jmin <= tree->ul->jmax) || (tree->ul->jmin <= jmax && jmax <= tree->ul->jmax))
    )
    {
      zmax = std::max(zmax, QueryHZBufferTree(tree->ul, imin, imax, jmin, jmax));
    }
    if (tree->ur != nullptr
      && ((tree->ur->imin <= imin && imin <= tree->ur->imax) || (tree->ur->imin <= imax && imax <= tree->ur->imax))
      && ((tree->ur->jmin <= jmin && jmin <= tree->ur->jmax) || (tree->ur->jmin <= jmax && jmax <= tree->ur->jmax))
    )
    {
      zmax = std::max(zmax, QueryHZBufferTree(tree->ur, imin, imax, jmin, jmax));
    }
    ASSERT(zmax != -1E9);
    return zmax;
  }

  // COMMENT: If We Modified Z Buffer At [i, j], We Need Update The Tree. 
  // static void UpdateHZBufferTree(HZBufferNode* tree, const int i, const int j, const float z) NOEXCEPT
  // {
  //   // COMMENT: If The Pixel Is Inside The Tree. Update It.
  //   if (tree->imin <= i && i <= tree->imax && tree->jmin <= j && j <= tree->jmax)
  //   {
  //     tree->zmax = std::max(tree->zmax, z);
  //     if (tree->ll) UpdateHZBufferTree(tree->ll, i, j, z);
  //     if (tree->lr) UpdateHZBufferTree(tree->lr, i, j, z);
  //     if (tree->ul) UpdateHZBufferTree(tree->ul, i, j, z);
  //     if (tree->ur) UpdateHZBufferTree(tree->ur, i, j, z);
  //   }
  // }

  static void UpdateHZBufferTree(const Canvas& canvas, HZBufferNode* tree, const int imin, const int imax, const int jmin, const int jmax) NOEXCEPT
  {
    ASSERT(tree != nullptr);
    if (tree->imin == tree->imax && tree->jmin == tree->jmax)
    {
      tree->zmax = canvas.zbuffer[tree->imin][tree->jmin];
      return;
    }
    if (tree->ll != nullptr
      && ((tree->ll->imin <= imin && imin <= tree->ll->imax) || (tree->ll->imin <= imax && imax <= tree->ll->imax))
      && ((tree->ll->jmin <= jmin && jmin <= tree->ll->jmax) || (tree->ll->jmin <= jmax && jmax <= tree->ll->jmax))
    )
    {
      UpdateHZBufferTree(canvas, tree->ll, imin, imax, jmin, jmax);
      tree->zmax = std::max(tree->zmax, tree->ll->zmax);
    }
    if (tree->lr != nullptr
      && ((tree->lr->imin <= imin && imin <= tree->lr->imax) || (tree->lr->imin <= imax && imax <= tree->lr->imax))
      && ((tree->lr->jmin <= jmin && jmin <= tree->lr->jmax) || (tree->lr->jmin <= jmax && jmax <= tree->lr->jmax))
    )
    {
      UpdateHZBufferTree(canvas, tree->lr, imin, imax, jmin, jmax);
      tree->zmax = std::max(tree->zmax, tree->lr->zmax);
    }
    if (tree->ul != nullptr
      && ((tree->ul->imin <= imin && imin <= tree->ul->imax) || (tree->ul->imin <= imax && imax <= tree->ul->imax))
      && ((tree->ul->jmin <= jmin && jmin <= tree->ul->jmax) || (tree->ul->jmin <= jmax && jmax <= tree->ul->jmax))
    )
    {
      UpdateHZBufferTree(canvas, tree->ul, imin, imax, jmin, jmax);
      tree->zmax = std::max(tree->zmax, tree->ul->zmax);
    }
    if (tree->ur != nullptr
      && ((tree->ur->imin <= imin && imin <= tree->ur->imax) || (tree->ur->imin <= imax && imax <= tree->ur->imax))
      && ((tree->ur->jmin <= jmin && jmin <= tree->ur->jmax) || (tree->ur->jmin <= jmax && jmax <= tree->ur->jmax))
    )
    {
      UpdateHZBufferTree(canvas, tree->ur, imin, imax, jmin, jmax);
      tree->zmax = std::max(tree->zmax, tree->ur->zmax);
    }
  }
  
};

#endif //ACCELERATOR_H
