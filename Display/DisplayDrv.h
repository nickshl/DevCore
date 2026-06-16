// *****************************************************************************
// @file DisplayDrv.h
// @author Nicolai Shlapunov
//
// @details DevCore: Display Driver Class, header
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

#ifndef DisplayDrv_h
#define DisplayDrv_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "DevCfg.h"
#include "DevCfgRtos.h"
#include "Framework/AppTask.h"

#include "Display/UpdateAreaProcessor.h"

#include "Interfaces/IDisplay.h"
#include "Interfaces/ITouchscreen.h"
#include "Display/VisObject.h"
#include "Display/VisList.h"
#include "Display/Primitives.h"
#include "Display/Strng.h"
#include "Display/StringAligned.h"
#include "Display/MultiLineString.h"
#include "Display/Image.h"
#include "Display/TiledMap.h"

// *****************************************************************************
// ***   Display Driver Class   ************************************************
// *****************************************************************************
class DisplayDrv : public AppTask
{
  public:
    // *************************************************************************
    // ***   Update Mode   *****************************************************
    // *************************************************************************
    enum UpdateMode
    {
      UPDATE_TOP_BOTTOM,
      UPDATE_LEFT_RIGHT
    };

    // *************************************************************************
    // ***   Public: Get Instance   ********************************************
    // *************************************************************************
    static DisplayDrv& GetInstance(void);

    // *************************************************************************
    // ***   Public: InitTask from AppTask to prevent warning   ****************
    // *************************************************************************
    using AppTask::InitTask;

    // *************************************************************************
    // ***   Public: Init Display Driver Task   ********************************
    // *************************************************************************
    Result InitTask(IDisplay& in_display);

    // *************************************************************************
    // ***   Public: Init Display Driver Task   ********************************
    // *************************************************************************
    Result InitTask(IDisplay& in_display, ITouchscreen& in_touch);

    // *************************************************************************
    // ***   Public: Display Driver Setup   ************************************
    // *************************************************************************
    virtual Result Setup();

    // *************************************************************************
    // ***   Public: Display Driver Loop   *************************************
    // *************************************************************************
    virtual Result Loop();

    // *************************************************************************
    // ***   Public: Set display driver   **************************************
    // *************************************************************************
    Result SetDisplayDrv(IDisplay& in_display);

    // *************************************************************************
    // ***   Public: Get display driver   **************************************
    // *************************************************************************
    IDisplay* GetDisplayDrv(void) {return display;}

    // *************************************************************************
    // ***   Public: Get list   ************************************************
    // *************************************************************************
    VisList* GetVisList(void) {return &list;}

    // *************************************************************************
    // ***   Public: Lock display   ********************************************
    // *************************************************************************
    inline Result LockDisplay(uint32_t wait_ms = portMAX_DELAY) {return frame_mutex.Lock(wait_ms);}

    // *************************************************************************
    // ***   Public: Unlock display   ******************************************
    // *************************************************************************
    inline Result UnlockDisplay(void) {return frame_mutex.Release();}

    // *************************************************************************
    // ***   Public: Lock display line   ***************************************
    // *************************************************************************
    inline Result LockDisplayLine(uint32_t wait_ms = portMAX_DELAY) {return line_mutex.Lock(wait_ms);}

    // *************************************************************************
    // ***   Public: Unlock display line   *************************************
    // *************************************************************************
    inline Result UnlockDisplayLine(void) {return line_mutex.Release();}

    // *************************************************************************
    // ***   Public: Update display   ******************************************
    // *************************************************************************
    inline Result UpdateDisplay(void) {return screen_update.Give();}

    // *************************************************************************
    // ***   Public: Invalidate Display   **************************************
    // *************************************************************************
    inline Result InvalidateDisplay() {return InvalidateArea(0, 0, width, height);}

    // *************************************************************************
    // ***   Public: Invalidate Area   *****************************************
    // *************************************************************************
    Result InvalidateArea(int16_t start_x, int16_t start_y, int16_t end_x, int16_t end_y);

    // *************************************************************************
    // ***   Public: Invert Display   ******************************************
    // *************************************************************************
    void InvertDisplay(bool invert);

    // *************************************************************************
    // ***   Public: Set Rotation   ********************************************
    // *************************************************************************
    void SetRotation(IDisplay::Rotation rot);

