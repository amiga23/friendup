/*©agpl*************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
* Copyright (c) Friend Software Labs AS. All rights reserved.                  *
*                                                                              *
* Licensed under the Source EULA. Please refer to the copy of the GNU Affero   *
* General Public License, found in the file license_agpl.txt.                  *
*                                                                              *
*****************************************************************************©*/

// Start her up!
Application.run = function( msg, iface )
{
	var w = new View( {
		title: i18n( 'i18n_look_and_feel_title' ),
		width: 700,
		height: 600
	} );
	this.mainView = w;
	
	var f = new File( 'Progdir:Templates/gui.html' );
	f.i18n();
	f.onLoad = function( data )
	{
		w.setContent( data );
	}
	f.load();

	// Set app in single mode
	this.setSingleInstance( true );	
	
	w.onClose = function()
	{
		Application.quit();
	}
}
