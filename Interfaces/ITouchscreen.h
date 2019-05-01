//******************************************************************************
//  @file ITouchscreen.h
//  @author Nicolai Shlapunov
//
//  @details DevCore: Touchscreen driver interface, header
//
//  @section LICENSE
//
//   Software License Agreement (BSD License)
//
//   Copyright (c) 2018, Devtronic & Nicolai Shlapunov
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   3. Neither the name of the Devtronic nor the names of its contributors
//      may be used to endorse or promote products derived from this software
//      without specific prior written permission.
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
//******************************************************************************

#ifndef ITouchscreen_h
#define ITouchscreen_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include "DevCfg.h"

// *****************************************************************************
// ***   Touchscreen Driver Interface   ****************************************
// *****************************************************************************
class ITouchscreen
{
  public:
    // Coefficient for calibration
    const static int32_t COEF = 100;

    // *************************************************************************
    // ***   Public: Constructor   *********************************************
    // *************************************************************************
    explicit ITouchscreen() {};

    // *************************************************************************
    // ***   Public: Destructor   **********************************************
    // *************************************************************************
    virtual ~ITouchscreen() {};

    // *************************************************************************
    // ***   Public: Init   ****************************************************
    // *************************************************************************
    // * Init function. Send init sequence to touchscreen controller.
    virtual Result Init(void) = 0;

    // *************************************************************************
    // ***   Public: IsTouch   *************************************************
    // *************************************************************************
    // * Check touched or not by T_IRQ pin. Return true if touched.
    virtual bool IsTouched(void) = 0;

    // *************************************************************************
    // ***   Public: GetRawXY   ************************************************
    // *************************************************************************
    // * Return raw X and Y coordinates. If touched - return true.
    virtual bool GetRawXY(int32_t& x, int32_t& y) = 0;

    // *************************************************************************
    // ***   Public: GetXY   ***************************************************
    // *************************************************************************
    // * Return recalculated using calibration constants X and Y coordinates.
    // * If touched - return true. Can be used for second calibration.
    virtual bool GetXY(int32_t& x, int32_t& y) = 0;

    // *************************************************************************
    // ***   Public: SetCalibrationConsts   ************************************
    // *************************************************************************
    // * Set calibration constants. Must be call for calibration touchscreen.
    virtual Result SetCalibrationConsts(int32_t nkx, int32_t nky, int32_t nbx, int32_t nby) {return Result::ERR_NOT_IMPLEMENTED;}

  private:
    // *************************************************************************
    // ***   Private: Constructors and assign operator - prevent copying   *****
    // *************************************************************************
    ITouchscreen(const ITouchscreen&);
};

#endif
