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
#include "network-performance-client.h"

// INTERNAL INCLUDES
#include <base/interfaces/socket-interface.h>
#include <base/performance-logging/networking/network-performance-protocol.h>
#include <base/performance-logging/networking/event/automation.h>
#include <boost/bind.hpp>

#include <stdio.h>
#include <string>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
const char UNKNOWN_CMD[]= "Command or parameter invalid, type help for list of commands\n";
#define GREEN  "\033[01;32m"
#define NORMAL  "\e[m"
}


NetworkPerformanceClient::NetworkPerformanceClient(  SocketInterface *socket,
                                                     unsigned int clientId,
                                                     TriggerEventFactoryInterface& triggerEventFactory,
                                                     ClientSendDataInterface& sendDataInterface,
                                                     SocketFactoryInterface& socketFactory )
: mSocket( socket ),
  mMarkerBitmask( PerformanceMarker::FILTERING_DISABLED ),
  mTriggerEventFactory( triggerEventFactory ),
  mSendDataInterface( sendDataInterface ),
  mSocketFactoryInterface( socketFactory ),
  mClientId( clientId ),
  mConsoleClient(false)
{

}

NetworkPerformanceClient::~NetworkPerformanceClient()
{
  if( mSocket->SocketIsOpen() )
  {
    mSocket->CloseSocket();
  }
  mSocketFactoryInterface.DestroySocket( mSocket );
}

unsigned int NetworkPerformanceClient::GetId() const
{
  return mClientId;
}

SocketInterface& NetworkPerformanceClient::GetSocket()
{
  return *mSocket;
}

bool NetworkPerformanceClient::WriteSocket( const void* buffer, unsigned int bufferSizeInBytes )
{
  return mSocket->Write( buffer, bufferSizeInBytes );
}

bool NetworkPerformanceClient::TransmitMarker( const PerformanceMarker& marker, const char* const description )
{
  if( ! marker.IsFilterEnabled( mMarkerBitmask ) )
  {
    return true;
  }
  if( mConsoleClient )
  {
    // write out the time stamp
    char buffer[64];
    int size = snprintf( buffer, sizeof(buffer),"%d.%06d (seconds), %s\n",
                                    marker.GetTimeStamp().seconds,
                                    marker.GetTimeStamp().microseconds,
                                    description );

   return mSocket->Write( buffer, size );

  }

  // todo serialize the data
  return false;
}

void NetworkPerformanceClient::ExitSelect()
{
  mSocket->ExitSelect();
}


void NetworkPerformanceClient::ProcessCommand( char* buffer, unsigned int bufferSizeInBytes )
{
  // if connected via console, then strip off the carriage return, and switch to console mode
  if( buffer[ bufferSizeInBytes - 1] == '\n')
  {
    buffer[ bufferSizeInBytes - 1] = 0;
    mConsoleClient = true;
  }
  unsigned int param(0);
  std::string stringParam;
  PerformanceProtocol::CommandId commandId( PerformanceProtocol::UNKNOWN_COMMAND );

  bool ok =  PerformanceProtocol::GetCommandId( buffer, bufferSizeInBytes, commandId, param, stringParam );
  if( !ok )
  {
    WriteSocket( UNKNOWN_CMD, sizeof(UNKNOWN_CMD) );
    return;
  }
  std::string response;

  switch( commandId )
  {
    case PerformanceProtocol::HELP_MESSAGE:
    {
      response = PerformanceProtocol::GetHelpMessage();
      break;
    }

    case PerformanceProtocol::ENABLE_TIME_MARKER_BIT_MASK:
    {
      mMarkerBitmask  = static_cast<  PerformanceMarker::MarkerFilter >( param );
      response = "enable time marker ";
      break;
    }

    case PerformanceProtocol::DUMP_SCENE_GRAPH:
    {
      // this needs to be run on the main thread, use the trigger event....
      boost::function<void()> functor = boost::bind( &Automation::DumpScene, mClientId, &mSendDataInterface );

      // create a trigger event that automatically deletes itself after the callback has run in the main thread
      TriggerEventInterface *interface = mTriggerEventFactory.CreateTriggerEvent( functor, TriggerEventInterface::DELETE_AFTER_TRIGGER );

      // asynchronous call, the call back will be run sometime later on the main thread
      interface->Trigger();
      break;
    }

    case PerformanceProtocol::SET_PROPERTIES:
    {
      // this needs to be run on the main thread, use the trigger event....
      boost::function<void()> functor = boost::bind( &Automation::SetProperty, stringParam );

      // create a trigger event that automatically deletes itself after the callback has run in the main thread
      TriggerEventInterface *interface = mTriggerEventFactory.CreateTriggerEvent( functor, TriggerEventInterface::DELETE_AFTER_TRIGGER );

      // asynchronous call, the call back will be run sometime later on the main thread
      interface->Trigger();
      break;
    }

    case PerformanceProtocol::LIST_METRICS_AVAILABLE:
    case PerformanceProtocol::ENABLE_METRIC:
    case PerformanceProtocol::DISABLE_METRIC:
    {
      response="Metrics currently not supported";
      break;
    }
    default:
    {
      response = UNKNOWN_CMD;
      break;
    }
  }
  if( ! response.empty() )
  {
    // add a carriage return for console clients
    if( mConsoleClient )
    {
      response+="\n";
    }
    WriteSocket( response.c_str(), response.length()  );
  }
}



} // namespace Internal

} // namespace Adaptor

} // namespace Dali
