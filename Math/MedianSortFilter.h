// *****************************************************************************
// @file MedianSortFilter.h
// @author Nicolai Shlapunov
//
// @details Median template class, header
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

#ifndef MedianSortFilter_h
#define MedianSortFilter_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************

#include "CircularBuffer.h"

// *****************************************************************************
// ***   Median template class   ***********************************************
// *****************************************************************************
template <class T, int N, class ST = T> class MedianSortFilter : public CircularBuffer<T, N, ST>
{
  public:
    // *************************************************************************
    // ***   Public: Median   **************************************************
    // *************************************************************************
    MedianSortFilter()
    {
      // Clear array
      Clear();
    }

    // *************************************************************************
    // ***   Public: ~Median   ************************************************
    // *************************************************************************
    ~MedianSortFilter() {};

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

      // Calculate median
      CalcMedian();

      // Return result
      return median;
    }

    // *************************************************************************
    // ***   Public: GetMedian   ***********************************************
    // *************************************************************************
    inline T GetMedian(void)
    {
      return median;
    }

    // *************************************************************************
    // ***   Public: GetSum   **************************************************
    // *************************************************************************
    inline T GetSum(void)
    {
      return sum;
    }

    // *************************************************************************
    // ***   Public: GetAverage   **********************************************
    // *************************************************************************
    inline T GetAverage(void)
    {
      return (static_cast<T>(CircularBuffer<T,N,ST>::GetItemsCnt()) ? sum / static_cast<T>(CircularBuffer<T,N,ST>::GetItemsCnt()) : 0);
    }

    // *************************************************************************
    // ***   Public: GetAverage   **********************************************
    // *************************************************************************
    T GetAverage(uint32_t n)
    {
      // Get sum for n elements and correct n if needed
      T avg = CircularBuffer<T,N,ST>::GetSumByCnt(n);
      // Find average
      avg = avg / static_cast<T>(n);
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
      // Return result
      return GetAverage();
    }

    // *************************************************************************
    // ***   Public: Clear   ***************************************************
    // *************************************************************************
    void Clear(void)
    {
      // Clear median value
      median = 0;
      // Clear sum of array value
      sum = 0;
      // Clear Circular Buffer
      CircularBuffer<T,N,ST>::Clear();
    }

  private:
    // Array for calculating median
    T array_sorted[N] = {0};
    // Median value
    T median = 0;
    // Sum of array value
    T sum = 0;

    // *************************************************************************
    // ***   Private: CalcMedian   *********************************************
    // *************************************************************************
    void CalcMedian()
    {
      // Helper to calculate the median. Copies the buffer into the temp area,
      // then calls Hoare's in-place selection algorithm to obtain the median.
      for(uint32_t i = 0u; i < CircularBuffer<T,N,ST>::GetItemsCnt(); i++)
      {
        array_sorted[i] = CircularBuffer<T,N,ST>::array[i];
      }
      // Calculate median
      median = Select(0, CircularBuffer<T,N,ST>::GetItemsCnt() - 1, CircularBuffer<T,N,ST>::GetItemsCnt() / 2);
    }

    // *************************************************************************
    // ***   Private: Partition   **********************************************
    // *************************************************************************
    int Partition(int l, int r, int p)
    {
      // Partition function, like from quicksort. l and r are the left and right
      // bounds of the array (m_tmp), respectively, and p is the pivot index.
      T tmp;
      T pv = array_sorted[p];
      array_sorted[p] = array_sorted[r];
      array_sorted[r] = pv;
      int s = l;
      for(int i = l; i < r; i++)
      {
        if(array_sorted[i] < pv)
        {
          tmp = array_sorted[s];
          array_sorted[s] = array_sorted[i];
          array_sorted[i] = tmp;
          s++;
        }
      }
      tmp = array_sorted[s];
      array_sorted[s] = array_sorted[r];
      array_sorted[r] = tmp;
      return s;
    }

    // *************************************************************************
    // ***   Private: Select   *************************************************
    // *************************************************************************
    T Select(int l, int r, int k)
    {
      // Result variable
      T result = 0;
      // Hoare's quickselect. l and r are the array bounds, and k conveys that
      // we want to return the k-th value
      if(l == r)
      {
        result = array_sorted[l];
      }
      else
      {
        int32_t p = (l + r) / 2;
        p = Partition(l, r, p);
        if(p == k)
        {
          result = array_sorted[k];
        }
        else if(k < p)
        {
          result = Select(l, p - 1, k);
        }
        else
        {
          result = Select(p + 1, r, k);
        }
      }
      return result;
    }
};

#endif
