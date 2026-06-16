// *****************************************************************************
// @file RollingAverage.h
// @author Nicolai Shlapunov
//
// @details Rolling Average template class, header
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

#ifndef RollingAverage_h
#define RollingAverage_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************

#include "CircularBuffer.h"

//#include <climits>

// TODO: remove!
//#include <cmath>

// *****************************************************************************
// ***   Rolling Average template class   **************************************
// *****************************************************************************
// Template options: data type, size of rolling average buffer array, and sum type.
// For example if we have uint16_t members, it sum can overflow uint16_t, so
// we will need to use uint32_t as sum type to avoid overflow.
template <class T, int N, class ST = T> class RollingAverage : public CircularBuffer<T, N, ST>
{
  public:
    // *************************************************************************
    // ***   Public: RollingAverage   ******************************************
    // *************************************************************************
    RollingAverage()
    {
      // Clear array
      Clear();
    }

    // *************************************************************************
    // ***   Public: ~RollingAverage   *****************************************
    // *************************************************************************
    ~RollingAverage() {};

    // *************************************************************************
    // ***   Public: Add   *****************************************************
    // *************************************************************************
    T Add(T value)
    {
      // If array filled
      if(CircularBuffer<T,N,ST>::filled == true)
      {
        // Decrease average sum by last value
        sum -= CircularBuffer<T,N,ST>::array[CircularBuffer<T,N,ST>::position];
      }
      // Increase sum by added value
      sum += value;

      // Add value into circular buffer
      CircularBuffer<T,N,ST>::Add(value);

      // Find average
      average = sum / static_cast<ST>(CircularBuffer<T,N,ST>::GetItemsCnt());

      // Return result
      return average;
    }

    // *************************************************************************
    // ***   Public: GetSum   **************************************************
    // *************************************************************************
    inline ST GetSum(void)
    {
      return sum;
    }

    // *************************************************************************
    // ***   Public: GetAverage   **********************************************
    // *************************************************************************
    inline T GetAverage(void)
    {
      return average;
    }

    // *************************************************************************
    // ***   Public: GetAverage   **********************************************
    // *************************************************************************
    T GetAverage(uint32_t n)
    {
      // Get sum for n elements and correct n if needed
      ST avg = CircularBuffer<T,N,ST>::GetSumByCnt(n);
      // Find average
      avg = avg / static_cast<ST>(n);
      // Return result
      return avg;
    }

    // *************************************************************************
    // ***   Public: RecalculateAverage   **************************************
    // *************************************************************************
    T RecalculateAverage(void)
    {
      // Calculate sum from array
      sum = CircularBuffer<T,N,ST>::GetSum();
      // Find average
      average = sum / static_cast<ST>(CircularBuffer<T,N,ST>::GetItemsCnt());
      // Return result
      return average;
    }

//    // *************************************************************************
//    // ***   Public: GetStandardDeviation   ************************************
//    // *************************************************************************
//    T GetStandardDeviation(void)
//    {
//      // Get maximum difference value
//      T max_val = CircularBuffer<T,N,ST>::GetMaxValue() - average;
//      // Max difference allowed for calculate square root without overflow
//      T max_dif = std::numeric_limits<T>::max() >> ((sizeof(T) * CHAR_BIT) / 2U);
//      // Shift variable
//      uint32_t shift = 0u;
//      // Find shift
//      while(max_val > max_dif)
//      {
//        max_val >>= 1u;
//        shift++;
//      }
//      // Find corrected average for calculations
//      T avg = average >> shift;
//      // Clear sum
//      T variance = 0;
//      // Store count
//      uint32_t n = CircularBuffer<T,N,ST>::GetItemsCnt();
//      // Calculate sum from array
//      for(uint32_t i = 0u; i < n; i++)
//      {
//        // Get value from buffer
//        T val = CircularBuffer<T,N,ST>::array[i] >> shift;
//        // Find difference between value and average, result always positive
//        if(val > avg) val = val - avg;
//        else          val = avg - val;
//        // Calculate variance
//        variance += (val * val) / static_cast<T>(n);
//      }
//      // Find deviation
//      T deviation = static_cast<T>(sqrt((float)variance));
//      // Revert back correction
//      deviation = deviation << shift;
//      // Return result
//      return deviation;
//    }

    // *************************************************************************
    // ***   Public: Clear   ***************************************************
    // *************************************************************************
    void Clear(void)
    {
      // Clear sum of array value
      sum = 0;
      // Clear average
      average = 0;
      // Clear Circular Buffer
      CircularBuffer<T,N,ST>::Clear();
    }

  private:
    // Sum of array value
    ST sum = 0;
    // Average value
    T average = 0;
};

#endif
