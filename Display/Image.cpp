//******************************************************************************
//  @file Image.cpp
//  @author Nicolai Shlapunov
//
//  @details DevCore: Image Visual Object Class, implementation
//
//  @copyright Copyright (c) 2016, Devtronic & Nicolai Shlapunov
//             All rights reserved.
//
//  @section SUPPORT
//
//   Devtronic invests time and resources providing this open source code,
//   please support Devtronic and open-source hardware/software by
//   donations and/or purchasing products from Devtronic.
//
//******************************************************************************

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include "Image.h"

// *****************************************************************************
// *****************************************************************************
// ***   Image   ***************************************************************
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
Image::Image(int32_t x, int32_t y, const ImageDesc& img_dsc)
{
  x_start = x;
  y_start = y;
  width = img_dsc.width;
  height = img_dsc.height;
  x_end = x + width - 1;
  y_end = y + height - 1;
  bits_per_pixel = img_dsc.bits_per_pixel;
  img = img_dsc.img;
  palette = img_dsc.palette;
  transparent_color = img_dsc.transparent_color;
  hor_mirror = false;
}

// *****************************************************************************
// ***   Set Image function   **************************************************
// *****************************************************************************
void Image::SetImage(const ImageDesc& img_dsc)
{
  LockVisObject();
  // Update image only if something changed
  if((img != img_dsc.img) || (width != img_dsc.width) || (height != img_dsc.height) || (palette != img_dsc.palette) || (transparent_color != img_dsc.transparent_color))
  {
    InvalidateObjArea();
    width = img_dsc.width;
    height = img_dsc.height;
    x_end = x_start + width - 1;
    y_end = y_start + height - 1;
    bits_per_pixel = img_dsc.bits_per_pixel;
    img = img_dsc.img;
    palette = img_dsc.palette;
    transparent_color = img_dsc.transparent_color;
    InvalidateObjArea();
  }
  UnlockVisObject();
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void Image::DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t start_x)
{
  // Draw only if needed
  if((line >= y_start) && (line <= y_end) && (img != nullptr))
  {
    // Find idx in the image buffer
    uint32_t idx = (line - y_start) * width;
    // Find start x position
    int32_t start = x_start - start_x;
    // Prevent write in memory before buffer
    if(start < 0)
    {
      // Minus minus - plus
      idx -= start;
      start = 0;
    }
    // Find start x position
    int32_t end = x_end - start_x;
    // Prevent buffer overflow
    if(end >= n) end = n - 1;
    // Delta for cycle increment/decrement
    int32_t delta = 1;
    // Flip horizontally if needed
    if(hor_mirror)
    {
      idx += end - start;
      // Set delta to minus one for decrement cycle
      delta = -1;
    }
    // Draw image
    if(palette != nullptr)
    {
      // Get pointer to palette image data
      uint8_t* p_img = (uint8_t*)img;
      // Pixels data copy cycle
      for(int32_t i = start; i <= end; i++)
      {
        // Get pixel data
        color_t data = palette[p_img[idx]];
        // Change index
        idx += delta;
        // If not transparent - output to buffer
        if((int32_t)data != transparent_color) buf[i] = data;
      }
    }
    else
    {
      // Get pointer to image data
      color_t* p_img = (color_t*)img;
      // Pixels data copy cycle
      for(int32_t i = start; i <= end; i++)
      {
        // Get pixel data
        color_t data = p_img[idx];
        // Change index
        idx += delta;
        // If not transparent - output to buffer
        if((int32_t)data != transparent_color) buf[i] = data;
      }
    }
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void Image::DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t start_y)
{
  // Draw only if needed
  if((row >= x_start) && (row <= x_end))
  {
    // Find start x position
    int32_t start = y_start - start_y;
    // Prevent write in memory before buffer
    if(start < 0) start = 0;
    // Find start x position
    int32_t end = y_end - start_y;
    // Prevent buffer overflow
    if(end >= n) end = n - 1;
    // Have sense draw only if end pointer in buffer
    if(end > 0)
    {
      // Not implemented yet
    }
  }
}

// *****************************************************************************
// *****************************************************************************
// ***   ImagePalette   ********************************************************
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
ImagePalette::ImagePalette(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t* p_img, const color_t* p_palette)
{
  x_start = x;
  y_start = y;
  x_end = x + w - 1;
  y_end = y + h - 1;
  width = w;
  height = h;
  img = p_img;
  palette = p_palette;
}

// *****************************************************************************
// ***   Set Image function   **************************************************
// *****************************************************************************
Result ImagePalette::SetImage(const ImageDesc& img_dsc)
{
  Result result = Result::RESULT_OK;

  if(img_dsc.bits_per_pixel == sizeof(uint8_t) * 8u)
  {
    LockVisObject();
    // Update image only if something changed
    if((img != img_dsc.img) || (width != img_dsc.width) || (height != img_dsc.height) || (palette != img_dsc.palette))
    {
      InvalidateObjArea();
      width = img_dsc.width;
      height = img_dsc.height;
      x_end = x_start + width - 1;
      y_end = y_start + height - 1;
      img = img_dsc.imgp;
      palette = img_dsc.palette;
      InvalidateObjArea();
    }
    UnlockVisObject();
  }
  else
  {
    // Error - only palette images is supported
    result = Result::ERR_BAD_PARAMETER;
  }

  return result;
}

// *****************************************************************************
// ***   Set Image function   **************************************************
// *****************************************************************************
void ImagePalette::SetImage(const uint8_t* p_img, int32_t w, int32_t h, const color_t* p_palette)
{
  LockVisObject();
  // Update image only if something changed
  if((img != p_img) || (width != w) || (height != h) || (palette != p_palette))
  {
    InvalidateObjArea();
    width = w;
    height = h;
    x_end = x_start + width - 1;
    y_end = y_start + height - 1;
    img = p_img;
    palette = p_palette;
    InvalidateObjArea();
  }
  UnlockVisObject();
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void ImagePalette::DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t start_x)
{
  // Draw only if needed
  if((line >= y_start) && (line <= y_end) && (img != nullptr) && (palette != nullptr))
  {
    // Find start x position
    int32_t start = x_start - start_x;
    // Prevent write in memory before buffer
    if(start < 0) start = 0;
    // Find start x position
    int32_t end = x_end - start_x;
    // Prevent buffer overflow
    if(end >= n) end = n - 1;
    // Have sense draw only if end pointer in buffer
    if(x_end > 0)
    {
      int idx = (line - y_start) * width;
      for(int32_t i = start; i <= end; i++)
      {
        buf[i] = palette[img[idx++]];
      }
    }
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void ImagePalette::DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t start_y)
{
  // Draw only if needed
  if((row >= x_start) && (row <= x_end) && (img != nullptr) && (palette != nullptr))
  {
    // Find start x position
    int32_t start = y_start - start_y;
    // Prevent write in memory before buffer
    if(start < 0) start = 0;
    // Find start x position
    int32_t end = y_end - start_y;
    // Prevent buffer overflow
    if(end >= n) end = n - 1;
    // Have sense draw only if end pointer in buffer
    if(end > 0)
    {
      // Not implemented yet
    }
  }
}

// *****************************************************************************
// *****************************************************************************
// ***   ImageBitmap   *********************************************************
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
ImageBitmap::ImageBitmap(int32_t x, int32_t y, int32_t w, int32_t h, const color_t* p_img)
{
  x_start = x;
  y_start = y;
  x_end = x + w - 1;
  y_end = y + h - 1;
  width = w;
  height = h;
  img = p_img;
}

// *****************************************************************************
// ***   Set Image function   **************************************************
// *****************************************************************************
Result ImageBitmap::SetImage(const ImageDesc& img_dsc)
{
  Result result = Result::RESULT_OK;

  if(img_dsc.bits_per_pixel == sizeof(color_t) * 8u)
  {
    LockVisObject();
    // Update image only if something changed
    if((img != img_dsc.img) || (width != img_dsc.width) || (height != img_dsc.height))
    {
      InvalidateObjArea();
      width = img_dsc.width;
      height = img_dsc.height;
      x_end = x_start + width - 1;
      y_end = y_start + height - 1;
      img = img_dsc.imgb;
      InvalidateObjArea();
    }
    UnlockVisObject();
  }
  else
  {
    // Error - only bitmap images is supported
    result = Result::ERR_BAD_PARAMETER;
  }

  return result;
}

// *************************************************************************
// ***   Set Image function   **********************************************
// *************************************************************************
void ImageBitmap::SetImage(const color_t* p_img, int32_t w, int32_t h)
{
  LockVisObject();
  // Update image only if something changed
  if((img != p_img) || (width != w) || (height != h))
  {
    InvalidateObjArea();
    width = w;
    height = h;
    x_end = x_start + width - 1;
    y_end = y_start + height - 1;
    img = p_img;
    InvalidateObjArea();
  }
  UnlockVisObject();
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void ImageBitmap::DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t start_x)
{
  // Draw only if needed
  if((line >= y_start) && (line <= y_end) && (img != nullptr))
  {
    // Find start x position
    int32_t start = x_start - start_x;
    // Prevent write in memory before buffer
    if(start < 0) start = 0;
    // Find start x position
    int32_t end = x_end - start_x;
    // Prevent buffer overflow
    if(end >= n) end = n - 1;
    // Have sense draw only if end pointer in buffer
    if(x_end > 0)
    {
      int idx = (line - y_start) * width;
      for(int32_t i = start; i <= end; i++)
      {
        buf[i] = img[idx++];
      }
    }
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void ImageBitmap::DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t start_y)
{
  // Draw only if needed
  if((row >= x_start) && (row <= x_end) && (img != nullptr))
  {
    // Find start x position
    int32_t start = y_start - start_y;
    // Prevent write in memory before buffer
    if(start < 0) start = 0;
    // Find start x position
    int32_t end = y_end - start_y;
    // Prevent buffer overflow
    if(end >= n) end = n - 1;
    // Have sense draw only if end pointer in buffer
    if(end > 0)
    {
      // Not implemented yet
    }
  }
}

// *****************************************************************************
// *****************************************************************************
// ***   Palettes   ************************************************************
// *****************************************************************************
// *****************************************************************************

#if defined(COLOR_24BIT)

// *****************************************************************************
// ***   Palette with 8R-8G-4B levels (bits 3-3-2)   ***************************
// *****************************************************************************
const color_t PALETTE_884[256] = {
0x00000000, 0x00000024, 0x00000048, 0x0000006D, 0x00000091, 0x000000B6, 0x000000DA, 0x000000FF,
0x00002400, 0x00002424, 0x00002448, 0x0000246D, 0x00002491, 0x000024B6, 0x000024DA, 0x000024FF,
0x00004800, 0x00004824, 0x00004848, 0x0000486D, 0x00004891, 0x000048B6, 0x000048DA, 0x000048FF,
0x00006D00, 0x00006D24, 0x00006D48, 0x00006D6D, 0x00006D91, 0x00006DB6, 0x00006DDA, 0x00006DFF,
0x00009100, 0x00009124, 0x00009148, 0x0000916D, 0x00009191, 0x000091B6, 0x000091DA, 0x000091FF,
0x0000B600, 0x0000B624, 0x0000B648, 0x0000B66D, 0x0000B691, 0x0000B6B6, 0x0000B6DA, 0x0000B6FF,
0x0000DA00, 0x0000DA24, 0x0000DA48, 0x0000DA6D, 0x0000DA91, 0x0000DAB6, 0x0000DADA, 0x0000DAFF,
0x0000FF00, 0x0000FF24, 0x0000FF48, 0x0000FF6D, 0x0000FF91, 0x0000FFB6, 0x0000FFDA, 0x0000FFFF,
0x00550000, 0x00550024, 0x00550048, 0x0055006D, 0x00550091, 0x005500B6, 0x005500DA, 0x005500FF,
0x00552400, 0x00552424, 0x00552448, 0x0055246D, 0x00552491, 0x005524B6, 0x005524DA, 0x005524FF,
0x00554800, 0x00554824, 0x00554848, 0x0055486D, 0x00554891, 0x005548B6, 0x005548DA, 0x005548FF,
0x00556D00, 0x00556D24, 0x00556D48, 0x00556D6D, 0x00556D91, 0x00556DB6, 0x00556DDA, 0x00556DFF,
0x00559100, 0x00559124, 0x00559148, 0x0055916D, 0x00559191, 0x005591B6, 0x005591DA, 0x005591FF,
0x0055B600, 0x0055B624, 0x0055B648, 0x0055B66D, 0x0055B691, 0x0055B6B6, 0x0055B6DA, 0x0055B6FF,
0x0055DA00, 0x0055DA24, 0x0055DA48, 0x0055DA6D, 0x0055DA91, 0x0055DAB6, 0x0055DADA, 0x0055DAFF,
0x0055FF00, 0x0055FF24, 0x0055FF48, 0x0055FF6D, 0x0055FF91, 0x0055FFB6, 0x0055FFDA, 0x0055FFFF,
0x00AA0000, 0x00AA0024, 0x00AA0048, 0x00AA006D, 0x00AA0091, 0x00AA00B6, 0x00AA00DA, 0x00AA00FF,
0x00AA2400, 0x00AA2424, 0x00AA2448, 0x00AA246D, 0x00AA2491, 0x00AA24B6, 0x00AA24DA, 0x00AA24FF,
0x00AA4800, 0x00AA4824, 0x00AA4848, 0x00AA486D, 0x00AA4891, 0x00AA48B6, 0x00AA48DA, 0x00AA48FF,
0x00AA6D00, 0x00AA6D24, 0x00AA6D48, 0x00AA6D6D, 0x00AA6D91, 0x00AA6DB6, 0x00AA6DDA, 0x00AA6DFF,
0x00AA9100, 0x00AA9124, 0x00AA9148, 0x00AA916D, 0x00AA9191, 0x00AA91B6, 0x00AA91DA, 0x00AA91FF,
0x00AAB600, 0x00AAB624, 0x00AAB648, 0x00AAB66D, 0x00AAB691, 0x00AAB6B6, 0x00AAB6DA, 0x00AAB6FF,
0x00AADA00, 0x00AADA24, 0x00AADA48, 0x00AADA6D, 0x00AADA91, 0x00AADAB6, 0x00AADADA, 0x00AADAFF,
0x00AAFF00, 0x00AAFF24, 0x00AAFF48, 0x00AAFF6D, 0x00AAFF91, 0x00AAFFB6, 0x00AAFFDA, 0x00AAFFFF,
0x00FF0000, 0x00FF0024, 0x00FF0048, 0x00FF006D, 0x00FF0091, 0x00FF00B6, 0x00FF00DA, 0x00FF00FF,
0x00FF2400, 0x00FF2424, 0x00FF2448, 0x00FF246D, 0x00FF2491, 0x00FF24B6, 0x00FF24DA, 0x00FF24FF,
0x00FF4800, 0x00FF4824, 0x00FF4848, 0x00FF486D, 0x00FF4891, 0x00FF48B6, 0x00FF48DA, 0x00FF48FF,
0x00FF6D00, 0x00FF6D24, 0x00FF6D48, 0x00FF6D6D, 0x00FF6D91, 0x00FF6DB6, 0x00FF6DDA, 0x00FF6DFF,
0x00FF9100, 0x00FF9124, 0x00FF9148, 0x00FF916D, 0x00FF9191, 0x00FF91B6, 0x00FF91DA, 0x00FF91FF,
0x00FFB600, 0x00FFB624, 0x00FFB648, 0x00FFB66D, 0x00FFB691, 0x00FFB6B6, 0x00FFB6DA, 0x00FFB6FF,
0x00FFDA00, 0x00FFDA24, 0x00FFDA48, 0x00FFDA6D, 0x00FFDA91, 0x00FFDAB6, 0x00FFDADA, 0x00FFDAFF,
0x00FFFF00, 0x00FFFF24, 0x00FFFF48, 0x00FFFF6D, 0x00FFFF91, 0x00FFFFB6, 0x00FFFFDA, 0x00FFFFFF};

// *****************************************************************************
// ***   Palette with 7R-7G-5B levels (245 colors)   ***************************
// *****************************************************************************
const color_t PALETTE_775[256] = {
0x00000000, 0x0000002A, 0x00000055, 0x0000007F, 0x000000AA, 0x000000D4, 0x000000FF, 0x00002A00,
0x00002A2A, 0x00002A55, 0x00002A7F, 0x00002AAA, 0x00002AD4, 0x00002AFF, 0x00005500, 0x0000552A,
0x00005555, 0x0000557F, 0x000055AA, 0x000055D4, 0x000055FF, 0x00007F00, 0x00007F2A, 0x00007F55,
0x00007F7F, 0x00007FAA, 0x00007FD4, 0x00007FFF, 0x0000AA00, 0x0000AA2A, 0x0000AA55, 0x0000AA7F,
0x0000AAAA, 0x0000AAD4, 0x0000AAFF, 0x0000D400, 0x0000D42A, 0x0000D455, 0x0000D47F, 0x0000D4AA,
0x0000D4D4, 0x0000D4FF, 0x0000FF00, 0x0000FF2A, 0x0000FF55, 0x0000FF7F, 0x0000FFAA, 0x0000FFD4,
0x0000FFFF, 0x003F0000, 0x003F002A, 0x003F0055, 0x003F007F, 0x003F00AA, 0x003F00D4, 0x003F00FF,
0x003F2A00, 0x003F2A2A, 0x003F2A55, 0x003F2A7F, 0x003F2AAA, 0x003F2AD4, 0x003F2AFF, 0x003F5500,
0x003F552A, 0x003F5555, 0x003F557F, 0x003F55AA, 0x003F55D4, 0x003F55FF, 0x003F7F00, 0x003F7F2A,
0x003F7F55, 0x003F7F7F, 0x003F7FAA, 0x003F7FD4, 0x003F7FFF, 0x003FAA00, 0x003FAA2A, 0x003FAA55,
0x003FAA7F, 0x003FAAAA, 0x003FAAD4, 0x003FAAFF, 0x003FD400, 0x003FD42A, 0x003FD455, 0x003FD47F,
0x003FD4AA, 0x003FD4D4, 0x003FD4FF, 0x003FFF00, 0x003FFF2A, 0x003FFF55, 0x003FFF7F, 0x003FFFAA,
0x003FFFD4, 0x003FFFFF, 0x007F0000, 0x007F002A, 0x007F0055, 0x007F007F, 0x007F00AA, 0x007F00D4,
0x007F00FF, 0x007F2A00, 0x007F2A2A, 0x007F2A55, 0x007F2A7F, 0x007F2AAA, 0x007F2AD4, 0x007F2AFF,
0x007F5500, 0x007F552A, 0x007F5555, 0x007F557F, 0x007F55AA, 0x007F55D4, 0x007F55FF, 0x007F7F00,
0x007F7F2A, 0x007F7F55, 0x007F7F7F, 0x007F7FAA, 0x007F7FD4, 0x007F7FFF, 0x007FAA00, 0x007FAA2A,
0x007FAA55, 0x007FAA7F, 0x007FAAAA, 0x007FAAD4, 0x007FAAFF, 0x007FD400, 0x007FD42A, 0x007FD455,
0x007FD47F, 0x007FD4AA, 0x007FD4D4, 0x007FD4FF, 0x007FFF00, 0x007FFF2A, 0x007FFF55, 0x007FFF7F,
0x007FFFAA, 0x007FFFD4, 0x007FFFFF, 0x00BF0000, 0x00BF002A, 0x00BF0055, 0x00BF007F, 0x00BF00AA,
0x00BF00D4, 0x00BF00FF, 0x00BF2A00, 0x00BF2A2A, 0x00BF2A55, 0x00BF2A7F, 0x00BF2AAA, 0x00BF2AD4,
0x00BF2AFF, 0x00BF5500, 0x00BF552A, 0x00BF5555, 0x00BF557F, 0x00BF55AA, 0x00BF55D4, 0x00BF55FF,
0x00BF7F00, 0x00BF7F2A, 0x00BF7F55, 0x00BF7F7F, 0x00BF7FAA, 0x00BF7FD4, 0x00BF7FFF, 0x00BFAA00,
0x00BFAA2A, 0x00BFAA55, 0x00BFAA7F, 0x00BFAAAA, 0x00BFAAD4, 0x00BFAAFF, 0x00BFD400, 0x00BFD42A,
0x00BFD455, 0x00BFD47F, 0x00BFD4AA, 0x00BFD4D4, 0x00BFD4FF, 0x00BFFF00, 0x00BFFF2A, 0x00BFFF55,
0x00BFFF7F, 0x00BFFFAA, 0x00BFFFD4, 0x00BFFFFF, 0x00FF0000, 0x00FF002A, 0x00FF0055, 0x00FF007F,
0x00FF00AA, 0x00FF00D4, 0x00FF00FF, 0x00FF2A00, 0x00FF2A2A, 0x00FF2A55, 0x00FF2A7F, 0x00FF2AAA,
0x00FF2AD4, 0x00FF2AFF, 0x00FF5500, 0x00FF552A, 0x00FF5555, 0x00FF557F, 0x00FF55AA, 0x00FF55D4,
0x00FF55FF, 0x00FF7F00, 0x00FF7F2A, 0x00FF7F55, 0x00FF7F7F, 0x00FF7FAA, 0x00FF7FD4, 0x00FF7FFF,
0x00FFAA00, 0x00FFAA2A, 0x00FFAA55, 0x00FFAA7F, 0x00FFAAAA, 0x00FFAAD4, 0x00FFAAFF, 0x00FFD400,
0x00FFD42A, 0x00FFD455, 0x00FFD47F, 0x00FFD4AA, 0x00FFD4D4, 0x00FFD4FF, 0x00FFFF00, 0x00FFFF2A,
0x00FFFF55, 0x00FFFF7F, 0x00FFFFAA, 0x00FFFFD4, 0x00FFFFFF, 0x00000000, 0x0000002A, 0x00000055,
0x0000007F, 0x000000AA, 0x000000D4, 0x000000FF, 0x00002A00, 0x00002A2A, 0x00002A55, 0x00002A7F};

// *****************************************************************************
// ***   Palette with 6R-7G-6B levels (252 colors)   ***************************
// *****************************************************************************
const color_t PALETTE_676[256] = {
0x00000000, 0x00000033, 0x00000066, 0x00000099, 0x000000CC, 0x000000FF, 0x00002A00, 0x00002A33,
0x00002A66, 0x00002A99, 0x00002ACC, 0x00002AFF, 0x00005500, 0x00005533, 0x00005566, 0x00005599,
0x000055CC, 0x000055FF, 0x00007F00, 0x00007F33, 0x00007F66, 0x00007F99, 0x00007FCC, 0x00007FFF,
0x0000AA00, 0x0000AA33, 0x0000AA66, 0x0000AA99, 0x0000AACC, 0x0000AAFF, 0x0000D400, 0x0000D433,
0x0000D466, 0x0000D499, 0x0000D4CC, 0x0000D4FF, 0x0000FF00, 0x0000FF33, 0x0000FF66, 0x0000FF99,
0x0000FFCC, 0x0000FFFF, 0x00330000, 0x00330033, 0x00330066, 0x00330099, 0x003300CC, 0x003300FF,
0x00332A00, 0x00332A33, 0x00332A66, 0x00332A99, 0x00332ACC, 0x00332AFF, 0x00335500, 0x00335533,
0x00335566, 0x00335599, 0x003355CC, 0x003355FF, 0x00337F00, 0x00337F33, 0x00337F66, 0x00337F99,
0x00337FCC, 0x00337FFF, 0x0033AA00, 0x0033AA33, 0x0033AA66, 0x0033AA99, 0x0033AACC, 0x0033AAFF,
0x0033D400, 0x0033D433, 0x0033D466, 0x0033D499, 0x0033D4CC, 0x0033D4FF, 0x0033FF00, 0x0033FF33,
0x0033FF66, 0x0033FF99, 0x0033FFCC, 0x0033FFFF, 0x00660000, 0x00660033, 0x00660066, 0x00660099,
0x006600CC, 0x006600FF, 0x00662A00, 0x00662A33, 0x00662A66, 0x00662A99, 0x00662ACC, 0x00662AFF,
0x00665500, 0x00665533, 0x00665566, 0x00665599, 0x006655CC, 0x006655FF, 0x00667F00, 0x00667F33,
0x00667F66, 0x00667F99, 0x00667FCC, 0x00667FFF, 0x0066AA00, 0x0066AA33, 0x0066AA66, 0x0066AA99,
0x0066AACC, 0x0066AAFF, 0x0066D400, 0x0066D433, 0x0066D466, 0x0066D499, 0x0066D4CC, 0x0066D4FF,
0x0066FF00, 0x0066FF33, 0x0066FF66, 0x0066FF99, 0x0066FFCC, 0x0066FFFF, 0x00990000, 0x00990033,
0x00990066, 0x00990099, 0x009900CC, 0x009900FF, 0x00992A00, 0x00992A33, 0x00992A66, 0x00992A99,
0x00992ACC, 0x00992AFF, 0x00995500, 0x00995533, 0x00995566, 0x00995599, 0x009955CC, 0x009955FF,
0x00997F00, 0x00997F33, 0x00997F66, 0x00997F99, 0x00997FCC, 0x00997FFF, 0x0099AA00, 0x0099AA33,
0x0099AA66, 0x0099AA99, 0x0099AACC, 0x0099AAFF, 0x0099D400, 0x0099D433, 0x0099D466, 0x0099D499,
0x0099D4CC, 0x0099D4FF, 0x0099FF00, 0x0099FF33, 0x0099FF66, 0x0099FF99, 0x0099FFCC, 0x0099FFFF,
0x00CC0000, 0x00CC0033, 0x00CC0066, 0x00CC0099, 0x00CC00CC, 0x00CC00FF, 0x00CC2A00, 0x00CC2A33,
0x00CC2A66, 0x00CC2A99, 0x00CC2ACC, 0x00CC2AFF, 0x00CC5500, 0x00CC5533, 0x00CC5566, 0x00CC5599,
0x00CC55CC, 0x00CC55FF, 0x00CC7F00, 0x00CC7F33, 0x00CC7F66, 0x00CC7F99, 0x00CC7FCC, 0x00CC7FFF,
0x00CCAA00, 0x00CCAA33, 0x00CCAA66, 0x00CCAA99, 0x00CCAACC, 0x00CCAAFF, 0x00CCD400, 0x00CCD433,
0x00CCD466, 0x00CCD499, 0x00CCD4CC, 0x00CCD4FF, 0x00CCFF00, 0x00CCFF33, 0x00CCFF66, 0x00CCFF99,
0x00CCFFCC, 0x00CCFFFF, 0x00FF0000, 0x00FF0033, 0x00FF0066, 0x00FF0099, 0x00FF00CC, 0x00FF00FF,
0x00FF2A00, 0x00FF2A33, 0x00FF2A66, 0x00FF2A99, 0x00FF2ACC, 0x00FF2AFF, 0x00FF5500, 0x00FF5533,
0x00FF5566, 0x00FF5599, 0x00FF55CC, 0x00FF55FF, 0x00FF7F00, 0x00FF7F33, 0x00FF7F66, 0x00FF7F99,
0x00FF7FCC, 0x00FF7FFF, 0x00FFAA00, 0x00FFAA33, 0x00FFAA66, 0x00FFAA99, 0x00FFAACC, 0x00FFAAFF,
0x00FFD400, 0x00FFD433, 0x00FFD466, 0x00FFD499, 0x00FFD4CC, 0x00FFD4FF, 0x00FFFF00, 0x00FFFF33,
0x00FFFF66, 0x00FFFF99, 0x00FFFFCC, 0x00FFFFFF, 0x00000000, 0x00000033, 0x00000066, 0x00000099};

#elif defined(COLOR_16BIT)
// *****************************************************************************
// ***   Palette with 8R-8G-4B levels (bits 3-3-2)   ***************************
// *****************************************************************************
const color_t PALETTE_884[256] = {
0x0000, 0x0020, 0x0040, 0x0068, 0x0088, 0x00B0, 0x00D0, 0x00F8, 0x0001, 0x0021, 0x0041, 0x0069, 0x0089, 0x00B1, 0x00D1, 0x00F9, 
0x2002, 0x2022, 0x2042, 0x206A, 0x208A, 0x20B2, 0x20D2, 0x20FA, 0x4003, 0x4023, 0x4043, 0x406B, 0x408B, 0x40B3, 0x40D3, 0x40FB, 
0x6004, 0x6024, 0x6044, 0x606C, 0x608C, 0x60B4, 0x60D4, 0x60FC, 0x8005, 0x8025, 0x8045, 0x806D, 0x808D, 0x80B5, 0x80D5, 0x80FD, 
0xA006, 0xA026, 0xA046, 0xA06E, 0xA08E, 0xA0B6, 0xA0D6, 0xA0FE, 0xE007, 0xE027, 0xE047, 0xE06F, 0xE08F, 0xE0B7, 0xE0D7, 0xE0FF, 
0x0A00, 0x0A20, 0x0A40, 0x0A68, 0x0A88, 0x0AB0, 0x0AD0, 0x0AF8, 0x0A01, 0x0A21, 0x0A41, 0x0A69, 0x0A89, 0x0AB1, 0x0AD1, 0x0AF9, 
0x2A02, 0x2A22, 0x2A42, 0x2A6A, 0x2A8A, 0x2AB2, 0x2AD2, 0x2AFA, 0x4A03, 0x4A23, 0x4A43, 0x4A6B, 0x4A8B, 0x4AB3, 0x4AD3, 0x4AFB, 
0x6A04, 0x6A24, 0x6A44, 0x6A6C, 0x6A8C, 0x6AB4, 0x6AD4, 0x6AFC, 0x8A05, 0x8A25, 0x8A45, 0x8A6D, 0x8A8D, 0x8AB5, 0x8AD5, 0x8AFD, 
0xAA06, 0xAA26, 0xAA46, 0xAA6E, 0xAA8E, 0xAAB6, 0xAAD6, 0xAAFE, 0xEA07, 0xEA27, 0xEA47, 0xEA6F, 0xEA8F, 0xEAB7, 0xEAD7, 0xEAFF, 
0x1400, 0x1420, 0x1440, 0x1468, 0x1488, 0x14B0, 0x14D0, 0x14F8, 0x1401, 0x1421, 0x1441, 0x1469, 0x1489, 0x14B1, 0x14D1, 0x14F9, 
0x3402, 0x3422, 0x3442, 0x346A, 0x348A, 0x34B2, 0x34D2, 0x34FA, 0x5403, 0x5423, 0x5443, 0x546B, 0x548B, 0x54B3, 0x54D3, 0x54FB, 
0x7404, 0x7424, 0x7444, 0x746C, 0x748C, 0x74B4, 0x74D4, 0x74FC, 0x9405, 0x9425, 0x9445, 0x946D, 0x948D, 0x94B5, 0x94D5, 0x94FD, 
0xB406, 0xB426, 0xB446, 0xB46E, 0xB48E, 0xB4B6, 0xB4D6, 0xB4FE, 0xF407, 0xF427, 0xF447, 0xF46F, 0xF48F, 0xF4B7, 0xF4D7, 0xF4FF, 
0x1F00, 0x1F20, 0x1F40, 0x1F68, 0x1F88, 0x1FB0, 0x1FD0, 0x1FF8, 0x1F01, 0x1F21, 0x1F41, 0x1F69, 0x1F89, 0x1FB1, 0x1FD1, 0x1FF9, 
0x3F02, 0x3F22, 0x3F42, 0x3F6A, 0x3F8A, 0x3FB2, 0x3FD2, 0x3FFA, 0x5F03, 0x5F23, 0x5F43, 0x5F6B, 0x5F8B, 0x5FB3, 0x5FD3, 0x5FFB, 
0x7F04, 0x7F24, 0x7F44, 0x7F6C, 0x7F8C, 0x7FB4, 0x7FD4, 0x7FFC, 0x9F05, 0x9F25, 0x9F45, 0x9F6D, 0x9F8D, 0x9FB5, 0x9FD5, 0x9FFD, 
0xBF06, 0xBF26, 0xBF46, 0xBF6E, 0xBF8E, 0xBFB6, 0xBFD6, 0xBFFE, 0xFF07, 0xFF27, 0xFF47, 0xFF6F, 0xFF8F, 0xFFB7, 0xFFD7, 0xFFFF};

// *****************************************************************************
// ***   Palette with 7R-7G-5B levels (245 colors)   ***************************
// *****************************************************************************
const color_t PALETTE_775[256] = {
0x0000, 0x0028, 0x0050, 0x0078, 0x00A0, 0x00C8, 0x00F8, 0x4001, 0x4029, 0x4051, 0x4079, 0x40A1, 0x40C9, 0x40F9, 0xA002, 0xA02A, 
0xA052, 0xA07A, 0xA0A2, 0xA0CA, 0xA0FA, 0xE003, 0xE02B, 0xE053, 0xE07B, 0xE0A3, 0xE0CB, 0xE0FB, 0x4005, 0x402D, 0x4055, 0x407D, 
0x40A5, 0x40CD, 0x40FD, 0x8006, 0x802E, 0x8056, 0x807E, 0x80A6, 0x80CE, 0x80FE, 0xE007, 0xE02F, 0xE057, 0xE07F, 0xE0A7, 0xE0CF, 
0xE0FF, 0x0700, 0x0728, 0x0750, 0x0778, 0x07A0, 0x07C8, 0x07F8, 0x4701, 0x4729, 0x4751, 0x4779, 0x47A1, 0x47C9, 0x47F9, 0xA702, 
0xA72A, 0xA752, 0xA77A, 0xA7A2, 0xA7CA, 0xA7FA, 0xE703, 0xE72B, 0xE753, 0xE77B, 0xE7A3, 0xE7CB, 0xE7FB, 0x4705, 0x472D, 0x4755, 
0x477D, 0x47A5, 0x47CD, 0x47FD, 0x8706, 0x872E, 0x8756, 0x877E, 0x87A6, 0x87CE, 0x87FE, 0xE707, 0xE72F, 0xE757, 0xE77F, 0xE7A7, 
0xE7CF, 0xE7FF, 0x0F00, 0x0F28, 0x0F50, 0x0F78, 0x0FA0, 0x0FC8, 0x0FF8, 0x4F01, 0x4F29, 0x4F51, 0x4F79, 0x4FA1, 0x4FC9, 0x4FF9, 
0xAF02, 0xAF2A, 0xAF52, 0xAF7A, 0xAFA2, 0xAFCA, 0xAFFA, 0xEF03, 0xEF2B, 0xEF53, 0xEF7B, 0xEFA3, 0xEFCB, 0xEFFB, 0x4F05, 0x4F2D, 
0x4F55, 0x4F7D, 0x4FA5, 0x4FCD, 0x4FFD, 0x8F06, 0x8F2E, 0x8F56, 0x8F7E, 0x8FA6, 0x8FCE, 0x8FFE, 0xEF07, 0xEF2F, 0xEF57, 0xEF7F, 
0xEFA7, 0xEFCF, 0xEFFF, 0x1700, 0x1728, 0x1750, 0x1778, 0x17A0, 0x17C8, 0x17F8, 0x5701, 0x5729, 0x5751, 0x5779, 0x57A1, 0x57C9, 
0x57F9, 0xB702, 0xB72A, 0xB752, 0xB77A, 0xB7A2, 0xB7CA, 0xB7FA, 0xF703, 0xF72B, 0xF753, 0xF77B, 0xF7A3, 0xF7CB, 0xF7FB, 0x5705, 
0x572D, 0x5755, 0x577D, 0x57A5, 0x57CD, 0x57FD, 0x9706, 0x972E, 0x9756, 0x977E, 0x97A6, 0x97CE, 0x97FE, 0xF707, 0xF72F, 0xF757, 
0xF77F, 0xF7A7, 0xF7CF, 0xF7FF, 0x1F00, 0x1F28, 0x1F50, 0x1F78, 0x1FA0, 0x1FC8, 0x1FF8, 0x5F01, 0x5F29, 0x5F51, 0x5F79, 0x5FA1, 
0x5FC9, 0x5FF9, 0xBF02, 0xBF2A, 0xBF52, 0xBF7A, 0xBFA2, 0xBFCA, 0xBFFA, 0xFF03, 0xFF2B, 0xFF53, 0xFF7B, 0xFFA3, 0xFFCB, 0xFFFB, 
0x5F05, 0x5F2D, 0x5F55, 0x5F7D, 0x5FA5, 0x5FCD, 0x5FFD, 0x9F06, 0x9F2E, 0x9F56, 0x9F7E, 0x9FA6, 0x9FCE, 0x9FFE, 0xFF07, 0xFF2F, 
0xFF57, 0xFF7F, 0xFFA7, 0xFFCF, 0xFFFF, 0x0000, 0x0028, 0x0050, 0x0078, 0x00A0, 0x00C8, 0x00F8, 0x4001, 0x4029, 0x4051, 0x4079};

// *****************************************************************************
// ***   Palette with 6R-7G-6B levels (252 colors)   ***************************
// *****************************************************************************
const color_t PALETTE_676[256] = {
0x0000, 0x0030, 0x0060, 0x0090, 0x00C0, 0x00F8, 0x4001, 0x4031, 0x4061, 0x4091, 0x40C1, 0x40F9, 0xA002, 0xA032, 0xA062, 0xA092, 
0xA0C2, 0xA0FA, 0xE003, 0xE033, 0xE063, 0xE093, 0xE0C3, 0xE0FB, 0x4005, 0x4035, 0x4065, 0x4095, 0x40C5, 0x40FD, 0x8006, 0x8036, 
0x8066, 0x8096, 0x80C6, 0x80FE, 0xE007, 0xE037, 0xE067, 0xE097, 0xE0C7, 0xE0FF, 0x0600, 0x0630, 0x0660, 0x0690, 0x06C0, 0x06F8, 
0x4601, 0x4631, 0x4661, 0x4691, 0x46C1, 0x46F9, 0xA602, 0xA632, 0xA662, 0xA692, 0xA6C2, 0xA6FA, 0xE603, 0xE633, 0xE663, 0xE693, 
0xE6C3, 0xE6FB, 0x4605, 0x4635, 0x4665, 0x4695, 0x46C5, 0x46FD, 0x8606, 0x8636, 0x8666, 0x8696, 0x86C6, 0x86FE, 0xE607, 0xE637, 
0xE667, 0xE697, 0xE6C7, 0xE6FF, 0x0C00, 0x0C30, 0x0C60, 0x0C90, 0x0CC0, 0x0CF8, 0x4C01, 0x4C31, 0x4C61, 0x4C91, 0x4CC1, 0x4CF9, 
0xAC02, 0xAC32, 0xAC62, 0xAC92, 0xACC2, 0xACFA, 0xEC03, 0xEC33, 0xEC63, 0xEC93, 0xECC3, 0xECFB, 0x4C05, 0x4C35, 0x4C65, 0x4C95, 
0x4CC5, 0x4CFD, 0x8C06, 0x8C36, 0x8C66, 0x8C96, 0x8CC6, 0x8CFE, 0xEC07, 0xEC37, 0xEC67, 0xEC97, 0xECC7, 0xECFF, 0x1200, 0x1230, 
0x1260, 0x1290, 0x12C0, 0x12F8, 0x5201, 0x5231, 0x5261, 0x5291, 0x52C1, 0x52F9, 0xB202, 0xB232, 0xB262, 0xB292, 0xB2C2, 0xB2FA, 
0xF203, 0xF233, 0xF263, 0xF293, 0xF2C3, 0xF2FB, 0x5205, 0x5235, 0x5265, 0x5295, 0x52C5, 0x52FD, 0x9206, 0x9236, 0x9266, 0x9296, 
0x92C6, 0x92FE, 0xF207, 0xF237, 0xF267, 0xF297, 0xF2C7, 0xF2FF, 0x1800, 0x1830, 0x1860, 0x1890, 0x18C0, 0x18F8, 0x5801, 0x5831, 
0x5861, 0x5891, 0x58C1, 0x58F9, 0xB802, 0xB832, 0xB862, 0xB892, 0xB8C2, 0xB8FA, 0xF803, 0xF833, 0xF863, 0xF893, 0xF8C3, 0xF8FB, 
0x5805, 0x5835, 0x5865, 0x5895, 0x58C5, 0x58FD, 0x9806, 0x9836, 0x9866, 0x9896, 0x98C6, 0x98FE, 0xF807, 0xF837, 0xF867, 0xF897, 
0xF8C7, 0xF8FF, 0x1F00, 0x1F30, 0x1F60, 0x1F90, 0x1FC0, 0x1FF8, 0x5F01, 0x5F31, 0x5F61, 0x5F91, 0x5FC1, 0x5FF9, 0xBF02, 0xBF32, 
0xBF62, 0xBF92, 0xBFC2, 0xBFFA, 0xFF03, 0xFF33, 0xFF63, 0xFF93, 0xFFC3, 0xFFFB, 0x5F05, 0x5F35, 0x5F65, 0x5F95, 0x5FC5, 0x5FFD, 
0x9F06, 0x9F36, 0x9F66, 0x9F96, 0x9FC6, 0x9FFE, 0xFF07, 0xFF37, 0xFF67, 0xFF97, 0xFFC7, 0xFFFF, 0x0000, 0x0030, 0x0060, 0x0090};

#endif
