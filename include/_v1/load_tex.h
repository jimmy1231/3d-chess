#ifndef __RENDER_LOAD_TEX_H__
#define __RENDER_LOAD_TEX_H__

#include <CImg/CImg.h>

#define LOC(x,y,W) ( ((y)*(W) + (x)) * 3 )

/* Make sure dimensions are the closest powers of 2 */
inline int clamp_dim(int dim) {
  int _dim = 1; /* 2^0 */
  while (true) {
    if (_dim > dim) {
      break;
    }
    _dim *= 2;
  }

  return _dim / 2;
}

unsigned char *load_tex(const std::string &filepath,
                        int &width,
                        int &height) {
  unsigned char *data;
  /*
   * Basically we just have to reverse the bytes
   * to make it bottom up - since OpenGL processes
   * textures from the bottom-left pixel to the
   * top-right pixel (rather than top-left to bottom-
   * right), like usual
   */
  cimg_library::CImg<unsigned char> img;
  img.load(filepath.c_str());

  int N = clamp_dim(img.height());
  int M = clamp_dim(img.width());
  printf("Image loaded: %dx%d\n", N, M);

  data = (unsigned char *)malloc(3*N*M*sizeof(unsigned char));;
  int x, y, _y;
  for (y=0, _y=N-1; y<N; y++, _y--) {
    for (x=0; x<M; x++) {
      unsigned char r, g, b;
      int loc = LOC(x,y,M);

      r = img(x, _y, 0, 0);
      g = img(x, _y, 0, 1);
      b = img(x, _y, 0, 2);

      data[loc] = r;
      data[loc+1] = g;
      data[loc+2] = b;
    }
  }

  printf("Image transferred\n");
  width = M;
  height = N;
  return data;
}

#endif /* GL_UNSIGNED_SHORT_5_5_5_1 */ 