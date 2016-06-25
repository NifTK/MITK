/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/
#include "mitkPluginActivator.h"
#include "QmitkIGTTutorialView.h"
#include "QmitkIGTTrackingLabView.h"
#include "OpenIGTLinkExample.h"
#include "OpenIGTLinkProviderExample.h"
#include "OpenIGTLinkPlugin.h"

namespace mitk {

void PluginActivator::start(ctkPluginContext* context)
{
  BERRY_REGISTER_EXTENSION_CLASS(QmitkIGTTutorialView, context)
  BERRY_REGISTER_EXTENSION_CLASS(QmitkIGTTrackingLabView, context)
  BERRY_REGISTER_EXTENSION_CLASS(OpenIGTLinkExample, context)
  BERRY_REGISTER_EXTENSION_CLASS(OpenIGTLinkProviderExample, context)
  BERRY_REGISTER_EXTENSION_CLASS(OpenIGTLinkPlugin, context)
}

void PluginActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
}

}
