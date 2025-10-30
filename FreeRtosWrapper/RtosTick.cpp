//******************************************************************************
//  @file RtosTick.cpp
//  @author Nicolai Shlapunov
//
//  @details DevCore: FreeRTOS Tick Wrapper Class, implementation
//
//  @copyright Copyright (c) 2018, Devtronic & Nicolai Shlapunov
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

#include "RtosTick.h"
#include "Rtos.h"

#include "portmacro.h"

// *****************************************************************************
// ***   Public: GetTickCount   ************************************************
// *****************************************************************************
uint32_t RtosTick::GetTickCount()
{
  uint32_t tick_count = 0U;

  // Check handler mode
  if(Rtos::IsInHandlerMode())
  {
    tick_count = xTaskGetTickCountFromISR();
  }
  else
  {
    tick_count = xTaskGetTickCount();
  }

  return tick_count;
}

// *****************************************************************************
// ***   Public: GetTickDifference   *******************************************
// *****************************************************************************
uint32_t RtosTick::GetTickDifference(uint32_t start_val, uint32_t cur_val)
{
  uint32_t result = 0u;

  // Check overflow
  if(start_val > cur_val)
  {
    // start_val can be greater cur_val only if tick counter rollover.
    // If ticks rollover, we should calculate deference between maximum
    // uint32 value and start value, then add current tick count
    result = (UINT32_MAX - start_val) + cur_val;
  }
  else
  {
    result = cur_val - start_val;
  }

  // Return result
  return result;
}

// *****************************************************************************
// ***   Public: CheckTickDifference   *****************************************
// *****************************************************************************
bool RtosTick::CheckTickDifference(uint32_t start_val, uint32_t check_value)
{
  return (GetTickDifference(start_val, GetTickCount()) >= check_value);
}

// *****************************************************************************
// ***   Public: GetTimeMs   ***************************************************
// *****************************************************************************
uint32_t RtosTick::GetTimeMs()
{
  return TicksToMs(GetTickCount());
}

// *****************************************************************************
// ***   Public: CheckTimeDifferenceMs   ***************************************
// *****************************************************************************
bool RtosTick::CheckTimeDifferenceMs(uint32_t start_val_ms, uint32_t check_value_ms)
{
  return (TicksToMs(GetTickDifference(MsToTicks(start_val_ms), GetTickCount())) >= check_value_ms);
}

// *****************************************************************************
// ***   Public: DelayTicks   **************************************************
// *****************************************************************************
void RtosTick::DelayTicks(uint32_t ticks)
{
  vTaskDelay(ticks);
}

// *****************************************************************************
// ***   Public: DelayMs   *****************************************************
// *****************************************************************************
void RtosTick::DelayMs(uint32_t time_ms)
{
  vTaskDelay(MsToTicks(time_ms));
}

// *****************************************************************************
// ***   Public: DelayUntilTicks   *********************************************
// *****************************************************************************
void RtosTick::DelayUntilTicks(uint32_t& last_wake_ticks, uint32_t ticks)
{
  vTaskDelayUntil(&last_wake_ticks, ticks);
}

// *****************************************************************************
// ***   Public: DelayUntilMs   ************************************************
// *****************************************************************************
void RtosTick::DelayUntilMs(uint32_t& last_wake_ticks, uint32_t time_ms)
{
  vTaskDelayUntil(&last_wake_ticks, MsToTicks(time_ms));
}

// *****************************************************************************
// ***   Public: MsToTicks   ***************************************************
// *****************************************************************************
uint32_t RtosTick::MsToTicks(uint32_t time_ms)
{
  return(time_ms * portTICK_PERIOD_MS);
}

// *****************************************************************************
// ***   Public: TicksToMs   ***************************************************
// *****************************************************************************
uint32_t RtosTick::TicksToMs(uint32_t ticks)
{
  return(ticks / portTICK_PERIOD_MS);
}

