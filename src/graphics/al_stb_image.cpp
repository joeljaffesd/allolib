#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_TGA
#define STBI_NO_PSD
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_GIF
#define STBI_NO_PNM
#include "stb_image.h"

#include "al_stb_image.hpp"

al_stbImageData al_stbLoadImage(const char* filename) {
  al_stbImageData img;
  int w, h, n;
  // n will contain number of channels originally in image file
  // last parameter '4': force 4 channels
  img.data = stbi_load(filename, &w, &h, &n, 4);
  if (!img.data) {
    img.width = 0;
    img.height = 0;
  } else {
    img.width = w;
    img.height = h;
  }
  return img;  // empty object (size of data = 0)
}

void al_stbFreeImage(al_stbImageData* img) {
  stbi_image_free(img->data);
  img->width = 0;
  img->height = 0;
  img->data = nullptr;
}
