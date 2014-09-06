/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

#include <dali/public-api/dali-core.h>

#include "public-api/adaptor-framework/evas-plugin.h"
#include "public-api/adaptor-framework/common/adaptor.h"
#include "public-api/adaptor-framework/common/render-surface.h"
#include "public-api/adaptor-framework/common/orientation.h"
#include "public-api/adaptor-framework/common/timer.h"

using namespace Dali;

/*****************************************************************************
 * Test to see if the dali-adaptor is linking correctly.
 */

class LinkerApp : public ConnectionTracker
{
public:
  LinkerApp(EvasPlugin &app)
  {
    app.InitSignal().Connect(this, &LinkerApp::Create);
  }

public:

  void Create(EvasPlugin& app)
  {
  }
};

/*****************************************************************************/

int
main(int argc, char **argv)
{
  boost::any nullPtr;

  EvasPlugin *evasPlugin = new EvasPlugin(nullPtr);

  LinkerApp linkerApp (*evasPlugin);

  evasPlugin->Run();

  delete evasPlugin;
  return 0;
}

