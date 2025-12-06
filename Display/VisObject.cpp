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
#include "VisList.h"

#include "DisplayDrv.h" // for AddVisObjectToList() and DelVisObjectFromList()

// Default list is nullptr until redefined
VisList* VisObject::default_list = nullptr;

// *****************************************************************************
// ***   VisObject   ***********************************************************
// *****************************************************************************
VisObject::VisObject()
{
  list = default_list;
}

// *****************************************************************************
// ***   Destructor   **********************************************************
// *****************************************************************************
VisObject::~VisObject()
{
  // Remove object from object list before delete
  Hide();
}

// *****************************************************************************
// ***   Lock Visual Object  ***************************************************
// *****************************************************************************
Result VisObject::LockVisObject()
{
  // Lock line
  return list->display_drv->LockDisplayLine();
};

// *****************************************************************************
// ***   Unlock Visual Object   ************************************************
// *****************************************************************************
Result VisObject::UnlockVisObject()
{
  // Unlock line
  return list->display_drv->UnlockDisplayLine();
};

// *****************************************************************************
// ***   SetList   *************************************************************
// *****************************************************************************
Result VisObject::SetList(VisList& l)
{
  Result result = Result::RESULT_OK;

  // Check if already in this list
  if(list == &l)
  {
    ; // If already in requested list - do nothing, return ok
  }
  else if(!IsInList()) // We can change list only if object doesn't added to the list chain(hidden)
  {
    list = &l;
  }
  else
  {
    result = Result::ERR_CANNOT_EXECUTE;
  }

  return result;
}

// *****************************************************************************
// ***   Show Visual Object   **************************************************
// *****************************************************************************
Result VisObject::Show(uint32_t z_pos)
{
  // z_pos is 0 by default. Next code allow call Show() without explicit Z and
  // have the same Z as previous calls.
  if(z_pos != 0)
  {
    z = z_pos;
  }
  // Invalidate area for update. We need to force invalidation there since
  // Visual Object isn't show yet
  InvalidateObjArea(true);
  // Add to VisObject List
  return list->AddVisObjectToList(this, z);
}

// *****************************************************************************
// ***   Hide Visual Object   **************************************************
// *****************************************************************************
Result VisObject::Hide(void)
{
  // Invalidate area for update
  InvalidateObjArea();
  // Delete from VisObject List
  return list->DelVisObjectFromList(this);
}

// *****************************************************************************
// ***   Check status of Show Visual Object   **********************************
// *****************************************************************************
bool VisObject::IsShow(void)
{
  // Check if VisObject in the list, and if it is check if list is show
  return (IsInList() && list->IsShow());
}

// *****************************************************************************
// ***   Check status of Show Visual Object   **********************************
// *****************************************************************************
bool VisObject::IsInList(void)
{
  // If any pointer is not null - object in list
  return ((p_next != nullptr) || (p_prev != nullptr));
}

// *****************************************************************************
// ***   Move Visual Object   **************************************************
// *****************************************************************************
Result VisObject::Move(int32_t x, int32_t y, bool is_delta)
{
  // Lock object for changes
  Result result = LockVisObject();
  // Check result
  if(result.IsGood())
  {
    // Check if we need to move object at all - prevent unnecessary call InvalidateObjArea() function
    if( ((is_delta == false) && ((x != x_start) || (y != y_start))) ||
        ((is_delta == true)  && ((x != 0)       || (y != 0))) )
    {
      // Invalidate area before move to redraw area object move from
      InvalidateObjArea();
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
      // Invalidate area after move to redraw area object move to
      InvalidateObjArea();
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
void VisObject::Action(ActionType action, int32_t tx, int32_t ty, int32_t tpx, int32_t tpy)
{
  // Empty function. We can do active object without custom Action function
  // for cover active object with lower Z.
}

// *****************************************************************************
// ***   Invalidate Display Area   *********************************************
// *****************************************************************************
void VisObject::InvalidateObjArea(bool force)
{
#if defined(UPDATE_AREA_ENABLED)
  // Only if VisObject is show
  if(IsShow() || force)
  {
    // Invalidate area
    list->InvalidateArea(x_start, y_start, x_end, y_end);
  }
#endif
}
