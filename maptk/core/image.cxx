/*ckwg +5
 * Copyright 2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include "image.h"
#include <cstring>


namespace maptk
{

/// Default Constructor
image_memory
::image_memory()
: data_(0),
  size_(0)
{
}


/// Constructor - allocated n bytes
image_memory
::image_memory(size_t n)
: data_(new char[n]),
  size_(n)
{
}


/// Copy Constructor
image_memory
::image_memory(const image_memory& other)
: data_(new char[other.size()]),
  size_(other.size())
{
  std::memcpy(data_, other.data_, size_);
}


/// Destructor
image_memory
::~image_memory()
{
  delete [] reinterpret_cast<char*>(data_);
}


/// Assignment operator
image_memory&
image_memory
::operator=(const image_memory& other)
{
  if( this == &other )
  {
    return *this;
  }
  set_size(other.size());
  std::memcpy(data_, other.data_, size_);
  return *this;
}


/// Return a pointer to the allocated memory
void*
image_memory
::data()
{
  return data_;
}


/// Reallocate memory for n bytes
/// Do nothing if size has not changed
void
image_memory
::set_size(size_t n)
{
  if( size_ == n )
  {
    return;
  }
  delete [] reinterpret_cast<char*>(data_);
  data_ = 0;
  if (n > 0)
  {
    data_ = new char[n];
  }
  size_ = n;
}

//======================================================================


/// Default Constructor
image
::image()
: data_(),
  first_pixel_(NULL),
  width_(0),
  height_(0),
  depth_(0),
  w_step_(0),
  h_step_(0),
  d_step_(0)
{
}


/// Constructor that allocates image memory
image
::image(size_t width, size_t height, size_t depth, bool interleave)
: data_(new image_memory(width*height*depth)),
  first_pixel_(reinterpret_cast<byte*>(data_->data())),
  width_(width),
  height_(height),
  depth_(depth),
  w_step_(1),
  h_step_(width),
  d_step_(width*height)
{
  if( interleave )
  {
    d_step_ = 1;
    w_step_ = depth;
    h_step_ = depth*width;
  }
}


/// Constructor that points at existing memory
image
::image(const byte* first_pixel, size_t width, size_t height, size_t depth,
        ptrdiff_t w_step, ptrdiff_t h_step, ptrdiff_t d_step)
: data_(),
  first_pixel_(const_cast<byte*>(first_pixel)),
  width_(width),
  height_(height),
  depth_(depth),
  w_step_(w_step),
  h_step_(h_step),
  d_step_(d_step)
{
}


/// Constructor that shares memory with another image
image
::image(const image_memory_sptr& mem,
        const byte* first_pixel, size_t width, size_t height, size_t depth,
        ptrdiff_t w_step, ptrdiff_t h_step, ptrdiff_t d_step)
: data_(mem),
  first_pixel_(const_cast<byte*>(first_pixel)),
  width_(width),
  height_(height),
  depth_(depth),
  w_step_(w_step),
  h_step_(h_step),
  d_step_(d_step)
{
}


/// Copy Constructor
/// the new image will share the same memory as the old image
image
::image(const image& other)
: data_(other.data_),
  first_pixel_(other.first_pixel_),
  width_(other.width_),
  height_(other.height_),
  depth_(other.depth_),
  w_step_(other.w_step_),
  h_step_(other.h_step_),
  d_step_(other.d_step_)
{
}


/// Assignment operator
const image&
image
::operator=(const image& other)
{
  if( this == &other )
  {
    return *this;
  }
  data_ = other.data_;
  first_pixel_ = other.first_pixel_;
  width_ = other.width_;
  height_ = other.height_;
  depth_ = other.depth_;
  w_step_ = other.w_step_;
  h_step_ = other.h_step_;
  d_step_ = other.d_step_;
  return *this;
}


/// The size of the image data in bytes
/// This size includes all allocated image memory,
/// which could be larger than width*height*depth.
size_t
image
::size() const
{
  if( !data_ )
  {
    return 0;
  }
  return data_->size();
}


/// Compare to images to see if the pixels have the same values.
/// This does not require that the images have the same memory layout,
/// only that the images have the same dimensions and pixel values.
bool equal_content(const image& img1, const image& img2)
{
  if( img1.width()  != img2.width()  ||
      img1.height() != img2.height() ||
      img1.depth()  != img2.depth()  )
  {
    return false;
  }
  for( unsigned k=0; k<img1.depth(); ++k)
  {
    for( unsigned j=0; j<img1.height(); ++j)
    {
      for( unsigned i=0; i<img1.width(); ++i)
      {
        if( img1(i,j,k) != img2(i,j,k) )
        {
          return false;
        }
      }
    }
  }
  return true;
}


} // end namespace maptk
