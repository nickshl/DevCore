//******************************************************************************
//  @file FramMB85.h
//  @author Nicolai Shlapunov
//
//  @details DevCore: FRAM MB85*** driver, header
//
//  @section LICENSE
//
//   Software License Agreement (Modified BSD License)
//
//   Copyright (c) 2025, Devtronic & Nicolai Shlapunov
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   3. Neither the name of the Devtronic nor the names of its contributors
//      may be used to endorse or promote products derived from this software
//      without specific prior written permission.
//   4. Redistribution and use of this software other than as permitted under
//      this license is void and will automatically terminate your rights under
//      this license.
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
//  @section SUPPORT
//
//   Devtronic invests time and resources providing this open source code,
//   please support Devtronic and open-source hardware/software by
//   donations and/or purchasing products from Devtronic.
//
//******************************************************************************

#ifndef FramMB85_h
#define FramMB85_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include "DevCfg.h"
#include "IIic.h"
#include "IGpio.h"

// *****************************************************************************
// ***   FRAM MB85*** Driver Class   *******************************************
// *****************************************************************************
class FramMB85
{
  public:
    // *************************************************************************
    // ***   Public: Constructor   *********************************************
    // *************************************************************************
    explicit FramMB85(IIic& iic_ref, IGpio* wp = nullptr, uint32_t size = 0x2000u, uint32_t buffer_size = 32u) : size_bytes(size), buffer_size_bytes(buffer_size), iic(iic_ref), write_protection(wp) {buf = new uint8_t[buffer_size + 2u];}

    // *************************************************************************
    // ***   Public: Constructor   *********************************************
    // *************************************************************************
    ~FramMB85() {delete buf;}

    // *************************************************************************
    // ***   Public: Init   ****************************************************
    // *************************************************************************
    Result Init();

    // *************************************************************************
    // ***   Public: Read   ****************************************************
    // *************************************************************************
    Result Read(uint16_t addr, uint8_t* rx_buf_ptr, uint16_t size);

    // *************************************************************************
    // ***   Public: Write   ***************************************************
    // *************************************************************************
    Result Write(uint16_t addr, uint8_t* tx_buf_ptr, uint16_t size);

    // *************************************************************************
    // ***   Public: GetSize   *************************************************
    // *************************************************************************
    uint8_t GetSize() {return size_bytes;}

    // *************************************************************************
    // ***   Public: GetBufferSize   *******************************************
    // *************************************************************************
    uint8_t GetBufferSize() {return buffer_size_bytes;}

  private:
    // Chip address
    static const uint8_t I2C_ADDR = 0x50U; // 0xA0 shifted by 1

    // Size of EEPROM in bytes
    uint32_t size_bytes = 0x2000u;

    // Buffer size in bytes
    uint32_t buffer_size_bytes = 32u;

    // Buffer pointer to allocate memory for address + data
    uint8_t* buf = nullptr;

    // Reference to the I2C handle
    IIic& iic;
    // Write protection pin
    IGpio* write_protection = nullptr;

    // *************************************************************************
    // ***   Private: Constructors and assign operator - prevent copying   *****
    // *************************************************************************
    FramMB85();
    FramMB85(const FramMB85&);
    FramMB85& operator=(const FramMB85);
};

#endif
