/*©agpl*************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
* Copyright (c) Friend Software Labs AS. All rights reserved.                  *
*                                                                              *
* Licensed under the Source EULA. Please refer to the copy of the GNU Affero   *
* General Public License, found in the file license_agpl.txt.                  *
*                                                                              *
*****************************************************************************©*/

Application.run = function( msg, iface )
{
	// Start with empty playlist
	this.playlist = [];
	this.index = 0;
	this.miniplaylist = false;
	
	var w = new View( {
		title: 'Friend Jukebox',
		width: 400,
		height: 160,
		'min-width': 400,
		'max-width': 400,
		'min-height': 160,
		'max-height': 160,
		resize: false
	} );
	
	this.playlistFilename = false;
	
	w.onClose = function()
	{
		Application.quit();
	}
	
	this.mainView = w;
	
	var f = new File( 'Progdir:Templates/main.html' );
	f.i18n();
	f.onLoad = function( data )
	{
		w.setContent( data );
	}
	f.load();
	
	this.redrawMenu();
	
	// Started with arguments
	if( msg.args )
		this.handleFiles( msg.args );
}

// Add the files of as playlist
Application.addPlaylist = function ( fname )
{
	var f = new File( fname );
	f.onLoad = function( data )
	{
		try
		{
			var js = JSON.parse( data );
			Application.receiveMessage( { 
				command: 'append_to_playlist_and_play',
				items: js
			} );
		}
		catch( e )
		{
			Notify( { title: 'Could not handle file', text: 'File was corrupted or not a valid playlist.' } );
		}
	}
	f.load();
}


// Handle files by path
Application.handleFiles = function( args )
{
	// We start with a bang!
	if( args )
	{
		var ext = args.split( '.' ).pop();
		if( ext.length )
		{
			switch( ext.toLowerCase() )
			{
				case 'mp3':
				case 'ogg':
				case 'flac':
				case 'wav':
					var fn = args.split( ':' )[1];
					if( fn.indexOf( '/' ) > 0 )
						fn = fn.split( '/' ).pop();
					this.receiveMessage( {
						command: 'append_to_playlist_and_play',
						items: [ { Filename: fn, Path: args } ]
					} );
					break;
				case 'pls':
					this.addPlaylist( args );
					break;
			}
		}
		else
		{
			Notify( { title: 'Unhandled file format', text: 'Could not read ' + args } );
		}
	}
}

// Redraws the main application pulldown menu
Application.redrawMenu = function()
{
	this.mainView.setMenuItems( [
		{
			name: i18n( 'i18n_file' ),
			items: [
				{
					name: i18n( 'i18n_about_exotica' ),
					command: 'about_exotica'
				},
				{
					name: i18n( 'i18n_quit' ),
					command: 'quit'
				}
			]
		},
		{
			name: i18n( 'i18n_playlist' ),
			items: [
				{
					name: i18n( 'i18n_load_playlist' ),
					command: 'load_playlist'
				},
				{
					name: i18n( 'i18n_edit_playlist' ),
					command: 'edit_playlist'
				},
				{
					name: i18n( 'i18n_toggle_mini_playlist' + ( this.miniplaylist ? '_hide' : '_show' ) ),
					command: 'toggle_miniplaylist',
					playlist: this.playlist,
					index: this.index
				}
			]
		}
	] );
}

// About exotica view window
Application.openAbout = function()
{
	if( this.aboutWindow ) return;
	this.aboutWindow = new View( {
		title: i18n( 'i18n_about_exotica' ),
		width: 400,
		height: 300
	} );
	var v = this.aboutWindow;
	v.onClose = function()
	{
		Application.aboutWindow = false;
	}
	var f = new File( 'Progdir:Templates/about.html' );
	f.i18n();
	f.onLoad = function( data )
	{
		v.setContent( data );
	}
	f.load();
}

// Shows the playlist editor
Application.editPlaylist = function()
{
	if( this.playlistWindow ) return;
	this.playlistWindow = new View( {
		title: i18n( 'i18n_edit_playlist' ),
		width: 900,
		height: 600
	} );
	var p = this.playlistWindow;
	p.onClose = function()
	{
		Application.playlistWindow = false;
	}
	
	p.setMenuItems ( [
		{
			name: i18n( 'i18n_file' ),
			items: [
				{
					name: i18n( 'i18n_load_playlist' ),
					command: 'load_playlist'
				},
				{
					name: i18n( 'i18n_save_playlist' ),
					command: 'save_playlist'
				},
				{
					name: i18n( 'i18n_save_playlist_as' ),
					command: 'save_playlist_as'
				},
				{
					name: i18n( 'i18n_add_to_playlist' ),
					command: 'add_to_playlist'
				},
				{
					name: i18n( 'i18n_clear_playlist' ),
					command: 'clear_playlist'
				},
				{
					name: i18n( 'i18n_close' ),
					command: 'close_playlisteditor'
				}
			]
		}
	] );
	
	var f = new File( 'Progdir:Templates/playlisteditor.html' );
	f.i18n();
	f.onLoad = function( data )
	{
		p.setContent( data, function()
		{
			p.sendMessage( {
				command: 'refresh',
				items: Application.playlist
			} );
		} );
	}
	f.load();
}

