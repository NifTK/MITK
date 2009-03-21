/*=========================================================================

 Program:   openCherry Platform
 Language:  C++
 Date:      $Date$
 Version:   $Revision: XXXXXX $

 Copyright (c) German Cancer Research Center, Division of Medical and
 Biological Informatics. All rights reserved.
 See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/

#ifndef __CHERRY_WEAK_POINTER_H__
#define __CHERRY_WEAK_POINTER_H__

#include <exception>
#include <iostream>

#include "cherryMessage.h"
#include "cherrySmartPointer.h"

namespace cherry
{

/** \class WeakPointer
 *  \brief implements a WeakPointer class to deal with circular reference problems.
 *
 *
 * The WeakPointer class implements smart pointer semantics without increasing the reference count.
 * It registers itself at the Object it points to in order to get notified of its destruction and sets its internal pointer to 0.
 * To be able to access an object through a weak pointer, you either use SmartPointer(const WeakPointer&)
 * or the WeakPointer::Lock() method. The first approach throws a BadWeakPointerException if
 * the object has been destroyed, the second returns an empty SmartPointer.
 */
template<class TObjectType>
class WeakPointer
{

public:

  typedef TObjectType ObjectType;

  /** Default Constructor */
  WeakPointer() :
    m_Pointer(0)
  {

  }

  /** Constructor */
  template<class Other>
  explicit WeakPointer(cherry::SmartPointer<Other> sptr) :
    m_Pointer(dynamic_cast<ObjectType*>(sptr.GetPointer()))
  {

    if (m_Pointer)
      m_Pointer->AddDestroyListener(MessageDelegate<WeakPointer> (this,
        &WeakPointer::ObjectDestroyed));

  }

  /** constructor  */
  template<class Other>
  WeakPointer(const WeakPointer<Other>& p) :
    m_Pointer(dynamic_cast<ObjectType*>(p.m_Pointer))
  {
    if (m_Pointer)
      m_Pointer->AddDestroyListener(MessageDelegate<WeakPointer> (this,
        &WeakPointer::ObjectDestroyed));
  }

  /** Copy constructor  */
  WeakPointer(const WeakPointer& p) :
    m_Pointer(p.m_Pointer)
  {
    if (m_Pointer)
      m_Pointer->AddDestroyListener(MessageDelegate<WeakPointer> (this,
        &WeakPointer::ObjectDestroyed));
  }

  template<class Other>
  WeakPointer &operator =(const SmartPointer<Other> &r)
  {
    if (m_Pointer)
      m_Pointer->RemoveDestroyListener(MessageDelegate<WeakPointer> (this,
          &WeakPointer::ObjectDestroyed));

    m_Pointer = const_cast<ObjectType*>(r.GetPointer());
    if (m_Pointer)
      m_Pointer->AddDestroyListener(MessageDelegate<WeakPointer> (this,
          &WeakPointer::ObjectDestroyed));

    return *this;
  }

  WeakPointer& operator=(const WeakPointer& other)
  {
    if (m_Pointer)
      m_Pointer->RemoveDestroyListener(MessageDelegate<WeakPointer> (this,
          &WeakPointer::ObjectDestroyed));

    this->m_Pointer = other.m_Pointer;
    if (m_Pointer)
      m_Pointer->AddDestroyListener(MessageDelegate<WeakPointer> (this,
          &WeakPointer::ObjectDestroyed));

    return *this;
  }

  template<class Other>
  WeakPointer& operator=(const WeakPointer<Other>& other)
  {
    if (m_Pointer)
      m_Pointer->RemoveDestroyListener(MessageDelegate<WeakPointer> (this,
          &WeakPointer::ObjectDestroyed));

    this->m_Pointer = const_cast<ObjectType*>(other.m_Pointer);
    if (m_Pointer)
      m_Pointer->AddDestroyListener(MessageDelegate<WeakPointer> (this,
          &WeakPointer::ObjectDestroyed));

    return *this;
  }

  /** Template comparison operators. */
  template<typename R>
  bool operator ==(const R* o) const
  {
    return (m_Pointer == 0 ? o == 0 : (o && m_Pointer->operator==(o)));
  }

  template<typename R>
  bool operator ==(const WeakPointer<R>& r) const
  {
    const R* o = r.m_Pointer;
    return (m_Pointer == 0 ? o == 0 : (o && m_Pointer->operator==(o)));
  }

  /** lock method is used to access the referring object  */
  SmartPointer<ObjectType> Lock() const
  {
    SmartPointer<ObjectType> sp(m_Pointer);
    return sp;
  }

  void Reset()
  {
    if (m_Pointer)
      m_Pointer->RemoveDestroyListener(MessageDelegate<WeakPointer> (this,
          &WeakPointer::ObjectDestroyed));

    m_Pointer = 0;
  }

  bool Expired() const
  {
    return m_Pointer == 0;
  }

  /** Destructor */
  ~ WeakPointer()
  {
    if (m_Pointer)
      m_Pointer->RemoveDestroyListener(MessageDelegate<WeakPointer> (this,
          &WeakPointer::ObjectDestroyed));
  }

private:

  template<class Y> friend class SmartPointer;
  template<class Y> friend class WeakPointer;

  ObjectType *m_Pointer;

  void ObjectDestroyed()
  {
    m_Pointer = 0;
  }

};

}

#endif // __CHERRY_WEAK_POINTER_H__
