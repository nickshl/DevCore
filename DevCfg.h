// *****************************************************************************
// @file DevCfg.h
// @author Nicolai Shlapunov
//
// @details DevCore: Config file, header
//
// @section COPYRIGHT
//
//  Copyright (c) 2016-2026, Devtronic & Nicolai Shlapunov
//  All rights reserved.
//
// @section LICENSE
//
//  SPDX-License-Identifier: BSD-3-Clause
//
//  Software License Agreement (BSD 3-Clause License)
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  3. Neither the name of Devtronic nor the names of its contributors may be
//     used to endorse or promote products derived from this software without
//     specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
// @section SUPPORT
//
//  Devtronic invests time and resources providing this open source code,
//  please support Devtronic and open-source hardware/software by
//  donations and/or purchasing products from Devtronic.
//
// *****************************************************************************

#ifndef DevCfg_h
#define DevCfg_h

// *****************************************************************************
// ***   Result include to include Result enum   *******************************
// *****************************************************************************
#include "Framework/Result.h"

// *****************************************************************************
// ***   DevCfgRtos to include RTOS wrapper   **********************************
// *****************************************************************************
#include "DevCfgRtos.h"

// *****************************************************************************
// ***   User configuration include   ******************************************
// *****************************************************************************
#include "DevCfgUsr.h"

// *****************************************************************************
// ***   Other includes   ******************************************************
// *****************************************************************************
#include "main.h"
#include <new> // for std::nothrow_t and std::bad_alloc

// *****************************************************************************
// ***   HAL Hardware includes or dummies   ************************************
// *****************************************************************************

//// ***   ADC   *****************************************************************
//#ifdef HAL_ADC_MODULE_ENABLED
//#include "adc.h"
//#else
//typedef uint32_t ADC_HandleTypeDef; // Dummy ADC handle for compilation
//#endif

//// ***   DAC   *****************************************************************
//#ifdef HAL_DAC_MODULE_ENABLED
//#include "dac.h"
//#else
//typedef uint32_t DAC_HandleTypeDef; // Dummy DAC handle for compilation
//#endif

// *****************************************************************************
// ***   System tasks stack sizes   ********************************************
// *****************************************************************************
#if !defined(DISPLAY_DRV_TASK_STACK_SIZE)
  #define DISPLAY_DRV_TASK_STACK_SIZE (1024u)
#endif
#if !defined(BUTTON_DRV_TASK_STACK_SIZE)
  #define BUTTON_DRV_TASK_STACK_SIZE RTOS_MINIMAL_STACK_SIZE
#endif
#if !defined(SOUND_DRV_TASK_STACK_SIZE)
  #define SOUND_DRV_TASK_STACK_SIZE (RTOS_MINIMAL_STACK_SIZE)
#endif

// *****************************************************************************
// ***   System tasks priorities   *********************************************
// *****************************************************************************
#if !defined(DISPLAY_DRV_TASK_PRIORITY)
  #define DISPLAY_DRV_TASK_PRIORITY (RTOS_IDLE_TASK_PRIORITY + 1u)
#endif
#if !defined(BUTTON_DRV_TASK_PRIORITY)
  #define BUTTON_DRV_TASK_PRIORITY (RTOS_IDLE_TASK_PRIORITY + 2u)
#endif
#if !defined(SOUND_DRV_TASK_PRIORITY)
  #define SOUND_DRV_TASK_PRIORITY (RTOS_IDLE_TASK_PRIORITY + 2u)
#endif

// Timer Task priority should be high. Otherwise if some task with highest
// priority will take over for long enough period, timer task wont be able to
// call timer callback for AppTask. If AppTask will not receive timer message
// within two times of timer period it will error out(queue empty error).
#if (RTOS_TIMER_TASK_PRIORITY != (RTOS_MAX_PRIORITIES - 1u))
  #warning "When Timer Task priority isn't highest"
#endif

// *****************************************************************************
// ***   Display Configuration   ***********************************************
// *****************************************************************************

// Max line in pixels for allocate buffer in Display Driver. Usually equal to
// maximum number of pixels in line. But sometimes can be greater than that.
// For example ILI9488 uses 18 bit color(3 bytes per pixel) and if 16 bit color 
// is used(2 bytes per pixel) in order to prepare data display driver need 1.5
// times more memory
#if !defined(DISPLAY_MAX_BUF_LEN)
#define DISPLAY_MAX_BUF_LEN 320
#endif

// By default there only one update area, that merges all update requests
// by making multiple areas, there can be multiple non-intersect areas(intersect
// areas still will be merged into one).
//#define MULTIPLE_UPDATE_AREAS 32

// If MULTIPLE_UPDATE_AREAS defined, UPDATE_AREA_ENABLED have to be defined too
#if defined(MULTIPLE_UPDATE_AREAS) && !defined(UPDATE_AREA_ENABLED)
#define UPDATE_AREA_ENABLED
#endif

// Color depth used by display
#if !defined(COLOR_24BIT) && !defined(COLOR_16BIT) && !defined(COLOR_3BIT)
#define COLOR_16BIT
#endif

#if defined(COLOR_24BIT)
// ***   Color type define   ***************************************************
typedef uint32_t color_t;
// ***   Color definitions   ***************************************************
enum Color
{
  COLOR_BLACK           = 0x00000000, //   0,   0,   0
  COLOR_VERYDARKGREY    = 0x00202020, //  32,  32,  32
  COLOR_DARKGREY        = 0x00404040, //  64,  64,  64
  COLOR_GREY            = 0x00808080, // 128, 128, 128
  COLOR_LIGHTGREY       = 0x00C0C0C0, // 192, 192, 192
  COLOR_WHITE           = 0x00FFFFFF, // 255, 255, 255

