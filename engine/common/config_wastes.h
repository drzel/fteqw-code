// The Wastes' config.h
// We support both GL and Vulkan.
// I want to get this mostly running on all systems post 2000.

//general rebranding
#define DISTRIBUTION "VTW"
#define DISTRIBUTIONLONG "Vera Visions"
#define FULLENGINENAME "The Wastes"
#define ENGINEWEBSITE "https://www.vera-visions.com/"
#define BRANDING_ICON "wastes.ico"

//filesystem rebranding
#define GAME_SHORTNAME		"wastes"	//short alphanumeric description
#define GAME_FULLNAME		FULLENGINENAME 	//full name of the game we're playing
#define GAME_BASEGAMES		GAME_SHORTNAME	//comma-separate list of basegame strings to use
#define GAME_PROTOCOL		"FTE-TW"	//so other games won't show up in the server browser
#define GAME_DEFAULTPORT	23000	// This is our port. Don't mess with it or use it for any custom games
//#define GAME_IDENTIFYINGFILES	NULL	//with multiple games, this string-list gives verification that the basedir is actually valid. if null, will just be assumed correct.
//#define GAME_DOWNLOADSURL	NULL	//url for the package manger to update from
//#define GAME_DEFAULTCMDS	NULL	//a string containing the things you want to 

// All my fault -eukara
#define ENGINE_ROUTING
#undef USE_INTERNAL_BULLET
#define MENU_NATIVECODE // Will make a debut eventually, hopefully

// What do we use
#ifndef GLQUAKE
#define GLQUAKE
#endif

#ifndef VKQUAKE
#define VKQUAKE
#endif

#define PLUGINS		//support for external plugins (like huds or fancy menus or whatever)
#define HAVE_PACKET
#define QUAKETC
#define AVAIL_OPENAL
#define AVAIL_ZLIB
#define AVAIL_OGGVORBIS
#define NOMEDIA
#define CL_MASTER
#define CSQC_DAT
#define MENU_DAT
#define PSET_SCRIPT
#define VOICECHAT
#define RTLIGHTS
#ifndef MULTITHREAD
#define MULTITHREAD	//misc basic multithreading - dsound, downloads, basic stuff that's unlikely to have race conditions.
#endif
#define LOADERTHREAD	// worker threads for loading misc stuff. falls back on main thread if not supported.
#define USEAREAGRID		// world collision optimisation
#define USERBE
#define IMAGEFMT_DDS	// .dds files embed mipmaps and texture compression. faster to load.
#define DECOMPRESS_ETC2
#define DECOMPRESS_RGTC
#define DECOMPRESS_S3TC
#define USE_VORBISFILE
#define NOBUILTINMENUS
#define NOLEGACY //just spike trying to kill off crappy crap...
#define AVAIL_DINPUT

#ifndef DEBUG
#define NOQCDESCRIPTIONS 2 //if 2, disables writing fteextensions.qc completely.
#endif

// Various package formats
#define PACKAGE_PK3
#define PACKAGE_TEXWAD // HL content support, WAD3
#define PACKAGE_Q1PAK // HL content support, PAK
#undef PACKAGE_DOOMWAD

// Map formats
#define Q3BSPS // What we use exclusively
#define Q1BSPS // HL content support
#undef Q2BSPS
#undef RFBSPS
#undef TERRAIN
#undef DOOMWADS
#undef MAP_PROC

// Model formats
#define INTERQUAKEMODELS
#define HALFLIFEMODELS // HL content support
#undef SPRMODELS
#undef SP2MODELS
#undef DSPMODELS
#undef MD1MODELS
#undef MD2MODELS
#undef MD3MODELS
#undef MD5MODELS
#undef ZYMOTICMODELS
#undef DPMMODELS
#undef PSKMODELS

