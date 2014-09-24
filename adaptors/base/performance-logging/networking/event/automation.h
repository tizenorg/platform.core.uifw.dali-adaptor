#ifndef __DALI_INTERNAL_ADAPTOR_AUTOMATION_H__
#define __DALI_INTERNAL_ADAPTOR_AUTOMATION_H__

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

#include <base/performance-logging/networking/client-send-data-interface.h>
#include <string>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * @brief The automation functions allow a way to control Dali via a network socket.
 *
 * The functions must be called from the event thread only.
 *
 * Any functions which require a response to be sent back to the network client
 * use the ClientSendDataInterface interface.

 * E.g.
 * Dali network client thread            <---- "dump_scene" from network
 * Dali main thread       "json data"    ----->   network
 *
 */
namespace Automation
{

/**
 * @brief Sets properties on an Actor.
 * No ClientSendDataInterface required, as no response is sent back
 * @param message set property message
 */
void SetProperty( std::string message );


/**
 * @brief Dumps the actor tree to the client
 * @param clientId unique network client id
 * @param sendData interface to transmit data to the client
 */
void DumpScene( unsigned int clientId, ClientSendDataInterface* sendData );


void ClearDifferences();

/**
 * @brief Dumps the current frame. only dumps properties changed between runs
 * @param clientId unique network client id
 * @param sendData interface to transmit data to the client
 */
void DumpFrame( unsigned int clientId, ClientSendDataInterface* sendData );


/**
 * @brief Dumps the actor tree to the client
 * @param clientId unique network client id
 * @param sendData interface to transmit data to the client
 */
void GetResource( unsigned int clientId, ClientSendDataInterface* sendData, unsigned int id);


} // namespace Automation

} // namespace Internal

} // namespace Adaptor

} // namespace Dali

#endif
