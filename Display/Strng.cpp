//******************************************************************************
//  @file Strng.cpp
//  @author Nicolai Shlapunov
//
//  @details DevCore: String Visual Object Class, implementation
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
#include "Strng.h"

#include <cstring> // for strlen()
#include <cstring>
#include <stdarg.h>
#include <stdio.h>

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
String::String(const char* str, int32_t x, int32_t y, color_t tc, Font& font)
{
  SetParams(str, x, y, tc, font);
}

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
String::String(const char* str, int32_t x, int32_t y, color_t tc, color_t bgc, Font& font)
{
  SetParams(str, x, y, tc, bgc, font);
}

// *****************************************************************************
// ***   SetParams   ***********************************************************
// *****************************************************************************
void String::SetParams(const char* str, int32_t x, int32_t y, color_t tc, Font& font)
{
  // Lock object for changes
  LockVisObject();
  // Do changes
  string = (const uint8_t*)str;
  x_start = x;
  y_start = y;
  txt_color = tc;
  bg_color = 0;
  font_ptr = &font;
  transpatent_bg = true;
  width = font.GetCharW() * strlen(str) * scale;
  height = font.GetCharH() * scale;
  x_end = x + width - 1;
  y_end = y + height - 1;
  rotation = 0;
  // Invalidate area
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   SetParams   ***********************************************************
// *****************************************************************************
void String::SetParams(const char* str, int32_t x, int32_t y, color_t tc, color_t bgc, Font& font)
{
  // Lock object for changes
  LockVisObject();
  // Do changes
  string = (const uint8_t*)str;
  x_start = x;
  y_start = y;
  txt_color = tc;
  bg_color = bgc;
  font_ptr = &font;
  transpatent_bg = false;
  width = font.GetCharW() * strlen(str) * scale;
  height = font.GetCharH() * scale;
  x_end = x + width - 1;
  y_end = y + height - 1;
  rotation = 0;
  // Invalidate area
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   SetColor   ************************************************************
// *****************************************************************************
void String::SetColor(color_t tc, color_t bgc, bool is_trnsp)
{
  // No Lock/Unlock since it will not break anythyng
  txt_color = tc;
  bg_color = bgc;
  transpatent_bg = is_trnsp;
  // Invalidate area
  InvalidateObjArea();
}

// *****************************************************************************
// ***   Public: SetFont   *****************************************************
// *****************************************************************************
void String::SetFont(Font& font)
{
  // Lock object for changes
  LockVisObject();
  // Do changes
  font_ptr = &font;
  width = font.GetCharW() * strlen((const char*)string) * scale;
  height = font.GetCharH() * scale;
  x_end = x_start + width - 1;
  y_end = y_start + height - 1;
  // Invalidate area
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   SetParams   ***********************************************************
// *****************************************************************************
void String::SetScale(uint8_t s)
{
  // Scale have to be positive number
  if(s > 0u)
  {
    // Lock object for changes
    LockVisObject();
    // Do changes
    scale = s;
    width = GetFontW() * strlen((const char*)string) * scale;
    height = GetFontH() * scale;
    x_end = x_start + width - 1;
    y_end = y_start + height - 1;
    // Invalidate area
    InvalidateObjArea();
    // Unlock object after changes
    UnlockVisObject();
  }
}

// *****************************************************************************
// ***   SetString   ***********************************************************
// *****************************************************************************
void String::SetString(const char* str)
{
  // Lock object for changes
  LockVisObject();
  // Set new pointer to string
  string = (const uint8_t*)str;
  width = GetFontW() * strlen(str) * scale;
  x_end = x_start + width - 1;
  // Invalidate area
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   SetString   ***********************************************************
// *****************************************************************************
void String::SetString(char* buf, uint32_t len, const char* format, ...)
{
  // Lock object for changes
  LockVisObject();
  // Argument list
  va_list arglist;
  // Create string
  va_start(arglist, format);
  vsnprintf(buf, len, format, arglist);
  va_end(arglist);
  //Set new pointer to string
  string = (const uint8_t*)buf;
  width = GetFontW() * strlen(buf) * scale;
  x_end = x_start + width - 1;
  // Invalidate area
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void String::DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t start_x)
{
  // Draw only if needed
  if((line >= y_start) && (line <= y_end) && (string != nullptr) && (font_ptr != nullptr))
  {
    // Current symbol X position
    int32_t x = x_start - start_x;
    // Number of bytes need skipped for draw line
    uint32_t skip_bytes = ((line - y_start) / scale) * GetFontBytePerChar() / GetFontH();
    // Pointer to string. Will increment for get characters.
    const uint8_t* str = string;

    // While we have symbols
    while(*str != '\0')
    {
      uint32_t b = 0;
      uint32_t w = 0;
      // Get pointer to character data
      const uint8_t* char_ptr = font_ptr->GetCharGataPtr(*str);
      // Get all symbol line
      for(uint32_t i = 0; i < GetFontBytePerChar() / GetFontH(); i++)
      {
        b |= char_ptr[skip_bytes + i] << (i*8);
      }
      // Output symbol line
      while(w < GetFontW())
      {
        for(uint8_t i = 0u; i < scale; i++)
        {
          // Put color in buffer only if visible
          if((x >= 0) && (x < n))
          {
            if((b&1) == 1)
            {
              buf[x] = txt_color;
            }
            else if(transpatent_bg == false)
            {
              buf[x] = bg_color;
            }
            else
            {
              // Empty statement
            }
          }
          x++;
        }
        b >>= 1;
        w++;
      }
      str++;
    }
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void String::DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t start_y)
{
  // Draw only if needed
  if((row >= x_start) && (row <= x_end) && (string != nullptr) && (font_ptr != nullptr))
  {
    // Find line in symbol
    int16_t start = y_start - start_y;
    // Find line in symbol
    int16_t line = (row - x_start);
    
    if(line >= 0)
    {
      // Get symbol
      uint8_t c = string[line / GetFontW()];
      // Get pointer to character data
      const uint8_t* char_ptr = font_ptr->GetCharGataPtr(c);
      // Find line in symbol
      line %= GetFontW();
      // Index to symbol in data array
      uint16_t bytes_per_line = GetFontBytePerChar() / GetFontH();
      // Get symbols lines
      for(int32_t i = 0; i < (int32_t)GetFontH(); i++)
      {
        if((start+i > 0) && (start+i < n))
        {
          // Find byte that conteins line data
          uint8_t b = char_ptr[i * bytes_per_line + line/8u];
          // Draw a pixel
          if(b & (1U << (line%8u)))
          {
            buf[start+i] = txt_color;
          }
          else if(transpatent_bg == false)
          {
            buf[start+i] = bg_color;
          }
          else
          {
            // Empty statement
          }
        }
      }
    }
  }
}
