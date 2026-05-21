//******************************************************************************
//  @file FramMB85.cpp
//  @author Nicolai Shlapunov
//
//  @details DevCore: FRAM MB85*** driver, implementation
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
#include "FramMB85.h"

#include "string.h" // for memcpy()

// *****************************************************************************
// ***   Public: Init   ********************************************************
// *****************************************************************************
Result FramMB85::Init()
{
  Result result = Result::RESULT_OK;
  iic.SetTxTimeout(1u);
  iic.SetRxTimeout(1u);
  // Enable write protection
  if(write_protection != nullptr)
  {
    write_protection->SetHigh();
  }
  return result;
}

// *****************************************************************************
// ***   Public: Read   ********************************************************
// *****************************************************************************
Result FramMB85::Read(uint16_t addr, uint8_t* rx_buf_ptr, uint16_t size)
{
  Result result = Result::RESULT_OK;

  // Check input parameters
  if(rx_buf_ptr == nullptr)
  {
    // Set error
    result = Result::ERR_NULL_PTR;
  }
  else if(addr + size > size_bytes)
  {
    // Set error
    result = Result::ERR_INVALID_SIZE;
  }
  else
  {
    // Transfer: write two bytes address then read data
    result = iic.Transfer(I2C_ADDR, (uint8_t*)&addr, sizeof(addr), rx_buf_ptr, size);
  }

  return result;
}

// *****************************************************************************
// ***   Public: Write   *******************************************************
// *****************************************************************************
Result FramMB85::Write(uint16_t addr, uint8_t* tx_buf_ptr, uint16_t size)
{
  Result result = Result::RESULT_OK;

  // Check input parameters
  if(tx_buf_ptr == nullptr)
  {
    // Set error
    result = Result::ERR_NULL_PTR;
  }
  else if(addr + size > size_bytes)
  {
    // Set error
    result = Result::ERR_INVALID_SIZE;
  }
  else
  {
    // Disable write protection
    if(write_protection != nullptr)
    {
      write_protection->SetLow();
    }
    // Cycle for write pages
    while(size && result.IsGood())
    {
      // Get data size
      uint8_t data_size = size < buffer_size_bytes ? size : buffer_size_bytes;
      // Decrease number of remaining bytes
      size -= data_size;
      // Store address
      buf[0u] = (addr >> 8) & 0xFF; // MSB
      buf[1u] = addr & 0xFF; // LSB
      // Increment address for next transaction
      addr += data_size;
      // Copy data
      memcpy(buf + 2u, tx_buf_ptr, data_size);
      // Advance data pointer to next chunk
      tx_buf_ptr += data_size;
      // Transfer
      result = iic.Write(I2C_ADDR, buf, 2u + data_size);

      // MB85RS2MT does not take long time to write data like Flash memories
      // or EEPROM, and MB85RS2MT takes no wait time
    }
    // Enable write protection
    if(write_protection != nullptr)
    {
      write_protection->SetHigh();
    }
  }

  return result;
}

