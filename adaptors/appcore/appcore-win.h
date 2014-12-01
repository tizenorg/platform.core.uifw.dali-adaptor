#ifndef __DALI_ITNERNAL_ADAPTOR_APPCORE_WIN_H
#define __DALI_ITNERNAL_ADAPTOR_APPCORE_WIN_H

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Find and raise windows associated with this process id.
 * Sets errno if there are any system errors.
 * Note, this is a C function, as XWindowAttributes has a structure element called "class"
 *
 * @param[in] pid A process ID
 * @return 0 on success, -1 on error, setting errno.
 */
int AppcoreRaiseWin(pid_t pid);

#ifdef __cplusplus
}
#endif

#endif
