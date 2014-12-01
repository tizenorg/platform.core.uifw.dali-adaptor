/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <locale.h>
#include <libintl.h>
#include <stdlib.h>
#include <errno.h>

#include <vconf.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

void UpdateLanguage()
{
  char *lang = vconf_get_str(VCONFKEY_LANGSET);
  if (lang)
  {
    setenv("LANG", lang, 1);
    setenv("LC_MESSAGES", lang, 1);

    char* retVal = setlocale(LC_ALL, "");
    if (retVal == NULL)
    {
      retVal = setlocale(LC_ALL, vconf_get_str(VCONFKEY_LANGSET));
    }
    free(lang);
  }
}

void UpdateRegion()
{
  // Set region
  //
  char *region = vconf_get_str(VCONFKEY_REGIONFORMAT);
  if (region)
  {
    setenv("LC_CTYPE", region, 1);
    setenv("LC_NUMERIC", region, 1);
    setenv("LC_TIME", region, 1);
    setenv("LC_COLLATE", region, 1);
    setenv("LC_MONETARY", region, 1);
    setenv("LC_PAPER", region, 1);
    setenv("LC_NAME", region, 1);
    setenv("LC_ADDRESS", region, 1);
    setenv("LC_TELEPHONE", region, 1);
    setenv("LC_MEASUREMENT", region, 1);
    setenv("LC_IDENTIFICATION", region, 1);
    free(region);
  }
}

void SetupEnvironment(const char* domain, const char* name)
{
  UpdateLanguage();
  UpdateRegion();

  // Set domain
  bindtextdomain(domain, name);
  textdomain(domain);
}

} // Adaptor
} // Internal
} // Dali
