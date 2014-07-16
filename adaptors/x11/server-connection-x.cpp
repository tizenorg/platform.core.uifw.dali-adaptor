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
#include "server-connection.h"

// EXTERNAL INCLUDES
#include <Ecore.h>
#include <Ecore_Ipc.h>

#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gIndicatorLogFilter;
#endif

struct ServerConnection::Impl
{
  typedef std::vector<Ecore_Event_Handler *> Handlers;

  struct Service
  {
    const char *name;
    int         num;
    bool        isSystem;
  };

  Service           mService;
  bool              mConnected;
  Observer*         mObserver;
  Ecore_Ipc_Server* mIpcServer;
  Handlers          mIpcHandlers;

  Impl( const char*                 serviceName,
        int                         serviceNumber,
        bool                        isSystem,
        ServerConnection::Observer* observer);
  ~Impl();

  bool IsConnected();
  void OnDisconnect();
  bool SendEvent( int event, const void *data, int size );
  bool SendEvent( int event, int ref, int ref_to, const void *data, int size );
  void CloseConnection();
  Eina_Bool IpcServerAdd( void *data, int type, void *event );
  Eina_Bool IpcServerDel( void *data, int type, void *event );
  Eina_Bool IpcServerData( void *data, int type, void *event );

};

ServerConnection::ServerConnection( const char*                 serviceName,
                                    int                         serviceNumber,
                                    bool                        isSystem,
                                    ServerConnection::Observer* observer)
{
  mImpl = new impl(serviceName, serviceNumber, isSystem, observer);
}

ServerConnection::~ServerConnection()
{
  delete mImpl;
}

bool ServerConnection::SendEvent( int event, const void *data, int size )
{
  return mImpl->SendEvent(event, 0, 0, data, size);
}

bool ServerConnection::SendEvent( int event, int ref, int ref_to, const void *data, int size )
{
  return mImpl->SendEvent( event, ref, ref_to, data, size );
}

void ServerConnection::CloseConnection()
{
  mImpl->CloseConnection();
}

ServerConnection::Impl(
  const char*                 serviceName,
  int                         serviceNumber,
  bool                        isSystem,
  ServerConnection::Observer* observer)

: mConnected(false),
  mObserver(observer)
{
  Ecore_Ipc_Type ipctype = ECORE_IPC_LOCAL_USER;

  ecore_ipc_init();
  mService.name = eina_stringshare_add(serviceName);
  mService.num = serviceNumber;
  mService.isSystem = isSystem;

  if (mService.isSystem)
  {
    ipctype = ECORE_IPC_LOCAL_SYSTEM;
  }

  DALI_LOG_INFO( gIndicatorLogFilter, Debug::General, "ServerConnection: Connecting to %s %d\n", mService.name, mService.num );

  mIpcServer = ecore_ipc_server_connect( ipctype, (char *)mService.name, mService.num, this );

  if( !mIpcServer )
  {
    ecore_ipc_shutdown();
  }
  else
  {
    mIpcHandlers.push_back( ecore_event_handler_add( ECORE_IPC_EVENT_SERVER_ADD,
                                                     &ServerConnection::Impl::IpcServerAdd,
                                                     this ) );

    mIpcHandlers.push_back( ecore_event_handler_add( ECORE_IPC_EVENT_SERVER_DEL,
                                                     &ServerConnection::Impl::IpcServerDel,
                                                     this ) );

    mIpcHandlers.push_back( ecore_event_handler_add( ECORE_IPC_EVENT_SERVER_DATA,
                                                     &ServerConnection::Impl::IpcServerData,
                                                     this));

    mConnected = true;
  }
}


~ServerConnection::Impl()
{
  CloseConnection();

  if( mService.name != NULL )
  {
    eina_stringshare_del(mService.name);
  }

  for( Handlers::iterator iter = mIpcHandlers.begin(); iter != mIpcHandlers.end(); ++iter )
  {
    ecore_event_handler_del(*iter);
  }
  mIpcHandlers.clear();
}

bool ServerConnection::Impl::IsConnected()
{
  return mConnected;
}

void ServerConnection::Impl::OnDisconnect()
{
  mConnected = false;
  mIpcServer = NULL;
  ecore_ipc_shutdown();
  if( mObserver )
  {
    mObserver->ConnectionClosed();
  }
}

bool ServerConnection::Impl::SendEvent( int event, const void *data, int size )
{
  return SendEvent(event, 0, 0, data, size);
}

bool ServerConnection::Impl::SendEvent( int event, int ref, int ref_to, const void *data, int size )
{
  if( mIpcServer != NULL  && ecore_ipc_server_send(mIpcServer, MAJOR, event, ref, ref_to, 0, data, size) )
  {
    return true;
  }
  else
  {
    return false;
  }
}

void ServerConnection::Impl::CloseConnection()
{
  if( mConnected )
  {
    DALI_LOG_INFO( gIndicatorLogFilter, Debug::General, "ServerConnection: CloseConnection\n" );

    if( mIpcServer )
    {
      ecore_ipc_server_del( mIpcServer );
      mIpcServer = NULL;
    }

    ecore_ipc_shutdown();
    mConnected = false;
  }
}

Eina_Bool ServerConnection::Impl::IpcServerAdd( void *data, int /*type*/, void *event )
{
  DALI_LOG_INFO(gIndicatorLogFilter, Debug::General, "ServerConnection: IpcServerAdd\n" );

  return ECORE_CALLBACK_PASS_ON;
}

Eina_Bool ServerConnection::Impl::IpcServerDel( void *data, int /*type*/, void *event )
{
  DALI_LOG_INFO( gIndicatorLogFilter, Debug::General, "ServerConnection: IpcServerDel\n" );

  Ecore_Ipc_Event_Server_Del *e = static_cast<Ecore_Ipc_Event_Server_Del *>( event );
  ServerConnection* connection = static_cast<ServerConnection*>( data );

  if( connection != NULL )
  {
    if( connection->mIpcServer == e->server)
    {
      // No longer have a server connection
      connection->OnDisconnect();
    }
  }

  return ECORE_CALLBACK_PASS_ON;
}

Eina_Bool ServerConnection::Impl::IpcServerData( void *data, int /*type*/, void *event )
{
  DALI_LOG_INFO( gIndicatorLogFilter, Debug::General, "ServerConnection: IpcServerData\n" );

  Ecore_Ipc_Event_Server_Data *e = static_cast<Ecore_Ipc_Event_Server_Data *>( event );
  ServerConnection* connection = static_cast<ServerConnection*>( data );

  if( connection != NULL )
  {
    if( connection != ecore_ipc_server_data_get( e->server ) )
    {
      return ECORE_CALLBACK_PASS_ON;
    }

    if( e->major != MAJOR )
    {
      return ECORE_CALLBACK_PASS_ON;
    }

    if( connection->mObserver )
    {
      connection->mObserver->DataReceived( event );
    }
  }
  return ECORE_CALLBACK_PASS_ON;
}


} // Adaptor
} // Internal
} // Dali
