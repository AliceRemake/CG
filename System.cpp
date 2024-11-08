///**
//  ******************************************************************************
//  * @file           : System.cpp
//  * @author         : AliceRemake
//  * @brief          : None
//  * @attention      : None
//  * @date           : 24-11-6
//  ******************************************************************************
//  */
//
//
//
//#include <System.h>
//
//
//glm::mat4 Transform::Scale(const glm::vec3& v) noexcept {
//  return glm::scale(glm::mat4(1.0f), v);
//}
//
//glm::mat4 Transform::RotateX(const float radians) noexcept {
//  return glm::rotate(glm::mat4(1.0f), radians, glm::vec3(1.0f, 0.0f, 0.0f));
//}
//
//glm::mat4 Transform::RotateY(const float radians) noexcept {
//  return glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 1.0f, 0.0f));
//}
//
//glm::mat4 Transform::RotateZ(const float radians) noexcept {
//  return glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 0.0f, 1.0f));
//}
//
//glm::mat4 Transform::Translate(const glm::vec3& v) noexcept {
//  return glm::translate(glm::mat4(1.0f), v);
//}
//
//glm::mat4 Transform::Model(const ::Model& model) noexcept {
//  return Translate(model.translate) * RotateZ(model.rotate.z) * RotateY(model.rotate.y) * RotateX(model.rotate.x) * Scale(model.scale);
//}
//
//glm::mat4 Transform::View(const Camera& camera) noexcept {
//  return glm::lookAt(camera.position, camera.position + camera.direction, glm::vec3{0.0f, 1.0f, 0.0f});
//}
//
//glm::mat4 Transform::Project(const Camera& camera) noexcept {
//  return glm::perspective(camera.fov, camera.aspect, camera.near, camera.far);
//}
//
//glm::mat4 Transform::Viewport(const Canvas& canvas) noexcept {
//  // NOTE: glm stores column vectors
//  const int& w = canvas.width;
//  const int& h = canvas.height;
//  return glm::mat4 {
//    glm::vec4{      w/2.0f,        0.0f, 0.0f, 0.0f },
//    glm::vec4{        0.0f,      h/2.0f, 0.0f, 0.0f },
//    glm::vec4{        0.0f,        0.0f, 1.0f, 0.0f },
//    glm::vec4{ w/2.0f-0.5f, h/2.0f-0.5f, 0.0f, 1.0f },
//  };
//}
//
//uint32_t Rasterizer::MapColor(const Canvas&canvas, const Color& color) noexcept {
//  return SDL_MapRGB(canvas.pixel_format_details, nullptr, (Uint8)255.999f * std::clamp(color.r, 0.0f, 1.0f), (Uint8)255.999f * std::clamp(color.g, 0.0f, 1.0f), (Uint8)255.999f * std::clamp(color.b, 0.0f, 1.0f));
//}
//
//void Rasterizer::RenderPixel(const Canvas& canvas, const int i, const int j, const Color& color) noexcept {
//  if (i < 0 || i >= canvas.height || j < 0 || j >= canvas.width) return;
//  canvas.pixels[canvas.surface->w * (i + canvas.offsety) + j + canvas.offsetx] = MapColor(canvas, color);
//}
//
//void Rasterizer::RenderSegment(const Canvas& canvas, const int i, int jmin, int jmax, const Color& color) noexcept {
//  if (i < 0 || i >= canvas.height) return;
//  if (jmin > jmax || jmax < 0 || jmin >= canvas.width) return;
//  jmin = std::clamp(jmin, 0, canvas.width-1);
//  jmax = std::clamp(jmax, 0, canvas.width-1);
//  for (int j = jmin; j <= jmax; ++j) {
//    RenderPixel(canvas, i, j, color);
//  }
//}
//
//void Rasterizer::RenderRect(const Canvas& canvas, int imin, const int jmin, int imax, const int jmax, const Color& color) noexcept {
//  imin = std::clamp(imin, 0, canvas.height-1);
//  imax = std::clamp(imax, 0, canvas.height-1);
//  for (int i = imin; i <= imax; ++i) {
//    RenderSegment(canvas, i, jmin, jmax, color);
//  }
//}
//
//void Rasterizer::RenderLineDDA(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color) {
//  float length = std::max(std::abs(p1.x - p0.x), std::abs(p1.y - p0.y));
//  float dx = (p1.x - p0.x) / length;
//  float dy = (p1.y - p0.y) / length;
//  float i = 1.0f;
//  float x = p0.x;
//  float y = p0.y;
//  while(i <= length && x > 0.0f && x < canvas.width && y > 0.0f && y < canvas.height) {
//    RenderPixel(canvas, (int)std::floor(y), (int)std::floor(x), color);
//    x = x + dx;
//    y = y + dy;
//    i = i + 1.0f;
//  }
//}
//
//void Rasterizer::RenderLineBresenham(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color) {
//  const int dx = p1.x - p0.x;
//  const int dy = p1.y - p0.y;
//  int x = p0.x;
//  int y = p0.y;
//  if (dx == 0 && dy == 0) {
//    RenderPixel(canvas, x, y, color);
//    return;
//  }
//  const int delta_x = dx > 0 ? 1 : -1;
//  const int delta_y = dy > 0 ? 1 : -1;
//  if (std::abs(dx) > std::abs(dy)) {
//    const float m = std::abs((float)dy / (float)dx);
//    float e = m - 0.5f;
//    for (int i = 0; i <= std::abs(dx); ++i) {
//      RenderPixel(canvas, y, x, color);
//      while(e > 0.0f) {
//        y += delta_y;
//        e -= 1.0f;
//      }        
//      x += delta_x;
//      e += m;
//    }
//  } else {
//    const float m = std::abs((float)dx / (float)dy);
//    float e = m - 0.5f;
//    for (int i = 0; i <= std::abs(dy); ++i) {
//      RenderPixel(canvas, y, x, color);
//      while(e > 0.0f) {
//        x += delta_x;
//        e -= 1.0f;
//      }        
//      y += delta_y;
//      e += m;
//    }
//  }
//}
//
//void Rasterizer::RenderLine(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const Color& color) noexcept {
//  RenderLineBresenham(canvas, p0, p1, color);
//}
//
//void Rasterizer::RenderTriangleLine(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const glm::ivec2& p2, const Color& color) noexcept {
//  RenderLineBresenham(canvas, p0, p1, color);
//  RenderLineBresenham(canvas, p1, p2, color);
//  RenderLineBresenham(canvas, p2, p0, color);
//}
//
//void Rasterizer::RenderTriangleFill(const Canvas& canvas, const glm::ivec2& p0, const glm::ivec2& p1, const glm::ivec2& p2, const Color& color) noexcept {
//  std::vector points = {p0,p1,p2};
//  std::sort(points.begin(), points.end(), [](const glm::ivec2& lhs, const glm::ivec2& rhs) -> bool {
//    return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
//  });
//  if (points[0].y == points[2].y) {
//    RenderSegment(canvas, points[0].y, points[0].x, points[2].x, color);
//    return;
//  }
//  int jmin, jmax, dxl, dxr, delta_xl, delta_xr;
//  float ml, mr, el, er;
//  if (points[1].y != points[0].y) {
//    jmin = jmax = points[0].x;
//    if (points[0].x < points[1].x) {
//      dxl = points[2].x - points[0].x;
//      dxr = points[1].x - points[0].x;
//      ml = std::abs((float)dxl / (float)(points[2].y - points[0].y));
//      mr = std::abs((float)dxr / (float)(points[1].y - points[0].y));
//    } else {
//      dxl = points[1].x - points[0].x;
//      dxr = points[2].x - points[0].x;
//      ml = std::abs((float)dxl / (float)(points[1].y - points[0].y));
//      mr = std::abs((float)dxr / (float)(points[2].y - points[0].y));
//    }
//    delta_xl = dxl > 0 ? 1 : -1;
//    delta_xr = dxr > 0 ? 1 : -1;
//    el = ml - 0.5f;
//    er = mr - 0.5f;
//    for (int i = points[0].y; i < canvas.height && i < points[1].y; ++i) {
//      RenderSegment(canvas, i, std::min(jmin, jmax), std::max(jmin, jmax), color);
//      while(el > 0.0f) {
//        jmin += delta_xl;
//        el -= 1.0f;
//      }
//      while(er > 0.0f) {
//        jmax += delta_xr;
//        er -= 1.0f;
//      }
//      el += ml;
//      er += mr;
//    }
//  }
//  if (points[1].y != points[2].y) {
//    jmin = jmax = points[2].x;
//    if (points[0].x < points[1].x) {
//      dxl = points[0].x - points[2].x;
//      dxr = points[1].x - points[2].x;
//      ml = std::abs((float)dxl / (float)(points[0].y - points[2].y));
//      mr = std::abs((float)dxr / (float)(points[1].y - points[2].y));
//    } else {
//      dxl = points[1].x - points[2].x;
//      dxr = points[0].x - points[2].x;
//      ml = std::abs((float)dxl / (float)(points[1].y - points[2].y));
//      mr = std::abs((float)dxr / (float)(points[0].y - points[2].y));
//    }
//    delta_xl = dxl > 0 ? 1 : -1;
//    delta_xr = dxr > 0 ? 1 : -1;
//    el = ml - 0.5f;
//    er = mr - 0.5f;
//    for (int i = points[2].y; i >= 0 && i >= points[1].y; --i) {
//      RenderSegment(canvas, i, std::min(jmin, jmax), std::max(jmin, jmax), color);
//      while(el > 0.0f) {
//        jmin += delta_xl;
//        el -= 1.0f;
//      }
//      while(er > 0.0f) {
//        jmax += delta_xr;
//        er -= 1.0f;
//      }
//      el += ml;
//      er += mr;
//    }
//  }
//}
//
//// void RenderTriangleZBuffer(const Canvas& canvas, const Triangle& triangle) NOEXCEPT {
////   
//// }
//// void RenderTriangleFillZBuffer(const Canvas& canvas, const Triangle& triangle, const Color& color) noexcept {
////   AABB aabb = AABB::From(triangle);
//  
//  // std::vector points = {p0,p1,p2};
//  // std::sort(points.begin(), points.end(), [](const glm::ivec2& lhs, const glm::ivec2& rhs) -> bool {
//  //   return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
//  // });
//  // if (points[0].y >= points[2].y) return;
//  // int jmin, jmax, dxl, dxr, delta_xl, delta_xr;
//  // float ml, mr, el, er;
//  // if (points[1].y != points[0].y) {
//  //   jmin = jmax = points[0].x;
//  //   if (points[0].x < points[1].x) {
//  //     dxl = points[2].x - points[0].x;
//  //     dxr = points[1].x - points[0].x;
//  //     ml = std::abs((float)dxl / (float)(points[2].y - points[0].y));
//  //     mr = std::abs((float)dxr / (float)(points[1].y - points[0].y));
//  //   } else {
//  //     dxl = points[1].x - points[0].x;
//  //     dxr = points[2].x - points[0].x;
//  //     ml = std::abs((float)dxl / (float)(points[1].y - points[0].y));
//  //     mr = std::abs((float)dxr / (float)(points[2].y - points[0].y));
//  //   }
//  //   delta_xl = dxl > 0 ? 1 : -1;
//  //   delta_xr = dxr > 0 ? 1 : -1;
//  //   el = ml - 0.5f;
//  //   er = mr - 0.5f;
//  //   for (int i = std::max(0, points[0].y); i < canvas.height && i < points[1].y; ++i) {
//  //     RenderSegment(canvas, i, std::min(jmin, jmax), std::max(jmin, jmax), color);
//  //     while(el > 0.0f) {
//  //       jmin += delta_xl;
//  //       el -= 1.0f;
//  //     }
//  //     while(er > 0.0f) {
//  //       jmax += delta_xr;
//  //       er -= 1.0f;
//  //     }
//  //     el += ml;
//  //     er += mr;
//  //   }
//  // }
//  // if (points[1].y != points[2].y) {
//  //   jmin = jmax = points[2].x;
//  //   if (points[0].x < points[1].x) {
//  //     dxl = points[0].x - points[2].x;
//  //     dxr = points[1].x - points[2].x;
//  //     ml = std::abs((float)dxl / (float)(points[0].y - points[2].y));
//  //     mr = std::abs((float)dxr / (float)(points[1].y - points[2].y));
//  //   } else {
//  //     dxl = points[1].x - points[2].x;
//  //     dxr = points[0].x - points[2].x;
//  //     ml = std::abs((float)dxl / (float)(points[1].y - points[2].y));
//  //     mr = std::abs((float)dxr / (float)(points[0].y - points[2].y));
//  //   }
//  //   delta_xl = dxl > 0 ? 1 : -1;
//  //   delta_xr = dxr > 0 ? 1 : -1;
//  //   el = ml - 0.5f;
//  //   er = mr - 0.5f;
//  //   for (int i = std::min(canvas.height - 1, points[2].y); i >= 0 && i >= points[1].y; --i) {
//  //     RenderSegment(canvas, i, std::min(jmin, jmax), std::max(jmin, jmax), color);
//  //     while(el > 0.0f) {
//  //       jmin += delta_xl;
//  //       el -= 1.0f;
//  //     }
//  //     while(er > 0.0f) {
//  //       jmax += delta_xr;
//  //       er -= 1.0f;
//  //     }
//  //     el += ml;
//  //     er += mr;
//  //   }
//  // }
//// }
