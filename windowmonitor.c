// -*- coding:utf-8-unix; mode:c; -*-

//gcc gistfile1.c $(pkg-config --cflags --libs x11 xmu)
//-L/usr/X11R6/lib -lX11

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

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

struct Title_occurence {
  char* title;
  unsigned int occurences;
};

struct Class_occurence {
  char* title;
  unsigned int occurences;
};


int main( void )
{
    Display* d;
    Window w;
    setlocale( LC_ALL, "" );
    d = open_display();
    XSetErrorHandler( handle_error );
    struct List* titles_list = list_new();
    struct List* class_list = list_new();

    w = get_focus_window( d );
    w = get_top_window( d, w );
    w = get_named_window( d, w );
    char* window_title = get_window_title( d, w );
    char* window_class = get_window_class( d, w );

    

    printf( "window class: %s\nwindow title: %s\nidle: %ld\n", window_class, window_title, idle( d ) );
    free( window_class );
    free( window_title );
    XCloseDisplay( d );
}

