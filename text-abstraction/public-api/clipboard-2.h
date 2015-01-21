#ifndef __DALI_PLATFORM_TEXT_ABSTRACTION_CLIPBOARD_H__
#define __DALI_PLATFORM_TEXT_ABSTRACTION_CLIPBOARD_H__

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
 *
 */
#include <dali/public-api/object/base-handle.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{

namespace TextAbstraction
{
class Clipboard;
} // TextAbstraction
} // Internal

namespace TextAbstraction
{


/**
 *   Clipboard API
 *
 */
class DALI_IMPORT_API Clipboard : public BaseHandle
{

public:

    /**
     * @brief Create an uninitialized TextAbstraction handle.
     *
     */
    Clipboard();

    /**
     * @brief Destructor
     *
     * This is non-virtual since derived Handle types must not contain data or virtual methods.
     */
    ~Clipboard();

    /**
     * @brief This constructor is used by Clipboard::Get().
     *
     * @param[in] abstractionCLIPBOARD  A pointer to the internal Clipboard.
     */
    explicit DALI_INTERNAL Clipboard( Dali::Internal::TextAbstraction::Clipboard* clipboard);

    /**
     * @brief Retrieve a handle to the Clipboard instance.
     *
     * @return A handle to the Clipboard
     */
    static Clipboard Get();

    /**
      * @brief Send the given string to the clipboard.
      *
      * @param[in] itemData string to send to clip board
      * @return bool true if the internal clip board sending was successful.
      */
     bool SetItem( const std::string& itemData );

    /**
     * @brief Retreive the string at the given index in the clipboard.
     *
     * @param[in] index item in clipboard list to retrieve
     * @return string the text item at the current index.
     */
    std::string GetItem( unsigned int index );

    /**
     * @brief Returns the number of item currently in the clipboard.
     *
     * @return unsigned int number of clipboard items
     */
    unsigned int NumberOfItems();

    /**
     * @brief Show the clipboard window.
     */
    void ShowClipboard();

    /**
     * @brief Hide the clipboard window.
     */
    void HideClipboard();

};

} // namespace Clipboard

} // namespace Dali

#endif // __DALI_PLATFORM_TEXT_ABSTRACTION_CLIPBOARD_H__
