//******************************************************************************
//  @file Primitives.cpp
//  @author Nicolai Shlapunov
//
//  @details DevCore: Primitives Visual Object Classes(Box, Line, Circle), implementation
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
#include "Primitives.h"

#include <cstdlib> // for abs()

// *****************************************************************************
// *****************************************************************************
// ***   Box   *****************************************************************
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
Box::Box(int32_t x, int32_t y, int32_t w, int32_t h, color_t c, bool is_fill)
{
  SetParams(x, y, w, h, c, is_fill);
}

// *****************************************************************************
// ***   SetParams   ***********************************************************
// *****************************************************************************
void Box::SetParams(int32_t x, int32_t y, int32_t w, int32_t h, color_t c, bool is_fill)
{
  // Lock object for changes
  LockVisObject();
  // Invalidate area for old position/size
  InvalidateObjArea();
  // Do changes
  color = c;
  bg_color = c;
  x_start = x;
  y_start = y;
  x_end = x + w - 1;
  y_end = y + h - 1;
  width = w;
  height = h;
  rotation = 0;
  fill = is_fill;
  border_width = is_fill ? 0 : 1; // Set border 1 pixel if border isn't fill
  // Invalidate area for new position/size
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   SetBorderWidth   ******************************************************
// *****************************************************************************
void Box::SetBorderWidth(int32_t width)
{
  // Update box only if border width changed
  if(width != border_width)
  {
    // Lock object for changes
    LockVisObject();
    // Set new border width
    border_width = width;
    // Set background color the same if border width is zero
    if(border_width == 0) bg_color = color;
    // Invalidate area
    InvalidateObjArea();
    // Unlock object after changes
    UnlockVisObject();
  }
}

// *****************************************************************************
// ***   SetColor   ************************************************************
// *****************************************************************************
void Box::SetColor(color_t c)
{
  // Update box only if color changed
  if(c != color)
  {
    // Lock object for changes
    LockVisObject();
    // Do changes
    color = c;
    // Set background color the same if border width is zero
    if(border_width == 0) bg_color = c;
    // Invalidate area
    InvalidateObjArea();
    // Unlock object after changes
    UnlockVisObject();
  }
}

// *****************************************************************************
// ***   SetBackgroundColor   **************************************************
// *****************************************************************************
void Box::SetBackgroundColor(color_t bgc)
{
  // Update box only if background color changed
  if(bgc != bg_color)
  {
    // Lock object for changes
    LockVisObject();
    // Do changes
    bg_color = bgc;
    // Invalidate area
    InvalidateObjArea();
    // Unlock object after changes
    UnlockVisObject();
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void Box::DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t start_x)
{
  // Draw only if needed
  if((line >= y_start) && (line <= y_end))
  {
    // Find start x position
    int32_t start = x_start - start_x;
    // Find start x position
    int32_t end = x_end - start_x;
    // Have sense draw only if end pointer in buffer
    if(end > 0)
    {
      // If fill or first/last line - must be solid
      if(fill)
      {
        // Prevent buffer overflow
        if(end >= n) end = n - 1;
        // Fill the line, start value can't be negative to prevent write in memory before buffer
        for(int32_t i = (start < 0) ? 0 : start; i <= end; i++) buf[i] = bg_color;
      }
      // Fill top and bottom border lines (if border exist)
      if((line < y_start + border_width) || (line > y_end - border_width))
      {
        // Prevent buffer overflow
        if(end >= n) end = n - 1;
        // Fill the line, start value can't be negative to prevent write in memory before buffer
        for(int32_t i = (start < 0) ? 0 : start; i <= end; i++) buf[i] = color;
      }
      else
      {
        // Fill left and right borders
        for(int32_t i = 0; i < border_width; i++)
        {
          int32_t front_idx = start + i;
          int32_t back_idx = (x_end - start_x) - i;
          if((front_idx >= 0) && (front_idx < n)) buf[front_idx] = color;
          if((back_idx  >= 0) && (back_idx  < n)) buf[back_idx]  = color;
        }
      }
    }
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void Box::DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t start_y)
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
      // If fill or first/last row - must be solid
      if(fill || row == x_start || row == x_end)
      {
        for(int32_t i = start; i <= end; i++) buf[i] = color;
      }
      else
      {
        if(y_start - start_y >= 0) buf[start] = color;
        if(y_end   - start_y <  n) buf[end]   = color;
      }
    }
  }
}

