// *****************************************************************************
// @file VisList.cpp
// @author Nicolai Shlapunov
//
// @details DevCore: Visual Object Base Class, implementation
//
// @copyright Copyright (c) 2016, Devtronic & Nicolai Shlapunov
//            All rights reserved.
//
// @section SUPPORT
//
//  Devtronic invests time and resources providing this open source code,
//  please support Devtronic and open-source hardware/software by
//  donations and/or purchasing products from Devtronic.
//
// *****************************************************************************

// *****************************************************************************
// ***   Includes   ************************************************************
// *****************************************************************************
#include "VisList.h"

#include "DisplayDrv.h"

// *****************************************************************************
// ***   VisList   *************************************************************
// *****************************************************************************
VisList::VisList(DisplayDrv& drv)
{
  // Check list pointer, it should be populated with default list in VisObject
  // constructor. When the first list created there no default list, it set to
  // nullptr. However, code designed that every VisObject have to have "parent"
  // list, so for root list we have set pointer to itself.
  if(list == nullptr) list = this;
  // Save display driver
  display_drv = &drv;
}

// *****************************************************************************
// ***   VisList   *************************************************************
// *****************************************************************************
VisList::VisList()
{
  // Check list pointer, it should be populated with default list in VisObject
  // constructor.
  if(list != nullptr)
  {
    // Save display driver of parent list to this one
    display_drv = list->display_drv;
  }
  else
  {
    // This constructor should never be called if default list isn't set for
    // VisObject. Only VisList(DisplayDrv& drv) constructor can be used in this
    // case.
    Break();
  }
}

// *****************************************************************************
// ***   Public: SetDisplayDriver   ********************************************
// *****************************************************************************
Result VisList::SetDisplayDriver(DisplayDrv& drv)
{
  Result result = Result::RESULT_OK;

  // Check if display driver actually changed
  if(display_drv != &drv)
  {
    // Update display driver
    display_drv = &drv;

    // We need to go trough all object in the list, and if object is another
    // list we have to update display driver pointer to this list. However, we
    // couldn't figure out if object is a list, since dynamic_cast doesn't work
    // without rtti. So, instead we go trough all the list and call SetList() for
    // every object. If object is list, it will update it display driver pointer
    // to match parent list.
    // Set pointer to first element
    VisObject* p_obj = object_first;
    // Do for all objects in list
    while(p_obj != nullptr)
    {
      p_obj->SetList(*this);
      // Set pointer to next object in list
      p_obj = p_obj->p_next;
    }
  }

  return result;
}

// *****************************************************************************
// ***   SetList   *************************************************************
// *****************************************************************************
Result VisList::SetList(VisList& l)
{
  Result result = Result::RESULT_OK;

  // If list doesn't changed
  if(list == &l)
  {
    // Update display driver to current list to match parent list in case it changed
    SetDisplayDriver(*(list->display_drv));
  }
  else if(!IsInList()) // We can change list only if object doesn't added to the list chain(hidden)
  {
    // Set list
    list = &l;
    // Update display driver to current list to match parent list
    SetDisplayDriver(*(list->display_drv));
    // Set result
    result = Result::RESULT_OK;
  }
  else
  {
    result = Result::ERR_CANNOT_EXECUTE;
  }

  return result;
}

// *****************************************************************************
// ***   Public: SetParams   ***************************************************
// *****************************************************************************
void VisList::SetParams(int32_t x, int32_t y, int32_t w, int32_t h)
{
  // Lock object for changes
  LockVisObject();
  // Invalidate area for old position/size
  InvalidateObjArea();
  x_start = x;
  y_start = y;
  x_end = x + w - 1;
  y_end = y + h - 1;
  width = w;
  height = h;
  // Invalidate area for new position/size
  InvalidateObjArea();
  // Unlock object after changes
  UnlockVisObject();
}

