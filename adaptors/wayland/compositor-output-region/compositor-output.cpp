/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "compositor-output.h"



namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

const float MILLIMETRE_TO_INCH = 0.254;

void OutputGeometryCallback( void *data,
     struct wl_output *wl_output,
     int32_t x,
     int32_t y,
     int32_t physical_width,
     int32_t physical_height,
     int32_t subpixel,
     const char *make,
     const char *model,
     int32_t transform)
{
  CompositorOutput* output = static_cast< CompositorOutput* >( data );
  output->SetMonitorDimensions( physical_width, physical_height );
  printf("Monitor geometry %d %d %d %d \n", x,y, physical_width, physical_height);
}

void OutputModeCallback( void *data,
       struct wl_output *wl_output,
       uint32_t flags,
       int32_t width,
       int32_t height,
       int32_t refresh )
{

  if (flags & WL_OUTPUT_MODE_CURRENT)
  {
    CompositorOutput* output = static_cast< CompositorOutput* >( data );
    output->SetMonitorResolution( width, height );
    printf(" monitor refresh %d, width %d height %d \n", refresh, width, height );

  }
}

void  OutputCallbackDone(void *data,
       struct wl_output *wl_output)
{
  CompositorOutput* output = static_cast< CompositorOutput* >( data );
  output->CallbacksDone();
  printf("OutputCallbackDone \n");
}

void OutputCallbackScale(void *data,
        struct wl_output *wl_output,
        int32_t factor)
{
  printf("output scale\n");
}



//output typically corresponds to a monitor
const struct wl_output_listener OutputListener =
{
   OutputGeometryCallback,
   OutputModeCallback,
   OutputCallbackDone,
   OutputCallbackScale
};
}
CompositorOutput::CompositorOutput()
:mOutput( NULL ),
 mDpiHorizontal( 0 ),
 mDpiVertical( 0 ),
 mXresolution( 0 ),
 mYresolution( 0 ),
 mMonitorWidth( 0.f ),
 mMonitorHeight( 0.f ),
 mDataReady( false )
{

}

CompositorOutput::~CompositorOutput()
{
  if( mOutput )
  {
    wl_output_destroy( mOutput );
  }
}

void CompositorOutput::AddListener( WlOutput* outputInterface )
{
  mOutput = outputInterface;

  wl_output_add_listener( mOutput , &OutputListener, this);
}

void CompositorOutput::GetDpi( unsigned int& dpiHorizontal, unsigned int& dpiVertical ) const
{
  dpiHorizontal = mDpiHorizontal;
  dpiVertical = mDpiVertical;
}

bool CompositorOutput::DataReady() const
{
  return mDataReady;
}

void CompositorOutput::SetMonitorDimensions( unsigned int width, unsigned int height )
{
  mMonitorHeight = static_cast<float> ( height ) * MILLIMETRE_TO_INCH;
  mMonitorWidth = static_cast<float>( width ) * MILLIMETRE_TO_INCH;
}

void CompositorOutput::SetMonitorResolution( unsigned int width, unsigned int height )
{
  mXresolution =width;
  mYresolution = height;
}

void CompositorOutput::CallbacksDone()
{
  mDataReady = true;

  // formula for DPI is screen resolution / physical size in inches
  mDpiHorizontal =  static_cast<float>( mXresolution) / mMonitorWidth;
  mDpiVertical =  static_cast<float>( mYresolution )/ mMonitorHeight;

  printf(" resolution = %f,%f, monitor widht in inches = %f %f",mXresolution ,mYresolution ,mMonitorWidth, mMonitorHeight );

  printf("mDpiHorizontal = %d , mDpiVertical %d \n ", mDpiHorizontal, mDpiVertical);
}




} // Internal
} // Adaptor
} // Dali