  COLOR_VERYDARKRED     = 0x00000020, //   0,   0,  32
  COLOR_DARKRED         = 0x00000040, //   0,   0,  64
  COLOR_MEDIUMRED       = 0x00000080, //   0,   0, 128
  COLOR_LIGHTRED        = 0x000000C0, //   0,   0, 192
  COLOR_RED             = 0x000000FF, //   0,   0, 255

  COLOR_VERYDARKGREEN   = 0x00002000, //   0,  32,   0
  COLOR_DARKGREEN       = 0x00004000, //   0,  64,   0
  COLOR_MEDIUMGREEN     = 0x00008000, //   0, 128,   0
  COLOR_LIGHTGREEN      = 0x0000C000, //   0, 192,   0
  COLOR_GREEN           = 0x0000FF00, //   0, 255,   0

  COLOR_VERYDARKBLUE    = 0x00200000, //  32,   0,   0
  COLOR_DARKBLUE        = 0x00400000, //  64,   0,   0
  COLOR_MEDIUMBLUE      = 0x00800000, // 128,   0,   0
  COLOR_LIGHTBLUE       = 0x00C00000, // 192,   0,   0
  COLOR_BLUE            = 0x00FF0000, // 255,   0,   0

  COLOR_VERYDARKYELLOW  = 0x00002020, //  32,  32,   0
  COLOR_DARKYELLOW      = 0x00004040, //  64,  64,   0
  COLOR_MEDIUMYELLOW    = 0x00008080, // 128, 128,   0
  COLOR_LIGHTYELLOW     = 0x0000C0C0, // 192, 192,   0
  COLOR_YELLOW          = 0x0000FFFF, // 255, 255,   0

  COLOR_VERYDARKCYAN    = 0x00202000, //   0,  32,  32
  COLOR_DARKCYAN        = 0x00404000, //   0,  64,  64
  COLOR_MEDIUMCYAN      = 0x00808000, //   0, 128, 128
  COLOR_LIGHTCYAN       = 0x00C0C000, //   0, 192, 192
  COLOR_CYAN            = 0x00FFFF00, //   0, 255, 255

  COLOR_VERYDARKMAGENTA = 0x00200020, //  32,   0,  32
  COLOR_DARKMAGENTA     = 0x00400040, //  64,   0,  64
  COLOR_MEDIUMMAGENTA   = 0x00800080, // 128,   0, 128
  COLOR_LIGHTMAGENTA    = 0x00C000C0, // 192,   0, 192
  COLOR_MAGENTA         = 0x00FF00FF, // 255,   0, 255
};
#elif defined(COLOR_16BIT)
// ***   Color type define   ***************************************************
typedef uint16_t color_t;
// ***   Color definitions   ***************************************************
enum Color
{
  COLOR_BLACK           = 0x0000, //   0,   0,   0
  COLOR_VERYDARKGREY    = 0x0421, //  32,  32,  32
  COLOR_DARKGREY        = 0x0842, //  64,  64,  64
  COLOR_GREY            = 0x1084, // 128, 128, 128
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
#elif defined(COLOR_3BIT)
// ***   Color type define   ***************************************************
typedef uint8_t color_t;
// ***   Color definitions   ***************************************************
enum Color
{
  COLOR_BLACK           = 0x00, // 0, 0, 0
  COLOR_RED             = 0x04, // 1, 0, 0
  COLOR_YELLOW          = 0x05, // 1, 0, 1
  COLOR_GREEN           = 0x02, // 0, 1, 0
  COLOR_CYAN            = 0x06, // 1, 1, 0
  COLOR_BLUE            = 0x01, // 0, 0, 1
  COLOR_MAGENTA         = 0x03, // 0, 1, 1
  COLOR_WHITE           = 0x07, // 1, 1, 1

  // Color mapped to basic colors
  COLOR_DARKGREY = COLOR_BLACK,
  COLOR_GREY = COLOR_WHITE
};
#else
  #error NO COLOR DEPTH DEFINED
#endif

// *****************************************************************************
// ***   Macroses   ************************************************************
// *****************************************************************************

// Number of array elements
#define NumberOf(x) (sizeof(x)/sizeof((x)[0]))

// MIN and MAX
#if !defined(MIN)
  #define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif
#if !defined(MAX)
  #define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

// If Break() macro isn't defined - just define it as empty
#if !defined(Break)
  #define Break()
#endif

// *****************************************************************************
// ***   Overloaded operators   ************************************************
// *****************************************************************************
// Standard new operators
void* operator new(size_t sz);
void* operator new[](size_t sz);
// std::nothrow new operators
void* operator new(size_t sz, const std::nothrow_t&) noexcept;
void* operator new[](size_t sz, const std::nothrow_t&) noexcept;
// Standard delete operators
void operator delete(void* p) noexcept;
void operator delete[](void* p) noexcept;
void operator delete(void* p, size_t) noexcept;
void operator delete[](void* p, size_t) noexcept;
// std::nothrow delete operators
void operator delete(void* p, const std::nothrow_t&) noexcept;
void operator delete[](void* p, const std::nothrow_t&) noexcept;

#endif
