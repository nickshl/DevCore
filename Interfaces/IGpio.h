//******************************************************************************
//  @file IIic.h
//  @author Nicolai Shlapunov
//
//  @details DevCore: GPIO driver interface, header
//
//  @section LICENSE
//
//   Software License Agreement (Modified BSD License)
//
//   Copyright (c) 2018, Devtronic & Nicolai Shlapunov
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

#ifndef IGpio_h
#define IGpio_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include "DevCfg.h"

// *****************************************************************************
// ***   GPIO Driver Interface   ***********************************************
// *****************************************************************************
class IGpio
{
  public:
    // ***   State   ***********************************************************
    enum State : bool
    {
       LOW = false,
       HIGH = true
    };

    // ***   Type specifies the input or output   ******************************
    enum Type : uint8_t
    {
       INPUT = 0u,
       OUTPUT
    };

    // ***   Polarity   ********************************************************
    enum Polarity : uint8_t
    {
       NORMAL = 0u,
       INVERTED
    };

    // *************************************************************************
    // ***   Public: Constructor   *********************************************
    // *************************************************************************
    explicit IGpio(Type type_parm, Polarity polarity_parm = NORMAL) : type(type_parm),
                                                                      polarity(polarity_parm) {};

    // *************************************************************************
    // ***   Public: Destructor   **********************************************
    // *************************************************************************
    virtual ~IGpio() {};

    // *************************************************************************
    // ***   Public: Init   ****************************************************
    // *************************************************************************
    virtual Result Init() {return Result::ERR_NOT_IMPLEMENTED;}

    // *************************************************************************
    // ***   Public: DeInit   **************************************************
    // *************************************************************************
    virtual Result DeInit() {return Result::ERR_NOT_IMPLEMENTED;}

    // *************************************************************************
    // ***   Public: Read   ****************************************************
    // *************************************************************************
    virtual State Read() = 0;

    // *************************************************************************
    // ***   Public: Write   ***************************************************
    // *************************************************************************
    virtual void Write(State state) = 0;

    // *************************************************************************
    // ***   Public: SetHigh   *************************************************
    // *************************************************************************
    inline void SetHigh() {Write((polarity == NORMAL) ? HIGH : LOW);}

    // *************************************************************************
    // ***   Public: SetLow   **************************************************
    // *************************************************************************
    inline void SetLow() {Write((polarity == NORMAL) ? LOW : HIGH);}

    // *************************************************************************
    // ***   Public: IsHigh   **************************************************
    // *************************************************************************
    inline bool IsHigh() {return (Read() == ((polarity == NORMAL) ? HIGH : LOW));}

    // *************************************************************************
    // ***   Public: IsLow   ***************************************************
    // *************************************************************************
    inline bool IsLow() {return (Read() == ((polarity == NORMAL) ? LOW : HIGH));}

  protected:
    // Indicates type(Input/Output)
    Type type;
    // Indicates if this I/O is active low or active high
    Polarity polarity;

  private:
    // *************************************************************************
    // ***   Private: Constructors and assign operator - prevent copying   *****
    // *************************************************************************
    IGpio(const IGpio&);
};

#endif
