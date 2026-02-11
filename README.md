# public-docs

StyleGuide.html is the programming style guide for an image processing header-only library: the ISL.

ImageBase.hpp and DirectImage.hpp are two illustrative header files from that library (with inline implementations removed).

A few salient features:

 * the image pixel format is a template parameter
     - they can be, for example, 8-bit grayscale, 32-bit ARGB, floating point, or even Bayer
 * images are containers, with iterators
 * an image may be an arbitrary region of interest of a larger image, with a shared image buffer
 * padding operations are provided to facilitate convolution operations (not included here)
 * the convolution kernel classes provide two-level iteration
     - the first level iterates the kernel position over the image
     - the second level iterates over the image pixels at the current kernel position
     - kernel geometry is arbitrary and need not be contiguous
