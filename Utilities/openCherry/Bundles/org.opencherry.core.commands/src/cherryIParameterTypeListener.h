/*=========================================================================

 Program:   openCherry Platform
 Language:  C++
 Date:      $Date$
 Version:   $Revision$

 Copyright (c) German Cancer Research Center, Division of Medical and
 Biological Informatics. All rights reserved.
 See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/


#ifndef CHERRYIPARAMETERTYPELISTENER_H_
#define CHERRYIPARAMETERTYPELISTENER_H_

#include <cherryObject.h>
#include <cherryMacros.h>
#include <cherryMessage.h>

#include "cherryCommandsDll.h"

namespace cherry {

class ParameterTypeEvent;

/**
 * An instance of this interface can be used by clients to receive notification
 * of changes to one or more instances of {@link ParameterType}.
 * <p>
 * This interface may be implemented by clients.
 * </p>
 *
 * @see ParameterType#AddListener(IParameterTypeListener::Pointer)
 * @see ParameterType#RemoveListener(IParameterTypeListener::Pointer)
 */
struct CHERRY_COMMANDS IParameterTypeListener : public virtual Object {

  cherryInterfaceMacro(IParameterTypeListener, cherry)

  struct Events {

    typedef Message1<const SmartPointer<const ParameterTypeEvent> > Event;

    Event parameterTypeChanged;

    void AddListener(IParameterTypeListener::Pointer listener);
    void RemoveListener(IParameterTypeListener::Pointer listener);

    typedef MessageDelegate1<IParameterTypeListener, const SmartPointer<const ParameterTypeEvent> > Delegate;
  };

  /**
   * Notifies that one or more properties of an instance of
   * {@link ParameterType} have changed. Specific details are described in the
   * {@link ParameterTypeEvent}.
   *
   * @param parameterTypeEvent
   *            the event. Guaranteed not to be <code>null</code>.
   */
  virtual void ParameterTypeChanged(const SmartPointer<const ParameterTypeEvent> parameterTypeEvent) = 0;

};


}

#endif /* CHERRYIPARAMETERTYPELISTENER_H_ */
