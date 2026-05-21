//******************************************************************************
//  @file StringAligned.cpp
//  @author Nicolai Shlapunov
//
//  @details DevCore: StringAligned Visual Object Class, implementation
//
//  @copyright Copyright (c) 2026, Devtronic & Nicolai Shlapunov
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
#include <cstring>
#include <stdio.h>
#include <StringAligned.h>

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
StringAligned::StringAligned(const char* str, alignment_t algnmnt, int32_t x, int32_t y, uint32_t w, color_t tc, Font& font)
{
  SetParams(str, algnmnt, x, y, w, tc, font);
}

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
StringAligned::StringAligned(const char* str, alignment_t algnmnt, int32_t x, int32_t y, uint32_t w, color_t tc, color_t bgc, Font& font)
{
  SetParams(str, algnmnt, x, y, tc, bgc, font);
}

// *****************************************************************************
// ***   SetParams   ***********************************************************
// *****************************************************************************
void StringAligned::SetParams(const char* str, alignment_t algnmnt, int32_t x, int32_t y, uint32_t w, color_t tc, Font& font)
{
  // Lock object for changes
  LockVisObject();
  // Invalidate area for old string(needed if old string longer than new)
  InvalidateObjArea();
  // Do changes
  string = str;
  x_start = x;
  y_start = y;
  width = w;
  txt_color = tc;
  bg_color = 0;
  font_ptr = &font;
  transpatent_bg = true;
  alignment = algnmnt;
  // Recalculate size based on font and scale
  RecalculateSize();
  // Invalidate area for new string(needed if new string longer than old)
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   SetParams   ***********************************************************
// *****************************************************************************
void StringAligned::SetParams(const char* str, alignment_t algnmnt, int32_t x, int32_t y, uint32_t w, color_t tc, color_t bgc, Font& font)
{
  // Lock object for changes
  LockVisObject();
  // Invalidate area for old string(needed if old string longer than new)
  InvalidateObjArea();
  // Do changes
  string = str;
  x_start = x;
  y_start = y;
  width = w;
  txt_color = tc;
  bg_color = bgc;
  font_ptr = &font;
  transpatent_bg = false;
  alignment = algnmnt;
  // Recalculate size based on font and scale
  RecalculateSize();
  // Invalidate area for new string(needed if new string longer than old)
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   SetAlignment   ********************************************************
// *****************************************************************************
void StringAligned::SetAlignment(alignment_t algnmnt)
{
  // Lock object for changes
  LockVisObject();
  // Save alignment
  alignment = algnmnt;
  // Invalidate area for new string(needed if new string longer than old)
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   SetColor   ************************************************************
// *****************************************************************************
void StringAligned::SetColor(color_t tc, color_t bgc, bool is_trnsp)
{
  // Check if parameters changed
  if((tc != txt_color) || (bgc != bg_color) || (is_trnsp != transpatent_bg))
  {
    // No Lock/Unlock since it will not break anything
    txt_color = tc;
    bg_color = bgc;
    transpatent_bg = is_trnsp;
    // Invalidate area
    InvalidateObjArea();
  }
}

// *****************************************************************************
// ***   Public: SetFont   *****************************************************
// *****************************************************************************
void StringAligned::SetFont(Font& font)
{
  // Check if font changed
  if(&font != font_ptr)
  {
    // Lock object for changes
    LockVisObject();
    // Invalidate area for old string(needed if old string font bigger than new one)
    InvalidateObjArea();
    // Do changes
    font_ptr = &font;
    // Recalculate size based on font and scale
    RecalculateSize();
    // Invalidate area for new string(needed if new string font bigger than old one)
    InvalidateObjArea();
    // Unlock object after changes
    UnlockVisObject();
  }
}

// *****************************************************************************
// ***   SetParams   ***********************************************************
// *****************************************************************************
void StringAligned::SetScale(uint8_t s)
{
  // Scale have to be positive number
  if(s > 0u)
  {
    // Lock object for changes
    LockVisObject();
    // Invalidate area for old string(needed if old string scale bigger than new one)
    InvalidateObjArea();
    // Do changes
    scale = s;
    // Recalculate size based on font and scale
    RecalculateSize();
    // Invalidate area for new string(needed if new string scale bigger than old one)
    InvalidateObjArea();
    // Unlock object after changes
    UnlockVisObject();
  }
}

// *****************************************************************************
// ***   SetString   ***********************************************************
// *****************************************************************************
void StringAligned::SetString(const char* str, bool force)
{
  // Lock object for changes
  LockVisObject();
  // Ignore if same string set again
  if((string != str) || force)
  {
    // Invalidate area for old string(needed if old string longer than new)
    InvalidateObjArea();
    // Set new pointer to string
    string = str;
    // Since this function accept only pointer to constant string - clear length
    length = 0u;
    // Recalculate size based on font and scale
    RecalculateSize();
    // Invalidate area for new string(needed if new string longer than old)
    InvalidateObjArea();
  }
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   SetString   ***********************************************************
// *****************************************************************************
void StringAligned::SetString(char* buf, uint32_t len, const char* format, ...)
{
  // Argument list
  va_list arglist;
  va_start(arglist, format);
  // Lock object for changes
  LockVisObject();
  // Create string
  SetString((char*)buf, len, format, arglist);
  // Unlock object after changes
  UnlockVisObject();
  // End argument list
  va_end(arglist);
}

// *****************************************************************************
// ***   Printf   **************************************************************
// *****************************************************************************
void StringAligned::Printf(const char* format, ...)
{
  // If we don't have valid buffer - don't do anything
  if((string != nullptr) && (length != 0u))
  {
    // Argument list
    va_list arglist;
    va_start(arglist, format);
    // Lock object for changes
    LockVisObject();
    // Create string
    SetString((char*)string, length, format, arglist);
    // Unlock object after changes
    UnlockVisObject();
    // End argument list
    va_end(arglist);
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void StringAligned::DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t start_x)
{
  // Pointer to string. Will increment for get characters. Placed there because
  // of SetStringPtr() function existence.
  register const char* str = string;

  // Draw only if needed
  if((line >= y_start) && (line <= y_end) && (str != nullptr) && (font_ptr != nullptr))
  {
    // Current symbol X position
    int32_t x = x_start - start_x;
    // Number of bytes need skipped for draw line
    uint32_t skip_bytes = ((line - y_start) / scale) * GetFontBytePerChar() / GetFontH();

    // Calculate alignment
    if(alignment == CENTER)
    {
      x += (width - length_pixels) / 2;
    }
    else if(alignment == RIGHT)
    {
      x += width - length_pixels;
    }
    else
    {
      ; // Do nothing
    }

    // Find valid visible boundary
    int32_t visible_x_start = x_start - start_x;
    int32_t visible_x_end = x_end - start_x;
    if(visible_x_start < 0) visible_x_start = 0;
    if(visible_x_end >= n) visible_x_end = n - 1;

    // While we have symbols and still didn't reach object end
    while((*str != '\0') && (x < visible_x_end))
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
          if((x >= visible_x_start) && (x <= visible_x_end))
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
void StringAligned::DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t start_y)
{
  // Pointer to string
  register const char* str = string;

  // Draw only if needed
  if((row >= x_start) && (row <= x_end) && (str != nullptr) && (font_ptr != nullptr))
  {
    // Find line in symbol
    int16_t start = y_start - start_y;
    // Find line in symbol
    int16_t line = (row - x_start);

    if(line >= 0)
    {
      // Get symbol
      uint8_t c = str[line / GetFontW()];
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
          // Find byte that contains line data
          uint8_t b = char_ptr[i * bytes_per_line + line/8u];
          // Draw a pixel
          if(b & (1u << (line%8u)))
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

// *****************************************************************************
// ***   Private:  SetString   *************************************************
// *****************************************************************************
void StringAligned::SetString(char* buf, uint32_t len, const char* format, va_list& arglist)
{
  if((buf != nullptr) && (len != 0u))
  {
    // Invalidate area for old string(needed if old string longer than new)
    InvalidateObjArea();
    // Create string
    vsnprintf(buf, len, format, arglist);
    // Set new pointer to string
    string = buf;
    // Set length to use PrintString() function later
    length = len;
    // Recalculate size based on font and scale
    RecalculateSize();
    // Invalidate area for new string(needed if new string longer than old)
    InvalidateObjArea();
  }
}

// *****************************************************************************
// ***   Private: RecalculateSize   ********************************************
// *****************************************************************************
void StringAligned::RecalculateSize()
{
  // Calculated string full length in pixels
  length_pixels = strlen(string) * GetFontW() * scale;
  // Calculate height
  height = GetFontH() * scale;
  // Calculate end X and Y
  x_end = x_start + width - 1;
  y_end = y_start + height - 1;
}
