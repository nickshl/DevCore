//******************************************************************************
//  @file ST7789.cpp
//  @author Nicolai Shlapunov
//
//  @details DevCore: ST7789 Low Level Driver Class, implementation
//
//  @copyright Copyright (c) 2025, Devtronic & Nicolai Shlapunov
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
#include "ST7789.h"

// *****************************************************************************
// ***   Defines   *************************************************************
// *****************************************************************************

// Commands definitions

#define CMD_NOP     0x00
#define CMD_SWRESET 0x01
#define CMD_RDDID   0x04
#define CMD_RDDST   0x09

#define CMD_SLPIN   0x10
#define CMD_SLPOUT  0x11
#define CMD_PTLON   0x12
#define CMD_NORON   0x13

#define CMD_INVOFF  0x20
#define CMD_INVON   0x21
#define CMD_DISPOFF 0x28
#define CMD_DISPON  0x29
#define CMD_CASET   0x2A
#define CMD_RASET   0x2B
#define CMD_RAMWR   0x2C
#define CMD_RAMRD   0x2E

#define CMD_PTLAR   0x30
#define CMD_TEOFF   0x34
#define CMD_TEON    0x35
#define CMD_MADCTL  0x36
#define CMD_COLMOD  0x3A

#define MADCTL_MY   0x80 // Row Address Order
#define MADCTL_MX   0x40 // Column Address Order
#define MADCTL_MV   0x20 // Row / Column Exchange
#define MADCTL_ML   0x10 // Vertical Refresh Order
#define MADCTL_RGB  0x00 // RGB Order (No BGR bit)
#define MADCTL_BGR  0x08 // BGR Order
#define MADCTL_MH   0x04 // Horizontal Refresh ORDER

#define CMD_RDID1   0xDA
#define CMD_RDID2   0xDB
#define CMD_RDID3   0xDC
#define CMD_RDID4   0xDD


