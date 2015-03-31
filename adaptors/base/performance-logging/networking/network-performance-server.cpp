/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "network-performance-server.h"

// EXTERNAL INCLUDES
#include <boost/thread.hpp>

// INTERNAL INCLUDES
#include <base/performance-logging/performance-marker.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace // un-named namespace
{
const unsigned int SERVER_PORT = 3031;
const unsigned int MAXIMUM_PORTS_TO_TRY = 10; ///< if port in use, try up to SERVER_PORT + 10
const unsigned int CONNECTION_BACKLOG = 2;   ///<  maximum length of the queue of pending connections.
const unsigned int SOCKET_READ_BUFFER_SIZE = 4096;
typedef Vector< NetworkPerformanceClient*> ClientList;
}

NetworkPerformanceServer::NetworkPerformanceServer( AdaptorInternalServices& adaptorServices,
                                                    const EnvironmentOptions& logOptions )
: mTriggerEventFactory( adaptorServices.GetTriggerEventFactoryInterface() ),
  mSocketFactory( adaptorServices.GetSocketFactoryInterface() ),
  mLogOptions( logOptions ),
  mServerThread( NULL ),
  mListeningSocket( NULL ),
  mClientUniqueId( 0 ),
  mClientCount( 0 ),
  mLogFunctionInstalled( false )
{
}

NetworkPerformanceServer::~NetworkPerformanceServer()
{
  Stop();

  if( mLogFunctionInstalled )
  {
    mLogOptions.UnInstallLogFunction();
  }
}

void NetworkPerformanceServer::Start()
{
  // start a thread to listen for incoming connection requests
  if (! mServerThread )
  {
    if( mListeningSocket )
    {
      mSocketFactory.DestroySocket( mListeningSocket );
    }
    mListeningSocket = mSocketFactory.NewSocket( SocketInterface::TCP);
    mListeningSocket->ReuseAddress( true );

    bool bound = false;
    unsigned int basePort = 0;

    // try a small range of ports, so if multiple Dali apps are running you can select
    // which one to connect to
    while( !bound && ( basePort <  MAXIMUM_PORTS_TO_TRY ))
    {
      bound = mListeningSocket->Bind( SERVER_PORT + basePort );
      if( !bound )
      {
        basePort++;
      }
    }
    if(!bound )
    {
      DALI_LOG_ERROR("Failed to bind to a port \n");
      return;
    }

    mListeningSocket->Listen( CONNECTION_BACKLOG );

    // start a thread which will block waiting for new connections
    mServerThread = new boost::thread(boost::bind(&NetworkPerformanceServer::ConnectionListener, this));

    Dali::Integration::Log::LogMessage(Integration::Log::DebugInfo, "~~~ NetworkPerformanceServer started on port %d ~~~ \n",  SERVER_PORT + basePort);

  }
}
void NetworkPerformanceServer::Stop()
{
  if( !mServerThread )
  {
    return;
  }
  // close the server thread to prevent any new connections
  mListeningSocket->ExitSelect();

  // wait for the thread to exit.
  mServerThread->join();

  // close the socket
  mListeningSocket->CloseSocket();

  delete mServerThread;
  mSocketFactory.DestroySocket( mListeningSocket );

  mServerThread = NULL;
  mListeningSocket = NULL;

  // this will tell all client threads to quit
  StopClients();

  // wait for all threads to exit and the client count to hit zero
  {
    boost::mutex::scoped_lock lock(mClientListMutex);

    while (mClientCount != 0)
    {
      mClientCountUpdated.wait(lock);
    }
  }
}

bool NetworkPerformanceServer::IsRunning() const
{
  if (mServerThread )
  {
    return true;
  }
  return false;
}

void NetworkPerformanceServer::ClientThread( NetworkPerformanceClient* client )
{
  mClientCount++;

  SocketInterface& socket( client->GetSocket() );

  for( ;; )
  {
    SocketInterface::SelectReturn ret = socket.Select();

    if( ret == SocketInterface::DATA_AVAILABLE )
    {
     // Read
      char buffer[ SOCKET_READ_BUFFER_SIZE ];
      unsigned int  bytesRead;

      bool ok  = socket.Read( buffer, sizeof( buffer ) , bytesRead);
      if( ok && ( bytesRead > 0) )
      {
        client->ProcessCommand( buffer, bytesRead );
      }
      else   // if bytesRead == 0, then client closed connection, if ok == false then an error
      {
        DeleteClient( client );
        return;
      }
    }
    else // ret == QUIT or ERROR
    {
      DeleteClient( client);
      return;
    }
  }
}

