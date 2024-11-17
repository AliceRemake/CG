/**
  ******************************************************************************
  * @file           : Entity.cpp
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-13
  ******************************************************************************
  */



#include <Entity.h>
#include <Loader.h>

NODISCARD  Vertex Polygon::Center(const std::vector<Vertex>& vertices, const Polygon& polygon) NOEXCEPT
{
  return std::accumulate(polygon.vertices.begin(), polygon.vertices.end(), Vertex(0.0f), [&](const Vertex& acc, const uint32_t vertex) -> Vertex {
    return acc + vertices[vertex];
  }) / (float)polygon.vertices.size();
}

NODISCARD  Normal Polygon::Normal(const std::vector<Vertex>& vertices, const Polygon& polygon) NOEXCEPT
{
  if (polygon.vertices.size() < 3)
  {
    return ::Normal(0.0f);  
  }
  Vector v0 = vertices[polygon.vertices[0]] - vertices[polygon.vertices[1]];
  Vector v1 = vertices[polygon.vertices[1]] - vertices[polygon.vertices[2]];
  return glm::normalize(glm::cross(v0, v1));
}

NODISCARD  Vertex AABB::Center(const AABB& aabb) NOEXCEPT
{
  return (aabb.vmin + aabb.vmax) / 2.0f; 
}

NODISCARD  Vector AABB::Radius(const AABB& aabb) NOEXCEPT
{
  return (aabb.vmax - aabb.vmin) / 2.0f;
}

NODISCARD  AABB AABB::From(const std::vector<Vertex>& vertices, const Polygon& polygon) NOEXCEPT
{
  AABB aabb = {
    .vmin = glm::vec3(INF), .vmax = glm::vec3(-INF),
  };
  for (const auto& vertex : polygon.vertices)
  {
    aabb.vmin = glm::min(aabb.vmin, vertices[vertex]);  
    aabb.vmax = glm::max(aabb.vmax, vertices[vertex]);
  }
  return aabb;
}

NODISCARD  bool AABB::OverLap(const AABB& lhs, const AABB& rhs) NOEXCEPT
{
  return (lhs.vmin.x <= rhs.vmax.x && rhs.vmin.x <= lhs.vmax.x)
      && (lhs.vmin.y <= rhs.vmax.y && rhs.vmin.y <= lhs.vmax.y)
      && (lhs.vmin.z <= rhs.vmax.z && rhs.vmin.z <= lhs.vmax.z);
}

NODISCARD  Model Model::FromObj(const char* filename) NOEXCEPT
{
  Model model;
  if (Loader::LoadObj(filename, model) != Loader::SUCCESS)
  {
    Fatal("Can Not Create Model From Obj File: %s", filename);  
  }
  return model;
}

NODISCARD  FrameBuffer FrameBuffer::From(SDL_Window* window, const Color& bgc) NOEXCEPT
{
  FrameBuffer frame_buffer;
  frame_buffer.window = window;
  frame_buffer.surface = SDL_GetWindowSurface(window);
  frame_buffer.format = SDL_GetPixelFormatDetails(frame_buffer.surface->format);
  frame_buffer.width = frame_buffer.surface->w;
  frame_buffer.height = frame_buffer.surface->h;
  frame_buffer.bgc = bgc;
  frame_buffer.buffer = (Uint32*)frame_buffer.surface->pixels;
  return frame_buffer;
}

 void FrameBuffer::Display(const FrameBuffer& frame_buffer) NOEXCEPT
{
  SDL_UpdateWindowSurface(frame_buffer.window);
}

 void FrameBuffer::Clear(const FrameBuffer& frame_buffer) NOEXCEPT
{
  SDL_ClearSurface(frame_buffer.surface, frame_buffer.bgc.r, frame_buffer.bgc.g, frame_buffer.bgc.b, 0.0f);
}

NODISCARD  ZBuffer ZBuffer::From(const FrameBuffer& frame_buffer, const float bgz) NOEXCEPT
{
  ZBuffer z_buffer;
  z_buffer.width = frame_buffer.width;
  z_buffer.height = frame_buffer.height;
  z_buffer.bgz = bgz;
  z_buffer.buffer = new float*[z_buffer.height];
  for (int i = 0; i < z_buffer.height; ++i)
  {
    z_buffer.buffer[i] = new float[z_buffer.width];
  }
  return z_buffer;
}

 void ZBuffer::Clear(const ZBuffer& z_buffer) NOEXCEPT
{
  for (int i = 0; i < z_buffer.height; ++i)
  {
    for (int j = 0; j < z_buffer.width; ++j)
    {
      z_buffer.buffer[i][j] = z_buffer.bgz;
    }
  }
}