// *****************************************************************************
// *****************************************************************************
// ***   Shadow Box   **********************************************************
// *****************************************************************************
// *****************************************************************************

#if defined(COLOR_24BIT) // Shadow box available only for 24 bit color

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
ShadowBox::ShadowBox(int32_t x, int32_t y, int32_t w, int32_t h)
{
  SetParams(x, y, w, h);
}

// *****************************************************************************
// ***   SetParams   ***********************************************************
// *****************************************************************************
void ShadowBox::SetParams(int32_t x, int32_t y, int32_t w, int32_t h)
{
  // Lock object for changes
  LockVisObject();
  // Invalidate area for old position/size
  InvalidateObjArea();
  // Do changes
  x_start = x;
  y_start = y;
  x_end = x + w - 1;
  y_end = y + h - 1;
  width = w;
  height = h;
  rotation = 0;
  // Invalidate area for new position/size
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void ShadowBox::DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t start_x)
{
  // Draw only if needed
  if((line >= y_start) && (line <= y_end))
  {
    // Find start x position
    int32_t start = x_start - start_x;
    // Find start x position
    int32_t end = x_end - start_x;
    // Have sense draw only if end pointer in buffer
    if(end > 0)
    {
      // Prevent write in memory before buffer
      if(start < 0) start = 0;
      // Prevent buffer overflow
      if(end >= n) end = n - 1;
      // Fill the line
      for(int32_t i = start; i <= end; i++)
      {
        // Convert pointer to uint8_t to process color components individually
        uint8_t* color = reinterpret_cast<uint8_t*>(&buf[i]);
        // Process shadow
        color[0u] /= 2;
        color[1u] /= 2;
        color[2u] /= 2;
      }
    }
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void ShadowBox::DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t start_y)
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
      for(int32_t i = start; i <= end; i++)
      {
        // Convert pointer to uint8_t to process color components individually
        uint8_t* color = reinterpret_cast<uint8_t*>(&buf[i]);
        // Process shadow
        color[0u] /= 2;
        color[1u] /= 2;
        color[2u] /= 2;
      }
    }
  }
}

#endif

// *****************************************************************************
// *****************************************************************************
// ***   Line   ****************************************************************
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
Line::Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_t c)
{
  SetParams(x1, y1, x2, y2, c);
}

