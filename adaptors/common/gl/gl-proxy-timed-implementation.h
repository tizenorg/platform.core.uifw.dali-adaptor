#ifndef __DALI_INTERNAL_GL_PROXY_TIMED_IMPLEMENTATION_H__
#define __DALI_INTERNAL_GL_PROXY_TIMED_IMPLEMENTATION_H__

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

// INTERNAL INCLUDES
#include <gl/gl-implementation.h>
#include <base/interfaces/performance-interface.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

/**
 * @brief GlProxyTimedImplementation is a wrapper for the concrete implementation
 * of GlAbstraction that also gathers time information inside glClear.
 * glClear can be used by the DDK to check there is a buffer available
 * to render to. If there isn't it can wait inside the function until one is available.
 */
class GlProxyTimedImplementation : public GlImplementation
{
public:

  /**
   * @brief Constructor
   * @param environmentOptions to check how often to log results
   */
  GlProxyTimedImplementation(PerformanceInterface& performanceInterface)
  :mPerformanceInterface(performanceInterface)
  {
    mClearId = mPerformanceInterface.AddContext("GL_CLEAR");
  }

  /**
   * @brief Virtual destructor
   */
  virtual ~GlProxyTimedImplementation(){};

  /* OpenGL ES 2.0 API */
  virtual void Clear( GLbitfield mask )
  {
    mPerformanceInterface.AddMarker( PerformanceInterface::START, mClearId );

    GlImplementation::Clear( mask );

    mPerformanceInterface.AddMarker( PerformanceInterface::END, mClearId );
  }

private:

  PerformanceInterface& mPerformanceInterface;
  PerformanceInterface::ContextId mClearId;

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GL_PROXY_TIMED_IMPLEMENTATION_H__