void NetworkPerformanceServer::ConnectionListener()
{
  // install Dali logging function for this thread
  if( !mLogFunctionInstalled )
  {
    mLogOptions.InstallLogFunction();
    mLogFunctionInstalled = true;
  }

  for( ;; )
  {
    // this will block, waiting for a client to connect
    // or for mListeningSocket->ExitSelect() to be called

    SocketInterface::SelectReturn ret = mListeningSocket->Select();

    if( ret == SocketInterface::DATA_AVAILABLE )
    {
      SocketInterface* clientSocket = mListeningSocket->Accept();

      // new connection made, spawn a thread to handle it
      NetworkPerformanceClient* client = AddClient( clientSocket );
      new boost::thread(boost::bind(&NetworkPerformanceServer::ClientThread, this, client));
    }
    else // ret == SocketInterface::QUIT or SocketInterface::ERROR
    {
      return;
    }
  }
}

NetworkPerformanceClient* NetworkPerformanceServer::AddClient( SocketInterface* clientSocket )
{
  // This function is only called from the listening thread
  NetworkPerformanceClient* client= new NetworkPerformanceClient(clientSocket,
                                                                 mClientUniqueId++,
                                                                 mTriggerEventFactory,
                                                                 *this,
                                                                 mSocketFactory);

  // protect the mClients list which can be accessed from multiple threads.
  boost::mutex::scoped_lock sharedDatalock( mClientListMutex );

  mClients.PushBack( client );

  return client;
}

void NetworkPerformanceServer::DeleteClient( NetworkPerformanceClient* client )
{
  // protect the mClients list while modifying
  boost::mutex::scoped_lock sharedDatalock( mClientListMutex );

  // remove from the list, and delete it
  for( ClientList::Iterator iter = mClients.Begin(); iter != mClients.End() ; ++iter )
  {
    if( (*iter) == client )
    {
      mClients.Erase( iter );
      delete client;

      // if there server is shutting down, it waits for client count to hit zero
      mClientCount--;

      // lets the server know the client count has been modified
      mClientCountUpdated.notify_one();
      return;
    }
  }
}

void NetworkPerformanceServer::SendData( const char* const data, unsigned int bufferSizeInBytes,unsigned int clientId )
{
  if( ! mClientCount )
  {
    return;
  }

  // prevent clients been added / deleted while transmiting data
  boost::mutex::scoped_lock sharedDatalock( mClientListMutex );

  for( ClientList::Iterator iter = mClients.Begin(); iter != mClients.End() ; ++iter )
  {
    NetworkPerformanceClient* client = (*iter);
    if( client->GetId() == clientId )
    {
      client->WriteSocket(data ,bufferSizeInBytes);
      return;
    }
  }
}

void NetworkPerformanceServer::TransmitMarker( const PerformanceMarker& marker, const char* const description )
{
  if( ! IsRunning() )
  {
    return;
  }
  // prevent clients been added / deleted while transmiting data
  boost::mutex::scoped_lock sharedDatalock( mClientListMutex );

  for( ClientList::Iterator iter = mClients.Begin(); iter != mClients.End() ; ++iter )
  {
    NetworkPerformanceClient* client = (*iter);
    client->TransmitMarker( marker, description );
  }
}


void NetworkPerformanceServer::StopClients()
{
  // prevent clients been added / deleted while stopping all clients
  boost::mutex::scoped_lock sharedDatalock( mClientListMutex );

  for( ClientList::Iterator iter = mClients.Begin(); iter != mClients.End() ; ++iter )
  {
    NetworkPerformanceClient* client = (*iter);
    // stop the client from waiting for new commands, and exit from it's thread
    client->ExitSelect();
  }
}

} // namespace Internal

} // namespace Adaptor

} // namespace Dali
