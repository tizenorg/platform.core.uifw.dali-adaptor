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

// CLASS HEADER
#include "resource-loader.h"

// EXTERNAL HEADERS
#include <boost/thread.hpp>
#include <iostream>
#include <fstream>
#include <queue>

// INTERNAL HEADERS
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/set-wrapper.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/common/scoped-pointer.h>
#include "resource-requester-base.h"
#include "resource-bitmap-requester.h"
#include "resource-shader-requester.h"
#include "debug/resource-loader-debug.h"
#include "../interfaces/data-cache.h"


/**
 * A macro to expand an argument to a compile time constant string literal.
 * Wrapping the stringify in an outer macro, means that any macro passed as
 * "x" will be expanded before being turned into a string.
 * Use this for example to turn the current line number into a string:
 *   puts("The current line number is " DALI_TO_STRING(__LINE__) ".");
 */
#define DALI_TO_STRING_INNER(x) #x
#define DALI_TO_STRING(x) DALI_TO_STRING_INNER(x)

using namespace Dali::Integration;
using boost::mutex;
using boost::unique_lock;

namespace Dali
{

namespace SlpPlatform
{

namespace
{

#ifdef DEBUG_ENABLED
// For DEBUG_ENABLED profiling of distance field glyph generation
double GetTimeMicroseconds()
{
  timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);
  double seconds = time.tv_sec;
  seconds += 1e-3 * time.tv_nsec;
  return seconds;
}
#endif

} // unnamed namespace


struct ResourceLoader::ResourceLoaderImpl
{
  typedef std::pair<ResourceId, ResourceRequest>  RequestStorePair;
  typedef std::map<ResourceId, ResourceRequest>   RequestStore;
  typedef RequestStore::iterator                  RequestStoreIter;

  typedef std::queue<LoadedResource> LoadedQueue;
  typedef std::queue<SavedResource>  SavedQueue;
  typedef std::queue<FailedResource> FailedQueue;

  typedef std::pair<ResourceTypeId, ResourceRequesterBase*> RequestHandlerPair;
  typedef std::map<ResourceTypeId,  ResourceRequesterBase*> RequestHandlers;
  typedef RequestHandlers::iterator                         RequestHandlersIter;

  boost::mutex mQueueMutex;             ///< used to synchronize access to mLoadedQueue, mSavedQueue and mFailedQueue
  LoadedQueue  mPartiallyLoadedQueue;   ///< Partially complete load requests notifications are stored here until fetched by core
  LoadedQueue  mLoadedQueue;            ///< Completed load requests notifications are stored here until fetched by core
  SavedQueue   mSavedQueue;             ///< Completed save request notifications are stored here until fetched by core
  FailedQueue  mFailedLoads;            ///< Failed load request notifications are stored here until fetched by core
  FailedQueue  mFailedSaves;            ///< Failed save request notifications are stored here until fetched by core

  RequestHandlers mRequestHandlers;
  RequestStore mStoredRequests;         ///< Used to store load requests until loading is completed

  ResourceLoaderImpl( ResourceLoader* loader )
  {
    mRequestHandlers.insert(std::make_pair(ResourceBitmap, new ResourceBitmapRequester(*loader)));
    mRequestHandlers.insert(std::make_pair(ResourceShader, new ResourceShaderRequester(*loader)));
  }

  ~ResourceLoaderImpl()
  {
    // Delete resource handlers
    for( RequestHandlersIter it = mRequestHandlers.begin(); it != mRequestHandlers.end(); ++it )
    {
      ResourceRequesterBase* requestBase = it->second;
      delete requestBase;
    }
  }

  void Pause()
  {
    // Pause all the request handlers:
    for( RequestHandlersIter it = mRequestHandlers.begin(), end = mRequestHandlers.end(); it != end;  ++it )
    {
      ResourceRequesterBase * const requester = it->second;
      if( requester )
      {
        requester->Pause();
      }
    }
  }

  void Resume()
  {
    // Wake up all the request handlers:
    for( RequestHandlersIter it = mRequestHandlers.begin(), end = mRequestHandlers.end(); it != end;  ++it )
    {
      ResourceRequesterBase * const requester = it->second;
      if( requester )
      {
        requester->Resume();
      }
    }
  }

