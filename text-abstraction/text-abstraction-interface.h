#ifndef __DALI_PLATFORM_TEXT_ABSTRACTION_INTERFACE_H__
#define __DALI_PLATFORM_TEXT_ABSTRACTION_INTERFACE_H__

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
class TextAbstractionInterface;
} // TextAbstraction
} // Internal

namespace TextAbstraction
{


/**
 *   This will provide interfaces to:
 *
 *  - text processing ( segmentation, re-ordering, shaping)
 *  - glyph creation and metrics
 *  - font validation and matching ( does this font support text xyz)
 *  - keyboard, ime and clipboard functionality
 *
 */
class TextAbstractionInterface : public BaseHandle
{

public:

    /**
     * @brief Create an uninitialized TextAbstraction handle.
     *
     */
    TextAbstractionInterface();

    /**
     * @brief Destructor
     *
     * This is non-virtual since derived Handle types must not contain data or virtual methods.
     */
    ~TextAbstractionInterface();

    /**
     * @brief This constructor is used by TextAbstractionInterface::Get().
     *
     * @param[in] abstractionInterface  A pointer to the internal TextAbstractionInterface.
     */
    explicit DALI_INTERNAL TextAbstractionInterface( Dali::Internal::TextAbstraction::TextAbstractionInterface* abstractionInterface);

    /**
     * @brief Retrieve a handle to the TextAbstractionInterface instance.
     *
     * @return A handle to the TextAbstractionInterface
     */
    static TextAbstractionInterface Get();

};

} // namespace TextAbstractionInterface

} // namespace Dali

#endif // __DALI_PLATFORM_TEXT_ABSTRACTION_INTERFACE_H__
