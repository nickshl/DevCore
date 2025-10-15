//******************************************************************************
//  @file ButtonDrv.cpp
//  @author Nicolai Shlapunov
//
//  @details DevCore: Button Driver Class, implementation
//
//  @copyright Copyright (c) 2023, Devtronic & Nicolai Shlapunov
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
#include "ButtonDrv.h"
#include "Rtos.h"

#include <cstdlib>

// *****************************************************************************
// ***   Public: Get Instance   ************************************************
// *****************************************************************************
ButtonDrv& ButtonDrv::GetInstance(void)
{
  // This class is static and declared here
  static ButtonDrv button_drv;
  // Return reference to class
  return button_drv;
}

// *****************************************************************************
// ***   Public: Init Input Driver Task   **************************************
// *****************************************************************************
Result ButtonDrv::InitTask(IGpio* btns[], uint8_t btn_num)
{
  Result result = Result::RESULT_OK;

  // Buttons pointer shouldn't be null
  if(btns == nullptr)
  {
    result = Result::ERR_NULL_PTR;
  }
  // Number of buttons shouldn't exceed maximum
  else if(btn_num >= sizeof(uint32_t) * CHAR_BIT)
  {
    result = Result::ERR_BAD_PARAMETER;
  }
  // Task shouldn't be already initialized
  else if((buttons != nullptr) || (btn_pin_state_cnt != nullptr) || (btn_hold_cnt != nullptr))
  {
    result = Result::ERR_INVALID_ITEM;
  }
  else
  {
    // Pointer to GPIO class for buttons
    buttons = btns;
    // Number of buttons in the system
    number_of_buttons = btn_num;
    // Allocate memory for button states
    btn_state = new ButtonState[btn_num];
    // Allocate memory for button state counters
    btn_pin_state_cnt = new uint8_t[btn_num];
    // Allocate memory for button state counters
    btn_hold_cnt = new uint16_t[btn_num];
  }

  // Create task
  if(result.IsGood()) result = CreateTask();

  // Return result
  return result;
}

