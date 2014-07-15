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

#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <dali/dali.h>
#include <dali-test-suite-utils.h>
#include "slp-platform-abstraction.h"
#include "resource-collector.h"

using namespace Dali;
using namespace Dali::Integration;
using namespace Dali::Internal::Platform;

namespace
{
/**
 * The number of loads issued in test cases is a multiple of this. The higher it
 * is, the more the tests stress the system but the longer they take to run.
 * A value of 1000 is enough to make load tests take tens of seconds each
 * on desktop. */
const unsigned NUM_LOAD_GROUPS_TO_ISSUE = 200;

/**
 * The number of loads to issue when they will be cancelled.
 * Cancelled loads are cheap so we do a lot.
 */
const unsigned NUM_CANCELLED_LOAD_GROUPS_TO_ISSUE = NUM_LOAD_GROUPS_TO_ISSUE * 10;

/** The number of times to ask for resource load status. */
const unsigned MAX_NUM_RESOURCE_TRIES = 5;

/** Images that should load without issue. */
const char* const VALID_IMAGES[] = {
  TEST_IMAGE_DIR "/frac.jpg",
  TEST_IMAGE_DIR "/frac.24.bmp",
  TEST_IMAGE_DIR "/frac.png",
  TEST_IMAGE_DIR "/interlaced.gif",
  TEST_IMAGE_DIR "/pattern.gif"
};
const unsigned NUM_VALID_IMAGES = 5u;

///@ToDo: Add valid ktx, ico, and wbmp image examples.

/** Live platform abstraction recreated for each test case. */
Integration::PlatformAbstraction * gAbstraction = 0;

} // anon namespace

void utc_dali_loading_startup(void)
{
  test_return_value = TET_UNDEF;
  gAbstraction = CreatePlatformAbstraction();
}

void utc_dali_loading_cleanup(void)
{
  delete gAbstraction;
  gAbstraction = 0;

  test_return_value = TET_PASS;
}

// Positive test case for loading. Load lots and be sure it has succeeded.
int UtcDaliLoadCompletion(void)
{
  tet_printf("Running load completion test \n");

  DALI_ASSERT_ALWAYS( gAbstraction != 0 );

  // Start a bunch of loads that should work:

  const Dali::ImageAttributes attributes;
  const Dali::Integration::BitmapResourceType bitmapResourceType( attributes );
  Dali::Integration::LoadResourcePriority priority = Dali::Integration::LoadPriorityNormal;
  unsigned loadsLaunched = 0;

  for( unsigned loadGroup = 0; loadGroup < NUM_LOAD_GROUPS_TO_ISSUE; ++loadGroup )
  {
    for( unsigned validImage = 0; validImage < NUM_VALID_IMAGES; ++validImage )
    {
      gAbstraction->LoadResource( ResourceRequest( loadGroup * NUM_VALID_IMAGES + validImage + 1, bitmapResourceType, VALID_IMAGES[validImage], priority ) );
    }
    loadsLaunched += NUM_VALID_IMAGES;
  }

  // Drain the completed loads:

  Dali::Internal::Platform::ResourceCollector resourceSink;

  for( unsigned i = 0; i < MAX_NUM_RESOURCE_TRIES && resourceSink.mGrandTotalCompletions < loadsLaunched; ++i )
  {
    sleep( 1 );
    gAbstraction->GetResources( resourceSink );
  }

  // Check the loads completed as expected:

  tet_printf( "Issued Loads: %u, Completed Loads: %u, Successful Loads: %u, Failed Loads: %u \n", loadsLaunched, resourceSink.mGrandTotalCompletions, unsigned(resourceSink.mSuccessCounts.size()), unsigned(resourceSink.mFailureCounts.size()) );
  DALI_TEST_CHECK( loadsLaunched == resourceSink.mGrandTotalCompletions );
  DALI_TEST_CHECK( loadsLaunched == resourceSink.mSuccessCounts.size() );
  DALI_TEST_CHECK( 0 == resourceSink.mFailureCounts.size() );

  // Check that each success was reported exactly once:
  for( ResourceCounterMap::const_iterator it = resourceSink.mSuccessCounts.begin(), end = resourceSink.mSuccessCounts.end(); it != end; ++it )
  {
    DALI_TEST_CHECK( it->second == 1u );
  }

  END_TEST;
}

/**
 * @brief Test case for load cancellation.
 *
 * Load lots of images in batches, cancelling them all in reverse order to try
 * to catch in-flight ones, and assert that the wrong loads are never cancelled
 * and that all loads issued are either completed of cancelled.
 */
