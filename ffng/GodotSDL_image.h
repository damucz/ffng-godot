#ifndef GODOTSDL_IMAGE_HEADER
#define GODOTSDL_IMAGE_HEADER

#include "GodotSDL.h"

SDL_Surface* IMG_Load(const char* filename);
std::string IMG_GetError();

#endif