// *****************************************************************************
// ***   Public: Init screen   *************************************************
// *****************************************************************************
Result ST7789::Init(void)
{
  // Reset sequence. Used only if GPIO pin used as LCD reset.
  if(display_rst != nullptr)
  {
    display_rst->SetHigh(); // Pull up reset line
    Delay(5u);              // Wait for 5 ms
    display_rst->SetLow();  // Pull down reset line
    Delay(20u);             // Wait for 20 ms
    display_rst->SetHigh(); // Pull up reset line
    Delay(150u);            // Wait for 150 ms
  }

  // Software reset
  WriteCommand(CMD_SWRESET);
  Delay(150u); // Delay for execute previous command

  // Out of sleep mode
  WriteCommand(CMD_SLPOUT);
  Delay(10u); // Delay for execute previous command

  // Set color mode
  WriteCommand(CMD_COLMOD);
  WriteData(0x55); // 16-bit color
  Delay(10u); // Delay for execute previous command

  // Mem access ctrl (directions)
  WriteCommand(CMD_MADCTL);
  WriteData(0x08); // Row/col addr, bottom-top refresh

  // Column address set
  WriteCommand(CMD_CASET);
  WriteData(0x00); // XSTART = 0
  WriteData(0x00);
  WriteData(0x00); // XEND = 240
  WriteData(0xF0);

  // Row address set
  WriteCommand(CMD_RASET);
  WriteData(0x00); // YSTART = 0
  WriteData(0x00);
  WriteData(0x01); // YEND = 320
  WriteData(0x40);

  // Inversion
  WriteCommand(CMD_INVON);
  Delay(10u); // Delay for execute previous command

  // Normal display on
  WriteCommand(CMD_NORON);
  Delay(10u); // Delay for execute previous command

  // Main screen turn on
  WriteCommand(CMD_DISPON);
  Delay(10u); // Delay for execute previous command
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Write data stream to SPI   ************************************
// *****************************************************************************
Result ST7789::WriteDataStream(uint8_t* data, uint32_t n)
{
  // Data
  display_dc.SetHigh();
  // Pull down CS
  display_cs.SetLow();
  // Send data to screen
  Result result = spi.WriteAsync(data, n);
  // Return result
  return result;
}

// *****************************************************************************
// ***   Public: Check SPI transfer status   ***********************************
// *****************************************************************************
bool ST7789::IsTransferComplete(void)
{
  return spi.IsTransferComplete();
}

// *****************************************************************************
// ***   Public: Pull up CS line for LCD  **************************************
// *****************************************************************************
Result ST7789::StopTransfer(void)
{
  // In case if transfer isn't finished - abort it.
  if(spi.IsTransferComplete() == false)
  {
    spi.Abort();
  }
  // Pull up CS
  display_cs.SetHigh();
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Set output window   *******************************************
// *****************************************************************************
Result ST7789::SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  // Add offsets
  x0 += display_x_start;
  y0 += display_y_start;
  x1 += display_x_start;
  y1 += display_y_start;

  WriteCommand(CMD_CASET); // Column address set
  WriteData(x0 >> 8);
  WriteData(x0 & 0xFF); // XSTART 
  WriteData(x1 >> 8);
  WriteData(x1 & 0xFF); // XEND

  WriteCommand(CMD_RASET); // Row address set
  WriteData(y0 >> 8);
  WriteData(y0);        // YSTART
  WriteData(y1 >> 8);
  WriteData(y1);        // YEND

  WriteCommand(CMD_RAMWR); // write to RAM

  // Prepare for write data
  display_dc.SetHigh(); // Data

  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Set screen orientation   **************************************
// *****************************************************************************
Result ST7789::SetRotation(IDisplay::Rotation r)
{
  // 1.47", 1.69, 1.9", 2.0" displays (centered)
  int32_t col_start = ((320 - init_width) / 2);
  int32_t row_start = ((240 - init_height) / 2);

  rotation = r;
  WriteCommand(CMD_MADCTL);
  switch (rotation)
  {
    case IDisplay::ROTATION_TOP:
      WriteData(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
      width  = init_width;
      height = init_height;
      display_x_start = col_start;
      display_y_start = row_start;
      break;

    case IDisplay::ROTATION_LEFT:
      WriteData(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
      width  = init_height;
      height = init_width;
      display_x_start = row_start;
      display_y_start = col_start;
      break;

    case IDisplay::ROTATION_BOTTOM:
      WriteData(MADCTL_MV | MADCTL_RGB);
      width  = init_width;
      height = init_height;
      display_x_start = col_start;
      display_y_start = row_start;
      break;

    case IDisplay::ROTATION_RIGHT:
      WriteData(MADCTL_MX | MADCTL_RGB);
      width  = init_height;
      height = init_width;
      display_x_start = row_start;
      display_y_start = col_start;
      break;

    default:
      break;
  }
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Write color to screen   ***************************************
// *****************************************************************************
Result ST7789::PushColor(color_t color)
{
  display_dc.SetHigh(); // Data
  // Write color
  SpiWrite(color >> 8);
  SpiWrite(color);
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Draw one pixel on  screen   ***********************************
// *****************************************************************************
Result ST7789::DrawPixel(int16_t x, int16_t y, color_t color)
{
  if((x >= 0) && (x < width) && (y >= 0) && (y < height))
  {
    SetAddrWindow(x,y,x+1,y+1);
    // Write color
    SpiWrite(color >> 8);
    SpiWrite(color);
  }
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Draw vertical line   ******************************************
// *****************************************************************************
Result ST7789::DrawFastVLine(int16_t x, int16_t y, int16_t h, color_t color)
{
  // Rudimentary clipping
  if((x < width) && (y < height))
  {
    if((y+h-1) >= height) h = height-y;

    SetAddrWindow(x, y, x, y+h-1);

    // Swap bytes
    uint8_t clr = ((color >> 8) & 0x00FF) | ((color << 8) & 0xFF00);

    display_cs.SetLow(); // Pull down CS
    while(h--)
    {
      spi.Write(&clr, sizeof(clr));
    }
    display_cs.SetHigh(); // Pull up CS
  }
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Draw horizontal line   ****************************************
// *****************************************************************************
Result ST7789::DrawFastHLine(int16_t x, int16_t y, int16_t w, color_t color)
{
  if((x < width) && (y < height))
  {
    if((x+w-1) >= width)  w = width-x;

    SetAddrWindow(x, y, x+w-1, y);

    // Swap bytes
    uint8_t clr = ((color >> 8) & 0x00FF) | ((color << 8) & 0xFF00);

    display_cs.SetLow(); // Pull down CS
    while(w--)
    {
      spi.Write(&clr, sizeof(clr));
    }
    display_cs.SetHigh(); // Pull up CS
  }
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Fill rectangle on screen   ************************************
// *****************************************************************************
Result ST7789::FillRect(int16_t x, int16_t y, int16_t w, int16_t h, color_t color)
{
  if((x < width) && (y < height))
  {
    if((x + w - 1) >= width)  w = width  - x;
    if((y + h - 1) >= height) h = height - y;

    SetAddrWindow(x, y, x+w-1, y+h-1);

    // Swap bytes
    uint8_t clr = ((color >> 8) & 0x00FF) | ((color << 8) & 0xFF00);

    display_cs.SetLow(); // Pull down CS
    for(y = h; y > 0; y--)
    {
      for(x = w; x > 0; x--)
      {
        spi.Write(&clr, sizeof(clr));
      }
    }
    display_cs.SetHigh(); // Pull up CS
  }
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Invert display   **********************************************
// *****************************************************************************
Result ST7789::InvertDisplay(bool invert)
{
  WriteCommand(invert ? CMD_INVON : CMD_INVOFF);
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Private: Delay in ms    ***********************************************
// *****************************************************************************
inline void ST7789::Delay(uint32_t delay_ms)
{
  // If RTOS scheduler is running - we can use RTOS delay function to allow
  // other tasks to work, but if it not - use HAL_Delay.
  if(Rtos::IsSchedulerRunning()) RtosTick::DelayMs(delay_ms);
  else                           HAL_Delay(delay_ms);
}

// *****************************************************************************
// ***   Private: Write command to SPI   ***************************************
// *****************************************************************************
inline void ST7789::WriteCommand(uint8_t c)
{
  display_dc.SetLow(); // Command
  SpiWrite(c);
}

// *****************************************************************************
// ***   Private: Write data to SPI   ******************************************
// *****************************************************************************
inline void ST7789::WriteData(uint8_t c)
{
  display_dc.SetHigh(); // Data
  SpiWrite(c);
}

// *****************************************************************************
// ***   Private: Write byte to SPI   ******************************************
// *****************************************************************************
inline void ST7789::SpiWrite(uint8_t c)
{
  display_cs.SetLow(); // Pull down CS
  spi.Write(&c, sizeof(c));
  display_cs.SetHigh(); // Pull up CS
}

// *****************************************************************************
// ***   Private: Send read command ad read result   ***************************
// *****************************************************************************
uint8_t ST7789::ReadCommand(uint8_t c)
{
  // Set command mode
  display_dc.SetLow(); // Command
  SpiWrite(c);

  // Set data mode
  display_dc.SetHigh(); // Data
  // Receive data
  uint8_t r = SpiRead();

  // Return result
  return r;
}

// *****************************************************************************
// ***   Private: Read data from display   *************************************
// *****************************************************************************
inline uint8_t ST7789::ReadData(void)
{
  // Data
  display_dc.SetHigh(); // Data
  // Receive data
  uint8_t r = SpiRead();
  // Return result
  return r;
}

// *****************************************************************************
// ***   Private: Read data from SPI   *****************************************
// *****************************************************************************
inline uint8_t ST7789::SpiRead(void)
{
  // Result variable
  uint8_t r = 0;
  // Pull down CS
  display_cs.SetLow();
  // Receive data
  spi.Read(&r, sizeof(r));
  // Pull up CS
  display_cs.SetHigh();
  // Return result
  return r;
}