// Opens a playlist using a file dialog
Application.openPlaylist = function()
{
	if( this.of ) return;
	this.of = new Filedialog( this.playlistWindow, function( arr )
	{
		Application.of = false;
	}, '', 'load' );
}

// Adds items from an array to the playlist...
// If no items are specified, you get the option of selecting from a file dialog
Application.addToPlaylist = function( items )
{
	if( !items )
	{
		if( this.af ) return;
		this.af = new Filedialog( this.playlistWindow, function( arr )
		{
			if( arr.length )
			{
				for( var a = 0; a < arr.length; a++ )
				{
					Application.playlist.push( {
						Filename: arr[a].Filename, 
						Path: arr[a].Path
					} );
				}
				if( Application.playlistWindow )
				{
					Application.playlistWindow.sendMessage( {
						command: 'refresh',
						items: Application.playlist
					} );
				}
				Application.receiveMessage( { command: 'get_playlist' } );
			}
			Application.af = false;
		}, '', 'load' );
		return;
	}
	
}

// Receives events from OS and child windows
Application.receiveMessage = function( msg )
{
	if( !msg.command ) return;
	switch( msg.command )
	{
		case 'add_source':
			if( this.playlistWindow )
				this.playlistWindow.sendMessage( msg );
			break;
			
		// Toggle the visibility of the mini playlist
		case 'toggle_miniplaylist':
			this.miniplaylist = this.miniplaylist ? false : true;
			this.mainView.sendMessage( { command: 'toggle_miniplaylist' } );
			this.receiveMessage( { command: 'mini_playlist', index: this.index } );
			this.redrawMenu();
			break;
		// Redraw the mini playlist
		case 'mini_playlist':
			this.mainView.setFlag( 'resize', true );
			this.mainView.setFlag( 'min-height', this.miniplaylist ? 360 : 160 );
			this.mainView.setFlag( 'max-height', this.miniplaylist ? 360 : 160 );
			this.mainView.setFlag( 'height', this.miniplaylist ? 360 : 160 );
			this.mainView.setFlag( 'resize', false );
			this.mainView.sendMessage( { command: 'miniplaylist', playlist: this.playlist, index: this.index, visibility: this.miniplaylist } );
			break;
		case 'about_exotica':
			this.openAbout();
			break;
		case 'get_playlist':
			this.mainView.sendMessage( {
				command:  'updateplaylist',
				playlist: this.playlist,
				index:    this.index
			} );
			if( this.playlistWindow )
			{
				this.playlistWindow.sendMessage( {
					command: 'refresh',
					items: this.playlist
				} );
			}
			break;
		case 'edit_playlist':
			this.editPlaylist();
			break;
		case 'open_playlist':
			this.openPlaylist();
			break;
		case 'add_to_playlist':
			if( msg.items )
			{
				for( var a in msg.items )
					Application.playlist.push( msg.items[a] );
				Application.receiveMessage( { command: 'get_playlist' } );
			}
			else this.addToPlaylist();
			break;
		case 'clear_playlist':
			Application.playlist = [];
			Application.playlistWindow.sendMessage( { command: 'refresh', items: [] } );
			Application.receiveMessage( { command: 'get_playlist' } );
			break;
		case 'load_playlist':
			LoadPlaylist();
			break;
		case 'save_playlist':
			SavePlaylist( Application.playlistFilename );
			break;
		case 'save_playlist_as':
			Application.playlistFilename = false;
			SavePlaylist( false );
			break;
		case 'remove_from_playlist':
			var ne = [];
			for( var a = 0; a < this.playlist.length; a++ )
			{
				if( a == msg.item )
					continue;
				ne.push( this.playlist[a] );
			}
			this.playlist = ne;
			if( Application.playlistWindow )
			{
				Application.playlistWindow.sendMessage( {
					command: 'refresh',
					items: Application.playlist
				} );
			}
			Application.receiveMessage( { command: 'get_playlist' } );
			break;
		// Comes from playlist editor
		case 'set_playlist':
			this.playlist = msg.items;
			this.index = msg.index;
			this.mainView.sendMessage( {
				command:  'updateplaylist',
				playlist: this.playlist,
				index:    this.index
			} );
			break;
		case 'append_to_playlist_and_play':
			if( msg.items.length )
			{
				var added = 0;
				this.index = this.playlist.length;
				for( var a = 0; a < msg.items.length; a++ )
				{
					var it = msg.items[a];
					if( it.Filename.substr( it.Filename.length - 4, 4 ).toLowerCase() == '.pls' )
					{
						var f = new File( it.Path );
						f.onLoad = function( data )
						{
							var ad = 0;
							var m = data.match( /NumberOfEntries\=([0-9]+)/i );
							if( !m )
								return;
							var numOfEntr = parseInt( m[1] );
							var d = data.split( /[\r]{0,1}\n/i );
							//console.log( data );
							for( var a = 0; a < numOfEntr; a++ )
							{
								var path = '';
								var title = '';
								var spath = 'file' + (a+1);
								var stitle = 'title' + (a+1);
								for( var b = 0; b < d.length; b++ )
								{
									if( d[b].substr( 0, spath.length ).toLowerCase() == spath )
									{
										path = d[b].split( '=' )[1];
									}
									else if( d[b].substr( 0, stitle.length ).toLowerCase() == stitle )
									{
										title = d[b].split( '=' )[1];
									}
								}
								// Add it!
								if( path.length && title.length )
								{
									Application.playlist.push( {
										Filename: title,
										Path: path
									} );
									ad++;
								}
							}
							// Yo!
							if( ad > 0 )
							{
								Application.receiveMessage( { command: 'playsong' } );
								if( Application.playlistWindow )
								{
									Application.playlistWindow.sendMessage( {
										command: 'refresh',
										items: Application.playlist
									} );
								}
							}
							Application.receiveMessage( { command: 'get_playlist' } );
						}
						f.load();
					}
					else 
					{
						this.playlist.push( it );
						added++;
					}
				}
				if( added > 0 )
				{
					this.receiveMessage( { command: 'playsong' } );
					if( Application.playlistWindow )
					{
						Application.playlistWindow.sendMessage( {
							command: 'refresh',
							items: Application.playlist
						} );
						Application.receiveMessage ( { command: 'get_playlist' } );
					}
				}
			}
			break;
		case 'close_playlisteditor':
			if( this.playlistWindow )
				this.playlistWindow.close();
			break;
		case 'resizemainwindow':
			this.mainView.setFlag( 'min-height', msg.size );
			break;
		case 'playsongindex':
			this.index = msg.index;
		case 'playsong':
			this.mainView.sendMessage( { command: 'play', index: this.index, item: this.playlist[this.index] } );
			break;
		case 'seek':
			this.index += msg.dir;
			if( this.index < 0 ) this.index = this.playlist.length - 1;
			else if( this.index >= this.playlist.length )
				this.index = 0;
			this.mainView.sendMessage( { command: 'play', item: this.playlist[this.index], index: this.index } );
			break;
		case 'quit':
			Application.quit();
			break;
	}
}

