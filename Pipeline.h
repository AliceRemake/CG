/**
  ******************************************************************************
  * @file           : Pipeline.h
  * @author         : AliceRemake
  * @brief          : None
  * @attention      : None
  * @date           : 24-11-8
  ******************************************************************************
  */



#ifndef PIPELINE_H
#define PIPELINE_H

#include <Common.h>
#include <Entity.h>
#include <Shader.h>

// COMMENT: Pipeline System. For Rendering A Scene.
struct Pipeline
{
   static void Render(const Setting& setting, const Shader::Config& config, const Canvas& canvas, const Camera& camera, const Scene& scene) NOEXCEPT;
};

#endif //PIPELINE_H
