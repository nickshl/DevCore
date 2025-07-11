//******************************************************************************
//  @file ILI9488.cpp
//  @author Nicolai Shlapunov
//
//  @details DevCore: ILI9488 Low Level Driver Class, implementation
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
#include "ILI9488.h"

// *****************************************************************************
// ***   Defines   *************************************************************
// *****************************************************************************

// Commands definitions

#define CMD_NOP        0x00 // No Operation
#define CMD_SWRESET    0x01 // Software Reset
#define CMD_RDDID      0x04 // Read Display Identification Information
#define CMD_RDERRDSI   0x05 // Read Number of the Errors on DSI
#define CMD_RDDST      0x09 // Read Display Status

#define CMD_RDMODE     0x0A // Read Display Power Mode
#define CMD_RDMADCTL   0x0B // Read Display MADCTL
#define CMD_RDPIXFMT   0x0C // Read Display Pixel Format
#define CMD_RDIMGFMT   0x0D // Read Display Image Format
#define CMD_RDSIGMOD   0x0E // Read Display Signal Mode
#define CMD_RDSELFDIAG 0x0F // Read Display Self-Diagnostic Result

#define CMD_SLPIN      0x10 // Enter Sleep Mode
#define CMD_SLPOUT     0x11 // Sleep OUT
#define CMD_PTLON      0x12 // Partial Mode ON
#define CMD_NORON      0x13 // Normal Display Mode ON

#define CMD_INVOFF     0x20 // Display Inversion OFF
#define CMD_INVON      0x21 // Display Inversion ON
#define CMD_ALLPIXOFF  0x22 // All Pixel OFF
#define CMD_ALLPIXON   0x23 // All Pixel ON
#define CMD_DISPOFF    0x28 // Display OFF
#define CMD_DISPON     0x29 // Display ON

#define CMD_CASET      0x2A // Column Address Set
#define CMD_PASET      0x2B // Page Address Set
#define CMD_RAMWR      0x2C // Memory Write
#define CMD_RAMRD      0x2E // Memory Read

#define CMD_PTLAR      0x30 // Partial Area
#define CMD_VSCRDEF    0x33 // Vertical Scrolling Definition
#define CMD_TELOFF     0x34 // Tearing Effect Line OFF
#define CMD_TELON      0x35 // Tearing Effect Line ON
#define CMD_MADCTL     0x36 // Memory Access Control
#define CMD_VSAADDR    0x37 // Vertical Scrolling Start Address
#define CMD_IDLMOFF    0x38 // Idle Mode OFF
#define CMD_IDLMON     0x39 // Idle Mode ON
#define CMD_PIXFMT     0x3A // Pixel Format Set
#define CMD_MEMWC      0x3C // Memory Write Continue
#define CMD_MEMRC      0x3E // Memory Read Continue

#define CMD_RGBISC     0xB0 // RGB Interface Signal Control
#define CMD_FRMCTR1    0xB1 // Frame Control (In Normal Mode)
#define CMD_FRMCTR2    0xB2 // Frame Control (In Idle Mode)
#define CMD_FRMCTR3    0xB3 // Frame Control (In Partial Mode)
#define CMD_INVCTR     0xB4 // Display Inversion Control
#define CMD_BLKPC      0xB5 // Blanking Porch Control
#define CMD_DFUNCTR    0xB6 // Display Function Control

#define CMD_PWCTR1     0xC0 // Power Control 1
#define CMD_PWCTR2     0xC1 // Power Control 2
#define CMD_PWCTR3     0xC2 // Power Control 3
#define CMD_PWCTR4     0xC3 // Power Control 4
#define CMD_PWCTR5     0xC4 // Power Control 5
#define CMD_VMCTR1     0xC5 // VCOM Control 1

#define CMD_NVMEMWR    0xD0 // NV Memory Write
#define CMD_NVMEMPK    0xD1 // NV Memory Protection Key
#define CMD_NVMEMSR    0xD2 // NV Memory Status Read
#define CMD_READID4    0xD3 // Read ID4

#define CMD_RDID1      0xDA // Read ID1
#define CMD_RDID2      0xDB // Read ID2
#define CMD_RDID3      0xDC // Read ID3

#define CMD_GMCTRP1    0xE0 // Positive Gamma Correction
#define CMD_GMCTRN1    0xE1 // Negative Gamma Correction
#define CMD_DGCTRL1    0xE2 // Digital Gamma Control 1
#define CMD_DGCTRL2    0xE3 // Digital Gamma Control 2
#define CMD_SETIMGF    0xE9 // Set Image Function

