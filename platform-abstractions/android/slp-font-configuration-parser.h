#ifndef __DALI_SLP_PLATFORM_FONT_CONFIGURATION_PARSER_H__
#define __DALI_SLP_PLATFORM_FONT_CONFIGURATION_PARSER_H__

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

#include <string>

namespace Dali
{

namespace SlpPlatform
{

namespace FontConfigurationParser
{

/**
 * Parses the given font configuration file and sets fontFamily accordingly.
 * @param[in]   confFile    The configuration file to parse.
 * @param[out]  fontFamily  This gets set with the default font family specified in the conf file.
 * @param[out]  fontStyle   This gets set with the default font style specified in the conf file.
 */
void Parse(std::string confFile, std::string& fontFamily, std::string& fontStyle);

} // namespace FontConfParser

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_FONT_CONFIGURATION_PARSER_H__
