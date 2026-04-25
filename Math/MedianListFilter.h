//******************************************************************************
//  @file MedianListFilter.h
//  @author Nicolai Shlapunov
//
//  @details Median Filter template class, header
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

#ifndef MedianListFilter_h
#define MedianListFilter_h

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************

#include <limits>

// *****************************************************************************
// ***   MedianFilter Filter template class   **********************************
// *****************************************************************************
// Template options: Filtered data type, Size of filter array, and Index type
// used for double linked list. If size of data array is less than 255(254 last
// element idx) - use uint8_t, if less than 65535(65534 last element idx) - use
// uint16_t, else use uint32_t
template <class T, int N, class IT = uint16_t> class MedianListFilter
{
  public:
    // *************************************************************************
    // ***   Public: MedianFilter   ********************************************
    // *************************************************************************
    MedianListFilter()
    {
      // Clear array
      Clear();
    }

    // *************************************************************************
    // ***   Public: ~MedianFilter   *******************************************
    // *************************************************************************
    ~MedianListFilter() {};

    // *************************************************************************
    // ***   Public: Add   *****************************************************
    // *************************************************************************
    T Add(T value)
    {
      // If array isn't filled fully - insert new element at the end of list
      if(filled == false)
      {
        if(position != 0u)
        {
          if(value < array[median_idx].value)
          {
            // Get index of previous element
            IT idx = median_idx;
            // New value less or equal current value - travel list backwards
            while((value < array[idx].value) && (array[idx].prev_idx != N)) idx = array[idx].prev_idx;
            // Insert new element
            if(value < array[idx].value) InsertBefore(idx, position);
            else                         InsertAfter(idx, position);
          }
          else
          {
            // Get index of previous element
            IT idx = median_idx;
            // New value less than current - travel list backwards
            while((value >= array[idx].value) && (array[idx].next_idx != N)) idx = array[idx].next_idx;
            // Insert new element
            if(value >= array[idx].value) InsertAfter(idx, position);
            else                          InsertBefore(idx, position);
          }
        }

        // Search median index if array isn't filled fully
        IT med_idx = 0u;
        // Find the first element of sorted array
        for(IT i = 0u; i < GetItemsCnt(); i++)
        {
          med_idx = i;
          if(array[med_idx].prev_idx == N) break;
        }
        // Now search median element
        for(IT i = 0u; i < GetItemsCnt() / 2u; i++)
        {
          med_idx = array[med_idx].next_idx;
        }
        // Update median and head index
        median_idx = med_idx;
      }
      else
      {
        // *** First - remove element that we replaced from the list ***********

        // Set next idx pointer for previous element
        if(array[position].prev_idx < N)
        {
          array[array[position].prev_idx].next_idx = array[position].next_idx;
        }
        // Set previous idx pointer for next element
        if(array[position].next_idx < N)
        {
          array[array[position].next_idx].prev_idx = array[position].prev_idx;
        }

        // *** Then insert new element into the list ***************************

        // We need removed value to check if we need advance median index
        T removed_value = array[position].value;
        // Median copy since it may be removed
        MedianData median_data = array[median_idx];

        if(value < array[median_idx].value)
        {
          // Get index of previous element
          IT idx = array[median_idx].prev_idx;
          // New value less or equal current value - travel list backwards
          while((value < array[idx].value) && (array[idx].prev_idx != N)) idx = array[idx].prev_idx;
          // Insert new element
          if(value < array[idx].value) InsertBefore(idx, position);
          else                         InsertAfter(idx, position);
          // If we placed new element on the same side as removed value, median
          // index should not be advanced. If it placed on opposite side of
          // median - we should advance it in that direction to keep in in the
          // middle.
          if(removed_value > median_data.value)
          {
            // Advance index. We using copy of median there since it may be removed.
            median_idx = median_data.prev_idx;
            // New value can be placed in between median and previous value and
            // median may be removed, we need additional check.
            if(array[median_idx].next_idx == position) median_idx = position;
          }
        }
        else
        {
          // Get index of previous element
          IT idx = array[median_idx].next_idx;
          // New value greater or equal current value - travel list forwards
          while((value >= array[idx].value) && (array[idx].next_idx != N)) idx = array[idx].next_idx;
          // Insert new element
          if(value >= array[idx].value) InsertAfter(idx, position);
          else                          InsertBefore(idx, position);
          // If we placed new element on the same side as removed value, median
          // index should not be advanced. If it placed on opposite side of
          // median - we should advance it in that direction to keep in in the
          // middle.
          if(removed_value < median_data.value)
          {
            // Advance index. We using copy of median there since it may be removed.
            median_idx = median_data.next_idx;
            // New value can be placed in between median and previous value and
            // median may be removed, we need additional check.
            if(array[median_idx].prev_idx == position) median_idx = position;
          }
        }

        // Store new value in array. Done here to avoid overwrite median
        array[position].value = value;

        // *** Finally find the median index(if needed) ************************

        // If removed value equal median value - we don't know if it was below
        // or above median and if we need to advance median_idx. So, just
        // find new median.
        if(removed_value == median_data.value)
        {
          // Find the first element of sorted array
          for(IT i = 0u; i < GetItemsCnt(); i++)
          {
            // If element has no previous index - it is a first one
            if(array[i].prev_idx == N)
            {
              // Set median index to this element
              median_idx = i;
              // And break the cycle
              break;
            }
          }
          // Now search median element
          for(IT i = 0u; i < GetItemsCnt() / 2u; i++)
          {
            median_idx = array[median_idx].next_idx;
          }
        }
      }

      // Increase position
      position++;
      // Check overflow
      if(position >= N)
      {
        // Clear position
        position = 0u;
        // Full array filled
        filled = true;
      }

      // Return median value
      return array[median_idx].value;
    }

    // *************************************************************************
    // ***   Public: GetValueFromHead   ****************************************
    // *************************************************************************
    T GetValueFromHead(IT n)
    {
      IT idx = GetHeadIdx();
      // Check if n less than values
      if(n < GetItemsCnt())
      {
        // Add if necessary
        if(n > idx) idx += N;
        idx -= n; 
      }
      // Return result
      return array[idx].value;
    }

    // *************************************************************************
    // ***   Public: GetValueFromTail   ****************************************
    // *************************************************************************
    T GetValueFromTail(IT n)
    {
      IT idx = GetTailIdx();
      // Check if n less than values
      if(n < GetItemsCnt())
      {
        idx += n;
        // Decrease if necessary
        if(idx >= N) idx -= N;
      }
      // Return result
      return array[idx].value;
    }

    // *************************************************************************
    // ***   Public: IsFilled   ************************************************
    // *************************************************************************
    inline bool IsFilled(void)
    {
      return filled;
    }

    // *************************************************************************
    // ***   Public: GetHeadIdx   **********************************************
    // *************************************************************************
    IT GetHeadIdx(void)
    {
      // Local variable for store index
      IT head_idx = 0u;
      // Find head index
      if(position != 0u)
      {
        head_idx = position - 1u;
      }
      else
      {
        if(filled == true)
        {
          head_idx = N - 1u;
        }
      }
      // Return result
      return head_idx;
    }

    // *************************************************************************
    // ***   Public: GetTailIdx   **********************************************
    // *************************************************************************
    IT GetTailIdx(void)
    {
      // Local variable for store index
      IT tail_idx = 0u;
      // Find tail index
      if(filled == false)
      {
        tail_idx = 0u;
      }
      else
      {
        tail_idx = position;
      }
      // Return result
      return tail_idx;
    }

    // *************************************************************************
    // ***   Public: GetItemsCnt   *********************************************
    // *************************************************************************
    IT GetItemsCnt(void)
    {
      // Position by default
      IT n = position;
      // If array full filled
      if(filled == true)
      {
        // Calculate average using all elements
        n = N;
      }
      // Return result
      return n;
    }

    // *************************************************************************
    // ***   Public: GetMinValue   *********************************************
    // *************************************************************************
    T GetMinValue(void)
    {
      // Items count
      IT n = GetItemsCnt();

      // Clear max value
      T min_val = std::numeric_limits<T>::max();
      // Calculate sum from array
      for(IT i = 0u; i < n; i++)
      {
        if(array[i].value < min_val)
        {
          min_val = array[i].value;
        }
      }

      // Return result
      return min_val;
    }

    // *************************************************************************
    // ***   Public: GetMaxValue   *********************************************
    // *************************************************************************
    T GetMaxValue(void)
    {
      // Items count
      IT n = GetItemsCnt();

      // Clear max value
      T max_val = std::numeric_limits<T>::min();
      // Calculate sum from array
      for(IT i = 0u; i < n; i++)
      {
        if(array[i].value > max_val)
        {
          max_val = array[i].value;
        }
      }

      // Return result
      return max_val;
    }

    // *************************************************************************
    // ***   Public: GetSum   **************************************************
    // *************************************************************************
    T GetSum(void)
    {
      // Clear sum
      T sum = 0;
      // Store count
      IT n = GetItemsCnt();
      // Calculate sum from array
      for(IT i = 0u; i < n; i++)
      {
        sum += array[i].value;
      }
      // Return result
      return sum;
    }

    // *************************************************************************
    // ***   Public: GetAverage   **********************************************
    // *************************************************************************
    T GetAverage(void)
    {
      // Find average
      T avg = GetSum() / static_cast<T>(GetItemsCnt());
      // Return result
      return avg;
    }

    // *************************************************************************
    // ***   Public: GetMedian   ***********************************************
    // *************************************************************************
    inline T GetMedian(void)
    {
      return array[median_idx].value;
    }

    // *************************************************************************
    // ***   Public: Clear   ***************************************************
    // *************************************************************************
    void Clear(void)
    {
      // Clear position
      position = 0u;
      // Clear median index
      median_idx = 0;
      // Clear full fill flag
      filled = false;
      // Clear array
      for(IT i = 0u; i < N; i++)
      {
        array[i].value = 0;
        array[i].prev_idx = N;
        array[i].next_idx = N;
      }
    }

  protected:

    // Structure to calculate median
    struct MedianData
    {
      T value = 0;          // Value
      IT next_idx = N; // Index of previous element in array
      IT prev_idx = N; // Index of next element in array
    };
    // Array for calculating median
    MedianData array[N];

    // Position in array for next add operation
    IT position = 0u;
    // Median index
    IT median_idx = 0u;

    // Flag for track fill full array
    bool filled = false;

  private:

    // *************************************************************************
    // ***   Private: InsertBefore   *******************************************
    // *************************************************************************
    void InsertBefore(IT place_idx, IT element_idx)
    {
      // Save next element into current one
      array[element_idx].next_idx = place_idx;
      // Save previous element into current one
      array[element_idx].prev_idx = array[place_idx].prev_idx;
      // Save previous element for next one
      array[place_idx].prev_idx = element_idx;
      // If inserted element isn't a first element
      if(array[element_idx].prev_idx < N)
      {
        array[array[element_idx].prev_idx].next_idx = element_idx;
      }
    }

    // *************************************************************************
    // ***   Private: InsertAfter   ********************************************
    // *************************************************************************
    void InsertAfter(IT place_idx, IT element_idx)
    {
      // Save next element into current one
      array[element_idx].prev_idx = place_idx;
      // Save previous element into current one
      array[element_idx].next_idx = array[place_idx].next_idx;
      // Save previous element for next one
      array[place_idx].next_idx = element_idx;
      // If inserted element isn't a first element
      if(array[element_idx].next_idx < N)
      {
        array[array[element_idx].next_idx].prev_idx = element_idx;
      }
    }
};

#endif
