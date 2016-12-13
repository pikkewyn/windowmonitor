// -*- coding:utf-8-unix; mode:c; -*-

//gcc gistfile1.c $(pkg-config --cflags --libs x11 xmu)
//-L/usr/X11R6/lib -lX11

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

#include <X11/extensions/scrnsaver.h>
#include <X11/Xlib.h>           // `apt-get install libx11-dev`
#include <X11/Xmu/WinUtil.h>    // `apt-get install libxmu-dev`

#include "list.h"

#ifdef DEBUGON
#define DEBUG printf x
#else
#define DEBUG(x) do {} while(0)
#endif

Bool xerror = False;
static bool run = true;

Display* open_display()
{
	DEBUG( ( "connecting X server ... " ) );
	Display* d = XOpenDisplay( NULL );

	if( d == NULL )
	{
			DEBUG( ( "fail\n" ) );
			exit( 1 );
	}
	else
	{
			DEBUG( ( "success\n" ) );
	}

	return d;
}

int handle_error( Display* display, XErrorEvent* error )
{
	( void ) display;
	( void ) error;
	DEBUG( ( "ERROR: X11 error\n" ) );
	xerror = True;
	return 1;
}

Window get_focus_window( Display* d )
{
	Window w;
	int revert_to;
	XGetInputFocus( d, &w, &revert_to );

	if( xerror )
	{
			DEBUG( ( "fail\n" ) );
			exit( 1 );
	}
	else if( w == None )
	{
			DEBUG( ( "no focus window\n" ) );
			exit( 1 );
	}

	return w;
}

Window get_top_window( Display* d, Window start )
{
	Window w = start;
	Window parent = start;
	Window root = None;
	Window* children;
	unsigned int nchildren;
	Status s;
	DEBUG( ( "getting top window ... \n" ) );

	while( parent != root )
	{
			w = parent;
			s = XQueryTree( d, w, &root, &parent, &children, &nchildren ); // see man

			if( s )
			{
					XFree( children );
			}

			if( xerror )
			{
					DEBUG( ( "fail\n" ) );
					exit( 1 );
			}

			DEBUG( ( "  get parent (window: %d)\n", ( int )w ) );
	}

	DEBUG( ( "success (window: %d)\n", ( int )w ) );
	return w;
}

Window get_named_window( Display* d, Window start )
{
	Window w;
	DEBUG( ( "getting named window ... " ) );
	w = XmuClientWindow( d, start );

	if( w == start )
	{
			DEBUG( ( "fail\n" ) );
	}

	DEBUG( ( "success (window: %d)\n", ( int ) w ) );
	return w;
}

char* get_window_title( Display* d, Window w )
{
	XTextProperty prop;
	char* result = NULL;
	DEBUG( ( "window name:\n" ) );

	if( XGetWMName( d, w, &prop ) != 0 )
	{
			result = strdup( ( char* )prop.value );
	}

	XFree( prop.value );
	return result;
}

char* get_window_class( Display* d, Window w )
{
	Status s;
	XClassHint* class;
	class = XAllocClassHint();
	char* window_class = NULL;

	if( xerror )
	{
			DEBUG( ( "ERROR: XAllocClassHint\n" ) );
	}

	s = XGetClassHint( d, w, class );

	if( s )
	{
			window_class = strdup( class->res_name );
			DEBUG( ( "\tname: %s\n\tclass: %s\n", class->res_name, class->res_class ) );
	}
	else
	{
			DEBUG( ( "ERROR: XGetClassHint\n" ) );
	}

	XFree( class->res_name );
	XFree( class->res_class );
	XFree( class );
	return window_class;
}

unsigned long idle( Display* display )
{
	XScreenSaverInfo* SS_info = XScreenSaverAllocInfo();
	XScreenSaverQueryInfo( display, DefaultRootWindow( display ), SS_info );
	return SS_info->idle / 1000;
}

void sigint_handler( int const signum )
{
	run = false;
	printf( "signal handler\n" );
}

bool set_sigint_action()
{
	struct sigaction sig_action;
	sig_action.sa_handler = sigint_handler;
	sig_action.sa_flags = 0;
	sigemptyset( &sig_action.sa_mask );

	if( sigaction( SIGINT, &sig_action, NULL ) == -1 )
	{
			perror( "sigaction" );
			return false;
	}

	return true;
}

int LongestCommonSubstring( char const* str1, char const* str2 )
{
     if( str1 == NULL || str2 == NULL )
     {
          return 0;
     }

		 int str1_len = strlen( str1 );
		 int str2_len = strlen( str2 );

		 int buf_len = str1_len > str2_len ? str1_len : str2_len;

     int *curr = calloc( buf_len, 1 );
     int *prev = calloc( buf_len, 1 );
     int *swap = NULL;
     int maxSubstr = 0;

     for(int i = 0; i<str1_len; ++i)
     {
          for(int j = 0; j<str2_len; ++j)
          {
               if(str1[i] != str2[j])
               {
                    curr[j] = 0;
               }
               else
               {
                    if(i == 0 || j == 0)
                    {
                         curr[j] = 1;
                    }
                    else
                    {
                         curr[j] = 1 + prev[j-1];
                    }
                    //The next if can be replaced with:
                    //maxSubstr = max(maxSubstr, curr[j]);
                    //(You need algorithm.h library for using max())
                    if(maxSubstr < curr[j])
                    {
                         maxSubstr = curr[j];
                    }
               }
          }
          swap=curr;
          curr=prev;
          prev=swap;
     }
     free( curr );
     free( prev );
     return maxSubstr;
}

int main( void )
{
	if( set_sigint_action() == false )
			return EXIT_FAILURE;

	Display* d;
	Window w;
	setlocale( LC_ALL, "" );
	d = open_display();
	XSetErrorHandler( handle_error );

	struct List* title_list = list_new();

	while( run )
	{
		w = get_focus_window( d );
		w = get_top_window( d, w );
		w = get_named_window( d, w );
		char* window_title = get_window_title( d, w );
		char* window_class = get_window_class( d, w );

		list_accumulated_insert( title_list, window_title );
		printf( "window class: %s\nwindow title: %s\nidle: %ld\n", window_class, window_title, idle( d ) );
		
		sleep( 10 );
	}


	title_list->head = List_merge_sort( title_list->head );

	list_for_each( title_list, list_node_print );

	//free( window_class );
	//free( window_title );
	XCloseDisplay( d );
}

/*TODO:
* if idle < 10
* accumulate add to list title and class and timestamp ( in one struct !!), meanwhile remove older elements
* on SIGTERM:
	Catch SIGTERM. In the handler, read the /var/run/utmp file to get the runlevel. See the source code of the runlevel(8) command for reference.
	save to logfile
* on SIGUSR1...
* data can be also ketp in shared memory
* split by the last hyphen ( first from the end )
*/
  
