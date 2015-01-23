#ifndef  __DALI_CLIPBOARD_H__
#define  __DALI_CLIPBOARD_H__

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
 *
 */


// EXTERNAL INCLUDES
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/adaptor-framework/media-data-type.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{

namespace Adaptor
{
class Clipboard;
} // Adaptor
} // Internal

/**
 * @brief Interface to the system clipboard.
 *
 * Once a handle to the Clipboard is retrieved using Get() the Clipboard API can be used on it.
 *
 * The clipboard uses the MediaDataType as a container for data to be push and retrieved from the Clipboard.
 *
 * SetItem() pushes the data to the Clipboard, making it the head item.
 * GetItem() can be used to retrieve the last item pushed to the system Clipboard.
 *
 * The MediaDataType Class provides methods to package and strip data from itself to be used by the user.
 *
 * Show or Hide can be used if a graphical representation of the Clipboard is supported by the system.
 * If a graphical representation is supported then the user can connect to EmitContentSelectedSignal so when an item is selected from
 * the Clipboard that item is packaged into a MediaDataType and returned with the signal.
 */

class DALI_IMPORT_API Clipboard : public BaseHandle
{
public:

  // Typedefs

  typedef SignalV2< bool (Clipboard&, const Dali::MediaDataType&)> ClipboardPasteDataSignal;


  // Signal Names
  static const char* const SIGNAL_PASTE_DATA; ///< name "clipboard-paste-data

  /**
   * @brief Create an uninitialized Clipboard.
   *
   * this can be initialized with one of the derived Clipboard' New() methods
   */
  Clipboard();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Clipboard();

  /**
   * @brief This constructor is used by Adaptor::GetClipboard().
   *
   * @param[in] clipboard A pointer to the clipboard.
   */
  explicit DALI_INTERNAL Clipboard( Internal::Adaptor::Clipboard* clipboard );

  /**
   * @brief Retrieve a handle to the ClipboardEventNotifier instance.
   *
   * @return A handle to the Clipboard
   */
  static Clipboard Get();

  /**
   * @brief Send the given string to the clipboard.
   *
   * @param[in] data MediaDataType object holding the data to be pushed to Clipboard
   */
  void Copy( MediaDataType& data );

  /**
   * @brief Initiate Retrieval of the MediaDataType object from the clipboard.  If more than one item it will be the last item Set.
   *
   * Will trigger a EmitPasteDataSignal with the MediaDataType object.
   */
  void Paste();

  /**
   * @brief Show the Clipboard UI and let user select which item to paste.
   *
   * Will trigger a EmitPasteDataSignal with the MediaDataType object once user selects an item.
   *
   * This will fall back to Paste() if UI Clipboard not support by system.
   */
  void PasteWithUI();

  /**
   * @brief Returns the number of item currently in the Clipboard.
   *
   * @return unsigned int number of Clipboard items
   */
  bool IsEmpty();

  /**
   * @brief Signal from the Clipboard that an item has been selected for pasting.
   *
   * @param[out] data the MediaDataType object containing the data item retrieved from clipboard
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallback( Clipboard& clipboard, const MediaDataType data  );
   * @endcode
   *
   * @return The signal to connect to.
   */
  ClipboardPasteDataSignal& PasteDataSignal();

};
} // namespace Dali

#endif // __DALI_CLIPBOARD_H__