  ResourceRequesterBase* GetRequester(ResourceTypeId typeId)
  {
    ResourceRequesterBase* requestHandler = NULL;
    RequestHandlersIter iter = mRequestHandlers.find(typeId);
    if(iter != mRequestHandlers.end())
    {
      requestHandler = iter->second;
    }
    DALI_ASSERT_DEBUG(requestHandler && "All resource types should have a requester defined for them.");
    return requestHandler;
  }

  void LoadResource(const ResourceRequest& request)
  {
    // Store resource request for partial loaders. Will get cleaned up after load complete has finished
    StoreRequest(request);

    ResourceRequesterBase* requester = GetRequester(request.GetType()->id);
    if( requester )
    {
      ResourceRequest* storedRequest = GetRequest(request.GetId());
      if( storedRequest != NULL )
      {
        requester->LoadResource(*storedRequest); // Pass in stored request
      }
    }
    else
    {
      DALI_LOG_ERROR( "Unknown resource type (%u) with path \"%s\" in load request.\n", request.GetType()->id, request.GetPath().c_str() );
      DALI_ASSERT_DEBUG( 0 == "Unknown resource type in load request at " __FILE__ ", line " DALI_TO_STRING(__LINE__) ".\n" );
    }
  }

  void SaveResource(const ResourceRequest& request)
  {
    ResourceRequesterBase* requester = GetRequester( request.GetType()->id );
    if( requester )
    {
      requester->SaveResource( request );
    }
  }

  void CancelLoad(ResourceId id, ResourceTypeId typeId)
  {
    ResourceRequesterBase* requester = GetRequester(typeId);
    if( requester )
    {
      requester->CancelLoad( id, typeId );
    }
    ClearRequest( id );
  }

  LoadStatus LoadFurtherResources( LoadedResource partialResource )
  {
    LoadStatus loadStatus = RESOURCE_LOADING;
    RequestStoreIter iter = mStoredRequests.find(partialResource.id);

    if( mStoredRequests.end() != iter ) // else cancelled. Ignore response
    {
      ResourceRequest& request = iter->second;
      ResourceRequesterBase* requester = GetRequester(request.GetType()->id);
      if( requester )
      {
        loadStatus = requester->LoadFurtherResources( request, partialResource );
      }

      DALI_LOG_INFO(gLoaderFilter, Debug::General, "ResourceLoader::LoadFurtherResources( ID:%u complete: %s)\n",  request.GetId(), loadStatus==RESOURCE_LOADING?"Loading":loadStatus==RESOURCE_PARTIALLY_LOADED?"PARTIAL":"COMPLETE" );
    }

    if( loadStatus == RESOURCE_COMPLETELY_LOADED )
    {
      ClearRequest( partialResource.id );
    }

    return loadStatus;
  }

  bool IsLoading()
  {
    // TODO - not used - remove?
    DALI_ASSERT_DEBUG( 0 == "IsLoading() Is not implemented so don't call it." );
    return true;
  }

  void GetResources(ResourceCache& cache)
  {
    // Fill the resource cache

    unique_lock<mutex> lock(mQueueMutex);

    // iterate through the partially loaded resources
    while (!mPartiallyLoadedQueue.empty())
    {
      LoadedResource loaded( mPartiallyLoadedQueue.front() );
      mPartiallyLoadedQueue.pop();
      LoadStatus loadStatus = LoadFurtherResources( loaded );
      cache.LoadResponse( loaded.id, loaded.type, loaded.resource, loadStatus );
    }

    // iterate through the successfully loaded resources
    while (!mLoadedQueue.empty())
    {
      LoadedResource loaded( mLoadedQueue.front() );
      mLoadedQueue.pop();
      ClearRequest( loaded.id );
      cache.LoadResponse( loaded.id, loaded.type, loaded.resource, RESOURCE_COMPLETELY_LOADED );
    }

    // iterate through the successfully saved resources
    while (!mSavedQueue.empty())
    {
      SavedResource saved(mSavedQueue.front());
      mSavedQueue.pop();
      cache.SaveComplete(saved.id, saved.type);
    }

    // iterate through the resources which failed to load
    while (!mFailedLoads.empty())
    {
      FailedResource failed(mFailedLoads.front());
      mFailedLoads.pop();
      ClearRequest(failed.id);
      cache.LoadFailed(failed.id, failed.failureType);
    }

    // iterate through the resources which failed to save
    while (!mFailedSaves.empty())
    {
      FailedResource failed(mFailedSaves.front());
      mFailedSaves.pop();
      cache.SaveFailed(failed.id, failed.failureType);
    }
  }