    // *************************************************************************
    // ***   Public: Set Update Mode   *****************************************
    // *************************************************************************
    void SetUpdateMode(UpdateMode mode);

    // *************************************************************************
    // ***   Public: Set Background Color   ************************************
    // *************************************************************************
    inline void SetBackgroundColor(color_t color) {bkg_color = color;}

    // *************************************************************************
    // ***   Public: GetScreenW   **********************************************
    // *************************************************************************
    inline int32_t GetScreenW(void) {return width;}

    // *************************************************************************
    // ***   Public: GetScreenH   **********************************************
    // *************************************************************************
    inline int32_t GetScreenH(void) {return height;}

    // *************************************************************************
    // ***   Public: Set touchscreen driver(or clear if nullptr passed)   ******
    // *************************************************************************
    Result SetTouchDrv(ITouchscreen* in_touch);

    // *************************************************************************
    // ***   Public: Get touchscreen driver   **********************************
    // *************************************************************************
    ITouchscreen* GetTouchDrv(void) {return touch;}

    // *************************************************************************
    // ***   Public: Get Touch X and Y coordinate   ****************************
    // *************************************************************************
    bool GetTouchXY(int32_t& x, int32_t& y);

    // *************************************************************************
    // ***   Public: Check touch   *********************************************
    // *************************************************************************
    bool IsTouched();

    // *************************************************************************
    // ***   Public: Calibrate Touchscreen   ***********************************
    // *************************************************************************
    void TouchCalibrate();

  private:
    // Display driver object
    IDisplay* display = nullptr;

    // Touchscreen driver object
    ITouchscreen* touch = nullptr;

    // List with visual objects
    VisList list = VisList(*this);

    // Background color
    color_t bkg_color = COLOR_BLACK;

    //Inversion
    bool inversion = false;
    // Rotation
    IDisplay::Rotation rotation = IDisplay::ROTATION_TOP;
    // Update mode: true - vertical, false = horizontal
    UpdateMode update_mode = UPDATE_TOP_BOTTOM;
    // Variables for update screen mode
    int32_t width = 0;
    int32_t height = 0;
    // Double Screen Line buffer
    color_t scr_buf[2][DISPLAY_MAX_BUF_LEN];
    // Current screen line
    uint8_t scr_line_idx = 0u;

#if defined(UPDATE_AREA_ENABLED)
    // Area to update
    UpdateArea_t area;
  #if defined(MULTIPLE_UPDATE_AREAS)
    // For multiple areas
    UpdateAreaProcessor<MULTIPLE_UPDATE_AREAS> areas;
  #else
    // Dirty flag
    bool is_dirty = false;
  #endif
#endif

    // Touch coordinates and state
    bool is_touch = false;
    int32_t tx = 0;
    int32_t ty = 0;

#if defined(DISPLAY_DEBUG_INFO)
    // FPS multiplied to 10
    volatile uint32_t fps_x10 = 0U;
    // Buffer for print FPS string
    char str[32] = {"       "};
    // FPS string
    String fps_str;
#endif

#if defined(DISPLAY_DEBUG_TOUCH)
    // Touch debug circle
    Circle touch_cir;
#endif

    // Semaphore for update screen
    RtosSemaphore screen_update;
    // Mutex to synchronize when drawing lines
    RtosRecursiveMutex line_mutex;
    // Mutex to synchronize when drawing frames
    RtosRecursiveMutex frame_mutex;
    // Mutex for synchronize when reads touch coordinates
    RtosMutex touchscreen_mutex;

    // *************************************************************************
    // ***   Private: SwapData   ***********************************************
    // *************************************************************************
    static inline void SwapData(int16_t& a, int16_t& b)
    {
      int16_t tmp = a;
      a = b;
      b = tmp;
    }

    // *************************************************************************
    // ** Private constructor: Only GetInstance() allow to access this class ***
    // *************************************************************************
    DisplayDrv() : AppTask(DISPLAY_DRV_TASK_STACK_SIZE, DISPLAY_DRV_TASK_PRIORITY, "DisplayDrv")
    {
      // Set default list for all VisObjects. Every object should have list. If
      // default list isn't set, SetList() should be explicitly called for every
      // VisObject. To simplify in case only one display is present, default list
      // is used. In order to get full advantage of it DisplayDrv::GetInstance()
      // should be called before creation any objects that contains VisObjects.
      VisObject::default_list = &list;
    };
};

#endif
