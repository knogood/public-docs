/**
 *  @file  ImageBase.hpp
 *
 *  @brief  A base class for images.
 *
 *  A base class for images.
 */

  #ifndef   ISL_IMAGE_IMAGEBASE_HPP_INCLUDED
    #define ISL_IMAGE_IMAGEBASE_HPP_INCLUDED

    #include <ISL/Geometry/Box2D.hpp>
    #include <ISL/Geometry/Point2D.hpp>

    #include <limits>
    #include <stdexcept>
    #include <type_traits>

    #include <cassert>
    #include <cstddef>
    #include <cstdint>


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

    namespace ISL::Image
      {
//
//  Types and constants ...
//
        ///  @brief  packed information about the image pixel format
        ///          <ul>
        ///            <li>bits 0-7: the number of bits per sample</li>
        ///            <li>bits 8-15: the number of samples per pixel</li>
        ///            <li>bits 16-27: the number of entries in the lookup table</li>
        ///            <li>bits 28-31: reserved
        ///          </ul>
        using PixelInfo = std::uint32_t;

        ///  an image coordinate
        using Coordinate = std::int32_t;
        ///  the coordinates of a pixel
        using Coordinates = ISL::Geometry::Point2D<ISL::Image::Coordinate>;
        ///  the bounds of an image or buffer
        using Bounds = ISL::Geometry::Box2D<ISL::Image::Coordinates>;
        ///  the size of an image
        using Size = ISL::Image::Bounds::Size;

        static_assert (std::is_signed_v<Size>);  // also used for difference
      }


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

    namespace ISL::Image
      {

/**
 *  @brief  A base class for images.
 *
 *  A base class which establishes the pixel characteristics and the bounds for an image.
 */

        class ImageBase
          {
//
//  Constructors and destructor ...
//
            public:
              ImageBase(const ISL::Image::PixelInfo& pixelInfo_,
                        const ISL::Image::Bounds&    bounds_);
              virtual ~ImageBase();

              ImageBase(const ImageBase&  src);
              ImageBase(      ImageBase&& src) noexcept;

              ImageBase& operator = (const ImageBase&  rhs);
              ImageBase& operator = (      ImageBase&& rhs) noexcept;
//
//  Accessors ...
//
            public:
              ISL::Image::PixelInfo PixelInfo() const;
              int    BitsPerSample() const;
              int  SamplesPerPixel() const;
              int  LookupTableSize() const;
              bool UsesLookupTable() const;

              const ISL::Image::Bounds& Bounds() const;
              bool IsEmpty() const;

              ISL::Image::Size  Width() const;
              ISL::Image::Size Height() const;
              ISL::Image::Size   Size() const;
//
//  Data ...
//
            protected:
              ///  the pixel info
              ISL::Image::PixelInfo pixelInfo;
              ///  the image bounds
              ISL::Image::Bounds bounds;
          };
      }

  #endif
