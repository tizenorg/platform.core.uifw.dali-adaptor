#ifndef __DALI_INTERNAL_ADAPTOR_NETWORK_PERFORMANCE_SERVER_H__
#define __DALI_INTERNAL_ADAPTOR_NETWORK_PERFORMANCE_SERVER_H__

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

// INTERNAL INCLUDES
#include <base/environment-options.h>
#include <base/performance-logging/networking/network-performance-client.h>
#include <dali/public-api/common/dali-vector.h>
#include <base/interfaces/adaptor-internal-services.h>

// EXTERNAL INCLUDES
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>


namespace boost
{
class thread;
}
namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class SocketInterface;
class PerformanceMarker;

/**
 *  @brief  The class listens for incoming connections on a dedicated thread.
 *
 *  When a new connection is established a client thread is spawned to
 *  handle that connection, along with a NetworkPerformanceClient object.
 *  The NetworkPerformanceClient object performs processing of incoming
 *  commands and holds the per-client state information for performance monitoring.
 *
 *  Server->Start()
 *  - Open socket
 *  - Spawns a thread to listen for incoming connections
 *  <---- New connection
 *  - Spawns a client thread to communicate with new client
 *
 *  Server->Stop()
 *  - Stops listening thread
 *  - Stops all client threads
 */
class NetworkPerformanceServer : public ClientSendDataInterface
{

public:

  /**
   * @brief Constructor
   * @param adaptorServices adaptor internal services
   * @param logOptions log options
   */
  NetworkPerformanceServer(  AdaptorInternalServices& adaptorServices, const EnvironmentOptions& logOptions );


  /**
   * @brief Start the server, to be called form Dali main thread
   */
  void Start();

  /**
   * @brief Stop the server, to be called form Dali main thread
   */
  void Stop();

  /**
   * @brief Transmit a marker to any clients are listening for this marker.
   * @param marker performance marker
   */
  void TransmitMarker( const PerformanceMarker& marker );

  /**
   * Destructor
   */
  virtual ~NetworkPerformanceServer();

protected:

  /**
   * @copydoc ClientSendDataInterface::SendData
   */
  virtual void SendData( const char* data, unsigned int bufferSizeInBytes, unsigned int clientId );

private:
  /**
   * @brief Client thread function
   * @param client network client object
   */
  void ClientThread( NetworkPerformanceClient* client );

  /**
   * @brief Stop all client threads
   */
  void StopClients();

  /**
   * @brief Waits for new connections to be made
   */
  void ConnectionListener();

  /**
   * @brief Add a new client to the client list
   */
  NetworkPerformanceClient* AddClient( SocketInterface* clientSocket );

  /**
   * @brief Delete a client from the client list
   */
  void DeleteClient( NetworkPerformanceClient* client );


  bool mLogFunctionInstalled:1;       ///< whether the log function is installed
  boost::mutex mClientListMutex;      ///< mutex
  const EnvironmentOptions& mLogOptions;      ///< log options
  boost::thread* mServerThread;       ///< thread that listens for new connections
  SocketInterface* mListeningSocket;  ///< socket used to listen for new connections
  unsigned int mClientUniqueId;       ///< increments for every client connection
  Dali::Vector< NetworkPerformanceClient* > mClients; ///< list of connected clients
  unsigned int mClientCount;                          ///< client count
  boost::condition_variable mClientCountUpdated;      ///< used to say the client count has changed
  TriggerEventFactoryInterface& mTriggerEventFactory; ///< used to create trigger events
  SocketFactoryInterface& mSocketFactory;             ///< used to create sockets
};


} // namespace Internal

} // namespace Adaptor

} // namespace Dali

#endif //__DALI_INTERNAL_ADAPTOR_NETWORK_PERFORMANCE_SERVER_H__
