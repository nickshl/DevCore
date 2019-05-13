//******************************************************************************
//  @file VisObject.cpp
//  @author Nicolai Shlapunov
//
//  @details DevCore: Visual Object Base Class, implementation
//
//  @copyright Copyright (c) 2016, Devtronic & Nicolai Shlapunov
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
#include "VisObject.h"
#include "DisplayDrv.h" // for DelVisObjectFromList()

// *****************************************************************************
// ***   Destructor   **********************************************************
// *****************************************************************************
VisObject::~VisObject()
{
  // Remove object from object list before delete
  DisplayDrv::GetInstance().DelVisObjectFromList(this);
}

// *****************************************************************************
// ***   Lock Visual Object  ***************************************************
// *****************************************************************************
Result VisObject::LockVisObject()
{
  // Lock line
  return DisplayDrv::GetInstance().LockDisplayLine();
};

// *****************************************************************************
// ***   Unlock Visual Object   ************************************************
// *****************************************************************************
Result VisObject::UnlockVisObject()
{
  // Unlock line
  return DisplayDrv::GetInstance().UnlockDisplayLine();
};

// *****************************************************************************
// ***   Show Visual Object   **************************************************
// *****************************************************************************
Result VisObject::Show(uint32_t z_pos)
{
  // Z position is 0 by default. In this case we can use 0 here as "no pos" flag
  if(z_pos != 0)
  {
    z = z_pos;
  }
  // Add to VisObject List
  return DisplayDrv::GetInstance().AddVisObjectToList(this, z);
}

// *****************************************************************************
// ***   Hide Visual Object   **************************************************
// *****************************************************************************
Result VisObject::Hide(void)
{
  // Delete from VisObject List
  return DisplayDrv::GetInstance().DelVisObjectFromList(this);
}

// *****************************************************************************
// ***   Check status of Show Visual Object   **********************************
// *****************************************************************************
bool VisObject::IsShow(void)
{
  // Return false by default
  bool ret = false;
  // If any pointer is not null - object in list
  if( (p_next != nullptr) || (p_prev != nullptr) )
  {
    ret = true;
  }
  // Return result
  return ret;
}

// *****************************************************************************
// ***   Move Visual Object   **************************************************
// *****************************************************************************
Result VisObject::Move(int32_t x, int32_t y, bool is_delta)
{
  Result result;
  // Lock object for changes
  result = LockVisObject();
  // Check result
  if(result.IsGood())
  {
    // Make changes
    if(is_delta == true)
    {
      // Move object in delta coordinates
      x_start += x;
      y_start += y;
      x_end += x;
      y_end += y;
    }
    else
    {
      // Move object in absolute coordinates
      x_start = x;
      y_start = y;
      x_end = x + width - 1;
      y_end = y + height - 1;
    }
    // Unlock object after changes
    result = UnlockVisObject();
  }
  // Return result
  return result;
}

// *****************************************************************************
// ***   Action   **************************************************************
// *****************************************************************************
void VisObject::Action(ActionType action, int32_t tx, int32_t ty)
{
  // Empty function. We can do active object without custom Action function
  // for cover active object with lower Z.
}
