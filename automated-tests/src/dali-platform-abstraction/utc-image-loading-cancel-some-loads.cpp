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

#include "utc-image-loading-common.h"

void utc_image_loading_cancel_some_loads_startup(void)
{
  utc_dali_loading_startup();
}

void utc_image_loading_cancel_some_loads_cleanup(void)
{
  utc_dali_loading_cleanup();
}

/**
 * @brief Test case for load cancellation.
 *
 * Load lots, cancel a subset and be sure the wrong loads are never cancelled
 * and that all loads issued are either completed or cancelled.
 */
int UtcDaliCancelSomeLoads(void)
{
  tet_printf( "Running load cancel load subset test.\n" );

  DALI_ASSERT_ALWAYS( gAbstraction != 0 );

  // Start a bunch of loads that should work:

  Dali::Integration::LoadResourcePriority priority = LoadPriorityNormal;
  unsigned loadsLaunched = 0;

  std::set<Integration::ResourceId> cancelledLoadSet;

  for( unsigned loadGroup = 0; loadGroup < NUM_LOAD_GROUPS_TO_ISSUE; ++loadGroup )
  {
    // Issue load requests for a batch of images:
    for( unsigned validImage = 0; validImage < NUM_VALID_IMAGES; ++validImage )
    {
      const BitmapResourceType bitmapResourceType( gCancelAttributes[ loadsLaunched % gCancelAttributes.size() ] );
      const ResourceId resourceId = loadGroup * NUM_VALID_IMAGES + validImage + 1;
      gAbstraction->LoadResource( ResourceRequest( resourceId, bitmapResourceType, VALID_IMAGES[validImage], priority ) );
      loadsLaunched += 1;
    }

    // Let the first image in the batch start to load so we can try to cancel it in-flight:
    usleep( 17000 );
    ///@Note: The log should show cancellations of many in-flight loads in desktop builds with info-level logging enabled (e.g., "INFO: DALI: : CheckForCancellation: Cancelled in-flight resource (21)."). If it doesn't, the above delay may need to be adjusted.

    // Cancel just two loads (hopefully one in-flight and one queued):

    // Cancel first load, hopefully while it is in-flight:
    const ResourceId cancelledInFlight = loadGroup * NUM_VALID_IMAGES + 1;
    gAbstraction->CancelLoad( cancelledInFlight, ResourceBitmap );
    cancelledLoadSet.insert( cancelledInFlight );

    // Cancel second load, that is still queued:
    const ResourceId cancelledFromQueue = loadGroup * NUM_VALID_IMAGES + NUM_VALID_IMAGES;
    gAbstraction->CancelLoad( cancelledFromQueue, ResourceBitmap );
    cancelledLoadSet.insert( cancelledFromQueue );
  }

  // Drain the completed loads:

  Dali::Internal::Platform::ResourceCollector resourceSink;

  unsigned lastCompletions = -1;
  for( unsigned i = 0; i < MAX_NUM_RESOURCE_TRIES && resourceSink.mGrandTotalCompletions < loadsLaunched && resourceSink.mGrandTotalCompletions != lastCompletions; ++i )
  {
    lastCompletions = resourceSink.mGrandTotalCompletions;
    gAbstraction->GetResources( resourceSink );
    tet_printf( "Draining sleep %u, at total completion count %u of %u.\n", i, resourceSink.mGrandTotalCompletions, loadsLaunched );
    usleep( 100 * 1000 );
  }

  // Check the loads completed as expected:

  tet_printf( "Issued Loads: %u, Completed Loads: %u, Successful Loads: %u, Failed Loads: %u \n", loadsLaunched, resourceSink.mGrandTotalCompletions, unsigned(resourceSink.mSuccessCounts.size()), unsigned(resourceSink.mFailureCounts.size()) );
  DALI_TEST_CHECK( loadsLaunched >= resourceSink.mGrandTotalCompletions );
  DALI_TEST_CHECK( loadsLaunched >= resourceSink.mSuccessCounts.size() );
  DALI_TEST_CHECK( 0 == resourceSink.mFailureCounts.size() );

  // Check that if an image was not loaded, it is one of the ones that was cancelled:
  // This is the main point of this test case.
  for( unsigned resourceId = 1; resourceId <= NUM_LOAD_GROUPS_TO_ISSUE * NUM_VALID_IMAGES; ++resourceId )
  {
    if( resourceSink.mCompletionStatuses.find( resourceId ) == resourceSink.mCompletionStatuses.end() )
    {
      DALI_TEST_CHECK( cancelledLoadSet.find( resourceId ) != cancelledLoadSet.end() );
    }
  }

  // Check that each success was reported exactly once:
  for(ResourceCounterMap::const_iterator it = resourceSink.mSuccessCounts.begin(), end = resourceSink.mSuccessCounts.end(); it != end; ++it )
  {
    DALI_TEST_CHECK( it->second == 1u );
  }

  END_TEST;
}
