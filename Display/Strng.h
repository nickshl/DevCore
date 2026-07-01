// *****************************************************************************
// @file Strng.h
// @author Nicolai Shlapunov
//
// @details DevCore: String Visual Object Class, header
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

#ifndef String_h
#define String_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include "DevCfg.h"
#include "Interfaces/IDisplay.h"
#include "Display/VisObject.h"
#include "Display/Font.h"
#include "Display/Fonts/Font_4x6.h"
#include "Display/Fonts/Font_6x8.h"
#include "Display/Fonts/Font_8x8.h"
#include "Display/Fonts/Font_8x12.h"
#include "Display/Fonts/Font_10x18.h"
#include "Display/Fonts/Font_12x16.h"

#include <stdarg.h> // for va_list

// *****************************************************************************
// ***   String Class   ********************************************************
// *****************************************************************************
class String : public VisObject
{
  public:
    // *************************************************************************
    // ***   Public: Constructor   *********************************************
    // *************************************************************************
    String() {};
 
    // *************************************************************************
    // ***   Public: Constructor   *********************************************
    // *************************************************************************
    String(const char* str, int32_t x, int32_t y, color_t tc, Font& font);

    // *************************************************************************
    // ***   Public: Constructor   *********************************************
    // *************************************************************************
    String(const char* str, int32_t x, int32_t y, color_t tc, color_t bgc, Font& font);

    // *************************************************************************
    // ***   Public: SetParams   ***********************************************
    // *************************************************************************
    void SetParams(const char* str, int32_t x, int32_t y, color_t tc, Font& font);

    // *************************************************************************
    // ***   Public: SetParams   ***********************************************
    // *************************************************************************
    void SetParams(const char* str, int32_t x, int32_t y, color_t tc, color_t bgc, Font& font);

    // *************************************************************************
    // ***   Public: GetString   ***********************************************
    // *************************************************************************
    const char* GetString() {return string;}

    // *************************************************************************
    // ***   Public: SetString   ***********************************************
    // *************************************************************************
    void SetString(const char* str, bool force = false);

    // *************************************************************************
    // ***   Public: SetString   ***********************************************
    // *************************************************************************
    void SetString(char* buffer, uint32_t n, const char* format, ...);

    // *************************************************************************
    // ***   Public: Printf   **************************************************
    // *************************************************************************
    void Printf(const char* format, ...);

    // *************************************************************************
    // ***   Public: SetStringPtr   ********************************************
    // *************************************************************************
    void SetStringPtr(const char* str) {string = str; length = 0u;}

    // *************************************************************************
    // ***   Public: SetColor   ************************************************
    // *************************************************************************
    void SetColor(color_t tc, color_t bgc = 0U, bool is_trnsp = true);

    // *************************************************************************
    // ***   Public: SetFont   *************************************************
    // *************************************************************************
    void SetFont(Font& font);

    // *************************************************************************
    // ***   Public: GetFont   *************************************************
    // *************************************************************************
    Font& GetFont() {return *font_ptr;}

    // *************************************************************************
    // ***   Public: SetScale   ************************************************
    // *************************************************************************
    void SetScale(uint8_t s);

    // *************************************************************************
    // ***   Public: GetScale   ************************************************
    // *************************************************************************
    uint8_t GetScale(void) {return scale;}

    // *************************************************************************
    // ***   Public: Put line in buffer   **************************************
    // *************************************************************************
    virtual void DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t y = 0);

    // *************************************************************************
    // ***   Public: Put line in buffer   **************************************
    // *************************************************************************
    virtual void DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t x = 0);

    // *************************************************************************
    // ***   Public: GetFontW   ************************************************
    // *************************************************************************
    uint32_t GetFontW() {return ((font_ptr == nullptr) ? 0u : font_ptr->GetCharW());}

    // *************************************************************************
    // ***   Public: GetFontH   ************************************************
    // *************************************************************************
    uint32_t GetFontH() {return ((font_ptr == nullptr) ? 0u : font_ptr->GetCharH());}

    // *************************************************************************
    // ***   Public: GetFontBytePerChar   **************************************
    // *************************************************************************
    uint32_t GetFontBytePerChar() {return ((font_ptr == nullptr) ? 0u : font_ptr->GetBytesPerChar());}

  private:
    // Pointer to string
    const char* string = nullptr;
    // Font type
    Font* font_ptr = nullptr;
    // Text color
    color_t txt_color = 0u;
    // Background color
    color_t bg_color = 0u;
    // Length of buffer(0 - we can't use SetString as printf style)
    uint16_t length = 0u;
    // Scale
    uint8_t scale = 1u;
    // Is background transparent ?
    bool transpatent_bg = false;

    // *************************************************************************
    // ***   Private: SetString   **********************************************
    // *************************************************************************
    void SetString(char* buf, uint32_t len, const char* format, va_list& arglist);

    // *************************************************************************
    // ***   Private: RecalculateSize   ****************************************
    // *************************************************************************
    void RecalculateSize();
};

#endif
