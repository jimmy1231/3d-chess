#include "types.hpp"
#include "lib.hpp"
#include "helpers.h"

Data::Data(const char *f) : filename(f) {
  /*
   we can define this locally in this function because GL
   will copy all of this to GPU anyways, we do not need
   this data beyond defining it as a means to be copied.
   Note: this vertices data is already in screen coordinates,
   therefore, it is meant only for the fragment shader
   (i.e. no need for vertex processing)
  */
  load_obj(filename, data);
  vbo = init_static_array_vbo((void *)data.data(), 
                              data.size()*sizeof(ld_o::VBO_STRUCT));
  vao = init_array_VBO_STRUCT_vao(vbo,
                                  sizeof(ld_o::VBO_STRUCT));
}

void Data::print() { print_vbo(data); }
size_t Data::size() { return data.size(); }
