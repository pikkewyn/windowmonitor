// -*- coding:utf-8-unix; mode:c; -*-

//gcc gistfile1.c $(pkg-config --cflags --libs x11 xmu)
//-L/usr/X11R6/lib -lX11

#include <assert.h>
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

char* longest_common_string( char const* string1, char const* string2 ) 
{
  if( string1 == NULL || string2 == NULL )
    return NULL;

	int strlen1 = strlen( string1 );
	int strlen2 = strlen( string2 );

	int longest = 0;
	
  int **ptr = malloc( 2 * sizeof( int* ) );
	
  int* ret = calloc( strlen1 + 1, sizeof( int ) );

  for( int i = 0; i < 2; i++ )
		ptr[ i ] = calloc( strlen2, sizeof( int ) );

	int k = 0;
	
  for( int i = 0; i < strlen1; i++ ) 
  {
		memcpy( ptr[0], ptr[1], strlen2 * sizeof( int ) );
		for (int j = 0; j < strlen2; j++) 
    {
			if (string1[i] == string2[j]) 
      {
				if (i == 0 || j == 0) 
        {
					ptr[1][j] = 1;
				} else 
        {
					ptr[1][j] = ptr[0][j-1] + 1;
				}
				if (ptr[1][j] > longest) 
        {
					longest = ptr[1][j];
					k = 0;
					ret[k++] = longest;
				}
				if (ptr[1][j] == longest) 
        {
					ret[k++] = i;
					ret[k] = -1;
				}
			} else 
      {
				ptr[1][j] = 0;
			}
		}
	}

	for (int i = 0; i < 2; i++)
		free(ptr[i]);
	free(ptr);

	ret[0] = longest;
	
  char* result = calloc( strlen1, 1 ); 
  
	for( int i = 0; ret[++i] != -1 && i < strlen1; ) 
  {
		sprintf( result, "%.*s", longest, &string1[ret[i] - longest+1] );
	}

  free( ret );
  return result;
}

void test_longest_common_string( void )
{
    //char const* string1 = "Ala ma kota"; 
    //char const* string2 = ", a kot jest Ali"; 

    //char* result = longest_common_string( string1, string2 );
    //TODO: add assert

    //printf( "%s\n", result );
}

bool str_is_similar( char const* str1, char const* str2, int factor )
{
  bool result = false;
 
  if( strcmp( str1, str2 ) == 0 )
  {
      return true;
  }

  char* substring = longest_common_string( str1, str2 );

  if( substring == NULL )
  {
    return false;
  }

  if( strlen( substring ) >= factor )
    result = true;
  
  free( substring );

  return result;
}


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
  unsigned long result = SS_info->idle / 1000;
  XFree( SS_info );

  return result;
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


bool hyphen_split( char const* input, char** before, char** after )
{
  char* hyphen = strrchr( input, '-' );

  if( *before != NULL || *after != NULL )
  {
      return false;
  }

  if( hyphen == NULL || *( hyphen + 1 ) == '\0' ||  *( hyphen + 2 ) == '\0' )
  {
      return false;
  }

  *before = strndup( input, hyphen - input );
  *after = strdup( hyphen + 2 );

  return true;
}

void test_hypen_split( void )
{
  {
    char const* text = "- ";
    char* description = NULL;
    char* name = NULL;
    assert( hyphen_split( text, &description, &name ) == false );
  }
  {
    char const* text = "ala ma - kota";
    char* description = NULL;
    char* name = NULL;
    assert( hyphen_split( text, &description, &name ) == true );
    free( description );
    free( name );
  }
  {
    char const* text = "ala - ma";
    char* description = NULL;
    char* name = NULL;
    hyphen_split( text, &description, &name );
    assert( strcmp( description, "ala " ) == 0 );
    assert( strcmp( name, "ma" ) == 0 );

    free( description );
    free( name );
  }
}

int main( void )
{
  test_hypen_split();
  test_longest_common_string();
	
  if( set_sigint_action() == false )
			return EXIT_FAILURE;

	Display* d;
	Window w;
	setlocale( LC_ALL, "" );
	d = open_display();
	XSetErrorHandler( handle_error );

	struct List* title_list = list_new();
	struct List* description_list = list_new();

	while( run )
	{
		w = get_focus_window( d );
		w = get_top_window( d, w );
		w = get_named_window( d, w );
		
    char* window_title = get_window_title( d, w );
    char* description = NULL;
    char* title = NULL;
    int const factor = 5;
    
    if( hyphen_split( window_title, &description, &title ) )
    {
        free( window_title );
        list_accumulated_insert( title_list, title, str_is_similar, factor );
		    list_accumulated_insert( description_list, description, str_is_similar, factor );
        printf( "window title: %s\nwindow description: %s\nidle: %ld\n", title, description, idle( d ) );
    }
    else {
        list_accumulated_insert( title_list, window_title, str_is_similar, factor );
        printf( "window title: %s\nidle: %ld\n", window_title, idle( d ) );
    }
		
		
		sleep( 10 );
	}


	title_list->head = List_merge_sort( title_list->head );
	description_list->head = List_merge_sort( description_list->head );
  //TODO: seralize here to locked file

	list_for_each( title_list, list_node_print );
	list_for_each( description_list, list_node_print );

  list_free( title_list );
  list_free( description_list );

	//free( window_class );
	//free( window_title );
	XCloseDisplay( d );
}

/*TODO:
* if idle < 10
* accumulate add to list title  and timestamp ( in one struct !!), meanwhile remove older elements
* on SIGTERM:
	Catch SIGTERM. In the handler, read the /var/run/utmp file to get the runlevel. See the source code of the runlevel(8) command for reference.
	save to logfile
* on SIGUSR1 just dump sorted to file without ending( fork when proper variable is set and dump copy? )
* data can be also ketp in shared memory: ./ltalk2/test/ltalk-shared/server.c:    pthread_mutexattr_setpshared( &attributes, PTHREAD_PROCESS_SHARED );
* split by the last hyphen ( first from the end )
* handle new fields in list - actually three different lists
* use hypen split
* alfred-like display
* static functions, consts
* case insesitive characters comparison
*/
  
