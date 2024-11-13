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
// Ref: https://paulbourke.net/dataformats/obj/
struct Loader
{
  enum Result
  {
    SUCCESS,
    ERROR_OPEN_FILE,
    ERROR_CLOSE_FILE,
    ERROR_READ_FILE,
  };
  
  FORCE_INLINE static Result LoadObj(const char* filename, Model& model) NOEXCEPT;
};

#endif //LOADER_H