// *****************************************************************************
// ***   Public: Add Visual Object to object list   ****************************
// *****************************************************************************
Result VisList::AddVisObjectToList(VisObject* obj, uint32_t z)
{
  Result result = Result::ERR_NULL_PTR;

  if((obj != nullptr) && (obj->p_prev == nullptr) && (obj->p_next == nullptr) && (obj != object_first))
  {
    // Take semaphore before add to list
    display_drv->LockDisplayLine();
    // Set object Z
    obj->z = z;
    // If object list empty
    if(object_first == nullptr)
    {
      // Add object to list
      object_first = obj;
      // Set pointer to last object in the list
      object_last = obj;
    }
    else if(object_first->z > z)
    {
      // Set next element to current head element
      obj->p_next = object_first;
      // Set prev element to next after head element
      object_first->p_prev = obj;
      // Set new head for list
      object_first = obj;
    }
    else
    {
      // Set temporary pointer
      VisObject* p_last = object_first;
      // Find last element(last added object to the list should show on top of objects with same z)
      while((p_last->p_next != nullptr) && (p_last->p_next->z <= z)) p_last = p_last->p_next;
      // If it not last element
      if(p_last->p_next != nullptr)
      {
        // Set next pointer in object
        obj->p_next = p_last->p_next;
        // Set prev pointer in object
        obj->p_next->p_prev = obj;
      }
      else
      {
        // Set pointer to last object in the list
        object_last = obj;
      }
      // Set next pointer in prev element
      p_last->p_next = obj;
      // Set prev pointer to new object in list
      obj->p_prev = p_last;
    }
    // If object added to list is active
    if(obj->active)
    {
      SetActive(true); // Set active flag for the list
    }
    // Give semaphore after changes
    display_drv->UnlockDisplayLine();
    // Set return status
    result = Result::RESULT_OK;
  }

  return result;
}

// *****************************************************************************
// ***   Public: Delete Visual Object from object list   ***********************
// *****************************************************************************
Result VisList::DelVisObjectFromList(VisObject* obj)
{
  Result result = Result::ERR_NULL_PTR;

  if((obj != nullptr) && ((obj->p_prev != nullptr) || (obj->p_next != nullptr) || (obj == object_first)) )
  {
    // Take semaphore before add to list
    display_drv->LockDisplayLine();
    // Remove element from head
    if(obj == object_first)
    {
      // Set pointer to next object or clear pointer if no more elements
      object_first = obj->p_next;
      // Clear previous element in first object(if there is one)
      if(object_first != nullptr) object_first->p_prev = nullptr;
      else object_last = nullptr; // If there no first, there is no last
    }
    else if(obj == object_last) // Remove element from tail
    {
      // Set next pointer in previous object to nullptr
      obj->p_prev->p_next = nullptr;
      // Set pointer to previous object
      object_last = obj->p_prev;
    }
    else // Remove element from middle
    {
      obj->p_prev->p_next = obj->p_next;
      obj->p_next->p_prev = obj->p_prev;
    }
    // Clear pointers in object
    obj->p_prev = nullptr;
    obj->p_next = nullptr;
    // Give semaphore after changes
    display_drv->UnlockDisplayLine();
    // Set return status
    result = Result::RESULT_OK;
  }

  return result;
}

// *****************************************************************************
// ***   Public: Put line in buffer   ******************************************
// *****************************************************************************
void VisList::DrawInBufW(color_t* buf, int32_t n, int32_t line, int32_t start_x)
{
  // Draw object only if it fit list
  if((line >= y_start) && (line <= y_end))
  {
    // Set pointer to first element
    VisObject* p_obj = object_first;
    // Count
    int32_t cnt = ((start_x + n - 1) > x_end) ? (x_end - start_x + 1) : n;
    // Do for all objects
    while(p_obj != nullptr)
    {
      p_obj->DrawInBufW(buf, cnt, line - y_start, start_x - x_start);
      // Set pointer to next object in list
      p_obj = p_obj->p_next;
    }
  }
}

// *****************************************************************************
// ***   Public: Put line in buffer   ******************************************
// *****************************************************************************
void VisList::DrawInBufH(color_t* buf, int32_t n, int32_t row, int32_t start_y)
{
  if((row >= x_start) && (row <= x_end))
  {
    // Set pointer to first element
    VisObject* p_obj = object_first;
    // Do for all objects
    while(p_obj != nullptr)
    {
      p_obj->DrawInBufH(buf, n, row - x_start, start_y - y_start);
      // Set pointer to next object in list
      p_obj = p_obj->p_next;
    }
  }
}

