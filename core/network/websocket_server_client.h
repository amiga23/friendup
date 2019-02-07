/*©mit**************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
* Copyright (c) Friend Software Labs AS. All rights reserved.                  *
*                                                                              *
* Licensed under the Source EULA. Please refer to the copy of the MIT License, *
* found in the file license_mit.txt.                                           *
*                                                                              *
*****************************************************************************©*/
/** @file
 *
 *  WebSocket client definition
 *
 * file contain all functitons related to websocket client
 *
 *  @author PS (Pawel Stefanski)
 *  @date created 11/2016
 */

#ifndef __NETWORK_WEBSOCKET_SERVER_CLIENT_H__
#define __NETWORK_WEBSOCKET_SERVER_CLIENT_H__

#include <core/types.h>
#include <core/nodes.h>
#include <libwebsockets.h>
#include <util/friendqueue.h>

//
//
//

typedef struct WebsocketServerClient
{
	struct MinNode 					node;
	struct lws				 		*wsc_Wsi;
	int								wsc_InUseCounter;
	void							*wsc_UserSession;
	void 							*wsc_WebsocketsData;
	pthread_mutex_t					wsc_Mutex;
	FQueue							wsc_MsgQueue;
	FBOOL							wsc_ToBeRemoved;
	time_t							wsc_LastPingTime;
}WebsocketServerClient;

//
//
//

WebsocketServerClient *WebsocketServerClientNew();

//
//
//

void WebsocketServerClientDelete( WebsocketServerClient *cl );

#endif // __NETWORK_WEBSOCKET_SERVER_CLIENT__

