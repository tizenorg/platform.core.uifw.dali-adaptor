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
#include <xf86drm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/thread.hpp>

#include <dali/integration-api/core.h>
#include <dali/integration-api/platform-abstraction.h>

// INTERNAL INCLUDES
#include "vsync-notifier.h"
#include <base/interfaces/adaptor-internal-services.h>
#include <base/update-render-synchronization.h>
#include <base/environment-options.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

const unsigned int MICROSECONDS_PER_SECOND( 100000u );
const unsigned int TIME_PER_FRAME_IN_MICROSECONDS( 16667u );

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter = Integration::Log::Filter::New(Debug::Concise, false, "LOG_VSYNC_NOTIFIER");
#endif

} // unnamed namespace

VSyncNotifier::VSyncNotifier( UpdateRenderSynchronization& sync,
                              AdaptorInternalServices& adaptorInterfaces,
                              const EnvironmentOptions& environmentOptions )
: mUpdateRenderSync( sync ),
  mCore( adaptorInterfaces.GetCore() ),
  mPlatformAbstraction( adaptorInterfaces.GetPlatformAbstractionInterface() ),
  mVSyncMonitor( adaptorInterfaces.GetVSyncMonitorInterface() ),
  mThread( NULL ),
  mEnvironmentOptions( environmentOptions )
{
}

VSyncNotifier::~VSyncNotifier()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  Stop();
}

void VSyncNotifier::Start()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  if ( !mThread )
  {
    mVSyncMonitor->Initialize();

    mThread = new boost::thread( boost::bind( &VSyncNotifier::Run, this ) );
  }
}

void VSyncNotifier::Stop()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  if( mThread )
  {
    // wait for the thread to finish
    mThread->join();

    delete mThread;
    mThread = NULL;
  }

  mVSyncMonitor->Terminate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following is executed inside the notifier thread !!!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VSyncNotifier::Run()
{
  // install a function for logging
  mEnvironmentOptions.InstallLogFunction();

  unsigned int frameNumber( 0u );             // frameCount, updated when the thread is paused
  unsigned int currentSequenceNumber( 0u );   // platform specific vsync sequence number (increments with each vsync)
  unsigned int currentSeconds( 0u );          // timestamp at latest vsync
  unsigned int currentMicroseconds( 0u );     // timestamp at latest vsync
  unsigned int seconds( 0u );
  unsigned int microseconds( 0u );

  bool running( true );
  while( running )
  {
    bool validSync( true );

    // Hardware VSyncs available?
    if( mVSyncMonitor->UseHardware() )
    {
      // Yes..wait for hardware VSync
      validSync = mVSyncMonitor->DoSync( currentSequenceNumber, currentSeconds, currentMicroseconds );
    }
    else
    {
      // No..use software timer
      mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

      unsigned int timeDelta( MICROSECONDS_PER_SECOND * (seconds - currentSeconds) );
      if( microseconds < currentMicroseconds)
      {
        timeDelta += (microseconds + MICROSECONDS_PER_SECOND) - currentMicroseconds;
      }
      else
      {
        timeDelta += microseconds - currentMicroseconds;
      }

      if( timeDelta < TIME_PER_FRAME_IN_MICROSECONDS )
      {
          usleep( TIME_PER_FRAME_IN_MICROSECONDS - timeDelta );
      }
      else
      {
        usleep( TIME_PER_FRAME_IN_MICROSECONDS );
      }
    }

    running = mUpdateRenderSync.VSyncNotifierSyncWithUpdateAndRender( validSync, ++frameNumber, currentSeconds, currentMicroseconds );
  }

  // uninstall a function for logging
  mEnvironmentOptions.UnInstallLogFunction();

}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali


// CLASS HEADER
#include "android-native-image-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h> // NextPowerOfTwo
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/render-surface.h>
#include <internal/application-impl.h>
#include <internal/common/adaptor-impl.h>
#include <graphicbuffer/graphicbufferwrapper.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "nativeLogging.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

AndroidNativeImage* AndroidNativeImage::New(unsigned int width, unsigned int height, Dali::AndroidNativeImage::ColorDepth depth, Dali::Application application)
{
  Application& internalApp = Dali::Internal::Adaptor::GetImplementation(application);
  AndroidNativeImage* image = new AndroidNativeImage(width, height, depth, internalApp.GetAdaptor());

  // 2nd phase construction
  image->Initialize();

  return image;
}

AndroidNativeImage* AndroidNativeImage::New(unsigned int width, unsigned int height, Dali::AndroidNativeImage::ColorDepth depth, Dali::Adaptor& adaptor)
{
  AndroidNativeImage* image = new AndroidNativeImage(width, height, depth, adaptor);

  // 2nd phase construction
  image->Initialize();

  return image;
}

