// *****************************************************************************
// @file SoundDrv.cpp
// @author Nicolai Shlapunov
//
// @details DevCore: Sound Driver Class, implementation
//
// @copyright Copyright (c) 2017, Devtronic & Nicolai Shlapunov
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
#include "SoundDrv.h"

#include "DevCfgRtos.h"

// *****************************************************************************
// ***   Public: Get Instance   ************************************************
// *****************************************************************************
SoundDrv& SoundDrv::GetInstance(void)
{
  // This class is static and declared here
  static SoundDrv sound_drv;
  // Return reference to class
  return sound_drv;
}

// *****************************************************************************
// ***   Public: Init Sound Driver Task   **************************************
// *****************************************************************************
Result SoundDrv::InitTask(IPwm& pwm)
{
  // Save timer handle
  pwm_drv = &pwm;
  // Create task
  return AppTask::InitTask();
}

// *****************************************************************************
// ***   Public: Sound Driver Setup   ******************************************
// *****************************************************************************
Result SoundDrv::Setup()
{
  // Init ticks variable
  last_wake_ticks = RtosTick::GetTickCount();
  // Initiate PWM with 0 duty cycle
  pwm_drv->SetDutyCycle(0u);   // 0% duty cycle - silence
  pwm_drv->SetFrequency(440u); // Randomly selected frequency
  pwm_drv->StartPwm();
  // Always ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Sound Driver Loop   *******************************************
// *****************************************************************************
Result SoundDrv::Loop()
{
  // Flag
  bool is_playing = false;
  // Take mutex before start playing sound
  melody_mutex.Lock();
  // Delay for playing one frequency
  uint32_t current_delay_ms = delay_ms;
  // If no current melody or melody size is zero - skip playing
  if((sound_table != nullptr) && (sound_table_size != 0u))
  {
    // Set flag that still playing sound
    is_playing = true;
    // If frequency greater than 18 Hz
    if(((uint32_t)sound_table[sound_table_position] >> 4u) > 0x12u)
    {
      Tone(sound_table[sound_table_position] >> 4u);
    }
    else
    {
      // Otherwise "play" silence
      Tone(0u);
    }

    // Init ticks variable
    if(sound_table_position == 0u)
    {
      last_wake_ticks = RtosTick::GetTickCount();
    }
    // Get retry counter from table and calculate delay
    current_delay_ms *= sound_table[sound_table_position] & 0x0Fu;

    // Increase array index
    sound_table_position++;
    // If end of melody reached
    if(sound_table_position >= sound_table_size)
    {
      // If set repeat flag
      if(repeat == true)
      {
        // Reset index for play melody from beginning
        sound_table_position = 0u;
      }
      else
      {
        // Otherwise stop playing sound
        is_playing = false;
      }
    }
  }
  // Give mutex after start playing sound
  melody_mutex.Release();

  // Pause until next tick
  RtosTick::DelayUntilMs(last_wake_ticks, current_delay_ms);

  // Using semaphore here helps block this task while task wait request for
  // sound playing.
  if(is_playing == false)
  {
    // Stop playing sound before wait semaphore
    StopSound();
    // Wait semaphore for start play melody
    sound_update.Take();
  }

  // Always run
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Beep function   ***********************************************
// *****************************************************************************
void SoundDrv::Beep(uint16_t freq, uint16_t del, bool pause_after_play)
{
  // Take mutex before beeping - prevent play melody during beeping.
  melody_mutex.Lock();
  // Start play tone
  Tone(freq);
  // Delay
  RtosTick::DelayMs(del);
  // Stop play tone
  Tone(0);
  // If flag is set
  if(pause_after_play == true)
  {
    // Delay with same value as played sound
    RtosTick::DelayMs(del);
  }
  // Give mutex after beeping
  melody_mutex.Release();
}

// *****************************************************************************
// ***   Public: Play sound function   *****************************************
// *****************************************************************************
void SoundDrv::PlaySound(const uint16_t* melody, uint16_t size, uint16_t temp_ms, bool rep)
{
  // Parameters: pointer to melody table, size of melody and repetition flag.
  // Format of sounds: 0x***#, where *** frequency, # - delay in temp_ms intervals

  // If pointer is not nullptr, if size & freq time greater than zero
  if((melody != nullptr) && (size > 0u) && (temp_ms > 0u))
  {
    // If already playing any melody
    if(IsSoundPlayed() == true)
    {
      // Stop it first
      StopSound();
    }

    // Take mutex before start playing melody
    melody_mutex.Lock();
    // Set repeat flag for melody
    repeat = rep;
    // Set time for one frequency
    delay_ms = temp_ms;
    // Set initial index for melody
    sound_table_position = 0u;
    // Set melody size
    sound_table_size = size;
    // Set melody pointer
    sound_table = melody;
    // Give mutex after start playing melody
    melody_mutex.Release();

    // Give semaphore for start play melody
    sound_update.Give();
  }
}

// *****************************************************************************
// ***   Public: Stop sound function   *****************************************
// *****************************************************************************
void SoundDrv::StopSound(void)
{
  // Take mutex before stop playing sound
  melody_mutex.Lock();
  // Clear sound table pointer
  sound_table = nullptr;
  // Clear sound table size
  sound_table_size = 0u;
  // Clear sound table index
  sound_table_position = 0u;
  // Set time for one frequency
  delay_ms = 100U;
  // Set repeat flag for melody
  repeat = false;
  // Stop sound
  Tone(0u);
  // Give mutex after stop playing sound
  melody_mutex.Release();
}

// *****************************************************************************
// ***   Public: Mute sound function   *****************************************
// *****************************************************************************
void SoundDrv::Mute(bool mute_flag)
{
  // Set mute flag
  mute = mute_flag;
  // If mute flag is set - call Tone() for stop tone
  if(mute == true)
  {
    Tone(0u);
  }
}

// *****************************************************************************
// ***   Public: Is sound played function   ************************************
// *****************************************************************************
bool SoundDrv::IsSoundPlayed(void)
{
  // Return variable, false by default
  bool ret = false;
  // If sound_table is not nullptr - we still playing melody. No sense to use
  // mutex here - get pointer is atomic operation.
  if(sound_table != nullptr)
  {
    ret = true;
  }
  // Return result
  return ret;
}

// *****************************************************************************
// ***   Private: Process Button Input function   ******************************
// *****************************************************************************
void SoundDrv::Tone(uint16_t freq)
{
  if((freq > 20u) && (mute == false))
  {
    pwm_drv->SetFrequency(freq);
    pwm_drv->SetDutyCycle(UINT16_MAX / 2u); // 50% duty cycle
  }
  else
  {
    pwm_drv->SetDutyCycle(0u); // 0% duty cycle - silence
  }
}
