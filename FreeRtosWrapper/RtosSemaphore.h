// *****************************************************************************
// @file RtosSemaphore.h
// @author Nicolai Shlapunov
//
// @details DevCore: FreeRTOS Semaphore Wrapper Class, header
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

#ifndef RtosSemaphore_h
#define RtosSemaphore_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include "DevCfg.h"
#include "Rtos.h"
#include "semphr.h"

// *****************************************************************************
// ***   RtosSemaphore   *******************************************************
// *****************************************************************************
class RtosSemaphore
{
  public:
    // *************************************************************************
    // ***   Constructor   *****************************************************
    // *************************************************************************
    RtosSemaphore();

    // *************************************************************************
    // ***   Destructor   ******************************************************
    // *************************************************************************
    ~RtosSemaphore();

    // *************************************************************************
    // ***   Take   ************************************************************
    // *************************************************************************
    Result Take(TickType_t ticks_to_wait = portMAX_DELAY);

    // *************************************************************************
    // ***   Give   ************************************************************
    // *************************************************************************
    Result Give();

  private:
    // Semaphore handle
    SemaphoreHandle_t semaphore;
};

#endif
