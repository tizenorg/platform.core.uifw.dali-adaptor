//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include "command-line-options.h"

// EXTERNAL INCLUDES
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
struct Argument
{
  const char * const opt;
  const char * const optDescription;

  void Print()
  {
    std::cout << std::left << "  --";
    std::cout.width( 15 );
    std::cout << opt;
    std::cout << optDescription;
    std::cout << std::endl;
  }
};

Argument EXPECTED_ARGS[] =
{
  { "no-vsync", "Disable VSync on Render" },
  { "width",    "Stage Width"             },
  { "height",   "Stage Height"            },
  { "dpi",      "Emulated DPI"            },
  { "usage",    "Usage"                   },

  { NULL,       NULL                      }
};

} // unnamed namespace


CommandLineOptions::CommandLineOptions(int *argc, char **argv[])
: noVSyncOnRender(0),
  stageWidth(0), stageHeight(0)
{
  if ( *argc > 1 )
  {
    const struct option options[]=
    {
      { EXPECTED_ARGS[0].opt, no_argument,       &noVSyncOnRender, 1 },  // "--no-vsync"
      { EXPECTED_ARGS[1].opt, required_argument, NULL,             0 },  // "--width"
      { EXPECTED_ARGS[2].opt, required_argument, NULL,             0 },  // "--height"
      { EXPECTED_ARGS[3].opt, required_argument, NULL,             0 },  // "--dpi"
      { EXPECTED_ARGS[4].opt, no_argument,       NULL,             0 },  // "--usage"
      { 0, 0, 0, 0 } // end of options
    };

    int opt(0);
    int optIndex(0);
    int startIndex = optind;

    const char* optString = "whdu?:";

    while (opt != -1)
    {
      opt = getopt_long(*argc, *argv, optString, options, &optIndex);

      switch (opt)
      {
        case 0:
          // if setting vsync getopt set flag already
          break;

        case 'w':
          stageWidth = atoi(optarg);
          break;

        case 'h':
          stageHeight = atoi(optarg);
          break;

        case 'd':
          stageDPI.assign(optarg);
          break;

        case 'u':
        {
          // show usage
          std::cout << "Available options:" << std::endl;
          Argument* arg = EXPECTED_ARGS;
          while ( arg->opt )
          {
            arg->Print();
          }
          break;
        }

        default:
          break;
      }
    }

    // Take out the options we have processed
    if ( optind > startIndex )
    {
      // Just move the program name to just before the unhandled option.

      (*argv)[optind - 1] = (*argv)[0];

      *argv += optind - 1;
      *argc -= optind - 1;
      optind = 1;
    }
  }
}

CommandLineOptions::~CommandLineOptions()
{
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
