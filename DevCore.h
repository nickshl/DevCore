// *****************************************************************************
// @file DevCore.h
// @author Nicolai Shlapunov
//
// @details DevCore: Main DevCore include file, header
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

#ifndef DevCore_h
#define DevCore_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include <DevCfgRtos.h>
#include "DevCfg.h"

// *****************************************************************************
// ***   User configuration include   ******************************************
// *****************************************************************************
#include "DevCfgUsr.h"

// *****************************************************************************
// ***   RTOS wrapper include   ************************************************
// *****************************************************************************
#include "Framework/Result.h"
#include "Framework/AppTask.h"

// ***   Interfaces Headers   **************************************************
#include "Interfaces/ICallback.h"
#include "Interfaces/IDisplay.h"
#include "Interfaces/IGpio.h"
#include "Interfaces/IIic.h"
#include "Interfaces/ISpi.h"
#include "Interfaces/ITouchscreen.h"
#include "Interfaces/IUart.h"

// ***   Display Headers   *****************************************************
#include "Display/DisplayDrv.h"
#include "Display/Font.h"
#include "Display/FT6236.h"
#include "Display/GC9A01.h"
#include "Display/ILI9341.h"
#include "Display/ILI9488.h"
#include "Display/Image.h"
#include "Display/MultiLineString.h"
#include "Display/Primitives.h"
#include "Display/ST7789.h"
#include "Display/StringAligned.h"
#include "Display/Strng.h"
#include "Display/TiledMap.h"
#include "Display/UpdateAreaProcessor.h"
#include "Display/VisList.h"
#include "Display/VisObject.h"
#include "Display/XPT2046.h"
#include "Display/Fonts/Font_4x6.h"
#include "Display/Fonts/Font_6x8.h"
#include "Display/Fonts/Font_8x8.h"
#include "Display/Fonts/Font_8x12.h"
#include "Display/Fonts/Font_10x18.h"
#include "Display/Fonts/Font_12x16.h"

// ***   Drivers Headers   *****************************************************
#include "Drivers/DwtCycleCounter.h"
#include "Drivers/StHalGpio.h"
#include "Drivers/StHalIic.h"
#include "Drivers/StHalIicThreadSafe.h"
#include "Drivers/StHalSpi.h"
#include "Drivers/StHalUart.h"

// ***   Libraries Headers   ***************************************************
#include "Libraries/BoschBME280.h"
#include "Libraries/Eeprom24.h"
#include "Libraries/FramMB85.h"
#include "Libraries/Mlx90614.h"
#include "Libraries/Tcs34725.h"
#include "Libraries/Vl53l0x.h"

// ***   Math Headers   ********************************************************
#include "Math/CircularBuffer.h"
#include "Math/Crc32.h"
#include "Math/FIFO.h"
#include "Math/Hysteresis.h"
#include "Math/MedianListFilter.h"
#include "Math/MedianSortFilter.h"
#include "Math/RollingAverage.h"

// ***   Tasks Headers   *******************************************************
#include "Tasks/ButtonDrv.h"
#include "Tasks/SoundDrv.h"

// ***   UiEngine Headers   ****************************************************
#include "UiEngine/UiButton.h"

#endif