// Shortcut
function ShowPlaylist()
{
	Application.editPlaylist();
}

function SavePlaylist( fn )
{
	if( Application.playlistFilename )
	{
		var pl = JSON.stringify( Application.playlist );
		( new File() ).save( pl, Application.playlistFilename );
		return;
	}
	var path = fnam = false;
	if( fn )
	{
		path = fn.indexOf( '/' ) > 0 ? fn.split( '/' ) : fn.split( ':' );
		fnam = path.pop();
	}
	Filedialog( Application.mainView, function( fdat )
	{
		if( !fdat ) return;
		var pl = JSON.stringify( Application.playlist );
		( new File() ).save( pl, fdat );
		Application.playlistFilename = fdat;
	}, path, 'save', fnam, i18n( 'i18n_save_playlist' ) );
}

function LoadPlaylist()
{
	var fn = Application.playlistFilename;
	var path = fnam = false;
	if( fn )
	{
		path = fn.indexOf( '/' ) > 0 ? fn.split( '/' ) : fn.split( ':' );
		fnam = path.pop();
	}
	Filedialog( Application.mainView, function( fdat )
	{
		if( !fdat ) return;
		var f = new File( fdat[0].Path );
		f.onLoad = function( data )
		{
			Application.playlist = JSON.parse( data );
			if( Application.playlist )
			{
				Application.playlistFilename = fdat[0].Path;
			}
			else
			{
				Application.playlist = [];
			}
			if( Application.playlistWindow )
			{
				Application.playlistWindow.sendMessage( {
					command: 'refresh',
					items: Application.playlist
				} );
			}
			Application.mainView.sendMessage( { command: 'play', item: Application.playlist[Application.index] } );
			Application.receiveMessage( { command: 'get_playlist' } );
		}
		f.load();
	}, path, 'load', fnam, i18n( 'i18n_load_playlist' ) );
}