#define CMD_ADJCTRL2   0xF2 // Adjust Control 2
#define CMD_ADJCTRL3   0xF7 // Adjust Control 3
#define CMD_ADJCTRL4   0xF8 // Adjust Control 4
#define CMD_ADJCTRL5   0xF9 // Adjust Control 5
#define CMD_SPIRCS     0xFB // SPI Read Command Setting
#define CMD_ADJCTRL6   0xFC // Adjust Control 6
#define CMD_ADJCTRL7   0xFF // Adjust Control 7

// Memory Access Control register bits definitions

#define MADCTL_MY  0x80 // Row Address Order
#define MADCTL_MX  0x40 // Column Address Order
#define MADCTL_MV  0x20 // Row / Column Exchange
#define MADCTL_ML  0x10 // Vertical Refresh Order
#define MADCTL_BGR 0x08 // BGR Order
#define MADCTL_RGB 0x00 // RGB Order (No BGR bit)
#define MADCTL_MH  0x04 // Horizontal Refresh ORDER

// *****************************************************************************
// ***   Public: Init screen   *************************************************
// *****************************************************************************
Result ILI9488::Init(void)
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

  // Reset display
  WriteCommand(CMD_SWRESET);
  // Delay for execute previous command
  Delay(100u);

  // Power control 1
  WriteCommand(CMD_PWCTR1);
  WriteData(0x17); // Vreg1out
  WriteData(0x15); // Verg2out

  // Power control 2
  WriteCommand(CMD_PWCTR2);
  WriteData(0x41); // VGH,VGL

  // VCM control 1
  WriteCommand(CMD_VMCTR1);
  WriteData(0x00);
  WriteData(0x12); // Vcom
  WriteData(0x80);

  // Interface Pixel Format
  WriteCommand(CMD_PIXFMT);
#if defined(COLOR_3BIT)
  WriteData(0x11); // 0x11 - 3 bit
#else
  WriteData(0x66); // 0x66 - 18 bit, 0x55 - 16 bit(DOESN'T WORK!), 0x11 - 3 bit
#endif

  // Frame Control (In Normal Mode)
  WriteCommand(CMD_FRMCTR1);
  WriteData(0xA0); // Frame rate 60Hz

  // Adjust control 3: data from datasheet
  WriteCommand(CMD_ADJCTRL3);
  WriteData(0xA9);
  WriteData(0x51);
  WriteData(0x2C);
  WriteData(0x82);

  // Memory Access Control
  WriteCommand(CMD_MADCTL);
  WriteData(MADCTL_MV | MADCTL_BGR);

  // Positive Gamma Correction
  WriteCommand(CMD_GMCTRP1);
  WriteData(0x00);
  WriteData(0x03);
  WriteData(0x09);
  WriteData(0x08);
  WriteData(0x16);
  WriteData(0x0A);
  WriteData(0x3F);
  WriteData(0x78);
  WriteData(0x4C);
  WriteData(0x09);
  WriteData(0x0A);
  WriteData(0x08);
  WriteData(0x16);
  WriteData(0x1A);
  WriteData(0x0F);

  // Negative Gamma Correction
  WriteCommand(CMD_GMCTRN1);
  WriteData(0x00);
  WriteData(0x16);
  WriteData(0x19);
  WriteData(0x03);
  WriteData(0x0F);
  WriteData(0x05);
  WriteData(0x32);
  WriteData(0x45);
  WriteData(0x46);
  WriteData(0x04);
  WriteData(0x0E);
  WriteData(0x0D);
  WriteData(0x35);
  WriteData(0x37);
  WriteData(0x0F);

  // Interface Mode Control
  WriteCommand(CMD_RGBISC);
  WriteData(0x80); // SDO NOT USE

  // Display Inversion Control
  WriteCommand(CMD_INVCTR);
  WriteData(0x02); // 2-dot

  // Display Function Control RGB/MCU Interface Control
  WriteCommand(CMD_DFUNCTR);
  WriteData(0x02); // MCU
  WriteData(0x02); // Source, Gate scan direction

  WriteCommand(0xE9); // Set Image Function
  WriteData(0x00);    // Disable 24 bit data

  // Exit Sleep
  WriteCommand(CMD_SLPOUT);
  // Delay for execute previous command
  Delay(120u);
  // Display on
  WriteCommand(CMD_DISPON);

  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Prepare data (32 bit -> 24 bit)   *****************************
