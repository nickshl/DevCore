//******************************************************************************
//  @file ButtonDrv.h
//  @author Nicolai Shlapunov
//
//  @details DevCore: Button Driver Class, header
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

#ifndef ButtonDrv_h
#define ButtonDrv_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include "DevCfg.h"
#include "iGpio.h"
#include "AppTask.h"

#include "climits" // For CHAR_BIT

// *****************************************************************************
// * Input Driver Class. This class implement work with user input elements like 
// * buttons and encoders.
class ButtonDrv : public AppTask
{
  public:
    // *************************************************************************
    // ***   Possible state of a button   **************************************
    // *************************************************************************
    enum ButtonState : uint8_t
    {
      RELEASED = 0u, // Button released
      PRESSED = 1u,  // Button pressed
      HOLD = 2u,     // Button hold
      DOUBLE = 3u    // Button double click
    };

    // *************************************************************************
    // ***   Structure to describe callback   **********************************
    // *************************************************************************
    typedef struct CallbackListEntryStruct
    {
      private:
        AppTask* callback_task = nullptr;
        CallbackPtr callback = nullptr;
        void* obj_ptr = nullptr;
        uint8_t mask = 0u; // Mask for button callback list to handle particular button
        struct CallbackListEntryStruct* next = nullptr;
        struct CallbackListEntryStruct* prev = nullptr;
        // ButtonDrv is friend of structure for access the data
        friend class ButtonDrv;
    } CallbackListEntry;

    // *************************************************************************
    // ***   Public: Get Instance   ********************************************
    // *************************************************************************
    // * This class is singleton. For use this class you must call GetInstance()
    // * to receive reference to Input Driver class
    static ButtonDrv& GetInstance(void);

    // *************************************************************************
    // ***   Public: Public: Init Input Driver Task   **************************
    // *************************************************************************
    Result InitTask(IGpio* btns[], uint8_t btn_num);

    // *************************************************************************
    // ***   Public: Input Driver Setup   **************************************
    // *************************************************************************
    virtual Result Setup();

    // *************************************************************************
    // ***   Public: Input Driver Loop   ***************************************
    // *************************************************************************
    // * If FreeRTOS task used, this function just call ProcessInput() with 1 ms
    // * period. If FreeRTOS tick is 1 ms - this task must have highest priority 
    virtual Result Loop();

    // *************************************************************************
    // ***   Public: Add Buttons Callback handler   ****************************
    // *************************************************************************
    void AddButtonsCallbackHandler(AppTask* callback_task, CallbackPtr callback, void* obj_ptr, uint32_t mask, CallbackListEntry& cble);

    // *************************************************************************
    // ***   Public: Delete Buttons Callback handler   *************************
    // *************************************************************************
    void DeleteButtonsCallbackHandler(CallbackListEntry& cble);

    // *************************************************************************
    // ***   Public: Get button current state   ********************************
    // *************************************************************************
    inline ButtonState GetButtonState(uint8_t button) {return btn_state[button];}

    // *************************************************************************
    // ***   Public: Get button pin state   ************************************
    // *************************************************************************
    // Return button state change flag: true - changed, false - not changed
    bool GetButtonPinState(uint8_t button, bool& button_state);

    // *************************************************************************
    // ***   Public: Set button read delay count(ms)   *************************
    // *************************************************************************
    inline void SetButtonDelay(uint8_t delay) {button_read_delay_cnt = delay;}

    // *************************************************************************
    // ***   Public: Set button hold delay count(ms)   *************************
    // *************************************************************************
    inline void SetButtonHoldDelay(uint16_t delay) {button_hold_delay_ms = delay;}

    // *************************************************************************
    // ***   Public: Set button hold delay count(ms)   *************************
    // *************************************************************************
    inline void SetButtonDoubleDelay(uint16_t delay) {button_double_delay_ms = delay;}

  private:
    // How many cycles button must change state before state will be changed in
    // result returned by GetButtonState() function. For reduce debouncing
    uint8_t button_read_delay_cnt = 8u;
    // How much ms button must be pressed before button hold event will be
    uint16_t button_hold_delay_ms = 500u;
    // How much ms allowed between clicks in double click
    uint16_t button_double_delay_ms = 250u;

    // Pointer to GPIO class for buttons
    IGpio** buttons;
    // Number of buttons in the system
    uint8_t number_of_buttons = 0u;

    // Data for buttons. Since we using mask for buttons, up to 32 buttons supported.
    ButtonState* btn_state = nullptr; // Button state array
    uint32_t btn_pin_state; // Button state returned by GetButtonState() function
    uint32_t btn_pin_state_tmp; // Temporary button state for reduce debouncing
    uint8_t* btn_pin_state_cnt = nullptr; // Counters for reduce debouncing
    uint32_t* btn_press_time_ms = nullptr; // Timestamp for track button hold/double click time

    // Button handlers
    CallbackListEntry* btn_callback_list = nullptr;

    // Ticks variable
    uint32_t last_wake_ticks = 0u;

    // *************************************************************************
    // ***   Public: InitTask from AppTask to prevent warning   ****************
    // *************************************************************************
    using AppTask::InitTask;

    // *************************************************************************
    // ***   Private: Process Button Input function   **************************
    // *************************************************************************
    inline bool ProcessButtonInput(uint8_t button, uint32_t btn_mask);

    // *************************************************************************
    // ** Private constructor. Only GetInstance() allow to access this class. **
    // *************************************************************************
    ButtonDrv() : AppTask(INPUT_DRV_TASK_STACK_SIZE, INPUT_DRV_TASK_PRIORITY, "ButtonDrv") {};
};

#endif
