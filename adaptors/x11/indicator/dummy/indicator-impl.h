#ifndef __DALI_INTERNAL_INDICATOR_H__
#define __DALI_INTERNAL_INDICATOR_H__

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


// INTERNAL INCLUDES
#include <base/interfaces/indicator-interface.h>

namespace Dali
{

namespace Internal
{
namespace Adaptor
{
class Adaptor;

/**
 * Dummy indicator, does nothing.
 * Can be used when the indicator is not required.
 */
class Indicator : public IndicatorInterface
{

public:  // Dali::Internal::Adaptor::IndicicatorInterface
  /**
   * @copydoc Dali::Internal::IndicatorInterface::IndicatorInterface
   */
  Indicator( Adaptor* adaptor,
             Dali::Window::WindowOrientation orientation,
             Observer* observer );

  /**
   * @copydoc Dali::Internal::IndicatorInterface::~IndicatorInterface
   */
  virtual ~Indicator();


  virtual void SetAdaptor(Adaptor* adaptor);

  /**
   * @copydoc Dali::Internal::IndicatorInterface::GetActor
   */
  virtual virtual Dali::Actor GetActor();

  /**
   * @copydoc Dali::Internal::IndicatorInterface::Open
   */
  virtual void Open( Dali::Window::WindowOrientation orientation );

  /**
   * @copydoc Dali::Internal::IndicatorInterface::Close
   */
  virtual void Close();

  /**
   * @copydoc Dali::Internal::IndicatorInterface::SetOpacityMode
   */
  virtual void SetOpacityMode( Dali::Window::IndicatorBgOpacity mode );

  /**
   * @copydoc Dali::Internal::IndicatorInterface::SetVisible
   */
  virtual void SetVisible( Dali::Window::IndicatorVisibleMode visibleMode, bool forceUpdate = false );

  /**
   * @copydoc Dali::Internal::IndicatorInterface::IsConnected
   */
  virtual bool IsConnected();

  /**
   * @copydoc Dali::Internal::IndicatorInterface::SendMessage
   */
  virtual bool SendMessage( int messageDomain, int messageId, const void *data, int size );

};

} // Adaptor
} // Internal
} // Dali

#endif
