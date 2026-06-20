// *****************************************************************************
// @file StHalPwm.cpp
// @author Nicolai Shlapunov
//
// @details DevCore: STM32 HAL PWM driver, implementation
//
// @copyright Copyright (c) 2018, Devtronic & Nicolai Shlapunov
//            All rights reserved.
//
// @section SUPPORT
//
//  Devtronic invests time and resources providing this open source code,
//  please support Devtronic and open-source hardware/software by
//  donations and/or purchasing products from Devtronic.
//
// *****************************************************************************

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include "StHalPwm.h"

// *****************************************************************************
// ***   This driver can be compiled only if Timer configured in CubeMX   ******
// *****************************************************************************
#ifdef HAL_TIM_MODULE_ENABLED

// *****************************************************************************
// ***   Public: Init   ********************************************************
// *****************************************************************************
Result StHalPwm::Init()
{
  Result result = Result::RESULT_OK;

  // There can be timer initialization, but in HAL it all should be configured
  // in STM32CubeMX and initialized in main.c

  return result;
}

// *****************************************************************************
// ***   Public: GetFrequency   ************************************************
// *****************************************************************************
Result StHalPwm::GetFrequency(uint32_t& freq)
{
  Result result = Result::ERR_INVALID_ITEM;

  // Get prescaler from timer
  uint32_t prescaler = htim.Instance->PSC;
  // Calculate timer frequency
  freq = HAL_RCC_GetHCLKFreq() / (__HAL_TIM_GET_AUTORELOAD(&htim) + 1u) / (prescaler + 1u);
  // Override result
  result = Result::RESULT_OK;

  return result;
}

// *****************************************************************************
// ***   Public: SetFrequency   ************************************************
// *****************************************************************************
Result StHalPwm::SetFrequency(uint32_t freq)
{
  Result result = Result::ERR_BAD_PARAMETER;

  if(freq > 0u)
  {
    // Calculate prescaler and auto-reload register
    uint32_t total_ticks = HAL_RCC_GetHCLKFreq() / freq;
    uint32_t psc = total_ticks / (UINT16_MAX + 1u);
    uint32_t arr = total_ticks / (psc + 1u);
    // Pass calculated values to the timer
    __HAL_TIM_SET_PRESCALER(&htim, psc);
    __HAL_TIM_SET_AUTORELOAD(&htim, arr ? (arr - 1u) : 0u);
    // Update duty cycle in time after prescaler and ARR recalculation
    SetDutyCycle(duty_cycle);
    // Generate an update event to reload the Prescaler and the repetition
    // counter(only for TIM1 and TIM8) value immediately
    htim.Instance->EGR = TIM_EGR_UG;
    // Good result
    result = Result::RESULT_OK;
  }

  return result;
}

// *****************************************************************************
// ***   Public: SetDutyCycle   ************************************************
// *****************************************************************************
Result StHalPwm::SetDutyCycle(uint16_t duty)
{
  Result result = Result::RESULT_OK;

  // Save passed duty cycle value
  duty_cycle = duty;
  // Calculate new CCR value
  uint32_t pulse = __HAL_TIM_GET_AUTORELOAD(&htim) * duty_cycle / UINT16_MAX;
  // Update CCR
  __HAL_TIM_SET_COMPARE(&htim, channel, pulse);

  return result;
}

// *****************************************************************************
// ***   Public: StartPwm   ****************************************************
// *****************************************************************************
Result StHalPwm::StartPwm(void)
{
  // Start timer in Output Compare match mode
  return (HAL_TIM_PWM_Start(&htim, channel) == HAL_OK ? Result::RESULT_OK : Result::ERR_TIMER_START);
}

// *****************************************************************************
// ***   Public: StopPwm   *****************************************************
// *****************************************************************************
Result StHalPwm::StopPwm(void)
{
  // Stop timer
  HAL_TIM_PWM_Stop(&htim, channel);
  // Always good
  return Result::RESULT_OK;
}

#endif