  void AddPartiallyLoadedResource( LoadedResource& resource)
  {
    // Lock the LoadedQueue to store the loaded resource
    unique_lock<mutex> lock(mQueueMutex);

    mPartiallyLoadedQueue.push( resource );
  }

  void AddLoadedResource(LoadedResource& resource)
  {
    // Lock the LoadedQueue to store the loaded resource
    unique_lock<mutex> lock(mQueueMutex);

    mLoadedQueue.push( resource );
  }

  void AddSavedResource(SavedResource& resource)
  {
    // Lock the SavedQueue to store the loaded resource
    unique_lock<mutex> lock(mQueueMutex);

    mSavedQueue.push(resource);
  }

  void AddFailedLoad(FailedResource& resource)
  {
    // Lock the FailedQueue to store the failed resource information
    unique_lock<mutex> lock(mQueueMutex);

    mFailedLoads.push(resource);
  }

  void AddFailedSave(FailedResource& resource)
  {
    // Lock the FailedQueue to store the failed resource information
    unique_lock<mutex> lock(mQueueMutex);

    mFailedSaves.push(resource);
  }

  void StoreRequest( const ResourceRequest& request )
  {
    DALI_LOG_INFO(gLoaderFilter, Debug::Verbose, "ResourceLoader: StoreRequest(id:%u)\n", request.GetId());
    mStoredRequests.insert( RequestStorePair( request.GetId(), request ) ); // copy request as value type
  }

  ResourceRequest* GetRequest( ResourceId id )
  {
    ResourceRequest* found(NULL);
    DALI_LOG_INFO(gLoaderFilter, Debug::Verbose, "ResourceLoader: GetRequest(id:%u)\n", id);
    RequestStoreIter iter = mStoredRequests.find( id );
    if( mStoredRequests.end() != iter )
    {
      found = &iter->second;
    }
    return found;
  }

