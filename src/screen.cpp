#include <glad/glad.h>
#include "lib.hpp"

void screen::screenshot(const GLuint FBO,
                        const GLenum mode,
                        const screen::ImageType type, 
                        const int width,
                        const int height,
                        const char *output_file)
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

  unsigned char *data;
//  int data_size = ((3 * width + 3) & ~3) * height;
  int data_size = width * height;
  data = (unsigned char *)malloc(data_size);

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glReadBuffer(mode);
  glReadPixels(0, 0,
               width, height,
               GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
               data);

  FILE *f_out = fopen(output_file, "wb");
  fwrite(&tga_header, sizeof(tga_header), 1, f_out);
  fwrite(data, data_size, 1, f_out);
  fclose(f_out);

  free(data);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
