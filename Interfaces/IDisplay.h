//******************************************************************************
//  @file IDisplay.h
//  @author Nicolai Shlapunov
//
//  @details DevCore: Display driver interface, header
//
//  @section LICENSE
//
//   Software License Agreement (BSD License)
//
//   Copyright (c) 2016, Devtronic & Nicolai Shlapunov
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   3. Neither the name of the Devtronic nor the names of its contributors
//      may be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY DEVTRONIC ''AS IS'' AND ANY EXPRESS OR IMPLIED
//   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//   IN NO EVENT SHALL DEVTRONIC BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
//   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//******************************************************************************

#ifndef IDisplay_h
#define IDisplay_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include <DevCfg.h>

// *****************************************************************************
// ***   Enums   ***************************************************************
// *****************************************************************************

// Color definitions
enum Color
{
  COLOR_BLACK           = 0x0000, //   0,   0,   0
  COLOR_VERYDARKGREY    = 0xEF7B, //  32,  32,  32
  COLOR_DARKGREY        = 0xEF7B, //  64,  64,  64
  COLOR_GREY            = 0xEF7B, // 128, 128, 128
  COLOR_LIGHTGREY       = 0x18C6, // 192, 192, 192
  COLOR_WHITE           = 0xFFFF, // 255, 255, 255

  COLOR_VERYDARKRED     = 0x0018, //  32,   0,   0
  COLOR_DARKRED         = 0x0038, //  64,   0,   0
  COLOR_MEDIUMRED       = 0x0078, // 128,   0,   0
  COLOR_LIGHTRED        = 0x00B8, // 192,   0,   0
  COLOR_RED             = 0x00F8, // 255,   0,   0

  COLOR_VERYDARKGREEN   = 0xE000, //   0,  32,   0
  COLOR_DARKGREEN       = 0xE001, //   0,  64,   0
  COLOR_MEDIUMGREEN     = 0xE003, //   0, 128,   0
  COLOR_LIGHTGREEN      = 0xE005, //   0, 192,   0
  COLOR_GREEN           = 0xE007, //   0, 255,   0

  COLOR_VERYDARKBLUE    = 0x0300, //   0,   0,  32
  COLOR_DARKBLUE        = 0x0700, //   0,   0,  64
  COLOR_MEDIUMBLUE      = 0x0F00, //   0,   0, 128
  COLOR_LIGHTBLUE       = 0x1700, //   0,   0, 192
  COLOR_BLUE            = 0x1F00, //   0,   0, 255

  COLOR_VERYDARKYELLOW  = 0xE018, //  32,  32,   0
  COLOR_DARKYELLOW      = 0xE039, //  64,  64,   0
  COLOR_MEDIUMYELLOW    = 0xE07B, // 128, 128,   0
  COLOR_LIGHTYELLOW     = 0xE0BD, // 192, 192,   0
  COLOR_YELLOW          = 0xE0FF, // 255, 255,   0

  COLOR_VERYDARKCYAN    = 0xE300, //   0,  32,  32
  COLOR_DARKCYAN        = 0xE701, //   0,  64,  64
  COLOR_MEDIUMCYAN      = 0xEF03, //   0, 128, 128
  COLOR_LIGHTCYAN       = 0xF705, //   0, 192, 192
  COLOR_CYAN            = 0xFF07, //   0, 255, 255

  COLOR_VERYDARKMAGENTA = 0x0318, //  32,   0,  32
  COLOR_DARKMAGENTA     = 0x0738, //  64,   0,  64
  COLOR_MEDIUMMAGENTA   = 0x0F78, // 128,   0, 128
  COLOR_LIGHTMAGENTA    = 0x17B8, // 192,   0, 192
  COLOR_MAGENTA         = 0x1FF8, // 255,   0, 255
};

// *****************************************************************************
// ***   IDisplay   ************************************************************
// *****************************************************************************
class IDisplay
{
  public:
    // *************************************************************************
    // ***   Update Mode   *****************************************************
    // *************************************************************************
    enum Rotation
    {
      ROTATION_TOP = 0,
      ROTATION_LEFT,
      ROTATION_BOTTOM,
      ROTATION_RIGHT
    };

    // *************************************************************************
    // ***   Public: Constructor   *********************************************
    // *************************************************************************
    explicit IDisplay(int32_t in_width, int32_t in_height) :
      init_width(in_width), init_height(in_height), width(in_width), height(in_height) {};