  void ClearRequest( ResourceId resourceId )
  {
    DALI_LOG_INFO(gLoaderFilter, Debug::Verbose, "ResourceLoader: ClearRequest(id:%u)\n", resourceId);
    RequestStoreIter iter = mStoredRequests.find( resourceId );
    if( mStoredRequests.end() != iter ) // Can't assert here - cancel load may cross with load failed
    {
      mStoredRequests.erase( iter );
    }
  }
};

/********************************************************************************/
/****************************   RESOURCE LOADER METHODS  ************************/
/********************************************************************************/
ResourceLoader::ResourceLoader()
: mTerminateThread(0)
{
  mImpl = new ResourceLoaderImpl( this );
}

ResourceLoader::~ResourceLoader()
{
  // Flag that the ResourceLoader is exiting
  (void)__sync_or_and_fetch( &mTerminateThread, -1 );

  delete mImpl;
}

void ResourceLoader::Pause()
{
  mImpl->Pause();
}

void ResourceLoader::Resume()
{
  mImpl->Resume();
}

bool ResourceLoader::IsTerminating()
{
  return __sync_fetch_and_or( &mTerminateThread, 0 );
}

void ResourceLoader::GetResources(ResourceCache& cache)
{
  mImpl->GetResources( cache );
}

/********************************************************************************/
/**************************   CALLED FROM LOADER THREADS   **********************/
/********************************************************************************/

void ResourceLoader::AddPartiallyLoadedResource( LoadedResource& resource)
{
  mImpl->AddPartiallyLoadedResource( resource );
}

void ResourceLoader::AddLoadedResource(LoadedResource& resource)
{
  mImpl->AddLoadedResource( resource );
}

void ResourceLoader::AddSavedResource(SavedResource& resource)
{
  mImpl->AddSavedResource( resource );
}

void ResourceLoader::AddFailedLoad(FailedResource& resource)
{
  mImpl->AddFailedLoad( resource );
}

void ResourceLoader::AddFailedSave(FailedResource& resource)
{
  mImpl->AddFailedSave( resource );
}

/********************************************************************************/
/*********************   CALLED FROM PLATFORM ABSTRACTION  **********************/
/********************************************************************************/

void ResourceLoader::LoadResource(const ResourceRequest& request)
{
  mImpl->LoadResource(request);
}

void ResourceLoader::SaveResource(const ResourceRequest& request)
{
  mImpl->SaveResource(request);
}

void ResourceLoader::CancelLoad(ResourceId id, ResourceTypeId typeId)
{
  mImpl->CancelLoad(id, typeId);
}

bool ResourceLoader::IsLoading()
{
  return mImpl->IsLoading();
}

void ResourceLoader::SetDpi(unsigned int dpiHor, unsigned int dpiVer)
{
  // Unused
}

bool ResourceLoader::LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const
{
  DALI_LOG_TRACE_METHOD(gLoaderFilter);

  DALI_ASSERT_DEBUG( 0 != filename.length());

  bool result;

  std::filebuf buf;
  buf.open(filename.c_str(), std::ios::in | std::ios::binary);
  if( buf.is_open() )
  {
    std::istream stream(&buf);

    // determine data length
    stream.seekg(0, std::ios_base::end);
    unsigned int length = static_cast<unsigned int>( stream.tellg() );
    stream.seekg(0, std::ios_base::beg);

    // allocate a buffer
    buffer.resize(length);
    // read data into buffer
    stream.read(reinterpret_cast<char*>(buffer.data()), length);

    DALI_LOG_INFO(gLoaderFilter, Debug::Verbose, "ResourceLoader::LoadFile(%s) - loaded %d bytes\n", filename.c_str(), length);

    result = true;
  }
  else
  {
    DALI_LOG_INFO(gLoaderFilter, Debug::Verbose, "ResourceLoader::LoadFile(%s) - failed to load\n", filename.c_str());
    result = false;
  }

  return result;
}

std::string ResourceLoader::LoadFile(const std::string& filename) const
{
  DALI_LOG_TRACE_METHOD(gLoaderFilter);

  DALI_ASSERT_DEBUG( 0 != filename.length());

  std::string contents;

  std::filebuf buf;
  buf.open(filename.c_str(), std::ios::in);
  if( buf.is_open() )
  {
    std::istream stream(&buf);

    // determine data length
    stream.seekg(0, std::ios_base::end);
    unsigned int length = static_cast<unsigned int>( stream.tellg() );
    stream.seekg(0, std::ios_base::beg);

    // allocate a buffer
    contents.resize(length);
    // read data into buffer
    stream.read(&contents[0], length);

    DALI_LOG_INFO(gLoaderFilter, Debug::Verbose, "ResourceLoader::LoadFile(%s) - loaded %d bytes\n", filename.c_str(), length);
  }
  else
  {
    DALI_LOG_INFO(gLoaderFilter, Debug::Verbose, "ResourceLoader::LoadFile(%s) - failed to load\n", filename.c_str());
  }

  return contents;
}

bool ResourceLoader::SaveFile(const std::string& filename, std::vector< unsigned char >& buffer)
{
  DALI_LOG_TRACE_METHOD(gLoaderFilter);

  DALI_ASSERT_DEBUG( 0 != filename.length());

  bool result = false;

  std::filebuf buf;
  buf.open(filename.c_str(), std::ios::out | std::ios_base::trunc | std::ios::binary);
  if( buf.is_open() )
  {
    std::ostream stream(&buf);

    // determine size of buffer
    int length = static_cast<int>(buffer.size());

    // write contents of buffer to the file
    stream.write(reinterpret_cast<char*>(buffer.data()), length);

    if( !stream.bad() )
    {
      DALI_LOG_INFO(gLoaderFilter, Debug::Verbose, "ResourceLoader::SaveFile(%s) - wrote %d bytes\n", filename.c_str(), length);
      result = true;
    }
  }

#if defined(DEBUG_BUILD)
  if( !result )
  {
    DALI_LOG_INFO(gLoaderFilter, Debug::Verbose, "ResourceLoader::SaveFile(%s) - failed to load\n", filename.c_str());
  }
#endif

  return result;
}

} // namespace SlpPlatform

} // namespace Dali
