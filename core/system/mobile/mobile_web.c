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
 *  Mobile Web header
 *
 * All functions related to Mobile web calls
 *
 *  @author PS (Pawel Stefanski)
 *  @date created 08/09/2018
 */

#include <core/types.h>
#include <core/nodes.h>

#include "mobile_web.h"
#include <system/systembase.h>
#include <system/user/user_mobile_app.h>
#include <hardware/network.h>
#include <mobile_app/mobile_app.h>

//
//
//

Http *MobileWebRequest( void *m, char **urlpath, Http* request, UserSession *loggedSession, int *result )
{
	Http *response = NULL;
	SystemBase *l = (SystemBase *)m;
	MobileManager *mm = l->sl_MobileManager;
	
	if( urlpath[ 1 ] == NULL )
	{
		struct TagItem tags[] = {
			{ HTTP_HEADER_CONTENT_TYPE, (FULONG)  StringDuplicate( "text/html" ) },
			{	HTTP_HEADER_CONNECTION, (FULONG)StringDuplicate( "close" ) },
			{TAG_DONE, TAG_DONE}
		};
		
		response = HttpNewSimple( HTTP_200_OK,  tags );
		HttpAddTextContent( response, "fail<!--separate-->Function not found" );
		
		return response;
	}
	else
	{
		/// @cond WEB_CALL_DOCUMENTATION
		/**
		* 
		* <HR><H2>system.library/user/help</H2>Return available commands
		*
		* @param sessionid - (required) session id of logged user
		* @return avaiable user commands
		*/
		/// @endcond
		if( strcmp( urlpath[ 1 ], "help" ) == 0 )
		{
			struct TagItem tags[] = {
				{ HTTP_HEADER_CONTENT_TYPE, (FULONG)  StringDuplicate( "text/html" ) },
				{ HTTP_HEADER_CONNECTION, (FULONG)StringDuplicate( "close" ) },
				{TAG_DONE, TAG_DONE}
			};
		
			response = HttpNewSimple( HTTP_200_OK,  tags );
		
			HttpAddTextContent( response, "ok<!--separate-->{\"HELP\":\"commands: \"" 
				"create - create user in database" 
				",login - login user to system"
				",logout - logout user from system"
				"\"}" );
		
			*result = 200;
			
			return response;
		}
	}
	
	/// @cond WEB_CALL_DOCUMENTATION
	/**
	*
	* <HR><H2>system.library/mobile/createuma</H2>Create mobile user app.
	*
	* @param sessionid - (required) session id of logged user
	* @param apptoken -(required) application token
	* @param userid - user ID
	* @param appversion - application version
	* @param platform - platform name
	* @param version - platform version
	* @return { create: sucess, result: <ID> } when success, otherwise error with code
	*/
	/// @endcond
	
	if( strcmp( urlpath[ 1 ], "createuma" ) == 0 )
	{
		struct TagItem tags[] = {
			{ HTTP_HEADER_CONTENT_TYPE, (FULONG)StringDuplicate( "text/html" ) },
			{ HTTP_HEADER_CONNECTION, (FULONG)StringDuplicate( "close" ) },
			{TAG_DONE, TAG_DONE}
		};
		
		response = HttpNewSimple( HTTP_200_OK,  tags );
		
		FULONG uid = 0;
		char *apptoken = NULL;
		char *appversion = NULL;
		char *platform = NULL;
		char *version = NULL;
		FBOOL uappCreated = FALSE;
		
		DEBUG( "[MobileWebRequest] Create user mobile app!!\n" );
		
		HashmapElement *el = NULL;
		
		//if( UMUserIsAdmin( l->sl_UM, request, loggedSession->us_User )  == TRUE )
		{
			el = HttpGetPOSTParameter( request, "userid" );
			if( el != NULL )
			{
				char *next;
				uid = strtol( el->data, &next, 0 );
			}
			if( uid <= 0 )
			{
				uid = loggedSession->us_UserID;
			}
			
			el = HttpGetPOSTParameter( request, "apptoken" );
			if( el != NULL )
			{
				apptoken = UrlDecodeToMem( (char *)el->data );
				int z;
				for( z = 0 ; z < strlen( apptoken ) ; z++ )
				{
					if( apptoken[ z ] == ' ' )
					{
						apptoken[ z ] = 0;
						break;
					}
				}
			}
			
			el = HttpGetPOSTParameter( request, "appversion" );
			if( el != NULL )
			{
				appversion = UrlDecodeToMem( (char *)el->data );
			}
			
			el = HttpGetPOSTParameter( request, "platform" );
			if( el != NULL )
			{
				platform = UrlDecodeToMem( (char *)el->data );
			}
			
			el = HttpGetPOSTParameter( request, "version" );
			if( el != NULL )
			{
				version = UrlDecodeToMem( (char *)el->data );
			}
			
			if( uid > 0 && apptoken != NULL )
			{
				char buffer[ 256 ];
				int err = 0;
				FBOOL tokenFound = FALSE;
				
				SQLLibrary *sqllib  = l->LibrarySQLGet( l );
				if( sqllib != NULL )
				{
					// if entry with token already exist there is no need to create new one
					
					char query[ 512 ];
					snprintf( query, sizeof(query), "SELECT AppToken from `FUserMobileApp` where AppToken='%s'", apptoken );
					
					void *res = sqllib->Query( sqllib, query );
				
					if( res != NULL )
					{
						char **row;
						while( ( row = sqllib->FetchRow( sqllib, res ) ) )
						{
							if( row[ 0 ] != NULL )
							{
								tokenFound = TRUE;
							}
						}
						sqllib->FreeResult( sqllib, res );
					}
				
					if( tokenFound == FALSE )
					{
						int addErr = 1;	// 1 entry wasnt added, must be released
						
						UserMobileApp *ma = UserMobileAppNew();
						if( ma != NULL )
						{
							char ipbuffer[ 128 ];
							ma->uma_AppToken = apptoken;
							ma->uma_AppVersion = appversion;
							ma->uma_Platform = platform;
							ma->uma_PlatformVersion = version;
							ma->uma_UserID = uid;
							apptoken = appversion = platform = version = NULL;
					
							if( getLocalIP( ipbuffer, sizeof(ipbuffer) ) == 0 )
							{
								ma->uma_Core = StringDuplicate( ipbuffer );
							}
							else
							{
								ma->uma_Core = StringDuplicate("Error");
							}
					
							err = sqllib->Save( sqllib, UserMobileAppDesc, ma );
							
							addErr = MobileManagerAddUMA( l->sl_MobileManager, ma );
							
							if( err == 0 )
							{
								snprintf( buffer, sizeof(buffer), "ok<!--separate-->{ \"response\": \"0\", \"create\":\"%lu\" }", ma->uma_ID );
								HttpAddTextContent( response, buffer );
							}
						}
						else
						{
							//snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"%s\", \"code\":\"%d\" }", l->sl_Dictionary->d_Msg[DICT_CANNOT_ALLOCATE_MEMORY] , DICT_CANNOT_ALLOCATE_MEMORY );
							err = 2;
						}
						
						if( ma != NULL && addErr != 0 )
						{
							UserMobileAppDelete( ma );
						}
					}
					l->LibrarySQLDrop( l, sqllib );
				}
				
				if( tokenFound == TRUE )
				{
					snprintf( buffer, sizeof(buffer), "ok<!--separate-->{ \"response\": \"0\", \"create\":\"0\" }" );
					HttpAddTextContent( response, buffer );
				}
				else if( err != 0 )
				{
					snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"1\", \"code\":\"%d\" }" , err );
					HttpAddTextContent( response, buffer );
				}
			} // missing parameters
			else
			{
				char buffer[ 256 ];
				char buffer1[ 256 ];
				snprintf( buffer1, sizeof(buffer1), l->sl_Dictionary->d_Msg[DICT_PARAMETERS_MISSING], "userid, apptoken" );
				snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"%s\", \"code\":\"%d\" }", buffer1 , DICT_PARAMETERS_MISSING );
				HttpAddTextContent( response, buffer );
			}
		}

		//if( userCreated == TRUE )
		{
			if( apptoken != NULL )
			{
				FFree( apptoken );
			}
			if( appversion != NULL )
			{
				FFree( appversion );
			}
			if( platform != NULL )
			{
				FFree( platform );
			}
			if( version != NULL )
			{
				FFree( version );
			}
		}
		*result = 200;
	}
	
	/// @cond WEB_CALL_DOCUMENTATION
	/**
	*
	* <HR><H2>system.library/mobile/deleteuma</H2>Delete User Mobile Application entry.
	*
	* @param sessionid - (required) session id of logged user
	* @param id - (required) id of UserMobileApp which you want to delete
	* @return { Result: success} when success, otherwise error with code
	*/
	/// @endcond
	else if( strcmp( urlpath[ 1 ], "deleteuma" ) == 0 )
	{
		struct TagItem tags[] = {
			{ HTTP_HEADER_CONTENT_TYPE, (FULONG)  StringDuplicate( "text/html" ) },
			{	HTTP_HEADER_CONNECTION, (FULONG)StringDuplicate( "close" ) },
			{TAG_DONE, TAG_DONE}
		};
		
		response = HttpNewSimple( HTTP_200_OK,  tags );

		FULONG id = 0;
		
		DEBUG( "[MobileWebRequest] Delete UserMobileApp!!\n" );
		
		HashmapElement *el = HttpGetPOSTParameter( request, "id" );
		if( el != NULL )
		{
			char *next;
			id = strtol ( (char *)el->data, &next, 0 );
		}
		
		if( id > 0 )
		{
			SQLLibrary *sqllib  = l->LibrarySQLGet( l );
			if( sqllib != NULL )
			{
				char *tmpQuery = NULL;
				int querysize = 1024;
				
				if( ( tmpQuery = FCalloc( querysize, sizeof(char) ) ) != NULL )
				{
					sprintf( tmpQuery, "DELETE FROM `FUserMobileApp` WHERE ID=%lu", id );
					
					sqllib->QueryWithoutResults( sqllib, tmpQuery );
					FFree( tmpQuery );
					
					HttpAddTextContent( response, "ok<!--separate-->{ \"Result\": \"success\"}" );
				}
				else
				{
					char buffer[ 256 ];
					snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"%s\", \"code\":\"%d\" }", l->sl_Dictionary->d_Msg[DICT_CANNOT_ALLOCATE_MEMORY] , DICT_CANNOT_ALLOCATE_MEMORY );
					HttpAddTextContent( response, buffer );
				}
				
				l->LibrarySQLDrop( l, sqllib );
			}
			else
			{
				char buffer[ 256 ];
				snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"%s\", \"code\":\"%d\" }", l->sl_Dictionary->d_Msg[DICT_SQL_LIBRARY_NOT_FOUND] , DICT_SQL_LIBRARY_NOT_FOUND );
				HttpAddTextContent( response, buffer );
			}
		}
		else
		{
			char buffer[ 256 ];
			char buffer1[ 256 ];
			snprintf( buffer1, sizeof(buffer1), l->sl_Dictionary->d_Msg[DICT_PARAMETERS_MISSING], "id" );
			snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"%s\", \"code\":\"%d\" }", buffer1 , DICT_PARAMETERS_MISSING );
			HttpAddTextContent( response, buffer );
		}
	}
	
	/// @cond WEB_CALL_DOCUMENTATION
	/**
	*
	* <HR><H2>system.library/mobile/updateuma</H2>Update User Mobile Application
	*
	* @param sessionid - (required) session id of logged user
	* @param userid - (required) user ID
	* @param apptoken - application token
	* @param appversion - application version
	* @param platform - platform name
	* @param version - platform version
	* @return { create: sucess, result: <ID> } when success, otherwise error with code
	*/
	/// @endcond
	else if( strcmp( urlpath[ 1 ], "updateuma" ) == 0 )
	{
		
		struct TagItem tags[] = {
			{ HTTP_HEADER_CONTENT_TYPE, (FULONG)StringDuplicate( "text/html" ) },
			{ HTTP_HEADER_CONNECTION, (FULONG)StringDuplicate( "close" ) },
			{TAG_DONE, TAG_DONE}
		};
		
		response = HttpNewSimple( HTTP_200_OK,  tags );
		
		FULONG uid = 0, umaid = 0;
		char *apptoken = NULL;
		char *appversion = NULL;
		char *platform = NULL;
		char *version = NULL;
		FBOOL uappCreated = FALSE;
		
		DEBUG( "[MobileWebRequest] Create user mobile app!!\n" );
		
		HashmapElement *el = NULL;
		
		{
			el = HttpGetPOSTParameter( request, "id" );
			if( el != NULL )
			{
				char *next;
				umaid = strtol( el->data, &next, 0 );
			}
			
			el = HttpGetPOSTParameter( request, "userid" );
			if( el != NULL )
			{
				char *next;
				uid = strtol( el->data, &next, 0 );
			}
			
			el = HttpGetPOSTParameter( request, "apptoken" );
			if( el != NULL )
			{
				apptoken = UrlDecodeToMem( (char *)el->data );
			}
			
			el = HttpGetPOSTParameter( request, "appversion" );
			if( el != NULL )
			{
				appversion = UrlDecodeToMem( (char *)el->data );
			}
			
			el = HttpGetPOSTParameter( request, "platform" );
			if( el != NULL )
			{
				platform = UrlDecodeToMem( (char *)el->data );
			}
			
			el = HttpGetPOSTParameter( request, "version" );
			if( el != NULL )
			{
				version = UrlDecodeToMem( (char *)el->data );
			}
			
			if( umaid > 0 )
			{
				UserMobileApp *ma = MobleManagerGetByIDDB( mm, umaid );
				char buffer[ 256 ];
				int err = 0;
				
				if( ma != NULL )
				{
					if( apptoken != NULL )
					{
						if( ma->uma_AppToken != NULL )
						{
							FFree( ma->uma_AppToken );
						}
						ma->uma_AppToken = apptoken;
					}

					if( appversion != NULL )
					{
						if( ma->uma_AppVersion != NULL )
						{
							FFree( ma->uma_AppVersion );
						}
						ma->uma_AppVersion = appversion;
					}

					if( platform != NULL )
					{
						if( ma->uma_Platform != NULL )
						{
							FFree( ma->uma_Platform );
						}
						ma->uma_Platform = platform;
					}

					if( version != NULL )
					{
						if( ma->uma_PlatformVersion != NULL )
						{
							FFree( ma->uma_PlatformVersion );
						}
						ma->uma_PlatformVersion = version;
					}
					if( uid > 0 )
					{
						ma->uma_UserID = uid;
					}
					apptoken = appversion = platform = version = NULL;
					
					SQLLibrary *lsqllib = l->LibrarySQLGet( l );
					if( lsqllib != NULL )
					{
						err = lsqllib->Update( lsqllib, UserMobileAppDesc, ma );
		
						l->LibrarySQLDrop( l, lsqllib );
					}
					else
					{
						err = 1;
					}
				}
				else
				{
					//snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"%s\", \"code\":\"%d\" }", l->sl_Dictionary->d_Msg[DICT_CANNOT_ALLOCATE_MEMORY] , DICT_CANNOT_ALLOCATE_MEMORY );
					err = 2;
				}
				
				if( err == 0 )
				{
					snprintf( buffer, sizeof(buffer), "ok<!--separate-->{ \"response\": \"0\", \"updated\":\"%lu\" }", ma->uma_ID );
					HttpAddTextContent( response, buffer );
				}
				else
				{
					snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"1\", \"updated\":\"%d\" }" , err );
					HttpAddTextContent( response, buffer );
				}
				
			} // missing parameters
			else  // umaid
			{
				char buffer[ 256 ];
				char buffer1[ 256 ];
				snprintf( buffer1, sizeof(buffer1), l->sl_Dictionary->d_Msg[DICT_PARAMETERS_MISSING], "id" );
				snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"%s\", \"code\":\"%d\" }", buffer1 , DICT_PARAMETERS_MISSING );
				HttpAddTextContent( response, buffer );
			}
		}
		
		//if( userCreated == TRUE )
		{
			if( apptoken != NULL )
			{
				FFree( apptoken );
			}
			if( appversion != NULL )
			{
				FFree( appversion );
			}
			if( platform != NULL )
			{
				FFree( platform );
			}
			if( version != NULL )
			{
				FFree( version );
			}
		}
		*result = 200;
	}
	
	/// @cond WEB_CALL_DOCUMENTATION
	/**
	*
	* <HR><H2>system.library/mobile/getuma</H2>Get User Mobile Application
	*
	* @param sessionid - (required) session id of logged user
	* @param userid - (required) user ID
	* @return { create: sucess, result: <ID> } when success, otherwise error with code
	*/
	/// @endcond
	else if( strcmp( urlpath[ 1 ], "getuma" ) == 0 )
	{
		
		struct TagItem tags[] = {
			{ HTTP_HEADER_CONTENT_TYPE, (FULONG)StringDuplicate( "text/html" ) },
			{ HTTP_HEADER_CONNECTION, (FULONG)StringDuplicate( "close" ) },
			{TAG_DONE, TAG_DONE}
		};
		
		response = HttpNewSimple( HTTP_200_OK,  tags );
		
		FULONG uid = 0;
		FBOOL uappCreated = FALSE;
		
		DEBUG( "[MobileWebRequest] Create user mobile app!!\n" );
		
		HashmapElement *el = NULL;
		
		{
			el = HttpGetPOSTParameter( request, "userid" );
			if( el != NULL )
			{
				char *next;
				uid = strtol( el->data, &next, 0 );
			}
			
			if( uid > 0 )
			{
				MobileListEntry *ma = MobleManagerGetByUserIDDB( mm, uid );
				BufString *bs = BufStringNew();
				char buffer[ 512 ];
				int err = 0;
				
				BufStringAddSize( bs, "[", 1 );
				
				if( ma != NULL )
				{
					int pos = 0;
					MobileListEntry *loc = ma;
					
					FRIEND_MUTEX_LOCK( &(l->sl_MobileManager->mm_Mutex) );
					
					while( loc != NULL )
					{
						BufString *l = GetJSONFromStructure( UserMobileAppDesc, loc->mm_UMApp );
						if( l != NULL )
						{
							if( pos == 0 )
							{
								BufStringAddSize( bs, l->bs_Buffer, l->bs_Size );
							}
							else
							{
								BufStringAddSize( bs, ",", 1 );
								BufStringAddSize( bs, l->bs_Buffer, l->bs_Size );
							}
							BufStringDelete( l );
						}
						
						loc = (MobileListEntry *)loc->node.mln_Succ;
						pos++;
					}
					
					FRIEND_MUTEX_UNLOCK( &(l->sl_MobileManager->mm_Mutex) );
				}
				else
				{
					err = 2;
				}
				
				BufStringAddSize( bs, "]", 1 );
				
				if( err == 0 )
				{
					HttpSetContent( response, bs->bs_Buffer, bs->bs_Size );
					bs->bs_Buffer = NULL;
				}
				else
				{
					snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"1\", \"updated\":\"%d\" }" , err );
					HttpAddTextContent( response, buffer );
				}
				
				if( ma != NULL )
				{
					MobileListEntryDeleteAll( ma );
				}
			} // missing parameters
			else  // umaid
			{
				char buffer[ 256 ];
				char buffer1[ 256 ];
				snprintf( buffer1, sizeof(buffer1), l->sl_Dictionary->d_Msg[DICT_PARAMETERS_MISSING], "userid" );
				snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"%s\", \"code\":\"%d\" }", buffer1 , DICT_PARAMETERS_MISSING );
				HttpAddTextContent( response, buffer );
			}
		}
		*result = 200;
	}
	/// @cond WEB_CALL_DOCUMENTATION
	/**
	*
	* <HR><H2>system.library/mobile/updatenotification</H2>Update Mobile Notification
	*
	* @param sessionid - (required) session id of logged user
	* @param notifid - (required) mobile request ID
	* @param action - (required) action ( NOTIFY_ACTION_REGISTER = 0, NOTIFY_ACTION_READED, NOTIFY_ACTION_TIMEOUT )
	* @return { update: sucess, result: <ID> } when success, otherwise error with code
	*/
	/// @endcond
	else if( strcmp( urlpath[ 1 ], "updatenotification" ) == 0 )
	{
		
		struct TagItem tags[] = {
			{ HTTP_HEADER_CONTENT_TYPE, (FULONG)StringDuplicate( "text/html" ) },
			{ HTTP_HEADER_CONNECTION, (FULONG)StringDuplicate( "close" ) },
			{TAG_DONE, TAG_DONE}
		};
		
		response = HttpNewSimple( HTTP_200_OK,  tags );
		
		HashmapElement *el = NULL;
		FULONG notifid = 0;
		int action = -1;
		
		el = HttpGetPOSTParameter( request, "notifid" );
		if( el != NULL )
		{
			char *next;
			notifid = strtol( el->data, &next, 0 );
		}
		el = HttpGetPOSTParameter( request, "action" );
		if( el != NULL )
		{
			action = atoi( (char *)el->data );
		}
		
		if( action > 0 && notifid > 0 )	// register is not supported
		{
			char tmp[ 512 ];
			
			Notification *not = NotificationManagerRemoveNotification( l->sl_NotificationManager , notifid );
			int err = MobileAppNotifyUserUpdate( l, loggedSession->us_User->u_Name, not, notifid, action );
			if( not != NULL )
			{
				NotificationDelete( not );
			}
			
			if( err == 0 )
			{
				snprintf( tmp, sizeof(tmp), "{ \"update\": \"sucess\", \"result\":%d }", err );
			}
			else
			{
				snprintf( tmp, sizeof(tmp), "{ \"update\": \"fail\", \"result\":%d }", err );
			}
			
			HttpAddTextContent( response, tmp );
		}
		else
		{
			char buffer[ 256 ];
			char buffer1[ 256 ];
			snprintf( buffer1, sizeof(buffer1), l->sl_Dictionary->d_Msg[DICT_PARAMETERS_MISSING], "mobilereqid, action" );
			snprintf( buffer, sizeof(buffer), "fail<!--separate-->{ \"response\": \"%s\", \"code\":\"%d\" }", buffer1 , DICT_PARAMETERS_MISSING );
			HttpAddTextContent( response, buffer );
		}
	}
	
	/// @cond WEB_CALL_DOCUMENTATION
	/**
	*
	* <HR><H2>system.library/mobile/refreshuma</H2>Refresh User Mobile Application cache
	*
	* @param sessionid - (required) session id of logged user
	* @return { refresh: sucess, result: <error> } when success, otherwise error with code
	*/
	/// @endcond
	else if( strcmp( urlpath[ 1 ], "refreshuma" ) == 0 )
	{
		char buffer[ 256 ];
		
		struct TagItem tags[] = {
			{ HTTP_HEADER_CONTENT_TYPE, (FULONG)StringDuplicate( "text/html" ) },
			{ HTTP_HEADER_CONNECTION, (FULONG)StringDuplicate( "close" ) },
			{TAG_DONE, TAG_DONE}
		};
		
		response = HttpNewSimple( HTTP_200_OK,  tags );
		
		MobileManagerRefreshCache( mm );
		
		snprintf( buffer, sizeof(buffer), "ok<!--separate-->{ \"refresh\": \"sucess\", \"result\":\"0\" }" );
		HttpAddTextContent( response, buffer );
		
		*result = 200;
	}
	
	return response;
}

