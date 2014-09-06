#ifndef __DALI_INTERNAL_RENDER_SURFACE_H__
#define __DALI_INTERNAL_RENDER_SURFACE_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/render-surface.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Integration
{

class GlAbstraction;

} // namespace Integration

namespace Internal
{

namespace Adaptor
{

class EglInterface;

/**
 * This is the internal RenderSurface API
 */
class DALI_IMPORT_API RenderSurface : public Dali::RenderSurface
{
public:

  /**
   * Constructor
   */
  RenderSurface();

  /**
   * Destructor
   */
  virtual ~RenderSurface();

public: // API

  /**
   * Initialize EGL, RenderSurface should create egl display and initialize
   * @param egl implementation to use for the creation
   */
  virtual void InitializeEgl( EglInterface& egl ) = 0;

  /**
   * Creates EGL Surface
   * @param egl implementation to use for the creation
   */
  virtual void CreateEglSurface( EglInterface& egl ) = 0;

  /**
   * Replace the EGL Surface
   * @param egl implementation to use for the creation
   * @return true if context was lost
   */
  virtual bool ReplaceEGLSurface( EglInterface& egl ) = 0;

  /**
   * Resizes the underlying surface.
   * Only available for x window
   */
  virtual void MoveResize( Dali::PositionSize positionSize ) = 0;

  /**
   * Get DPI
   * @param dpiHorizontal set to the horizontal dpi
   * @param dpiVertical set to the vertical dpi
   */
  virtual void GetDpi( unsigned int& dpiHorizontal, unsigned int& dpiVertical ) const = 0;

  /**
   * Call to map the surface (only works if surface is a window)
   */
  virtual void Map() = 0;

  /**
   * Transfers the ownership of a display
   * @param newSurface to transfer
   */
  virtual void TransferDisplayOwner( Internal::Adaptor::RenderSurface& newSurface ) = 0;

  /**
   * Consumes any possible events on the queue so that there is no leaking between frames
   */
  virtual void ConsumeEvents() = 0;

  /**
   * Invoked by render thread before Core::Render
   * @param[in] egl The Egl interface
   * @param[in] glAbstraction OpenGLES abstraction interface
   * @return True if the operation is successful
   */
  virtual bool PreRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction ) = 0;

  /**
   * Invoked by render thread after Core::Render
   * @param[in] egl The Egl interface
   * @param[in] glAbstraction OpenGLES abstraction interface
   * @param[in] deltaTime Time (in microseconds) since PostRender was last called.
   * @return true if a vsync is required, otherwise false
   */
  virtual bool PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, unsigned int deltaTime ) = 0;

  /**
   * Invoked by render thread when the thread should be stop
   */
  virtual void StopRender() = 0;

  /**
   * Invoked by the application when the surface is lost.
   */
  virtual void SurfaceLost() = 0;
};

} // namespace Adaptor

} // namespace internal

} // namespace Dali

#endif // __DALI_INTERNAL_RENDER_SURFACE_H__