// *****************************************************************************
Result ILI9488::PrepareData(uint32_t* data, uint32_t n)
{
  // Change pointer to uint8_t
  uint8_t* dt = (uint8_t*)data;
  // Index for packed data
  uint32_t idx = 0;
  // Do packing 32 bit -> 24 bit
  for(uint32_t i = 0u; i < n*4; i++)
  {
    // Create 24-bit RGB values from 32 bit
    dt[idx++] = dt[i++];
    dt[idx++] = dt[i++];
    dt[idx++] = dt[i++];
  }
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Prepare data (16 bit -> 18 bit)   *****************************
// *****************************************************************************
Result ILI9488::PrepareData(uint16_t* data, uint32_t n)
{
  // Change pointer to uint8_t
  uint8_t* dt = (uint8_t*)data;
  // Index variable
  uint16_t idx = (n * 3u) - 1u;
  // Convert data
  for(int16_t i = n - 1u; i >= 0; i--)
  {
    uint16_t color = dt[i*2u+1u] | (dt[i*2u] << 8u);
    // Create 24-bit RGB values from 16 bit
    dt[idx--] = (color & 0x001F) << 3;
    dt[idx--] = (color & 0x07E0) >> 3; // >> 5 << 3
    dt[idx--] = (color & 0xF800) >> 8; // >> 11 << 3
  }
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Prepare data (16 bit -> 3 bit)   ******************************
// *****************************************************************************
Result ILI9488::PrepareData(uint8_t* data, uint32_t n)
{
  // Convert data
  for(uint32_t i = 0u; i < n / 2u; i++)
  {
//    uint16_t color1 = data[i*4u+1u] | (data[i*4u+0u] << 8u);
//    uint16_t color2 = data[i*4u+3u] | (data[i*4u+2u] << 8u);
//    // Create two 3-bit RGB values from two 16 bit
//    data[i] = ((color1 & 0x0010) ? 0x20 : 0x00) | ((color1 & 0x0400) ? 0x10 : 0x00) | ((color1 & 0x8000) ? 0x08 : 0x00) |
//              ((color2 & 0x0010) ? 0x04 : 0x00) | ((color2 & 0x0400) ? 0x02 : 0x00) | ((color2 & 0x8000) ? 0x01 : 0x00);

//    data[i] = ((data[i*4u+1u] & 0x10) ? 0x20 : 0x00) | ((data[i*4u+0u] & 0x04) ? 0x10 : 0x00) | ((data[i*4u+0u] & 0x80) ? 0x08 : 0x00) |
//              ((data[i*4u+3u] & 0x10) ? 0x04 : 0x00) | ((data[i*4u+2u] & 0x04) ? 0x02 : 0x00) | ((data[i*4u+2u] & 0x80) ? 0x01 : 0x00);

//    data[i] = ((data[i*4u+1u] & 0x10) << 1u) | ((data[i*4u+0u] & 0x04) << 2u) | ((data[i*4u+0u] & 0x80) >> 4u) |
//              ((data[i*4u+3u] & 0x10) >> 2u) | ((data[i*4u+2u] & 0x04) >> 1u) | ((data[i*4u+2u] & 0x80) >> 7u);

    //
    data[i] = ((data[i*2u + 0u] & 0x07) << 3u) | (data[i*2u + 1u] & 0x07);
  }
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Write data steram to SPI   ************************************
// *****************************************************************************
Result ILI9488::WriteDataStream(uint8_t* data, uint32_t n)
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
bool ILI9488::IsTransferComplete(void)
{
  return spi.IsTransferComplete();
}

// *****************************************************************************
// ***   Public: Pull up CS line for LCD  **************************************
// *****************************************************************************
Result ILI9488::StopTransfer(void)
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
Result ILI9488::SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  WriteCommand(CMD_CASET); // Column address set
  WriteData(x0 >> 8);
  WriteData(x0 & 0xFF); // XSTART 
  WriteData(x1 >> 8);
  WriteData(x1 & 0xFF); // XEND

  WriteCommand(CMD_PASET); // Row address set
  WriteData(y0 >> 8);
  WriteData(y0 & 0xFF); // YSTART
  WriteData(y1 >> 8);
  WriteData(y1 & 0xFF); // YEND

  WriteCommand(CMD_RAMWR); // write to RAM

  // Prepare for write data
  display_dc.SetHigh(); // Data

  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Set screen orientation   **************************************
// *****************************************************************************
Result ILI9488::SetRotation(IDisplay::Rotation r)
{
  rotation = r;
  WriteCommand(CMD_MADCTL);
  switch (rotation)
  {
    case IDisplay::ROTATION_BOTTOM:
      WriteData(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
      width  = init_width;
      height = init_height;
      break;

    case IDisplay::ROTATION_RIGHT:
      WriteData(MADCTL_MY | MADCTL_BGR);
      width  = init_height;
      height = init_width;
      break;

    case IDisplay::ROTATION_LEFT:
      WriteData(MADCTL_MX | MADCTL_BGR);
      width  = init_height;
      height = init_width;
      break;

    case IDisplay::ROTATION_TOP:
      WriteData(MADCTL_MV | MADCTL_BGR);
      width  = init_width;
      height = init_height;
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
Result ILI9488::PushColor(uint32_t color)
{
  display_dc.SetHigh(); // Data

  // Write color
  SpiWrite(((uint8_t*)&color)[0u]);
  SpiWrite(((uint8_t*)&color)[1u]);
  SpiWrite(((uint8_t*)&color)[2u]);

  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Write color to screen   ***************************************
// *****************************************************************************
Result ILI9488::PushColor(uint16_t color)
{
  display_dc.SetHigh(); // Data

  // Create 24-bit RGB values from 16 bit
  uint8_t r = (color & 0xF800) >> 11;
  uint8_t g = (color & 0x07E0) >> 5;
  uint8_t b = color & 0x001F;
  r = (r * 0xFF) / 31;
  g = (g * 0xFF) / 63;
  b = (b * 0xFF) / 31;
  // Write color
  SpiWrite(r);
  SpiWrite(g);
  SpiWrite(b);

  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Write color to screen   ***************************************
// *****************************************************************************
Result ILI9488::PushColor(uint8_t color)
{
  display_dc.SetHigh(); // Data

  // Write color
  SpiWrite(color);

  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Draw one pixel on  screen   ***********************************
// *****************************************************************************
Result ILI9488::DrawPixel(int16_t x, int16_t y, color_t color)
{
  if((x >= 0) && (x < width) && (y >= 0) && (y < height))
  {
    SetAddrWindow(x,y,x+1,y+1);
    // Write color
    PushColor(color);
  }
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Draw vertical line   ******************************************
// *****************************************************************************
Result ILI9488::DrawFastVLine(int16_t x, int16_t y, int16_t h, color_t color)
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
Result ILI9488::DrawFastHLine(int16_t x, int16_t y, int16_t w, color_t color)
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
Result ILI9488::FillRect(int16_t x, int16_t y, int16_t w, int16_t h, color_t color)
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
Result ILI9488::InvertDisplay(bool invert)
{
  WriteCommand(invert ? CMD_INVON : CMD_INVOFF);
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Private: Delay in ms    ***********************************************
// *****************************************************************************
inline void ILI9488::Delay(uint32_t delay_ms)
{
  // If RTOS scheduler is running - we can use RTOS delay function to allow
  // other tasks to work, but if it not - use HAL_Delay.
  if(Rtos::IsSchedulerRunning()) RtosTick::DelayMs(delay_ms);
  else                           HAL_Delay(delay_ms);
}

// *****************************************************************************
// ***   Private: Write command to SPI   ***************************************
// *****************************************************************************
inline void ILI9488::WriteCommand(uint8_t c)
{
  display_dc.SetLow(); // Command
  SpiWrite(c);
}

// *****************************************************************************
// ***   Private: Write data to SPI   ******************************************
// *****************************************************************************
inline void ILI9488::WriteData(uint8_t c)
{
  display_dc.SetHigh(); // Data
  SpiWrite(c);
}

// *****************************************************************************
// ***   Private: Write byte to SPI   ******************************************
// *****************************************************************************
inline void ILI9488::SpiWrite(uint8_t c)
{
  display_cs.SetLow(); // Pull down CS
  spi.Write(&c, sizeof(c));
  display_cs.SetHigh(); // Pull up CS
}

// *****************************************************************************
// ***   Private: Send read command ad read result   ***************************
// *****************************************************************************
uint8_t ILI9488::ReadCommand(uint8_t c)
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
inline uint8_t ILI9488::ReadData(void)
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
inline uint8_t ILI9488::SpiRead(void)
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
