#ifndef LUMINOUS_IMAGE_CODEC_JPEG_HPP
#define LUMINOUS_IMAGE_CODEC_JPEG_HPP

#include <Luminous/ImageCodec.hpp>

namespace Luminous
{

  /// Image codec to handle JPEG files
  class ImageCodecJPEG : public ImageCodec
  {
    public:
      virtual ~ImageCodecJPEG();
      virtual bool canRead(FILE * file);
      virtual std::string extensions() const;
      virtual std::string name() const;
      virtual bool ping(ImageInfo & image, FILE * file);
      virtual bool read(Image & image, FILE * file);
      virtual bool write(const Image & image, FILE * file);  
  };

}

#endif
