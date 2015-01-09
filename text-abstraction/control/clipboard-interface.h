#ifndef __DALI_PLATFORM_TEXT_ABSTRACTION_CLIPBOARD_INTERFACE_H__
#define __DALI_PLATFORM_TEXT_ABSTRACTION_CLIPBOARD_INTERFACE_H__

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

namespace Dali
{

namespace TextAbstraction
{

class ClipboardInterface
{



  /**
    * @brief Send the given string to the clipboard.
    *
    * @param[in] itemData string to send to clip board
    * @return bool true if the internal clip board sending was successful.
    */
   virtual bool SetItem( const std::string& itemData ) = 0;

   /**
    * @brief Retreive the string at the given index in the clipboard.
    *
    * @param[in] index item in clipboard list to retrieve
    * @return string the text item at the current index.
    */
   virtual std::string GetItem( unsigned int index ) = 0;

   /**
    * @brief Returns the number of item currently in the clipboard.
    *
    * @return unsigned int number of clipboard items
    */
   virtual unsigned int NumberOfItems() = 0;

   /**
    * @brief Show the clipboard window.
    */
   virtual void ShowClipboard() = 0;

   /**
    * @brief Hide the clipboard window.
    */
   virtual void HideClipboard() = 0;


private:

   ClipboardInterface() {}

   virtual ~ClipboardInterface() {}

  // Undefined copy constructor.
   ClipboardInterface( const ClipboardInterface& );

  // Undefined assignment operator.
  ClipboardInterface& operator=( const ClipboardInterface& );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_PLATFORM_TEXT_ABSTRACTION_CLIPBOARD_INTERFACE_H__
