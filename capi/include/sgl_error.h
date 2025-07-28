//
// Created by neo on 25-7-25.
//

#ifndef SDL_ERROR_H
#define SDL_ERROR_H

typedef enum sgl_error {
  SGL_SUCCESS = 0,
  SGL_FAILURE = 1,
  SGL_GPU_ERROR = 2,
  SGL_OUT_OF_MEMORY = 3,
  SGL_INVALID_ARGUMENT = 4,
} sgl_error_t;

#endif // SDL_ERROR_H
