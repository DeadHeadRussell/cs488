#include "Material.h"

#include <cstdio>
#include <GL/gl.h>
#include <GL/glu.h>
#include <jpeglib.h>
#include <tiffio.h>

using std::string;

Material::~Material() {}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks,
                             double shininess)
    : kd_(kd)
    , ks_(ks)
    , shininess_(shininess) {}

PhongMaterial::~PhongMaterial() {}

void PhongMaterial::Render() const {
  glColorMaterial(GL_FRONT, GL_SPECULAR);
  glColor3d(ks_.R(), ks_.G(), ks_.B());

  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glColor4d(kd_.R(), kd_.G(), kd_.B(), 0.5);

  glMaterialf(GL_FRONT, GL_SHININESS, shininess_);
}

Texture::Texture(string file_name) {
  // Pretty shitty way of determining file type but it works for me
  if (file_name.find(".jpg") != string::npos ||
      file_name.find(".jpeg") != string::npos) {
    tiff_ = false;
    jpeg_decompress_struct jpeg_info;
    jpeg_error_mgr jpeg_error;
    jpeg_info.err = jpeg_std_error(&jpeg_error);
    jpeg_create_decompress(&jpeg_info);

    FILE* in_file = fopen(file_name.c_str(), "rb");
    if (!in_file) {
      fprintf(stderr, "Error opening jpeg file %s\n", file_name.c_str());
      exit(-1);
    }
    jpeg_stdio_src(&jpeg_info, in_file);
    jpeg_read_header(&jpeg_info, TRUE);
    jpeg_start_decompress(&jpeg_info);

    width_ = jpeg_info.output_width;
    height_ = jpeg_info.output_height;
    components_ = jpeg_info.output_components;
    buffer_ = new unsigned char[components_ * width_ * height_];
    unsigned char* line;

    while (jpeg_info.output_scanline < jpeg_info.output_height) {
      line = buffer_ + components_ * width_ * jpeg_info.output_scanline;
      jpeg_read_scanlines(&jpeg_info, &line, 1);
    }

    jpeg_error.output_message((j_common_ptr)&jpeg_info);

    jpeg_finish_decompress(&jpeg_info);
    jpeg_destroy_decompress(&jpeg_info);
    fclose(in_file);
  } else if (file_name.find(".tif") != string::npos ||
             file_name.find(".tiff") != string::npos) {
    tiff_ = true;
    TIFF *tif = TIFFOpen(file_name.c_str(), "r");
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width_);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height_);
    buffer_ = (unsigned char*) _TIFFmalloc(width_ * height_ * sizeof(uint32));
    TIFFReadRGBAImage(tif, width_, height_, (uint32*)buffer_, 0);
    TIFFClose(tif);

    for (unsigned i = 0; i < width_ * height_; i++) {
      unsigned char r, g, b, a;
      a = buffer_[i*4];
      b = buffer_[i*4 + 1];
      g = buffer_[i*4 + 2];
      r = buffer_[i*4 + 3];

      buffer_[i*4] = r;
      buffer_[i*4 + 1] = b;
      buffer_[i*4 + 2] = g;
      buffer_[i*4 + 3] = a;
    }
  }

  // Create OpenGL Texture
  glGenTextures(1, &texture_object_);
  glBindTexture(GL_TEXTURE_2D, texture_object_);

  if (tiff_) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, buffer_);
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, buffer_);
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::~Texture() {
  glDeleteTextures(1, &texture_object_);
  if (tiff_) {
    _TIFFfree(buffer_);
  } else {
    delete[] buffer_;
  }
}

void Texture::Render() const {
  glBindTexture(GL_TEXTURE_2D, texture_object_);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

