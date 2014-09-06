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
#include "surface-image-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h> // NextPowerOfTwo
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/render-surface.h>
#include <internal/application-impl.h>
#include <internal/common/adaptor-impl.h>

#include <GLES2/gl2.h>
#include <EGL/eglext.h>
#include <android/log.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{


SurfaceImage* SurfaceImage::New(unsigned int width, unsigned int height, Dali::SurfaceImage::ColorDepth depth, Dali::Application application,  ISurfaceImageRenderer* renderer )
{
  Application& internalApp = Dali::Internal::Adaptor::GetImplementation(application);
  SurfaceImage* image = new SurfaceImage(width, height, depth, internalApp.GetAdaptor(), renderer);

  // 2nd phase construction
  image->Initialize();

  return image;
}

SurfaceImage* SurfaceImage::New(unsigned int width, unsigned int height, Dali::SurfaceImage::ColorDepth depth, Dali::Adaptor& adaptor,  ISurfaceImageRenderer* renderer )
{
  SurfaceImage* image = new SurfaceImage(width, height, depth, adaptor, renderer);

  // 2nd phase construction
  image->Initialize();

  return image;
}

SurfaceImage::SurfaceImage(unsigned int width, unsigned int height, Dali::SurfaceImage::ColorDepth depth, Dali::Adaptor& adaptor, ISurfaceImageRenderer* renderer)
: mWidth(width),
  mHeight(height),
  mPixelFormat(Pixel::RGB888),
  mColorDepth(depth),
  mRenderer(renderer),
  mSurfaceTextureCreated(false),
  mAdaptor(Internal::Adaptor::Adaptor::GetImplementation(adaptor))
{
}

void SurfaceImage::Initialize()
{
  // Get render-surface being used by Dali
  Dali::RenderSurface& surface = mAdaptor.GetSurface();

  // get the X11 display pointer and store it
  // it is used by eglCreateImageKHR, and XFreePixmap
  // Any other display (x-connection) will fail in eglCreateImageKHR
  //boost::any display = surface.GetDisplay();

  // get the pixel depth
  int depth = GetPixelDepth(mColorDepth);

  // set the pixel format
  SetPixelFormat(depth);

  // Get the X-Renderable for which the pixmap is created on
  //boost::any renderableSurface =  surface.GetSurface();

  // if Dali is using a Pixmap or Window to render to it doesn't matter because they have the same
  // underlying type of unsigned long
  //ANativeWindow* daliWindow = boost::any_cast< ANativeWindow* >(renderableSurface);
  // mPixmap = ecore_x_pixmap_new(daliWindow, mWidth, mHeight, depth);
}

SurfaceImage::~SurfaceImage()
{
  if ( NULL != mRenderer )
  {
    mRenderer->destroySurfaceTexture();
  }
  delete mRenderer;
  mRenderer = NULL;

  GlExtensionDestroy();
}

bool SurfaceImage::GlExtensionCreate()
{
  return true;
}

void SurfaceImage::GlExtensionDestroy()
{
}

unsigned int SurfaceImage::TargetTexture()
{
  if ( NULL != mRenderer )
  {
    mRenderer->render();
    mRenderer->updateTexture();
  }

  return 0;
}

void SurfaceImage::PrepareTexture()
{
  if ( NULL != mRenderer )
  {
    mRenderer->render();
    mRenderer->updateTexture();
  }
}

void SurfaceImage::DoCleanUpOnContextDestroyed()
{
  // Avoid destruction race condition
  if ( NULL != mRenderer )
  {
    mRenderer->destroySurfaceTexture();
  }
  mSurfaceTextureCreated = false;
}

void SurfaceImage::TextureCreated(unsigned textureId)
{
  if (!mRenderer) // ClearRenderer can be called before this
    return;

  if (mSurfaceTextureCreated)
  {
    mRenderer->destroySurfaceTexture();
  }

  mRenderer->createSurfaceTexture(mWidth, mHeight, textureId);
  mSurfaceTextureCreated = true;
}

int SurfaceImage::GetPixelDepth(Dali::SurfaceImage::ColorDepth depth) const
{
  switch (depth)
  {
    case Dali::SurfaceImage::COLOR_DEPTH_8: return 8;
    case Dali::SurfaceImage::COLOR_DEPTH_16: return 16;
    case Dali::SurfaceImage::COLOR_DEPTH_24: return 24;
    case Dali::SurfaceImage::COLOR_DEPTH_DEFAULT:
    case Dali::SurfaceImage::COLOR_DEPTH_32: return 32;
    default: DALI_ASSERT_DEBUG(0 && "unknown color enum"); return 0;
  }
}

void SurfaceImage::SetPixelFormat(int depth)
{
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

void SurfaceImage::ClearRenderer()
{
  mRenderer = NULL;
}

void SurfaceImage::RequestUpdate()
{
  mAdaptor.RequestUpdateOnce();
}

} // namespace Adaptor

} // namespace internal

} // namespace Dali
