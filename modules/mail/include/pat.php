<?php

/*©lgpl*************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
* Copyright (c) Friend Software Labs AS. All rights reserved.                  *
*                                                                              *
* Licensed under the Source EULA. Please refer to the copy of the GNU Lesser   *
* General Public License, found in the file license_lgpl.txt.                  *
*                                                                              *
*****************************************************************************©*/

	
// this file is included in module.php in the folder above...

global $User, $SqlDatabase, $args, $Logger;	

// STEP 1: check that we have valid settings for pat app
$ps = getPatSettings();


/* CYPHT stuff..... */
define('APP_PATH', $ps->fileroot . ( substr($ps->fileroot,-1) == '/' ? '' : '/' ) );
require APP_PATH.'lib/framework.php';
$config = new Hm_Site_Config_File(APP_PATH.'hm3.rc');
/* check config for db auth */
if ($config->get('auth_type') != 'DB') {
    die('fail<--separate-->Mail framework need to be set up with DB configuration for this to work. Please review your cypth installation.');
}

$auth = new Hm_Auth_DB($config);
$validuser = false;

//now check if we have a user, if not try to create or update password to match users.
if( $auth->check_credentials( $User->Name,$User->Password ) )
{
	$validuser = true;
	$Logger->log('Pat user is valid');
}
else if( $auth->create( $User->Name,$User->Password ) )
{
	$validuser = true;
	$Logger->log('Pat user created');
}
else if( $auth->change_pass($User->Name,$User->Password ) )
{
	$validuser = true;
	$Logger->log('Pat user pass updated');
}
else
{
	die('fail<!--separate-->Could not create/update Friend user at Pat end.');
}



/* we dont get included if $args->command is not set... so no need to check here. */
switch( $args->command )
{
	
				
	// make sure we have what we need to run Pat app that uses Cypth 
	case 'initpat':
		
		$o = new stdClass();
		$o->url = $ps->url;
		$o->user = $User->Name;
		$o->pass = $User->Password;
		
		die('ok<!--separate-->' . json_encode( $o ) );
		break;
		
	default:
		die('fail<--separate-->r u kidding me. salmon.');
		break;		
}
	
	
	
/* ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## ##--## */


function getPatSettings()
{
	global $cypthdb, $SqlDatabase;

	$rs = $SqlDatabase->FetchRow( "SELECT Data FROM FSetting s WHERE s.UserID = '-1' AND s.Type = 'mail' AND s.Key = 'patsettings';" );
	$settings = json_decode($rs['Data']);
	
	if( !$settings || !isset( $settings->fileroot ) || !isset( $settings->url ) ) die('fail<!--separate-->Invalid setting for pat detected');
	
	return $settings;
}

/*function connectCypthDb()
{
	global $cypthdb, $SqlDatabase;

	$rs = $SqlDatabase->FetchRow( "SELECT Data FROM FSetting s WHERE s.UserID = '-1' AND s.Type = 'mail' AND s.Key = 'patsettings';" );
	$settings = json_decode($rs['Data']);
	
	$cypthdb = new SqlDatabase( );
	if( $cypthdb->open(  $settings->dbhost,$settings->dbuser,$settings->dbpass ) )
	{
		if( $cypthdb->SelectDatabase( $settings->dbname ) )
		{
			return true;
		}
		else
			die('fail<!--separate-->Could find database '. $settings->dbname .'. Is your configuration correct?');
	}
	else
		die('fail<!--separate-->Could not connect to Mail database at '. $settings->dbhost .'. Is your configuration correct?');	
	
}
*/
	

die('fail<!--separate-->hæh? Laks.');	

?>