// *****************************************************************************
// ***   SetParams   ***********************************************************
// *****************************************************************************
void Line::SetParams(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_t c)
{
  // Lock object for changes
  LockVisObject();
  // Invalidate area for old position/size
  InvalidateObjArea();
  // Do changes
  color = c;
  x_start = x1;
  y_start = y1;
  x_end = x2;
  y_end = y2;
  width  = (x1 < x2) ? (x2 - x1) : (x1 - x2) + 1; // Width is one more than coordinates difference
  height = (y1 < y2) ? (y2 - y1) : (y1 - y2) + 1; // Height is one more than coordinates difference
  rotation = 0;
  // Invalidate area for new position/size
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void Line::DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t start_x)
{
  // Draw only if needed
  if(((line >= y_start) && (line <= y_end)) || ((line >= y_end) && (line <= y_start)))
  {
    const int32_t deltaX = abs(x_end - x_start);
    const int32_t deltaY = abs(y_end - y_start);
    const int32_t signX = x_start < x_end ? 1 : -1;
    const int32_t signY = y_start < y_end ? 1 : -1;

    int32_t error = deltaX - deltaY;

    int32_t x = x_start - start_x;
    int32_t y = y_start;

    int32_t end_x = x_end - start_x;
    while((x != end_x || y != y_end) && (y != line))
    {
      const int32_t error2 = error * 2;
      if(error2 > -deltaY) 
      {
        error -= deltaY;
        x += signX;
      }
      if(error2 < deltaX) 
      {
        error += deltaX;
        y += signY;
      }
    }

    // If we found current line
    if(y == line)
    {
      // Go and draw line
      do
      {
        if((x >= 0) && (x < n)) buf[x] = color;
        const int32_t error2 = error * 2;
        if(error2 > -deltaY) 
        {
          error -= deltaY;
          x += signX;
        }
        if(error2 < deltaX) 
        {
          break;
        }
      }
      while((x != end_x) || (y != y_end));
    }
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void Line::DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t start_y)
{
  // Draw only if needed
  if((row >= x_start) && (row <= x_end))
  {
    const int32_t deltaX = abs(y_end - y_start);
    const int32_t deltaY = abs(x_end - x_start);
    const int32_t signX = y_start < y_end ? 1 : -1;
    const int32_t signY = x_start < x_end ? 1 : -1;

    int32_t error = deltaX - deltaY;

    int32_t x = y_start - start_y;
    int32_t y = x_start;

    int32_t end_x = y_end - start_y;
    while((x != end_x || y != x_end) && (y != row))
    {
      const int32_t error2 = error * 2;
      if(error2 > -deltaY)
      {
        error -= deltaY;
        x += signX;
      }
      if(error2 < deltaX)
      {
        error += deltaX;
        y += signY;
      }
    }

    // If we found current line
    if(y == row)
    {
      // Go and draw line
      do
      {
        if((x >= 0) && (x < n)) buf[x] = color;
        const int32_t error2 = error * 2;
        if(error2 > -deltaY)
        {
          error -= deltaY;
          x += signX;
        }
        if(error2 < deltaX)
        {
          break;
        }
      }
      while((x != end_x) || (y != x_end));
    }
  }
}

// *****************************************************************************
// *****************************************************************************
// ***   Circle   **************************************************************
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
Circle::Circle(int32_t x, int32_t y, int32_t r, color_t c, bool is_fill, bool is_even)
{
  SetParams(x, y, r, c, is_fill, is_even);
}

// *****************************************************************************
// ***   SetParams   ***********************************************************
// *****************************************************************************
void Circle::SetParams(int32_t x, int32_t y, int32_t r, color_t c, bool is_fill, bool is_even)
{
  // Lock object for changes
  LockVisObject();
  // Invalidate area for old position/size
  InvalidateObjArea();
  // Do changes
  color = c;
  radius = r;
  x_start = x - r;
  y_start = y - r;
  x_end = x + r;
  y_end = y + r;
  width = r*2;
  height = r*2;
  rotation = 0;
  fill = is_fill;
  even = is_even;
  // Invalidate area for new position/size
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *************************************************************************
// ***   SetColor   **************************************II****************
// *************************************************************************
void Circle::SetColor(color_t c)
{
  if(c != color)
  {
    // Lock object for changes
    LockVisObject();
    // Do changes
    color = c;
    // Invalidate area
    InvalidateObjArea();
    // Unlock object after changes
    UnlockVisObject();
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void Circle::DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t start_x)
{
  // Draw only if needed
  if((line >= y_start) && (line <= y_end))
  {
    // We draw circle with center in 0,0 coordinates, those two variables to
    // calculate actual position
    int32_t x0 = x_start + radius - start_x;
    int32_t y0 = y_start + radius;
    // Variable for calculations, both for two branches
    int32_t t1 = radius >> 4;

    // Calculate point to chose algorithm. In this point X equal Y(if circle
    // center in 0,0 point). To calculate it we can use formula: Y = R / sqrt(2);
    // To avoid square root calculations we can precalculate it: Y = R / 1.4142;
    // We can replace division by multiplication on 1 / 1.4142: Y = R * 0.7071;
    // We also need point coordinate from the top of the circle, not from center,
    // so we need multiply R by 1 - 0.7071: Y = R * 0.2929;
    // Integer operations calculates faster than float point one, so we can
    // multiply radius by 2929 instead and then divide it by 10000:
    // int32_t shift = (radius * 2928) / 10000;
    // However, we can do better than that. We don't need to use base 10, we can
    // use base 2. In this case we can multiply radius by 0.2929 * 16384,
    // or ~4799, then we can divide result by 16384. But because it is power
    // of 2 number, we can replace division operation by shift operation:
    int32_t shift = (radius * 4799) >> 14;

    // Top and bottom half of circle(on Y axis)
    if((line <= y_start + shift) || (line >= y_end - shift - (even ? 1 : 0)))
    {
      // X and Y variables
      int32_t x = 0;
      int32_t y = radius;
      // Flag to break the cycle when no more new dots draw
      bool line_drawed = false;
      // Calculate cycle
      while(y >= x)
      {
        if((y0 + y == (even ? line + 1 : line)) || (y0 - y == line))
        {
          // Update buffer
          UpdateBuffer(buf, n, x0 - x, x0 + x);
          // We need this flag because for non-filled circle we draw one pixel
          // at a time and we may enter this section again and again until we
          // move to the next line ...
          line_drawed = true;
        }
        else
        {
          // ... at which point we have to break the cycle
          if(line_drawed == true) break;
        }
        // Circle calculation: Jesko's Method
        x++;
        t1 += x;
        int32_t t2 = t1 - y;
        if(t2 >= 0)
        {
          t1 = t2;
          y--;
        }
      }
    }
    else // Two middle half of circle(on Y axis)
    {
      int32_t x = radius;
      int32_t y = 0;
      // Calculate cycle
      while(x >= y)
      {
        if((y0 + y == (even ? line + 1 : line)) || (y0 - y == line))
        {
          // Update buffer
          UpdateBuffer(buf, n, x0 - x, x0 + x);
          // We know for sure, that Y will change at this point, so just break the cycle
          break;
        }
        // Circle calculation: Jesko's Method
        y++;
        t1 += y;
        int32_t t2 = t1 - x;
        if(t2 >= 0)
        {
          t1 = t2;
          x--;
        }
      }
    }
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void Circle::DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t start_y)
{
  if((row >= x_start) && (row <= x_end))
  {
    // Find start x position
    int32_t start = y_start - start_y;
    // Prevent write in memory before buffer
    if(start < 0) start = 0;
    // Find start x position
    int32_t end = y_end - start_y;
    // Prevent buffer overflow
    if(end > n) end = n;
    // Have sense draw only if end pointer in buffer
    if(end > 0)
    {
      for(int32_t i = start; i < end; i++) buf[i] = color;
    }
  }
}

// *****************************************************************************
// ***   Place line or two dots in buffer   ************************************
// *****************************************************************************
void Circle::UpdateBuffer(color_t* buf, int32_t n, int32_t xl, int32_t xr)
{
  if(fill)
  {
    if(xl < 0) xl = 0;
    if(xr < n) n = xr + (even ? 0 : 1); // We have to draw line from xl to xr including both coordinates, so we have to set n to xr + 1
    for(;xl < n; xl++)
    {
      buf[xl] = color;
    }
  }
  else
  {
    if((xl > 0) && (xl < n)) buf[xl] = color;
    if((xr > 0) && (xr < n)) buf[even ? xr - 1 : xr] = color;
  }
}

// *****************************************************************************
// *****************************************************************************
// ***   Triangle   ************************************************************
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// ***   Constructor   *********************************************************
// *****************************************************************************
Triangle::Triangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, color_t c, bool is_fill)
{
  SetParams(x1, y1, x2, y2, x3, y3, c, is_fill);
}

// *****************************************************************************
// ***   SetParams   ***********************************************************
// *****************************************************************************
void Triangle::SetParams(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, color_t c, bool is_fill)
{
  // Lock object for changes
  LockVisObject();
  // Invalidate area for old position/size
  InvalidateObjArea();
  // Do changes
  color = c;
  fill = is_fill;
  x_start = (x1 < x2) ? (x1 < x3 ? x1 : x3) : (x2 < x3 ? x2 : x3);
  y_start = (y1 < y2) ? (y1 < y3 ? y1 : y3) : (y2 < y3 ? y2 : y3);
  x_end = (x1 > x2) ? (x1 > x3 ? x1 : x3) : (x2 > x3 ? x2 : x3);
  y_end = (y1 > y2) ? (y1 > y3 ? y1 : y3) : (y2 > y3 ? y2 : y3);
  width  = x_end - x_start;
  height = y_end - y_start;
  // Lines for draw
  lines[0].x1 = x1;
  lines[0].y1 = y1;
  lines[0].x2 = x2;
  lines[0].y2 = y2;
  lines[1].x1 = x1;
  lines[1].y1 = y1;
  lines[1].x2 = x3;
  lines[1].y2 = y3;
  lines[2].x1 = x2;
  lines[2].y1 = y2;
  lines[2].x2 = x3;
  lines[2].y2 = y3;
  rotation = 0;
  // Invalidate area for new position/size
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void Triangle::DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t start_x)
{
  // Draw only if needed
  if((line >= y_start) && (line <= y_end))
  {
    int32_t x_min = 10000;
    int32_t x_max = -1;
    // Find minimum and maximum x for given screen line
    for(uint8_t i = 0u; i < 3u; i++)
    {
      // Draw only if needed
      if(((line >= lines[i].y1) && (line <= lines[i].y2)) || ((line >= lines[i].y2) && (line <= lines[i].y1)))
      {
        const int32_t deltaX = abs(lines[i].x2 - lines[i].x1);
        const int32_t deltaY = abs(lines[i].y2 - lines[i].y1);
        const int32_t signX = lines[i].x1 < lines[i].x2 ? 1 : -1;
        const int32_t signY = lines[i].y1 < lines[i].y2 ? 1 : -1;

        int32_t error = deltaX - deltaY;

        int32_t x = lines[i].x1 - start_x;
        int32_t y = lines[i].y1;

        int32_t end_x = lines[i].x2 - start_x;
        // Go trough cycle until reach end of line or current line
        while((x != end_x || y != lines[i].y2) && (y != line))
        {
          const int32_t error2 = error * 2;
          if(error2 > -deltaY) 
          {
            error -= deltaY;
            x += signX;
          }
          if(error2 < deltaX) 
          {
            error += deltaX;
            y += signY;
          }
        }
        // If we found current line
        if(y == line)
        {
          // Go and draw line
          do
          {
            if((x >= 0) && (x < n))
            {
              if(fill == false) buf[x] = color;
              else
              {
                x_min = x_min > x ? x : x_min;
                x_max = x_max < x ? x : x_max;
              }
            }
            const int32_t error2 = error * 2;
            if(error2 > -deltaY) 
            {
              error -= deltaY;
              x += signX;
            }
            if(error2 < deltaX) 
            {
              break;
            }
          }
          while((x != end_x) || (y != lines[i].y2));
        }
      }
    }
    if(fill)
    {
      for(uint16_t i = x_min; i <= x_max; i++)
      {
        buf[i] = color;
      }
    }
  }
}

// *****************************************************************************
// ***   Put line in buffer   **************************************************
// *****************************************************************************
void Triangle::DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t start_y)
{
  // Not implemented
}