NODISCARD std::vector<ZBH> ZBH::From(const Canvas& canvas) NOEXCEPT
{
    std::vector<ZBH> zbh_tree;

    zbh_tree.resize((Pow4(Log2(std::max(canvas.width, canvas.height)) + 1) - 1) / 3);

    int front = 0, rear = 0;
    static int* que = new int[zbh_tree.size()];

    const float bgz = canvas.z_buffer->bgz;
    
    zbh_tree[0] = ZBH
    {
        .valid = true,
        .zmax  = bgz,
        .xmin  = 0,
        .xmax  = canvas.width-1,
        .ymin  = 0,
        .ymax  = canvas.height-1,
    };

    if (zbh_tree[0].xmin == zbh_tree[0].xmax && zbh_tree[0].ymin == zbh_tree[0].ymax)
    {
        return zbh_tree;
    }

    que[rear++] = 0;

    while(front < rear)
    {
        const int cur = que[front++];
        const int xmin = zbh_tree[cur].xmin;
        const int xmax = zbh_tree[cur].xmax;
        const int ymin = zbh_tree[cur].ymin;
        const int ymax = zbh_tree[cur].ymax;
        const int xmid = (xmin + xmax) >> 1; 
        const int ymid = (ymin + ymax) >> 1; 

        for (int i = Child0(cur), j = 0; i <= Child3(cur) && (size_t)i < zbh_tree.size(); ++i, ++j)
        {
            zbh_tree[i].xmin = j & 1 ? xmid + 1 : xmin;
            zbh_tree[i].xmax = j & 1 ? xmax : xmid;
            zbh_tree[i].ymin = j & 2 ? ymid + 1 : ymin;
            zbh_tree[i].ymax = j & 2 ? ymax : ymid;
            zbh_tree[i].valid = true;
            zbh_tree[i].zmax = bgz;
            if (!(zbh_tree[i].xmin == zbh_tree[i].xmax && zbh_tree[i].ymin == zbh_tree[i].ymax))
            {
                que[rear++] = i;
            }
        }
    }
    
    return zbh_tree;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ZBH::Clear(std::vector<ZBH>& zbh_tree, Canvas& canvas) NOEXCEPT
{
    const float bgz = canvas.z_buffer->bgz;
    for (auto& zbh : zbh_tree)
    {
        zbh.zmax = bgz;
    }
}

NODISCARD float ZBH::Query(const std::vector<ZBH>& zbh_tree, const int xmin, const int xmax, const int ymin, const int ymax) NOEXCEPT
{
    ASSERT(!zbh_tree.empty());

    float zmax = -INF;

    int front = 0, rear = 0;
    static int* que = new int[zbh_tree.size()];

    que[rear++] = 0;
    
    while(front < rear)
    {
        const int cur = que[front++];

        ASSERT(zbh_tree[cur].valid);

        if (xmin <= zbh_tree[cur].xmin && zbh_tree[cur].xmax <= xmax && ymin <= zbh_tree[cur].ymin && zbh_tree[cur].ymax <= ymax)
        {
            zmax = std::max(zmax, zbh_tree[cur].zmax);
            continue;
        }

        for (int i = Child0(cur); i <= Child3(cur) && (size_t)i < zbh_tree.size(); ++i)
        {
            if (zbh_tree[i].valid && xmin <= zbh_tree[i].xmax && zbh_tree[i].xmin <= xmax && ymin <= zbh_tree[i].ymax && zbh_tree[i].ymin <= ymax)
            {
                que[rear++] = i;
            }
        }
    }

    ASSERT(zmax != -INF);
    
    return zmax;
}

void ZBH::Update(std::vector<ZBH>& zbh_tree, Canvas& canvas, const int xmin, const int xmax, const int ymin, const int ymax) NOEXCEPT
{
    ASSERT(!zbh_tree.empty());

    int top = 0;
    static int* stk = new int[zbh_tree.size()];

    stk[top++] = 0;

    for (int cur = 0; cur < top; ++cur)
    {
        ASSERT(zbh_tree[cur].valid);

        for (int i = Child0(cur), j = 0; i < Child3(cur) && (size_t)i < zbh_tree.size(); ++i, ++j)
        {
            if (zbh_tree[i].valid && xmin <= zbh_tree[i].xmax && zbh_tree[i].xmin <= xmax && ymin <= zbh_tree[i].ymax && zbh_tree[i].ymin <= ymax)
            {
                stk[top++] = i;
            }
        }
    }

    while(top--)
    {
        const int cur = stk[top];
        if (zbh_tree[cur].xmin == zbh_tree[cur].xmax && zbh_tree[cur].ymin == zbh_tree[cur].ymax)
        {
            zbh_tree[cur].zmax = canvas.z_buffer->buffer[zbh_tree[cur].ymin][zbh_tree[cur].xmin];
            continue;
        }
        for (int i = Child0(cur); i < Child3(cur) && (size_t)i < zbh_tree.size(); ++i)
        {
            if (zbh_tree[i].valid)
            {
                zbh_tree[cur].zmax = std::max(zbh_tree[cur].zmax, zbh_tree[i].zmax);
            }
        }
    }
}

NODISCARD  Canvas Canvas::From(FrameBuffer& frame_buffer, ZBuffer& z_buffer, const int offsetx, const int offsety, const int width, const int height) NOEXCEPT
{
  ASSERT(0 <= offsetx && 0 <= width && offsetx + width < frame_buffer.width);
  ASSERT(0 <= offsety && 0 <= height && offsety + height < frame_buffer.height);
  Canvas canvas;
  canvas.frame_buffer = &frame_buffer;
  canvas.z_buffer = &z_buffer;
  canvas.offsetx = offsetx;
  canvas.offsety = offsety;
  canvas.width = width;
  canvas.height = height;
  return canvas;
}