// *****************************************************************************
// ***   Public: Input Driver Setup   ******************************************
// *****************************************************************************
Result ButtonDrv::Setup()
{
  // Init ticks variable
  last_wake_ticks = RtosTick::GetTickCount();
  // Always Ok
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Input Driver Loop   ***************************************************
// *****************************************************************************
Result ButtonDrv::Loop()
{
  // Process all buttons
  for(uint32_t i = 0u, mask = 1u; i < number_of_buttons; i++, mask <<= 1u)
  {
    // Process button input and if button state is changed
    if(ProcessButtonInput(i, mask))
    {
      // Pointer to callback list element
      CallbackListEntry* btn_cbl = btn_callback_list;
      // Send notification
      while(btn_cbl != nullptr)
      {
        if(btn_cbl->mask & mask)
        {
          // If there no AppTask pointer
          if(btn_cbl->callback_task == nullptr)
          {
            // Call callback directly in ButtonDrv task(semaphores in other task may be needed!)
            btn_cbl->callback(btn_cbl->obj_ptr, this);
          }
          else
          {
            // Otherwise call it via AppTask to execute callback in target task
            btn_cbl->callback_task->Callback(btn_cbl->callback, btn_cbl->obj_ptr, this);
          }
          // Only first handler gets notification
          break;
        }
        btn_cbl = btn_cbl->next;
      }
    }
  }

  // Pause until next tick
  RtosTick::DelayUntilMs(last_wake_ticks, 1u);
  // Always run
  return Result::RESULT_OK;
}

// *****************************************************************************
// ***   Public: Add Buttons Callback handler   ********************************
// *****************************************************************************
void ButtonDrv::AddButtonsCallbackHandler(AppTask* callback_task, CallbackPtr callback, void* obj_ptr, uint32_t mask, CallbackListEntry& cble)
{
  // Set data in callback entry
  cble.callback_task = callback_task;
  cble.callback = callback;
  cble.obj_ptr = obj_ptr;
  cble.mask = mask;

  // If callback list is empty
  if(btn_callback_list == nullptr)
  {
    // Clear pointers just in case
    cble.next = nullptr;
    cble.prev = nullptr;
    // Set as first element
    btn_callback_list = &cble;
  }
  else // Otherwise
  {
    // Try to find element in list
    CallbackListEntry* btn_cbl = btn_callback_list;
    // Try to find this handler in the list
    while(btn_cbl != nullptr)
    {
      if(btn_cbl == &cble) break;   // Handler found - break cycle
      else btn_cbl = btn_cbl->next; // Handler not found - set next element pointer
    }
    // Check if this handler already in the list and if not
    if(btn_cbl != &cble)
    {
      // Set as previous into existing head element
      btn_callback_list->prev = &cble;
      // Set next element pointer
      cble.next = btn_callback_list;
      // Clear previous pointers just in case
      cble.prev = nullptr;
      // Set as head
      btn_callback_list = &cble;
    }
  }
}

// *****************************************************************************
// ***   Public: Delete Buttons Callback handler   *****************************
// *****************************************************************************
void ButtonDrv::DeleteButtonsCallbackHandler(CallbackListEntry& cble)
{
  // If requested first element in list
  if(btn_callback_list == &cble)
  {
    btn_callback_list = cble.next;
    btn_callback_list->prev = nullptr;
  }
  else // Otherwise
  {
    // Try to find element in list
    CallbackListEntry* btn_cbl = btn_callback_list;
    // Try to find this handler in the list
    while(btn_cbl != nullptr)
    {
      if(btn_cbl == &cble) break;   // Handler found - break cycle
      else btn_cbl = btn_cbl->next; // Handler not found - set next element pointer
    }
    // Check if this handler in the list
    if(btn_cbl == &cble)
    {
      // Set next pointer for previous element
      cble.prev->next = cble.next;
      // Id next element exist
      if(cble.next != nullptr)
      {
        // Set it as next element for previous one
        cble.next->prev = cble.prev;
      }
    }
  }
}

// *****************************************************************************
// ***   Public: Get button pin state   ****************************************
// *****************************************************************************
bool ButtonDrv::GetButtonPinState(uint8_t button, bool& button_state)
{
  bool ret = false;
  // If button state changed
  if((bool)(btn_pin_state & (1u << button)) != button_state)
  {
    // Store new state
    button_state = (btn_pin_state & (1u << button));
    ret = true;
  }
  // Return result
  return ret;
}

// *****************************************************************************
// ***   Private: Process Button Input function   ******************************
// *****************************************************************************
inline bool ButtonDrv::ProcessButtonInput(uint8_t button, uint32_t btn_mask)
{
  // Flag to return if button state was changed
  bool is_changed = false;
  // Read Button state
  bool new_status = buttons[button]->IsLow();

  // If button pin state has changed
  if((bool)(btn_pin_state & btn_mask) != new_status)
  {
    if(((bool)(btn_pin_state_tmp & btn_mask) == new_status) && (btn_pin_state_cnt[button] >= button_read_delay_cnt))
    {
      // Set button state
      if(new_status) btn_state[button] = ButtonState::PRESSED;
      else           btn_state[button] = ButtonState::RELEASED;
      // If temporary button state true and delay done - update state
      btn_pin_state &= ~btn_mask; // Clear old state
      btn_pin_state |= (uint32_t)new_status << button; // Set new state
      // Clear button hold counter
      btn_hold_cnt[button] = 0u;
      // Button state changed
      is_changed = true;
    }
    else if((bool)(btn_pin_state_tmp & btn_mask) == new_status)
    {
      // If temporary button state true and delay not done - increase counter
      btn_pin_state_cnt[button]++;
    }
    else
    {
      btn_pin_state_tmp &= ~btn_mask;            // Clear old state
      btn_pin_state_tmp |= new_status << button; // Set new state
      btn_pin_state_cnt[button] = 0u;
    }
  }
  else
  {
    // If button is pressed and there is no hold state yet
    if((new_status) && (btn_state[button] != ButtonState::HOLD))
    {
      if(btn_hold_cnt[button] >= button_hold_delay_cnt)
      {
        // Set hold state
        btn_state[button] = ButtonState::HOLD;
        // Button state changed
        is_changed = true;
      }
      else
      {
        // Increase hold counter
        btn_hold_cnt[button]++;
      }
    }
  }

  return is_changed;
}
