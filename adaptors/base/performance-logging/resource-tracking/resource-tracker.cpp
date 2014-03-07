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
#include "resource-tracker.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

void ResourceLogger()
{

  // Ensure that we only process one message per call ( TODO: add a frequency for updates of method )
  bool foundMessage = false;

  // Iterate through the thread objects and output their messages to DALI_LOG
  ResourceTrackingMessage message;
  for ( ResourceTrackingManager::ResourceTrackerConstIter TrackIter = gResourceTrackingManager->GetStartTrackerList(); TrackIter != gResourceTrackingManager->GetEndTrackerList(); ++TrackIter )
  {

    // Read the tracker for this thread
    ResourceTracking* tracker = *TrackIter;
    unsigned int thread = tracker->GetThread();

    // Read any pending message
    message = tracker->ReadNextMessage();
    foundMessage = true;

    // Process any message found
    switch( message.GetMessage() )
    {
      case ResourceTrackMessage::LOAD_BITMAP:
      {
        // Check to see if there is a hashed filename in the message?
        if ( message.GetParameter( 0 ) )
        {
          // Add the size of the file to the CPU running total
          gResourceTrackingManager->AddToCPUTracked( message.GetParameter( 3 ) );

          // Reverse lookup the hashed string from this thread's tracker
          std::string fileName = tracker->ReadString( message.GetParameter( 0 ) );

          // Strip off any preceeding folders
          unsigned int found = fileName.find_last_of('/') + 1;
          fileName = fileName.substr( found );

          // Write to log, unpacking the width and height from message
          LOG( LOG_INFO, "DALI", "[Resource Tracker] ThreadID: %i, TextureLoad: '%s', size: %ix%i, ResourceID: %i\n",
               thread,
               fileName.c_str(),
               message.GetParameter( 1 ) >> 16,
               message.GetParameter( 1 ) & 0xFFFF,
               message.GetParameter( 2 ) );

          // Associate the resourceId with the key ptr
          gResourceTrackingManager->InsertCPUResource( message.GetPointer(), message.GetParameter( 2 ) );
        }
        else
        {
          LOG( LOG_INFO, "DALI", "[Resource Tracker] ThreadID: %i, Created Texture at: %p, size: %ix%i\n",
               thread,
               message.GetPointer(),
               message.GetParameter( 1 ),
               message.GetParameter( 2 ) );
        }
        break;
      }

      case ResourceTrackMessage::CREATE_BUFFER:
      {
        gResourceTrackingManager->AddToGPUTracked( message.GetParameter( 3 ) );

        // Add this entry to the GPU resources tracker
        gResourceTrackingManager->InsertGPUResource( message.GetParameter( 2 ), message.GetPointer(), message.GetParameter( 3 ) );

        LOG( LOG_INFO, "DALI", "[Resource Tracker] ThreadID: %i, TextureCreate: size: %ix%i, to GLHandle: %i\n",
             thread,
             message.GetParameter( 0 ),
             message.GetParameter( 1 ),
             message.GetParameter( 2 ) );
        break;
      }

      case ResourceTrackMessage::DELETE_TEXTURE:
      {
        // Look to see if the upload / creation of this texture has been logged?
        Dali::Internal::Adaptor::ResourceTrackingManager::GPUResourcesIter GIter = gResourceTrackingManager->GetGPUResource( message.GetParameter( 0 ) );
        if ( GIter != gResourceTrackingManager->GetGPUResourcesEnd() )
        {

          // Take stored size away from running GPU total
          gResourceTrackingManager->TakeFromGPUTracked( GIter->second.size );

          // The pointer entry in the GPUslot structure contains the pointer that we can use as a key to lookup the resource ID
          Dali::Internal::Adaptor::ResourceTrackingManager::CPUResourcesIter CIter = gResourceTrackingManager->GetCPUResource( GIter->second.ptr );
          if ( CIter != gResourceTrackingManager->GetCPUResourcesEnd() )
          {
            LOG( LOG_INFO, "DALI", "[Resource Tracker] ThreadID: %i, TextureDelete: GLHandle: %i, (ResourceID: %i)\n",
                 thread,
                 message.GetParameter( 0 ),
                 CIter->second.id );

            // Indicate that resource is unloaded from GPU and erase record if CPU buffer no longer exists?
            gResourceTrackingManager->SetResourceGPUStatus( CIter, false );
            gResourceTrackingManager->EraseCPUResource( CIter );

          }
          else
          {
            // Cannot locate association with a specific resource, so...
            LOG( LOG_INFO, "DALI", "[Resource Tracker] ThreadID: %i, TextureDelete: GLHandle: %i, pointer: %p\n",
                 thread,
                 message.GetParameter( 0 ),
                 GIter->second.ptr );
          }

          // Remove the association with this handle
          gResourceTrackingManager->EraseGPUResource( GIter );
        }
        else
        {
          LOG( LOG_INFO, "DALI", "[Resource Tracker] ThreadID: %i, TextureDelete: GLHandle: %i\n",
               thread,
               message.GetParameter( 0 ) );
        }
        break;
      }

      case ResourceTrackMessage::DELETE_BUFFER:
      {
        gResourceTrackingManager->TakeFromCPUTracked( message.GetParameter( 3 ) );
        Dali::Internal::Adaptor::ResourceTrackingManager::CPUResourcesIter CIter = gResourceTrackingManager->GetCPUResource( message.GetPointer() );
        if ( CIter != gResourceTrackingManager->GetCPUResourcesEnd() )
        {
          LOG( LOG_INFO, "DALI", "[Resource Tracker] ThreadID: %i, BufferDelete: ResourceID: %i\n",
               thread,
               CIter->second );

          // Tell CPU slot that it's unloaded from CPU and erase if also unloaded from GPU
          gResourceTrackingManager->SetResourceCPUStatus( CIter, false );
          gResourceTrackingManager->EraseCPUResource( CIter );
        }
        else
        {
          LOG( LOG_INFO, "DALI", "[Resource Tracker] ThreadID: %i, BufferDelete: from address: %p\n",
               thread,
               message.GetPointer() );
        }
        break;
      }

      case ResourceTrackMessage::UPLOAD_TEXTURE:
      {
        // Add this entry to the GPU running total
        gResourceTrackingManager->AddToGPUTracked( message.GetParameter( 3 ) );

        // Associate pointer and buffer size with GLHandle key
        gResourceTrackingManager->InsertGPUResource( message.GetParameter( 2 ), message.GetPointer(), message.GetParameter( 3 ) );

        // Read the corresponding entry from the CPU resources tracker
        Dali::Internal::Adaptor::ResourceTrackingManager::CPUResourcesIter CIter = gResourceTrackingManager->GetCPUResource( message.GetPointer() );
        if ( CIter != gResourceTrackingManager->GetCPUResourcesEnd() )
        {
          LOG( LOG_INFO, "DALI", "[Resource Tracker] ThreadID: %i, TextureUpload: from ResourceID: %i, size: %ix%i to GLHandle: %i\n",
               thread,
               CIter->second,
               message.GetParameter( 0 ),
               message.GetParameter( 1 ),
               message.GetParameter( 2 ) );

          // Record that the texture has been uploaded
          gResourceTrackingManager->SetResourceGPUStatus( CIter, true );
        }
        else
        {
          LOG( LOG_INFO, "DALI", "[Resource Tracker] ThreadID: %i, TextureUpload: from address: %p, size: %ix%i to GLHandle: %i\n",
               thread,
               message.GetPointer(),
               message.GetParameter( 0 ),
               message.GetParameter( 1 ),
               message.GetParameter( 2 ) );
        }
        break;
      }
      default:
      {
        foundMessage = false;
      }
    }

    if ( foundMessage )
    {
      LOG( LOG_INFO, "DALI", "[Resource Tracker] CPU Total: %iK, GPU Total: %iK, CPU Peak: %iK, GPU Peak: %iK\n\n",
           gResourceTrackingManager->GetCPUTracked() / 1024U,
           gResourceTrackingManager->GetGPUTracked() / 1024U,
           gResourceTrackingManager->GetCPUPeak() / 1024U,
           gResourceTrackingManager->GetGPUPeak() / 1024U );
      break;
    }
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
