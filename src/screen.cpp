#include <glad/glad.h>
#include "lib.hpp"

static void 
save_as_tga(unsigned char *data,
            const size_t data_size,
            const size_t width,
            const size_t height,
            const bool free_data,
            const screen::ImageType type,
            const char *filename)
{
  /* 
   * Tightly pack - not used due to memory alignment
   * causing some fields to be offset from TGA
   * file specification
   */ 
  #pragma pack(1)
  struct {
    unsigned char identsize;    // Size of folowing ID field
    unsigned char cmaptype;     // Color map type 0 = none
    unsigned char imagetype;    // Image type 2 = rgb
    short cmapstart;            // First entry in palette
    short cmapsize;             // Number of entries in palette 
    unsigned char cmapbpp;      // Number of bits per palette entry
    short xorigin;              // X origin
    short yorigin;              // Y origin
    short width;                // Width in pixels
    short height;               // Height in pixels
    unsigned char bpp;          // Bits per pixel
    unsigned char descriptor;   // Descriptor bits
  } tga_header;

  unsigned char imagetype, bpp;
  using namespace screen;
  switch (type) {
    case ImageType::IMAGE_TYPE_GREYSCALE:
      imagetype = 3;
      bpp = 8;
      break;
    default:
    case ImageType::IMAGE_TYPE_RGB:
      imagetype = 2;
      bpp = 24;
      break;
  }

  memset(&tga_header, 0, sizeof(tga_header));
  tga_header.imagetype = imagetype;
  tga_header.bpp = bpp;
  tga_header.width = (short)width;
  tga_header.height = (short)height;

  FILE *f_out = fopen(filename, "wb");
  fwrite(&tga_header, sizeof(tga_header), 1, f_out);
  fwrite(data, data_size, 1, f_out);
  fclose(f_out);

  if (free_data) {
    free(data);
  }
}

unsigned char *
screen::read_depth_tex_layer(const GLuint tex,
                             const int width,
                             const int height,
                             const int layer)
{
  unsigned char *data;
  data = (unsigned char *)malloc(width * height);
  glGetTextureSubImage(tex,
                       0,
                       0,
                       0,
                       layer,
                       width,
                       height,
                       1,
                       GL_DEPTH_COMPONENT,
                       GL_UNSIGNED_BYTE,
                       width * height,
                       data);

  return data;
}

void 
screen::depth_3D_layer_screenshot(const GLuint tex,
                                  const int width,
                                  const int height,
                                  const int layer,
                                  const char *filename)
{
  unsigned char *data;
  data = screen::read_depth_tex_layer(tex,
                                      width,
                                      height,
                                      layer);
  save_as_tga(data,
              width * height,
              width,
              height,
              true,
              ImageType::IMAGE_TYPE_GREYSCALE,
              filename); 
}

void 
screen::depth_2D_screenshot(const GLuint FBO,
                            const GLenum mode,
                            const screen::ImageType type, 
                            const int width,
                            const int height,
                            const char *output_file)
{
  unsigned char *data;

  int data_size = width * height;
  data = (unsigned char *)malloc(data_size);

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glReadBuffer(mode);
  glReadPixels(0, 0,
               width, height,
               GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
               data);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  save_as_tga(data,
              data_size,
              width,
              height,
              true,
              ImageType::IMAGE_TYPE_GREYSCALE,
              output_file);
}