int UtcDaliCancelAllLoads(void)
{
  tet_printf( "Running load cancel-all test.\n" );

  DALI_ASSERT_ALWAYS( gAbstraction != 0 );

  // Start a bunch of loads that should work:

  const ImageAttributes attributes;
  const BitmapResourceType bitmapResourceType( attributes );
  Dali::Integration::LoadResourcePriority priority = LoadPriorityNormal;
  unsigned loadsLaunched = 0;

  for( unsigned loadGroup = 0; loadGroup < NUM_CANCELLED_LOAD_GROUPS_TO_ISSUE; ++loadGroup )
  {
    // Issue load requests for a batch of images:
    for( unsigned validImage = 0; validImage < NUM_VALID_IMAGES; ++validImage )
    {
      const ResourceId resourceId = loadGroup * NUM_VALID_IMAGES + validImage + 1;
      gAbstraction->LoadResource( ResourceRequest( resourceId, bitmapResourceType, VALID_IMAGES[validImage], priority ) );
    }
    loadsLaunched += NUM_VALID_IMAGES;

    // Let the first image in the batch start to load:
    usleep( 5000 ); // This number is tuned. Turn it up too much and all loads will complete and the test will take so long it seems to hang.

    // Cancel all the launched loads from oldest to newest:
    for( unsigned validImage = 0; validImage < NUM_VALID_IMAGES; ++validImage )
    {
      const ResourceId resourceId = loadGroup * NUM_VALID_IMAGES + validImage + 1;
      gAbstraction->CancelLoad( resourceId, ResourceBitmap );
    }
  }

  // Drain the completed loads:
  Dali::Internal::Platform::ResourceCollector resourceSink;

  for( unsigned i = 0; i < MAX_NUM_RESOURCE_TRIES && resourceSink.mGrandTotalCompletions < loadsLaunched; ++i )
  {
    usleep( 100 * 1000 );
    gAbstraction->GetResources( resourceSink );
  }

  // Check the loads completed as expected:

  tet_printf( "Issued Loads: %u, Completed Loads: %u, Successful Loads: %u, Failed Loads: %u \n", loadsLaunched, resourceSink.mGrandTotalCompletions, unsigned(resourceSink.mSuccessCounts.size()), unsigned(resourceSink.mFailureCounts.size()) );
  DALI_TEST_CHECK( loadsLaunched >= resourceSink.mGrandTotalCompletions );
  DALI_TEST_CHECK( loadsLaunched >= resourceSink.mSuccessCounts.size() );
  DALI_TEST_CHECK( 0 == resourceSink.mFailureCounts.size() );

  // Check that each success was reported exactly once:
  for( ResourceCounterMap::const_iterator it = resourceSink.mSuccessCounts.begin(), end = resourceSink.mSuccessCounts.end(); it != end; ++it )
  {
    DALI_TEST_CHECK( it->second == 1u );
  }

  END_TEST;
}

/**
 * @brief Test case for load cancellation.
 *
 * Load lots, cancel a subset and be sure the wrong loads are never cancelled
 * and that all loads issued are either completed of cancelled.
 */
int UtcDaliCancelSomeLoads(void)
{
  tet_printf( "Running load cancel load subset test.\n" );

  DALI_ASSERT_ALWAYS( gAbstraction != 0 );

  // Start a bunch of loads that should work:

  const ImageAttributes attributes;
  const BitmapResourceType bitmapResourceType( attributes );
  Dali::Integration::LoadResourcePriority priority = LoadPriorityNormal;
  unsigned loadsLaunched = 0;

  std::set<Integration::ResourceId> cancelledLoadSet;

  for( unsigned loadGroup = 0; loadGroup < NUM_LOAD_GROUPS_TO_ISSUE; ++loadGroup )
  {
    // Issue load requests for a batch of images:
    for( unsigned validImage = 0; validImage < NUM_VALID_IMAGES; ++validImage )
    {
      const ResourceId resourceId = loadGroup * NUM_VALID_IMAGES + validImage + 1;
      gAbstraction->LoadResource( ResourceRequest( resourceId, bitmapResourceType, VALID_IMAGES[validImage], priority ) );
    }
    loadsLaunched += NUM_VALID_IMAGES;

    // Let the first image in the batch start to load so we can try to cancel it in-flight:
    usleep( 17000 );
    ///@Note: The log should show cancellations of many in-flight loads (e.g., "INFO: DALI: : CheckForCancellation: Cancelled in-flight resource (21)."). If it doesn't, the above delay may need to be adjusted.

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

  for( unsigned i = 0; i < MAX_NUM_RESOURCE_TRIES && resourceSink.mGrandTotalCompletions < loadsLaunched; ++i )
  {
    tet_printf( "Draining sleep %u.\n", i );
    usleep( 1000000 );
    gAbstraction->GetResources( resourceSink );
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
