// *****************************************************************************
// @file StHalUart.h
// @author Nicolai Shlapunov
//
// @details DevCore: STM32 HAL UART driver, header
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

#ifndef StHalUart_h
#define StHalUart_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include "DevCfg.h"
#include "Interfaces/IUart.h"

// *****************************************************************************
// ***   This driver can be compiled only if UART configured in CubeMX   *******
// *****************************************************************************
#if defined(HAL_USART_MODULE_ENABLED) || defined(HAL_UART_MODULE_ENABLED)
#include "usart.h"
#else
typedef uint32_t UART_HandleTypeDef; // Dummy UART handle for compilation
#endif

// *****************************************************************************
// ***   STM32 HAL UART Driver Class   *****************************************
// *****************************************************************************
class StHalUart : public IUart
{
  public:
    // *************************************************************************
    // ***   Public: Constructor   *********************************************
    // *************************************************************************
    explicit StHalUart(UART_HandleTypeDef& huart_ref) : huart(huart_ref) {};

    // *************************************************************************
    // ***   Public: Init   ****************************************************
    // *************************************************************************
    Result Init();

    // *************************************************************************
    // ***   Public: DeInit   **************************************************
    // *************************************************************************
    Result DeInit();

    // *************************************************************************
    // ***   Public: Read   ****************************************************
    // *************************************************************************
    Result Read(uint8_t& rx_byte);

    // *************************************************************************
    // ***   Public: Read   ****************************************************
    // *************************************************************************
    Result Read(uint8_t* rx_buf_ptr, uint32_t& size);

    // *************************************************************************
    // ***   Public: Write   ***************************************************
    // *************************************************************************
    Result Write(uint8_t* tx_buf_ptr, uint32_t size);

    // *************************************************************************
    // ***   Public: Constructor   *********************************************
    // *************************************************************************
    bool IsTxComplete(void);

 private:
   // Size of buffer for receive data
   static const uint16_t RX_BUF_SIZE = 128U;

   // Buffer for receive data
   uint8_t rx_buf[RX_BUF_SIZE];

   // Reference to the UART handle
   UART_HandleTypeDef& huart;

   // Offset into DMA receive buffer of next character to receive
   uint16_t ndtr = 0U;

   // *************************************************************************
   // ***   Private: GetRxSize   **********************************************
   // *************************************************************************
   Result GetRxSize(uint16_t& rx_cnt);

   // *************************************************************************
   // ***   Private: StartRx   ************************************************
   // *************************************************************************
   Result StartRx();

   // *************************************************************************
   // ***   Private: Constructors and assign operator - prevent copying   *****
   // *************************************************************************
   StHalUart();
   StHalUart(const StHalUart&);
   StHalUart& operator=(const StHalUart);
};

#endif
