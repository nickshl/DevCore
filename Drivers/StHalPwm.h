// *****************************************************************************
// @file StHalPwm.h
// @author Nicolai Shlapunov
//
// @details DevCore: STM32 HAL PWM driver, header
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

#ifndef StHalPwm_h
#define StHalPwm_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include "DevCfg.h"
#include "Interfaces/IPwm.h"

// *****************************************************************************
// ***   This driver can be compiled only if Timer configured in CubeMX   ******
// *****************************************************************************
#ifdef HAL_TIM_MODULE_ENABLED
#include "tim.h"
#else
typedef uint32_t TIM_HandleTypeDef; // Dummy TIM handle for compilation
#endif

// *****************************************************************************
// ***   STM32 HAL PWM Driver Class   ******************************************
// *****************************************************************************
class StHalPwm : public IPwm
{
  public:
    // *************************************************************************
    // ***   Public: Constructor   *********************************************
    // *************************************************************************
    StHalPwm(TIM_HandleTypeDef& tim, uint32_t ch) : htim(tim), channel(ch) {Init();}

    // *************************************************************************
    // ***   Public: Init   ****************************************************
    // *************************************************************************
    virtual Result Init();

    // *************************************************************************
    // ***   Public: GetFrequency   ********************************************
    // *************************************************************************
    virtual Result GetFrequency(uint32_t& freq);

    // *************************************************************************
    // ***   Public: GetDutyCycle   ********************************************
    // *************************************************************************
    virtual Result GetDutyCycle(uint16_t& duty) {duty = duty_cycle; return Result::RESULT_OK;}

    // *************************************************************************
    // ***   Public: SetFrequency   ********************************************
    // *************************************************************************
    virtual Result SetFrequency(uint32_t freq);

    // *************************************************************************
    // ***   Public: SetDutyCycle   ********************************************
    // *************************************************************************
    virtual Result SetDutyCycle(uint16_t duty);

    // *************************************************************************
    // ***   Public: StartPwm   ************************************************
    // *************************************************************************
    virtual Result StartPwm(void);

    // *************************************************************************
    // ***   Public: StopPwm   *************************************************
    // *************************************************************************
    virtual Result StopPwm(void);

  private:
    // Duty cycle (0% by default)
    uint16_t duty_cycle = 0u;

    // Timer handle
    TIM_HandleTypeDef& htim;
    // Timer channel
    uint32_t channel;
};

#endif