// *****************************************************************************
// ***   Public: Action   ******************************************************
// *****************************************************************************
void VisList::Action(ActionType action, int32_t tx, int32_t ty, int32_t tpx, int32_t tpy)
{
  // All object in list relative to the list, not to the touch position.
  // So, we have to adjust touch coordinates for that.
  tx -= x_start;
  ty -= y_start;
  tpx -= x_start;
  tpy -= y_start;

  // Switch for process action
  switch(action)
  {
    // Touch action
    case VisObject::ACT_TOUCH:   // Intentional fall-trough
    case VisObject::ACT_UNTOUCH:
    {
      // Set pointer to first element
      VisObject* p_obj = object_last;
      // If list not empty
      if(p_obj != nullptr)
      {
        // Do for all objects
        while(p_obj != nullptr)
        {
          // If we found active object
          if(p_obj->active)
          {
            // And touch in this object area
            if(   (tx >= p_obj->GetStartX()) && (tx <= p_obj->GetEndX())
               && (ty >= p_obj->GetStartY()) && (ty <= p_obj->GetEndY()) )
            {
              // Call Action() function
              p_obj->Action(action, tx, ty, tpx, tpy);
              // No need check all other objects - only one object can be touched
              break;
            }
          }
          // Get previous object
          p_obj = p_obj->p_prev;
        }
      }
      break;
    }

    case VisObject::ACT_HOLD:    // Intentional fall-trough
    case VisObject::ACT_MOVE:    // Intentional fall-trough
    case VisObject::ACT_MOVEIN:  // Intentional fall-trough
    case VisObject::ACT_MOVEOUT:
    {
      // Set pointer to last element
      VisObject* p_obj = object_last;
      // If list not empty
      if(p_obj != nullptr)
      {
        // Do for all objects
        while(p_obj != nullptr)
        {
          // If we found active object
          if(p_obj->active)
          {
            // And touch in this object area
            if(   (tpx >= p_obj->GetStartX()) && (tpx <= p_obj->GetEndX())
               && (tpy >= p_obj->GetStartY()) && (tpy <= p_obj->GetEndY())
               && (tx >= p_obj->GetStartX()) && (tx <= p_obj->GetEndX())
               && (ty >= p_obj->GetStartY()) && (ty <= p_obj->GetEndY()) )
            {
              // Call Action() function for Move
              if((tx == tpx) && (ty == tpy))
              {
                p_obj->Action(VisObject::ACT_HOLD, tx, ty, tpx, tpy);
              }
              else
              {
                p_obj->Action(VisObject::ACT_MOVE, tx, ty, tpx, tpy);
              }
              // No need check all other objects - only one object can be touched
              break;
            }
            if(   (tpx >= p_obj->GetStartX()) && (tpx <= p_obj->GetEndX())
               && (tpy >= p_obj->GetStartY()) && (tpy <= p_obj->GetEndY())
               && (   ((tx < p_obj->GetStartX()) || (tx > p_obj->GetEndX()))
                   || ((ty < p_obj->GetStartY()) || (ty > p_obj->GetEndY())) ) )
            {
              // Call Action() function for Move Out
              p_obj->Action(VisObject::ACT_MOVEOUT, tx, ty, tpx, tpy);
            }
            if(   (tx >= p_obj->GetStartX()) && (tx <= p_obj->GetEndX())
               && (ty >= p_obj->GetStartY()) && (ty <= p_obj->GetEndY())
               && (   ((tpx < p_obj->GetStartX()) || (tpx > p_obj->GetEndX()))
                   || ((tpy < p_obj->GetStartY()) || (tpy > p_obj->GetEndY())) ) )
            {
              // Call Action() function for Move In
              p_obj->Action(VisObject::ACT_MOVEIN, tx, ty, tpx, tpy);
            }
          }
          // Get previous object
          p_obj = p_obj->p_prev;
        }
      }
      break;
    }

    case VisObject::ACT_MAX:
    default:
      break;
  }
}

// *****************************************************************************
// ***   Public: Invalidate Area   *********************************************
// *****************************************************************************
void VisList::InvalidateArea(int16_t start_x, int16_t start_y, int16_t end_x, int16_t end_y)
{
#if defined(UPDATE_AREA_ENABLED)
  // Find invalidate absolute coordinates
  int32_t sx = GetStartX() + start_x;
  int32_t sy = GetStartY() + start_y;
  int32_t ex = GetStartX() + end_x;
  int32_t ey = GetStartY() + end_y;
  // Clip coordinates
  sx = (sx < GetStartX()) ? GetStartX() : sx;
  sy = (sy < GetStartY()) ? GetStartY() : sy;
  ex = (ex > GetEndX()) ? GetEndX() : ex;
  ey = (ey > GetEndY()) ? GetEndY() : ey;
  // Check if it root list
  if(this == display_drv->GetVisList())
  {
    display_drv->InvalidateArea(sx, sy, ex, ey);
  }
  else if(this == list)
  {
    ; // List point to itself and it isn't display root list
  }
  else
  {
    list->InvalidateArea(sx, sy, ex, ey);
  }
#endif
}

// *****************************************************************************
// ***   Public: Check status of Show Visual Object   **************************
// *****************************************************************************
bool VisList::IsShow(void)
{
  // Return false by default
  bool ret = false;

  // Root list always show
  if(this == display_drv->GetVisList())
  {
    ret = true;
  }
  else if(this == list)
  {
    ret = false; // List point to itself and it isn't display root list
  }
  else
  {
    ret = VisObject::IsShow();
  }

  // Return result
  return ret;
}
