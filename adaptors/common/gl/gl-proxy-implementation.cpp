//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include "gl-proxy-implementation.h"

// EXTERNAL INCLUDES
#include <math.h>

// INTERNAL INCLUDES
#include <base/environment-options.h>
#include <dali/integration-api/debug.h>

namespace
{
const int NUM_FRAMES_PER_SECOND(60);
}


namespace Dali
{
namespace Internal
{
namespace Adaptor
{

Sampler::Sampler( const char* description )
: mDescription( description ),
  mAccumulated(0.0f),
  mAccumulatedSquare(0.0f),
  mMin(0.0f),
  mMax(0.0f),
  mNumSamples(0),
  mCurrentFrameCount(0)
{
}

void Sampler::Increment()
{
  mCurrentFrameCount++;
}

void Sampler::Reset()
{
  mAccumulated = 0.0f;
  mAccumulatedSquare= 0.0f;
  mMin = 0.0f;
  mMax = 0.0f;
  mNumSamples = 0;
  mCurrentFrameCount = 0;
}

void Sampler::Accumulate()
{
  if( mNumSamples == 0 )
  {
    mMin = mCurrentFrameCount;
    mMax = mCurrentFrameCount;
  }
  else
  {
    if(mCurrentFrameCount < mMin)
    {
      mMin = mCurrentFrameCount;
    }
    if(mCurrentFrameCount > mMax)
    {
      mMax = mCurrentFrameCount;
    }
  }

  mNumSamples++;

  mAccumulated += mCurrentFrameCount;
  mAccumulatedSquare += (mCurrentFrameCount * mCurrentFrameCount);
  mCurrentFrameCount = 0;
}
const char* Sampler::GetDescription() const
{
  return mDescription;
}

float Sampler::GetMeanValue() const
{
  float meanValue = 0;
  if( mNumSamples > 0 )
  {
    meanValue = mAccumulated / (float)mNumSamples;
  }
  return meanValue;
}

float Sampler::GetStandardDeviation() const
{
  float standardDeviation=0.0f;
  if( mNumSamples > 0 )
  {
    standardDeviation = sqrtf( mNumSamples * mAccumulatedSquare - (mAccumulated*mAccumulated)) / mNumSamples;
  }
  return standardDeviation;
}

float Sampler::GetMin() const
{
  return mMin;
}

float Sampler::GetMax() const
{
  return mMax;
}

GlProxyImplementation::GlProxyImplementation(EnvironmentOptions& environmentOptions)
: mEnvironmentOptions(environmentOptions),
  mClearSampler("Clear calls"),
  mBindBufferSampler( "Bind buffers"),
  mBindTextureSampler( "Bind textures"),
  mDrawSampler("Draw calls"),
  mUniformSampler("Uniform sets"),
  mUseProgramSampler("Used programs"),
  mFrameCount(0)
{
}

GlProxyImplementation::~GlProxyImplementation()
{
}

void GlProxyImplementation::PreRender()
{
}

void GlProxyImplementation::PostRender( unsigned int timeDelta )
{
  // Accumulate counts in each sampler
  AccumulateSamples();

  // When we reach the desired frame count, output the averages from the samples
  mFrameCount++;
  if( mFrameCount >= mEnvironmentOptions.GetGlesCallTime() * NUM_FRAMES_PER_SECOND )
  {
    LogResults();
    ResetSamplers();
  }
}

void GlProxyImplementation::Clear( GLbitfield mask )
{
  mClearSampler.Increment();
  GlImplementation::Clear(mask);
}

void GlProxyImplementation::BindBuffer( GLenum target, GLuint buffer )
{
  mBindBufferSampler.Increment();
  GlImplementation::BindBuffer(target,buffer);
}

void GlProxyImplementation::BindTexture( GLenum target, GLuint texture )
{
  mBindTextureSampler.Increment();
  GlImplementation::BindTexture(target,texture);
}

void GlProxyImplementation::DrawArrays( GLenum mode, GLint first, GLsizei count )
{
  mDrawSampler.Increment();
  GlImplementation::DrawArrays(mode,first,count);
}

void GlProxyImplementation::DrawElements( GLenum mode, GLsizei count, GLenum type, const void* indices )
{
  mDrawSampler.Increment();
  GlImplementation::DrawElements(mode,count,type,indices);
}

void GlProxyImplementation::Uniform1f( GLint location, GLfloat x )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform1f(location,x);
}

void GlProxyImplementation::Uniform1fv( GLint location, GLsizei count, const GLfloat* v )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform1fv(location,count,v);
}

void GlProxyImplementation::Uniform1i( GLint location, GLint x )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform1i(location,x);
}