// What do we NOT want to use
#undef MVD_RECORDING //server can record MVDs.
#undef D3D9QUAKE
#undef D3D11QUAKE
#undef D3D8QUAKE
#undef AVAIL_WASAPI	//windows advanced sound api
#undef AVAIL_DSOUND
#undef BOTLIB_STATIC	//q3 botlib
#undef AVAIL_XZDEC	//.xz decompression
#undef AVAIL_SPEEX	//.xz decompression
#undef AVAIL_GZDEC	//.gz decompression
#undef AVAIL_DZIP	//.dzip special-case archive support
#undef AVAIL_PNGLIB	//.png image format support (read+screenshots)
#undef AVAIL_JPEGLIB	//.jpeg image format support (read+screenshots)
#undef AVAIL_MP3_ACM	//.mp3 support (in windows).
#undef IMAGEFMT_KTX
#undef IMAGEFMT_PKM
#undef IMAGEFMT_BLP	//legacy crap
#undef NETPREPARSE	//allows for running both nq+qw on the same server (if not, protocol used must match gamecode).
#undef USE_SQLITE	//sql-database-as-file support
#undef QUAKESTATS	//defines STAT_HEALTH etc. if omitted, you'll need to provide that functionality yourself.
#undef QUAKEHUD		//support for drawing the vanilla hud.
#undef QWSKINS		//disabling this means no qw .pcx skins nor enemy/team skin/colour forcing
#undef SVRANKING	//legacy server-side ranking system.
#undef RAGDOLL		//ragdoll support. requires RBE support.
#undef HUFFNETWORK	//crappy network compression. probably needs reseeding.
#undef SVCHAT		//ancient lame builtin to support NPC-style chat...
#undef VM_Q1		//q1qvm implementation, to support ktx.
#undef Q2SERVER		//q2 server+gamecode.
#undef Q2CLIENT		//q2 client. file formats enabled separately.
#undef Q3CLIENT		//q3 client stuff.
#undef Q3SERVER		//q3 server stuff.
#undef HEXEN2		//runs hexen2 gamecode, supports hexen2 file formats.
#undef NQPROT		//act as an nq client/server, with nq gamecode.
#undef WEBSERVER	//sv_ftp + sv_http cvars.
#undef WEBCLIENT	//uri_get+any internal downloads etc
#undef RUNTIMELIGHTING	//automatic generation of .lit files
#undef R_XFLIP		//old silly thing
#undef TEXTEDITOR	//my funky text editor! its awesome!
#undef TCPCONNECT	//support for playing over tcp sockets, instead of just udp. compatible with qizmo.
#undef IRCCONNECT	//lame support for routing game packets via irc server. not a good idea.
#undef SUPPORT_ICE	//Internet Connectivity Establishment, for use by plugins to establish voice or game connections.
#undef PSET_CLASSIC	//support the 'classic' particle system, for that classic quake feel.
#undef HAVE_CDPLAYER	//includes cd playback. actual cds. named/numbered tracks are supported regardless (though you need to use the 'music' command to play them without this).
#undef QTERM
#undef SIDEVIEWS
#undef MAX_SPLITS
#undef SUBSERVERS
#undef SV_MASTER
#undef HAVE_MIXER	//openal only
#undef VM_LUA
#undef HLCLIENT
#undef HLSERVER
#undef FTPSERVER
//undef CLIENTONLY	//leave this up to the makefiles.
#undef HAVE_TCP
#undef HAVE_GNUTLS	//linux tls/dtls support
#undef HAVE_WINSSPI	//windows tls/dtls support
#undef HAVE_JUKEBOX	//includes built-in jukebox crap
#undef HAVE_MEDIA_DECODER	//can play cin/roq, more with plugins
#undef HAVE_MEDIA_ENCODER	//capture/capturedemo work.
#undef HAVE_SPEECHTOTEXT	//windows speech-to-text thing
//#define SAVEDGAMES			//Can save the game.
#undef PACKAGEMANAGER			//Allows the user to enable/disable/download packages and plugins.
#undef HEADLESSQUAKE
#undef WAYLANDQUAKE
#undef AVAIL_FREETYPE	// for truetype font rendering

#ifdef COMPILE_OPTS
//things to configure qclib, which annoyingly doesn't include this file itself
-DOMIT_QCC	//disable the built-in qcc 
-DSIMPLE_QCVM	//disable qc debugging and 32bit opcodes
#ifndef AVAIL_ZLIB
-DNO_ZLIB	//disable zlib
#endif

#ifdef USE_INTERNAL_BULLET	//makefile will respond to this by trying to link bullet into the engine itself, instead of as a plugin.
-DLINK_INTERNAL_BULLET
#endif

-DNO_SPEEX	//disable static speex
#ifndef BOTLIB_STATIC
-DNO_BOTLIB	//disable static botlib
#endif

-DLIBVORBISFILE_STATIC

//-Os //optimise for size instead of speed. less cpu cache needed means that its sometimes faster anyway.
#endif
