#ifndef __DALI_ENVIRONMENT_VARIABLE_H__
#define __DALI_ENVIRONMENT_VARIABLE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace EnvironmentVariable
{

/**
 * @brief Search the environment list for a given variable name and return a pointer to the C string that is associated with the matched environment list member.
 *
 * @param[in] variable Null-terminated character string identifying the name of the environmental variable to look for
 */
DALI_IMPORT_API const char * GetEnvironmentVariable( const char * variable );

} // namespace EnvironmentVariable

} // namespace Dali

#endif /*__DALI_ ENVIRONMENT_VARIABLE_H__ */