AndroidNativeImage::AndroidNativeImage(unsigned int width, unsigned int height, Dali::AndroidNativeImage::ColorDepth depth, Dali::Adaptor& adaptor)
: mWidth(width),
  mHeight(height),
  mPixelFormat(Pixel::RGBA8888),
  mGraphicBuffer(NULL),
  mEglImage(0),
  mRequestQueue(16),
  mColorDepth(Dali::AndroidNativeImage::COLOR_DEPTH_32),
  mAdaptor(Internal::Adaptor::Adaptor::GetImplementation(adaptor))
{
}

void AndroidNativeImage::Initialize()
{
  mGraphicBuffer = GraphicBufferWrapper::New(mWidth, mHeight);
}

AndroidNativeImage::~AndroidNativeImage()
{
  GlExtensionDestroy();

  if (mGraphicBuffer)
  {
    delete mGraphicBuffer;
    mGraphicBuffer = NULL;
  }

  Request* request = NULL;
  while(mRequestQueue.pop(request))
  {
    delete request;
  }
}

bool AndroidNativeImage::GlExtensionCreate()
{
  return true;
}

void AndroidNativeImage::GlExtensionDestroy()
{
  // TODO: what to do with the eglImage ?
  // eglDestroyImageKHR(eglGetCurrentDisplay(), mEglImage);
  // mEglImage = 0;
}

unsigned int AndroidNativeImage::TargetTexture()
{
  // perform any pending uploads.
  DrainQueue();

  if (mEglImage == 0)
  {
    mEglImage = mGraphicBuffer->CreateEGLImage();
  }

  if (mEglImage)
  {
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, mEglImage);
  }

  return 0;
}

void AndroidNativeImage::PrepareTexture()
{
  if (mEglImage)
  {
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, mEglImage);
  }
}

void AndroidNativeImage::DoCleanUpOnContextDestroyed()
{
}

void AndroidNativeImage::TextureCreated(unsigned textureId)
{
  if (mEglImage)
  {
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, mEglImage);
  }
}

int AndroidNativeImage::GetPixelDepth(Dali::AndroidNativeImage::ColorDepth depth) const
{
  switch (depth)
  {
    case Dali::AndroidNativeImage::COLOR_DEPTH_8: return 8;
    case Dali::AndroidNativeImage::COLOR_DEPTH_16: return 16;
    case Dali::AndroidNativeImage::COLOR_DEPTH_24: return 24;
    case Dali::AndroidNativeImage::COLOR_DEPTH_DEFAULT:
    case Dali::AndroidNativeImage::COLOR_DEPTH_32: return 32;
    default: DALI_ASSERT_DEBUG(0 && "unknown color enum"); return 0;
  }
}

void AndroidNativeImage::DrainQueue()
{
  if (mGraphicBuffer)
  {
    Request* request = NULL;
    while(mRequestQueue.pop(request))
    {
      GraphicBufferMemMap mm(mGraphicBuffer);
      if (mm.isValid())
      {
        uint32_t* target = (uint32_t*)mm.GetAddress();
        Android::ImageDataHandle data = request->mImageData;

        if (data->empty())
        {
          LOGI("Empty data in upload request !\n");
        }

        // This is a line-by-line copy as the two buffers have different strides
        for(int row = 0; row < data->GetHeight(); ++row)
        {
          uint8_t* sourceAddr = data->GetBuffer() + (row * data->GetStride());
          // GraphicBuffer stride is not in bytes!
          uint32_t* destAddr = (uint32_t*)target + (row  * mm.GetStride());
          memcpy(destAddr, sourceAddr, data->GetStride());
        }

        delete request;
      }
      else
      {
        // Keep request if the lock has failed.
        mRequestQueue.push(request);
      }
    }
  }
  else
  {
    LOGI("GraphicBuffer does not exist, storing request for later.\n");
  }
}

void AndroidNativeImage::UploadImage(Android::ImageDataHandle imageData)
{
  Request* request = new Request(imageData);
  mRequestQueue.push(request);
}

void AndroidNativeImage::RequestUpdate()
{
  mAdaptor.RequestUpdateOnce();
}

void AndroidNativeImage::SetPixelFormat(int depth)
{
  return;

  // store the pixel format based on the depth
  switch (depth)
  {
    case 8:
      mPixelFormat = Pixel::A8;
      break;

    case 16:
      mPixelFormat = Pixel::RGB565;
      break;

    case 32:
      mPixelFormat = Pixel::RGBA8888;
      break;

    case 24:
    default:
      mPixelFormat = Pixel::RGB888;
      break;
  }
}

} // namespace Adaptor

} // namespace internal

} // namespace Dali
