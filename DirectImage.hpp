/**
 *  @file  DirectImage.hpp
 *
 *  @brief  A class template for images with the pixel <em>values</em> stored in the
 *          image buffer.
 *
 *  A class template for images with the pixel <em>values</em> stored in the image buffer.
 */

  #ifndef   ISL_IMAGE_DIRECT_IMAGE_HPP_INCLUDED
    #define ISL_IMAGE_DIRECT_IMAGE_HPP_INCLUDED

    #include <ISL/Image/ImageBase.hpp>
    #include <ISL/Support/Option.hpp>
    #include <ISL/Support/PinnedCast.hpp>

    #include <algorithm>
    #include <atomic>
    #include <iterator>
    #include <memory>
    #include <stdexcept>
    #include <string>
    #include <type_traits>

    #include <cassert>
    #include <cstddef>
    #include <cstring>

    ///  option identifier for InitPixels
    extern const std::string Option_InitPixels;
    ///  option identifier for CopyPixels
    extern const std::string Option_CopyPixels;
    ///  option identifier for TransferOwnership
    extern const std::string Option_TransferOwnership;
    ///  option identifier for EndIterator
    extern const std::string Option_EndIterator;


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

    namespace ISL::Image
      {

//
//  Constructor options ...
//

        ///  initialize the pixels of an image upon its creation?
        using InitPixels = ISL::Support::Option<Option_InitPixels>;
        ///  copy the data with which an image is being initialized?
        using CopyPixels = ISL::Support::Option<Option_CopyPixels>;
        ///  transfer ownership of the image buffer to the new image?
        using TransferOwnership = ISL::Support::Option<Option_TransferOwnership>;
        ///  create an end iterator?
        using EndIterator = ISL::Support::Option<Option_EndIterator>;

/**
 *  @brief  A class template for images with the pixel <em>values</em> stored in the
 *          image buffer.
 *
 *  A class template for images with the pixel <em>values</em> stored in the image buffer.
 *  The template takes two arguments: the type used to store the pixel values, and the
 *  PixelInfo.  This class does not support packed pixels, but does support packed samples.
 *  One constructor can create an image which uses an externally managed image buffer;
 *  the copy constructors can create copies which share the image buffer of the source,
 *  even if the copy is of only part of the source image.
 *
 *  The class provides iterator and const_iterator classes for accessing from the first
 *  to the last pixels of an image.  To iterate over a rectangular region of interest,
 *  use a copy constructor to create an image of just that region, sharing the image
 *  buffer if appropriate, and iterate over the pixels of the copy.
 */

        template <typename              PixelT,
                  ISL::Image::PixelInfo pixInfo>
          class DirectImage : public ISL::Image::ImageBase
            {
//
//  Types ...
//
              public:
                ///  the pixel type
                using Pixel = PixelT;

              private:
                ///  the base iterator type
                template <typename IterPixelT,
                          typename IterImageT>
                  class iterator_base;  // forward declaration

                ///  @brief  the shared buffer information
                ///  @note   The callback is NOT propagated to clones.
                struct SharedInfo
                  {
                    ///  do we own the buffer?
                    const bool ownBuffer = true;
                    ///  the buffer reference count
                    std::atomic<int> refCount = 1;
                    ///  called when the refCount is decremented to zero
                    void (*release)(const void*, void*) = nullptr;
                    ///  passed as the second argument to the release callback
                    void* releaseContext = nullptr;
                  };

              public:
                ///  iterator to mutable pixels
                using iterator = iterator_base<PixelT,
                                               DirectImage>;
                ///  iterator to constant pixels
                using const_iterator = iterator_base<const PixelT,
                                                     const DirectImage>;

              public:
                class   FillPadder;
                class MirrorPadder;
                class   TilePadder;
//
//  Constructors and destructor ...
//
              public:
                DirectImage();
                ~DirectImage() override;

                DirectImage(ISL::Image::Size              width,
                            ISL::Image::Size              height,
                            ISL::Image::InitPixels        initPixels);
                DirectImage(const ISL::Image::Bounds&     bounds,
                            ISL::Image::InitPixels        initPixels);

                DirectImage(ISL::Image::Size              width,
                            ISL::Image::Size              height,
                            PixelT*                       pixels,
                            std::ptrdiff_t                pixelCount,
                            ISL::Image::CopyPixels        copyPixels,
                            ISL::Image::TransferOwnership transferOwnership);
                DirectImage(const ISL::Image::Bounds&     bounds,
                            PixelT*                       pixels,
                            std::ptrdiff_t                pixelCount,
                            ISL::Image::CopyPixels        copyPixels,
                            ISL::Image::TransferOwnership transferOwnership);

                DirectImage(const DirectImage&  src);
                DirectImage(      DirectImage&& src) noexcept;

                DirectImage(const DirectImage&        src,
                            int                       index,
                            int                       count);
                DirectImage(const DirectImage&        src,
                            const ISL::Image::Bounds& subImageBounds,
                            ISL::Image::CopyPixels    copyPixels);

                template <typename              PixelT2,
                          ISL::Image::PixelInfo pixInfo2>
                  explicit DirectImage(const ISL::Image::DirectImage<PixelT2,pixInfo2>& src);

                template <typename              PixelT2,
                          ISL::Image::PixelInfo pixInfo2,
                          typename              ScaleT>
                  DirectImage(const ISL::Image::DirectImage<PixelT2,pixInfo2>& src,
                              const ScaleT&                                    scaleFactor);

                template <typename MatrixT>
                  explicit DirectImage(const MatrixT& src);
                template <typename MatrixT>
                  MatrixT ToMatrix() const;

                DirectImage& operator = (const DirectImage&  rhs);
                DirectImage& operator = (      DirectImage&& rhs) noexcept;

                DirectImage clone() const;
              private:
                void   allocate(ISL::Image::InitPixels initPixels);
                void deallocate();
//
//  Accessors ...
//
              public:
                const ISL::Image::Bounds& BufferBounds() const;
                ISL::Image::Size  BufferWidth() const;
                ISL::Image::Size BufferHeight() const;
                ISL::Image::Size      Padding() const;
                bool PixelsAreContiguous() const;

                int RefCount() const;
                bool IsUnique() const;

                const PixelT* Buffer() const;
                      PixelT* Buffer();
                const PixelT* FirstPixel() const;
                      PixelT* FirstPixel();
                const PixelT* PixelPtr(const ISL::Image::Coordinates& point) const;
                      PixelT* PixelPtr(const ISL::Image::Coordinates& point);
                const PixelT* PixelPtr(ISL::Image::Coordinate x,
                                       ISL::Image::Coordinate y) const;
                      PixelT* PixelPtr(ISL::Image::Coordinate x,
                                       ISL::Image::Coordinate y);

                ISL::Image::Coordinates Coordinates(const PixelT* pixelPtr) const;

                bool operator == (const DirectImage& rhs) const;
//
//  Mutators ...
//
              public:
                void swap(DirectImage& src);
                void MakeUnique();
                void SetReleaseCallback(void (*callback)(const void*,void*),
                                        void* callbackData = nullptr);
                DirectImage& MovedTo(const ISL::Image::Coordinates& newMin);
//
//  Member data ...
//
              protected:
                ///  the bounds of the image buffer
                ISL::Image::Bounds bufferBounds;
                ///  the image buffer
                PixelT* buffer = nullptr;
                ///  the shared buffer information
                SharedInfo* sharedInfo = nullptr;
//
//  Iterator factory functions ...
//
              public:
                auto  begin()       ->       iterator;
                auto    end()       ->       iterator;
                auto  begin() const -> const_iterator;
                auto    end() const -> const_iterator;
                auto cbegin() const -> const_iterator;
                auto   cend() const -> const_iterator;
            };
      }


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

    namespace ISL::Image
      {

/**
 *  @brief  A base class template for DirectImage iterators.
 *
 *  A base class for the iterator and const_iterator classes.
 */

        template <typename              PixelT,
                  ISL::Image::PixelInfo pixInfo>
        template <typename              IterPixelT,
                  typename              IterImageT>
          class DirectImage<PixelT,pixInfo>::iterator_base
            {
//
//  Types ...
//
              public:
                ///  only forward iteration is supported
                using iterator_category = std::forward_iterator_tag;
                ///  dereferencing produces a pixel value
                using value_type = IterPixelT;
                ///  distance between points in an image
                using difference_type = ISL::Image::Size;
                ///  a pointer to a pixel
                using pointer = IterPixelT*;
                ///  a reference to a pixel
                using reference = IterPixelT&;
//
//  Constructor ...
//
              public:
                iterator_base();
                iterator_base(IterImageT*             image_,
                              ISL::Image::EndIterator endIterator);
//
//  Operators ...
//
              public:
                auto PixelPtr    () const -> pointer;
                auto operator -> () const -> pointer;
                auto operator *  () const -> reference;
                auto operator ++ ()                 -> iterator_base&;
                auto operator += (std::ptrdiff_t n) -> iterator_base&;
                bool operator == (const iterator_base& rhs) const;
//
//  Accessors ...
//
              public:
                IterImageT* Image() const;
                ISL::Image::Coordinates Coordinates() const;
//
//  Data ...
//
              private:
                ///  the current pointer
                pointer ptr = nullptr;
                ///  the end of the current run of contiguous pixels
                pointer endPtr = nullptr;
                ///  the last valid pointer
                pointer lastEndPtr = nullptr;
                ///  the length of runs of contiguous pixels
                difference_type runLength = 0;
                ///  the length of the padding between rows of pixels
                difference_type padding = 0;
                ///  a pointer to the image to which this iterator refers
                IterImageT* image = nullptr;
            };
      }


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

    namespace ISL::Image
      {

/**
 *  @brief  A class which pads an image with pixels of a given value.
 */

        template <typename              PixelT,
                  ISL::Image::PixelInfo pixInfo>
          class DirectImage<PixelT,pixInfo>::FillPadder
            {
              public:
                explicit FillPadder(const PixelT& pixelValue_);
                ISL::Image::DirectImage<PixelT,pixInfo>
                  PaddedImage
                    (const ISL::Image::DirectImage<PixelT,pixInfo>& srcImage,
                     const ISL::Image::Bounds&                      dstBounds) const;
                void PadImage(ISL::Image::DirectImage<PixelT,pixInfo>& dstImage,
                              const ISL::Image::Bounds&                srcBounds) const;
              private:
                ///  the pixel value with which to pad the image
                const PixelT pixelValue;
            };
      }


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

    namespace ISL::Image
      {

/**
 *  @brief  A class which pads an image by mirroring the source image.
 */

        template <typename              PixelT,
                  ISL::Image::PixelInfo pixInfo>
          class DirectImage<PixelT,pixInfo>::MirrorPadder
            {
              public:
                ISL::Image::DirectImage<PixelT,pixInfo>
                  PaddedImage
                    (const ISL::Image::DirectImage<PixelT,pixInfo>& srcImage,
                     const ISL::Image::Bounds&                      dstBounds) const;
                void PadImage(ISL::Image::DirectImage<PixelT,pixInfo>& dstImage,
                              const ISL::Image::Bounds&                srcBounds) const;
            };
      }


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

    namespace ISL::Image
      {

/**
 *  @brief  A class which pad an image by tiling the source image.
 */

        template <typename              PixelT,
                  ISL::Image::PixelInfo pixInfo>
          class DirectImage<PixelT,pixInfo>::TilePadder
            {
              public:
                ISL::Image::DirectImage<PixelT,pixInfo>
                  PaddedImage
                    (const ISL::Image::DirectImage<PixelT,pixInfo>& srcImage,
                     const ISL::Image::Bounds&                      dstBounds) const;
                void PadImage(ISL::Image::DirectImage<PixelT,pixInfo>& dstImage,
                              const ISL::Image::Bounds&                srcBounds) const;
            };
      }


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

//
//  Non-member swap function ...
//

    namespace ISL::Image
      {
        template <typename              PixelT,
                  ISL::Image::PixelInfo pixInfo>
          void swap(ISL::Image::DirectImage<PixelT,pixInfo>& A,
                    ISL::Image::DirectImage<PixelT,pixInfo>& B);
      }


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

//
//  The image transform functions ...
//

    namespace ISL::Image
      {
        template <typename ImageT,
                  typename FunctionT>
          ImageT Transform(const ImageT&    image,
                           const FunctionT& function);

        template <typename ImageT,
                  typename FunctionT>
          ImageT Transform(const ImageT&    image1,
                           const ImageT&    image2,
                           const FunctionT& function);
      }

  #endif
