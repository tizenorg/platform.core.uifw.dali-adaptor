#ifndef __DALI_INTERNAL_ANDROID_NATIVE_IMAGE_H__
#define __DALI_INTERNAL_ANDROID_NATIVE_IMAGE_H__

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


// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/android-native-image.h>
#include <dali/public-api/adaptor-framework/common/adaptor.h>
#include <public-api/adaptor-framework/application.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

class GraphicBufferWrapper;

namespace Dali
{

namespace Internal
{
  class EglImageExtensions;

namespace Adaptor
{


/**
 * Dali internal AndroidNativeImage.
 */
class AndroidNativeImage
{
public:

  /**
   * Create a new AndroidNativeImage internally.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] application reference to dali application
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   * @return A smart-pointer to a newly allocated image.
   */
  static AndroidNativeImage* New( unsigned int width,
                                  unsigned int height,
                                  Dali::AndroidNativeImage::ColorDepth depth,
                                  Dali::Application applications);

  /**
   * Create a new AndroidNativeImage internally.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] adaptor reference to dali adaptor
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   * @return A smart-pointer to a newly allocated image.
   */
  static AndroidNativeImage* New(unsigned int width,
                                 unsigned int height,
                                 Dali::AndroidNativeImage::ColorDepth depth,
                                 Dali::Adaptor& adaptor);

  /**
   * destructor
   */
  ~AndroidNativeImage();

  /**
   * @copydoc Dali::NativeImage::GlExtensionCreate()
   */
  bool GlExtensionCreate();

  /**
   * @copydoc Dali::NativeImage::GlExtensionDestroy()
   */
  void GlExtensionDestroy();

  /**
   * @copydoc Dali::NativeImage::TargetTexture()
   */
  unsigned int TargetTexture();

   /**
   * @copydoc Dali::NativeImage::TextureCreated()
   */
  void TextureCreated(unsigned textureId);

  /**
   * @copydoc Dali::NativeImage::PrepareTexture()
   */
  void PrepareTexture();

  /**
   * @copydoc Dali::NativeImage::GetWidth()
   */
  unsigned int GetWidth() const
  {
    return mWidth;
  }

  /**
   * @copydoc Dali::NativeImage::GetHeight()
   */
  unsigned int GetHeight() const
  {
    return mHeight;
  }

  /**
   * @return Pixel format
   */
  Pixel::Format GetPixelFormat() const
  {
    return mPixelFormat;
  }

  void UploadImage(Android::ImageDataHandle imageData);

  /**
   * @copydoc Dali::NativeImage::DoCleanUpOnContextDestroyed()
   */
  void DoCleanUpOnContextDestroyed();


  void RequestUpdate();

private:

  /**
   * Private constructor; @see AndroidNativeImage::New()
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] colour depth of the pixmap
   * @param[in] adaptor a reference to Dali adaptor
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   */
  AndroidNativeImage( unsigned int width,
                      unsigned int height,
                      Dali::AndroidNativeImage::ColorDepth depth,
                      Dali::Adaptor &adaptor);


  /**
   * 2nd phase construction.
   */
  void Initialize();

  /**
   * Uses X11 to get the default depth.
   * @param depth the PixelImage depth enum
   * @return default x11 pixel depth
   */
  int GetPixelDepth(Dali::AndroidNativeImage::ColorDepth depth) const;

  /**
   * Sets the pixel format based on the bit depth
   * @param depth depth in bytes
   */
  void SetPixelFormat(int depth);

  /**
   * Drains the stored request queue of bitmap uploads.
   */
  void DrainQueue();

private:

  struct Request
  {
    Android::ImageDataHandle mImageData;

    Request(Android::ImageDataHandle imageData) : mImageData(imageData)
    {     
    }
  };

  unsigned int mWidth;                                ///< pixmap width
  unsigned int mHeight;                               ///< pixmap heights
  Pixel::Format mPixelFormat;                         ///< pixmap pixel format
  Dali::AndroidNativeImage::ColorDepth mColorDepth;   ///< color depth of pixmap
  Adaptor& mAdaptor;                                  ///< adaptor
  GraphicBufferWrapper* mGraphicBuffer;               ///< GraphicBuffer's wrapper
  EGLImageKHR mEglImage;                              ///< eglImage

  boost::lockfree::queue<Request*> mRequestQueue;   

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PIXMAP_IMAGE_H__