    // *************************************************************************
    // ***   Public: Destructor   **********************************************
    // *************************************************************************
    virtual ~IDisplay() {};

    // *************************************************************************
    // ***   Public: Init screen   *********************************************
    // *************************************************************************
    virtual Result Init(void) {return Result::ERR_NOT_IMPLEMENTED;}

    // *************************************************************************
    // ***   Public: Write data stream to Display   ****************************
    // *************************************************************************
    virtual Result WriteDataStream(uint8_t* data, uint32_t n) = 0;

    // *************************************************************************
    // ***   Public: Check transfer status  ************************************
    // *************************************************************************
    virtual bool IsTransferComplete(void) = 0;

    // *************************************************************************
    // ***   Public: Stop transfer(i.e. Pull up CS line for LCD)  **************
    // *************************************************************************
    virtual Result StopTransfer(void) = 0;

    // *************************************************************************
    // ***   Public: Set output window   ***************************************
    // *************************************************************************
    virtual Result SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {return Result::ERR_NOT_IMPLEMENTED;}

    // *************************************************************************
    // ***   Public: Set screen orientation   **********************************
    // *************************************************************************
    virtual Result SetRotation(Rotation r) {return Result::ERR_NOT_IMPLEMENTED;}

    // *************************************************************************
    // ***   Public: Write color to screen   ***********************************
    // *************************************************************************
    virtual Result PushColor(uint16_t color) = 0;

    // *************************************************************************
    // ***   Public: Draw one pixel on  screen   *******************************
    // *************************************************************************
    virtual Result DrawPixel(int16_t x, int16_t y, uint16_t color) = 0;

    // *************************************************************************
    // ***   Public: Draw vertical line   **************************************
    // *************************************************************************
    virtual Result DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {return Result::ERR_NOT_IMPLEMENTED;}

    // *************************************************************************
    // ***   Public: Draw horizontal line   ************************************
    // *************************************************************************
    virtual Result DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {return Result::ERR_NOT_IMPLEMENTED;}

    // *************************************************************************
    // ***   Public: Fill full screen   ****************************************
    // *************************************************************************
    virtual Result FillScreen(uint16_t color) {return FillRect(0, 0, width, height, color);}

    // *************************************************************************
    // ***   Public: Fill rectangle on screen   ********************************
    // *************************************************************************
    virtual Result FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;

    // *************************************************************************
    // ***   Public: Invert display   ******************************************
    // *************************************************************************
    virtual Result InvertDisplay(bool invert) {return Result::ERR_NOT_IMPLEMENTED;}

    // *************************************************************************
    // ***   Public: Return screen width   *************************************
    // *************************************************************************
    inline int32_t GetWidth(void) {return width;}

    // *************************************************************************
    // ***   Public: Return screen height   ************************************
    // *************************************************************************
    inline int32_t GetHeight(void) {return height;}

    // *************************************************************************
    // ***   Return byte(s) per pixel   ****************************************
    // *************************************************************************
    inline int32_t GetBytesPerPixel(void) {return byte_per_pixel;}

    // *************************************************************************
    // ***   Public: Pass 8-bit (each) R,G,B, get back 16-bit packed color   ***
    // *************************************************************************
    uint16_t GetColor565(uint8_t r, uint8_t g, uint8_t b)
    {
      return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    // *************************************************************************
    // ***   Public: Return max line   *****************************************
    // *************************************************************************
    static constexpr int32_t GetMaxLine(void) {return DISPLAY_MAX_BUF_LEN;}

    // *************************************************************************
    // ***   Public: Return max line   *****************************************
    // *************************************************************************
    static constexpr int32_t GetMaxBpp(void) {return TFT_BPP;}

  protected:

    // Max line in pixels for allocate buffer in Display Driver
    static const int32_t DISPLAY_MAX_BUF_LEN = 320;
    // Max display byte per pixel for allocate buffer in Display Driver
    static const int32_t TFT_BPP = 2;

    // Byte(s) per pixel
    int32_t byte_per_pixel = TFT_BPP;
    // Width
    int32_t init_width = 0U;
    // Height
    int32_t init_height = 0U;
    // Width
    int32_t width = 0U;
    // Height
    int32_t height = 0U;
    // Rotation
    Rotation rotation = ROTATION_TOP;

  private:
    // *************************************************************************
    // ***   Private: Constructors and assign operator - prevent copying   *****
    // *************************************************************************
    IDisplay(const IDisplay&);
};

#endif