void GlProxyImplementation::Uniform1iv( GLint location, GLsizei count, const GLint* v )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform1iv(location,count,v);
}

void GlProxyImplementation::Uniform2f( GLint location, GLfloat x, GLfloat y)
{
  mUniformSampler.Increment();
  GlImplementation::Uniform2f(location,x,y);
}

void GlProxyImplementation::Uniform2fv( GLint location, GLsizei count, const GLfloat* v )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform2fv(location,count,v);
}

void GlProxyImplementation::Uniform2i( GLint location, GLint x, GLint y )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform2i(location,x,y);
}

void GlProxyImplementation::Uniform2iv( GLint location, GLsizei count, const GLint* v )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform2iv(location,count,v);
}

void GlProxyImplementation::Uniform3f( GLint location, GLfloat x, GLfloat y, GLfloat z)
{
  mUniformSampler.Increment();
  GlImplementation::Uniform3f(location,x,y,z);
}

void GlProxyImplementation::Uniform3fv( GLint location, GLsizei count, const GLfloat* v )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform3fv(location,count,v);
}

void GlProxyImplementation::Uniform3i( GLint location, GLint x, GLint y, GLint z )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform3i(location,x,y,z);
}

void GlProxyImplementation::Uniform3iv( GLint location, GLsizei count, const GLint* v )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform3iv(location,count,v);
}

void GlProxyImplementation::Uniform4f( GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform4f(location,x,y,z,w);
}

void GlProxyImplementation::Uniform4fv( GLint location, GLsizei count, const GLfloat* v )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform4fv(location,count,v);
}

void GlProxyImplementation::Uniform4i( GLint location, GLint x, GLint y, GLint z, GLint w )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform4i(location,x,y,z,w);
}

void GlProxyImplementation::Uniform4iv( GLint location, GLsizei count, const GLint* v )
{
  mUniformSampler.Increment();
  GlImplementation::Uniform4iv(location,count,v);
}

void GlProxyImplementation::UniformMatrix2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
  mUniformSampler.Increment();
  GlImplementation::UniformMatrix2fv(location,count,transpose,value);
}

void GlProxyImplementation::UniformMatrix3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
  mUniformSampler.Increment();
  GlImplementation::UniformMatrix3fv(location,count,transpose,value);
}

void GlProxyImplementation::UniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
  mUniformSampler.Increment();
  GlImplementation::UniformMatrix4fv(location,count,transpose,value);
}

void GlProxyImplementation::UseProgram( GLuint program )
{
  mUseProgramSampler.Increment();
  GlImplementation::UseProgram(program);
}

void GlProxyImplementation::AccumulateSamples()
{
  // Accumulate counts in each sampler
  mClearSampler.Accumulate();
  mBindBufferSampler.Accumulate();
  mBindTextureSampler.Accumulate();
  mDrawSampler.Accumulate();
  mUniformSampler.Accumulate();
  mUseProgramSampler.Accumulate();
}

void GlProxyImplementation::LogResults()
{
  Debug::LogMessage( Debug::DebugInfo, "OpenGL ES statistics sampled over %d frames) operations per frame:\n", mFrameCount );
  LogCalls( mClearSampler );
  LogCalls( mBindBufferSampler );
  LogCalls( mBindTextureSampler );
  LogCalls( mDrawSampler );
  LogCalls( mUniformSampler );
  LogCalls( mUseProgramSampler );
}

void GlProxyImplementation::LogCalls( const Sampler& sampler )
{
  Debug::LogMessage( Debug::DebugInfo, "  %s : Mean %5.2f  (Min:%5.2f, Max:%5.2f, StdDev:%5.2f)\n",
                     sampler.GetDescription(),
                     sampler.GetMeanValue(), sampler.GetMin(), sampler.GetMax(),
                     sampler.GetStandardDeviation() );
}

void GlProxyImplementation::ResetSamplers()
{
  mClearSampler.Reset();
  mBindBufferSampler.Reset();
  mBindTextureSampler.Reset();
  mDrawSampler.Reset();
  mUniformSampler.Reset();
  mUseProgramSampler.Reset();
  mFrameCount = 0;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
