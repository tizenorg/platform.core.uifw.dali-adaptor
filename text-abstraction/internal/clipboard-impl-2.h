#ifndef __DALI_INTERNAL_TEXT_ABSTRACTION_CLIPBOARD_IMPL_H__
#define __DALI_INTERNAL_TEXT_ABSTRACTION_CLIPBOARD_IMPL_H__

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

// EXTERNAL INCLUDES
#include <public-api/clipboard-2.h>
#include <dali/public-api/object/base-object.h>



namespace Dali
{

namespace Internal
{

namespace TextAbstraction
{

/**
 * Implementation of the Clipboard
 */

class Clipboard :  public Dali::BaseObject
{
public:

  /**
   * Constructor
   */
  Clipboard();

  /**
   * Destructor
   */
  ~Clipboard();

  /**
   * @copydoc Dali::Clipboard::Get()
   */
  static Dali::TextAbstraction::Clipboard Get();

  /**
   * @copydoc Dali::Clipboard::SetItem()
   */
  bool SetItem( const std::string& itemData );

  /**
   * @copydoc Dali::Clipboard::GetItem()
   */
  std::string GetItem( unsigned int index );

  /**
   * @copydoc Dali::Clipboard::NumberOfItems()
   */
  unsigned int NumberOfItems();

  /**
   * @copydoc Dali::Clipboard::ShowClipboard()
   */
  void ShowClipboard();

  /**
   * @copydoc Dali::Clipboard::HideClipboard()
   */
  void HideClipboard();

private:

  void*   mPlugin;
  Clipboard( const Clipboard& );
  Clipboard& operator=( Clipboard& );

}; // class Clipboard


} // namespace TextAbstraction

} // namespace Internal

inline static Internal::TextAbstraction::Clipboard& GetImplementation(Dali::TextAbstraction::Clipboard& clipboard)
{
  DALI_ASSERT_ALWAYS( clipboard && "clipboard handle is empty" );
  BaseObject& handle = clipboard.GetBaseObject();
  return static_cast<Internal::TextAbstraction::Clipboard&>(handle);
}

inline static const  Internal::TextAbstraction::Clipboard& GetImplementation(const Dali::TextAbstraction::Clipboard& clipboard)
{
  DALI_ASSERT_ALWAYS( clipboard && "clipboard handle is empty" );
  const BaseObject& handle = clipboard.GetBaseObject();
  return static_cast<const Internal::TextAbstraction::Clipboard&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_ABSTRACTION_CLIPBOARD_IMPL_H__
