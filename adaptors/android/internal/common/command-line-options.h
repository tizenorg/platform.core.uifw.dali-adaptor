#ifndef __DALI_INTERNAL_COMMAND_LINE_OPTIONS_H__
#define __DALI_INTERNAL_COMMAND_LINE_OPTIONS_H__

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

// EXTERNAL INCLUDES
#include <string>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Parses the passed command line arguments and sets the values stored within this
 * class appropriately.
 */
struct CommandLineOptions
{
public:

  /**
   * Constructor
   * @param[in]  argc  The number of arguments
   * @param[in]  argv  The argument list
   */
  CommandLineOptions(int argc, char *argv[]);

  /**
   * Destructor
   */
  ~CommandLineOptions();

public: // Command line parsed values

  int noVSyncOnRender; ///< If 1, then the user does not want VSync on Render
  int stageWidth; ///< The width of the stage required.  0 if not set.
  int stageHeight; ///< The height of the stage required.   0 if not set.
  std::string stageDPI; ///< DPI stored as hxv, where h is horizontal DPI and v is vertical DPI
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_COMMAND_LINE_OPTIONS_H__
